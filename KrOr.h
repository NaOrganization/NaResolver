#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace EncryptedText
{
    namespace Detail
    {
        [[nodiscard]] constexpr std::uint64_t RotateLeft(
            std::uint64_t value,
            unsigned int shift) noexcept
        {
            shift &= 63U;
            return (value << shift) | (value >> ((64U - shift) & 63U));
        }

        [[nodiscard]] constexpr std::uint64_t Mix(std::uint64_t value) noexcept
        {
            value ^= value >> 30U;
            value *= 0xBF58476D1CE4E5B9ULL;
            value ^= value >> 27U;
            value *= 0x94D049BB133111EBULL;
            return value ^ (value >> 31U);
        }

        [[nodiscard]] constexpr std::uint64_t NormalizeSeed(std::uint64_t seed) noexcept
        {
            return seed == 0ULL ? 0xA0761D6478BD642FULL : seed;
        }

        [[nodiscard]] constexpr std::uint64_t KeyWord(
            std::uint64_t seed,
            std::size_t blockIndex) noexcept
        {
            const std::uint64_t step =
                (static_cast<std::uint64_t>(blockIndex) + 1ULL) * 0x9E3779B97F4A7C15ULL;
            const unsigned int rotation = static_cast<unsigned int>((blockIndex * 17U) & 63U);
            return Mix(seed ^ step ^ RotateLeft(seed, rotation));
        }

        template <std::size_t N>
        constexpr void Absorb(std::uint64_t& state, const char(&text)[N]) noexcept
        {
            for (std::size_t i = 0U; i + 1U < N; ++i)
            {
                state ^= static_cast<unsigned char>(text[i]);
                state *= 0x100000001B3ULL;
                state = RotateLeft(state, 11U);
            }
        }

        template <typename CharT, std::size_t N>
        [[nodiscard]] consteval std::uint64_t HashLiteral(
            const CharT(&literal)[N]) noexcept
        {
            using UnsignedCharT = std::make_unsigned_t<CharT>;
            std::uint64_t state = 0xCBF29CE484222325ULL;

            for (std::size_t i = 0U; i < N; ++i)
            {
                std::uint64_t value = static_cast<std::uint64_t>(
                    static_cast<UnsignedCharT>(literal[i]));

                for (std::size_t byteIndex = 0U; byteIndex < sizeof(CharT); ++byteIndex)
                {
                    state ^= value & 0xFFULL;
                    state *= 0x100000001B3ULL;
                    value >>= 8U;
                }
            }

            state ^= static_cast<std::uint64_t>(N * sizeof(CharT));
            return NormalizeSeed(Mix(state));
        }

        template <std::size_t FileN>
        [[nodiscard]] constexpr std::uint64_t BuildCallSiteSeed(
            const char(&file)[FileN],
            std::uint64_t line,
            std::uint64_t counter) noexcept
        {
            std::uint64_t state = 0x6A09E667F3BCC909ULL;
            Absorb(state, file);
            state ^= Mix(line + 0x9E3779B97F4A7C15ULL);
            state ^= Mix(counter + 0xD1B54A32D192ED03ULL);
            return NormalizeSeed(Mix(state));
        }

        template <typename CharT, std::size_t N>
        [[nodiscard]] consteval std::uint64_t BuildBlock(
            const CharT(&literal)[N],
            std::uint64_t seed,
            std::size_t blockIndex) noexcept
        {
            constexpr std::size_t kCodeUnitBits = sizeof(CharT) * 8U;
            constexpr std::size_t kUnitsPerBlock = sizeof(std::uint64_t) / sizeof(CharT);
            constexpr std::uint64_t kCodeUnitMask = (1ULL << kCodeUnitBits) - 1ULL;
            std::uint64_t plain = Mix(seed ^ static_cast<std::uint64_t>(blockIndex));

            for (std::size_t unitIndex = 0U; unitIndex < kUnitsPerBlock; ++unitIndex)
            {
                const std::size_t literalIndex = blockIndex * kUnitsPerBlock + unitIndex;
                if (literalIndex >= N)
                {
                    break;
                }

                using UnsignedCharT = std::make_unsigned_t<CharT>;
                const std::uint64_t value = static_cast<std::uint64_t>(
                    static_cast<UnsignedCharT>(literal[literalIndex]));
                const std::size_t shift = unitIndex * kCodeUnitBits;
                plain &= ~(kCodeUnitMask << shift);
                plain |= (value & kCodeUnitMask) << shift;
            }

            return plain ^ KeyWord(seed, blockIndex);
        }

        [[nodiscard]] inline std::uint64_t RuntimeAnchor() noexcept
        {
            static const volatile std::uint64_t kRuntimeZero = 0ULL;
            return kRuntimeZero;
        }

        [[nodiscard]] inline std::uint64_t LoadFromRegister(std::uint64_t value) noexcept
        {
#if defined(__GNUC__) || defined(__clang__)
            asm("" : "=r"(value) : "0"(value));
            return value;
#else
            volatile std::uint64_t registerValue = value;
            return registerValue;
#endif
        }

        template <typename EncryptedT>
        [[nodiscard]] consteval typename EncryptedT::ValueType ReadCodeUnit(
            const EncryptedT& encrypted,
            std::size_t index) noexcept
        {
            using CharT = typename EncryptedT::ValueType;
            using UnsignedCharT = std::make_unsigned_t<CharT>;
            constexpr std::size_t kCodeUnitBits = sizeof(CharT) * 8U;
            constexpr std::uint64_t kCodeUnitMask = (1ULL << kCodeUnitBits) - 1ULL;
            const std::size_t blockIndex = index / EncryptedT::kUnitsPerBlock;
            const std::size_t unitIndex = index % EncryptedT::kUnitsPerBlock;
            const std::size_t shift = unitIndex * kCodeUnitBits;
            const std::uint64_t plain =
                encrypted.blocks[blockIndex] ^ KeyWord(encrypted.seed, blockIndex);
            return static_cast<CharT>(
                static_cast<UnsignedCharT>((plain >> shift) & kCodeUnitMask));
        }

        inline void SecureWipe(void* memory, std::size_t bytes) noexcept
        {
            volatile auto* cursor = static_cast<volatile unsigned char*>(memory);
            while (bytes-- != 0U)
            {
                *cursor++ = 0U;
            }
        }
    }

    template <typename CharT, std::size_t N>
    struct EncryptedString;

    template <typename CharT, std::size_t N>
    class DecryptedString final
    {
        static_assert(N > 0U, "EncryptedString requires a null-terminated literal.");

    public:
        using ValueType = CharT;
        using StringType = std::basic_string<CharT>;
        using ViewType = std::basic_string_view<CharT>;

        DecryptedString(const DecryptedString&) = delete;
        DecryptedString& operator=(const DecryptedString&) = delete;

        DecryptedString(DecryptedString&& other) noexcept
            : data_(other.data_), wiped_(other.wiped_)
        {
            other.Wipe();
        }

        DecryptedString& operator=(DecryptedString&& other) noexcept
        {
            if (this != &other)
            {
                Wipe();
                data_ = other.data_;
                wiped_ = other.wiped_;
                other.Wipe();
            }
            return *this;
        }

        ~DecryptedString() noexcept
        {
            Wipe();
        }

        [[nodiscard]] static constexpr std::size_t Size() noexcept
        {
            return N - 1U;
        }

        [[nodiscard]] static constexpr bool Empty() noexcept
        {
            return N == 1U;
        }

        [[nodiscard]] const CharT* CStr() const noexcept
        {
            return data_.data();
        }

        [[nodiscard]] const CharT* Data() const noexcept
        {
            return data_.data();
        }

        [[nodiscard]] ViewType View() const noexcept
        {
            return ViewType{ data_.data(), Size() };
        }

        [[nodiscard]] StringType ToString() const
        {
            return StringType{ data_.data(), Size() };
        }

        [[nodiscard]] const CharT& operator[](std::size_t index) const noexcept
        {
            return data_[index];
        }

        [[nodiscard]] operator const CharT* () const noexcept
        {
            return CStr();
        }

        [[nodiscard]] explicit operator ViewType() const noexcept
        {
            return View();
        }

        [[nodiscard]] explicit operator StringType() const
        {
            return ToString();
        }

        void Wipe() noexcept
        {
            Detail::SecureWipe(data_.data(), sizeof(data_));
            wiped_ = true;
        }

        [[nodiscard]] bool IsWiped() const noexcept
        {
            return wiped_;
        }

    private:
        template <typename, std::size_t>
        friend struct EncryptedString;

        DecryptedString() noexcept = default;

        std::array<CharT, N> data_ = {};
        bool wiped_ = false;
    };

    // All non-static data members intentionally remain public so this is a C++20
    // structural type and can be used directly as a non-type template parameter.
    template <typename CharT, std::size_t N>
    struct EncryptedString
    {
        static_assert(N > 0U, "EncryptedString requires a null-terminated literal.");
        static_assert(std::is_integral_v<CharT>, "EncryptedString supports integral character types only.");
        static_assert(sizeof(CharT) == 1U || sizeof(CharT) == 2U || sizeof(CharT) == 4U,
            "EncryptedString supports 8-bit, 16-bit, and 32-bit character types.");

        using ValueType = CharT;

        static constexpr std::size_t kUnitsPerBlock = sizeof(std::uint64_t) / sizeof(CharT);
        static constexpr std::size_t kBlockCount = (N + kUnitsPerBlock - 1U) / kUnitsPerBlock;

        std::array<std::uint64_t, kBlockCount> blocks = {};
        std::uint64_t seed = 0ULL;

        consteval EncryptedString(const CharT(&literal)[N]) noexcept
            : EncryptedString(literal, Detail::HashLiteral(literal))
        {
        }

        consteval EncryptedString(
            const CharT(&literal)[N],
            std::uint64_t seedValue) noexcept
            : seed(Detail::NormalizeSeed(seedValue))
        {
            for (std::size_t blockIndex = 0U; blockIndex < kBlockCount; ++blockIndex)
            {
                blocks[blockIndex] = Detail::BuildBlock(literal, seed, blockIndex);
            }
        }

        template <std::size_t OtherN>
        [[nodiscard]] consteval auto Concat(
            const EncryptedString<CharT, OtherN>& other) const noexcept
        {
            constexpr std::size_t kCombinedSize = N + OtherN - 1U;
            CharT combined[kCombinedSize] = {};

            for (std::size_t i = 0U; i + 1U < N; ++i)
            {
                combined[i] = Detail::ReadCodeUnit(*this, i);
            }

            for (std::size_t i = 0U; i < OtherN; ++i)
            {
                combined[N - 1U + i] = Detail::ReadCodeUnit(other, i);
            }

            const std::uint64_t combinedSeed = Detail::NormalizeSeed(Detail::Mix(
                seed ^
                Detail::RotateLeft(other.seed, 29U) ^
                (static_cast<std::uint64_t>(kCombinedSize) * 0x9E3779B97F4A7C15ULL)));
            return EncryptedString<CharT, kCombinedSize>{combined, combinedSeed};
        }

        [[nodiscard]] DecryptedString<CharT, N> Decrypt() const noexcept
        {
            constexpr std::size_t kCodeUnitBits = sizeof(CharT) * 8U;
            constexpr std::uint64_t kCodeUnitMask = (1ULL << kCodeUnitBits) - 1ULL;
            DecryptedString<CharT, N> result;
            const std::uint64_t runtimeSeed = seed ^ Detail::RuntimeAnchor();

            for (std::size_t blockIndex = 0U; blockIndex < kBlockCount; ++blockIndex)
            {
                const std::uint64_t cipher = Detail::LoadFromRegister(blocks[blockIndex]);
                const std::uint64_t plain = cipher ^ Detail::KeyWord(runtimeSeed, blockIndex);

                for (std::size_t unitIndex = 0U; unitIndex < kUnitsPerBlock; ++unitIndex)
                {
                    const std::size_t outputIndex = blockIndex * kUnitsPerBlock + unitIndex;
                    if (outputIndex >= N)
                    {
                        break;
                    }

                    const std::size_t shift = unitIndex * kCodeUnitBits;
                    using UnsignedCharT = std::make_unsigned_t<CharT>;
                    result.data_[outputIndex] = static_cast<CharT>(
                        static_cast<UnsignedCharT>((plain >> shift) & kCodeUnitMask));
                }
            }

            return result;
        }
    };

    template <typename CharT, std::size_t N>
    EncryptedString(const CharT(&)[N]) -> EncryptedString<CharT, N>;

    template <typename CharT, std::size_t N>
    [[nodiscard]] consteval auto MakeEncryptedString(
        const CharT(&literal)[N],
        std::uint64_t callSiteSeed) noexcept
    {
        const std::uint64_t seed = Detail::NormalizeSeed(
            Detail::HashLiteral(literal) ^ callSiteSeed);
        return EncryptedString<CharT, N>{literal, seed};
    }
}

#if defined(__COUNTER__)
#define ENCRYPTED_TEXT_DETAIL_COUNTER __COUNTER__
#else
#define ENCRYPTED_TEXT_DETAIL_COUNTER __LINE__
#endif

// Produces an EncryptedText::EncryptedString structural value suitable for use
// directly as a C++20 non-type template argument.
#define ENCRYPTED_STRING(literal)                                                                    \
    (::EncryptedText::MakeEncryptedString(                                                           \
        literal,                                                                                     \
        ::EncryptedText::Detail::BuildCallSiteSeed(                                                  \
            __FILE__,                                                                                \
            static_cast<std::uint64_t>(__LINE__),                                                    \
            static_cast<std::uint64_t>(ENCRYPTED_TEXT_DETAIL_COUNTER))))

#if defined(_M_X64) || defined(__SSE2__)
#include <immintrin.h>
#define KROR_DETAIL_HAS_SSE2 1
#elif defined(_M_ARM64) || defined(__aarch64__)
#include <arm_neon.h>
#define KROR_DETAIL_HAS_NEON 1
#endif

#if defined(_MSC_VER)
#define KROR_FORCEINLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#define KROR_FORCEINLINE inline __attribute__((always_inline))
#else
#define KROR_FORCEINLINE inline
#endif

#define KROR_DETAIL_JOIN_INNER(left, right) left##right
#define KROR_DETAIL_JOIN(left, right) KROR_DETAIL_JOIN_INNER(left, right)
#define KROR_DETAIL_LINE_U KROR_DETAIL_JOIN(__LINE__, U)

#if defined(KROR_BUILD_SEED)
#define KROR_DETAIL_BUILD_DATE "KrOr-reproducible-build"
#define KROR_DETAIL_BUILD_TIME "KrOr-reproducible-build"
#define KROR_DETAIL_USER_SEED static_cast<std::uint64_t>(KROR_BUILD_SEED)
#else
#define KROR_DETAIL_BUILD_DATE __DATE__
#define KROR_DETAIL_BUILD_TIME __TIME__
#define KROR_DETAIL_USER_SEED 0ULL
#endif

namespace KrOr
{
    inline constexpr std::uint32_t kVersionMajor = 1U;
    inline constexpr std::uint32_t kVersionMinor = 1U;
    inline constexpr std::uint32_t kVersionPatch = 0U;

    namespace Detail
    {
        template <typename CharT, std::size_t N, std::uint64_t Seed, typename Indices>
        class EncryptedLiteral;

        [[nodiscard]] constexpr std::uint64_t RotateLeft(std::uint64_t value, unsigned int shift) noexcept
        {
            shift &= 63U;
            return (value << shift) | (value >> ((64U - shift) & 63U));
        }

        [[nodiscard]] constexpr std::uint64_t RotateRight(std::uint64_t value, unsigned int shift) noexcept
        {
            shift &= 63U;
            return (value >> shift) | (value << ((64U - shift) & 63U));
        }

        [[nodiscard]] constexpr std::uint64_t ByteSwap(std::uint64_t value) noexcept
        {
            value = ((value & 0x00FF00FF00FF00FFULL) << 8U) |
                ((value & 0xFF00FF00FF00FF00ULL) >> 8U);
            value = ((value & 0x0000FFFF0000FFFFULL) << 16U) |
                ((value & 0xFFFF0000FFFF0000ULL) >> 16U);
            return (value << 32U) | (value >> 32U);
        }

        [[nodiscard]] constexpr std::uint64_t MixA(std::uint64_t value) noexcept
        {
            value ^= value >> 30U;
            value *= 0xBF58476D1CE4E5B9ULL;
            value ^= value >> 27U;
            value *= 0x94D049BB133111EBULL;
            return value ^ (value >> 31U);
        }

        [[nodiscard]] constexpr std::uint64_t MixB(std::uint64_t value) noexcept
        {
            value ^= value >> 33U;
            value *= 0xFF51AFD7ED558CCDULL;
            value ^= value >> 33U;
            value *= 0xC4CEB9FE1A85EC53ULL;
            return value ^ (value >> 33U);
        }

        [[nodiscard]] constexpr std::uint64_t MixC(std::uint64_t value) noexcept
        {
            value ^= RotateLeft(value, 25U) ^ RotateLeft(value, 47U);
            value *= 0x9E6C63D0676A9A99ULL;
            value ^= (value >> 23U) ^ (value >> 51U);
            value *= 0x9E6D62D06F6A9A9BULL;
            return value ^ (value >> 28U);
        }

        [[nodiscard]] constexpr std::uint64_t MixD(std::uint64_t value) noexcept
        {
            value ^= value >> 32U;
            value *= 0xD6E8FEB86659FD93ULL;
            value ^= value >> 32U;
            value *= 0xD6E8FEB86659FD93ULL;
            return value ^ (value >> 32U);
        }

        template <std::size_t N>
        consteval void Absorb(std::uint64_t& state, const char(&text)[N]) noexcept
        {
            for (std::size_t i = 0U; i + 1U < N; ++i)
            {
                state ^= static_cast<unsigned char>(text[i]);
                state *= 0x100000001B3ULL;
                state = RotateLeft(state, 11U);
            }
        }

        template <std::size_t FileN, std::size_t DateN, std::size_t TimeN>
        [[nodiscard]] consteval std::uint64_t BuildSeed(
            const char(&file)[FileN],
            const char(&date)[DateN],
            const char(&time)[TimeN],
            std::uint64_t line,
            std::uint64_t counter,
            std::uint64_t literalBytes,
            std::uint64_t userSeed) noexcept
        {
            std::uint64_t state = 0xCBF29CE484222325ULL ^ userSeed;
            Absorb(state, file);
            Absorb(state, date);
            Absorb(state, time);
            state ^= MixA(line + 0x9E3779B97F4A7C15ULL);
            state ^= MixB(counter + 0xD1B54A32D192ED03ULL);
            state ^= MixC(literalBytes + 0x94D049BB133111EBULL);
            const std::uint64_t seed = MixD(state);
            return seed == 0ULL ? 0xA0761D6478BD642FULL : seed;
        }

        template <std::size_t LiteralBytes>
        [[nodiscard]] consteval std::size_t CipherBlockCount() noexcept
        {
            return (LiteralBytes + sizeof(std::uint64_t) - 1U) / sizeof(std::uint64_t);
        }

        template <std::uint64_t Seed>
        [[nodiscard]] constexpr std::uint64_t KeyWord(
            std::uint64_t runtimeSeed,
            std::size_t index) noexcept
        {
            const std::uint64_t step =
                (static_cast<std::uint64_t>(index) + 1ULL) * 0x9E3779B97F4A7C15ULL;
            const std::uint64_t value = runtimeSeed ^ step ^ RotateLeft(Seed, static_cast<unsigned int>(index));

            if constexpr ((Seed & 3ULL) == 0ULL)
            {
                return MixA(value);
            }
            else if constexpr ((Seed & 3ULL) == 1ULL)
            {
                return MixB(value);
            }
            else if constexpr ((Seed & 3ULL) == 2ULL)
            {
                return MixC(value);
            }
            else
            {
                return MixD(value);
            }
        }

        template <std::uint64_t Seed>
        [[nodiscard]] constexpr std::uint64_t EncodeBlock(
            std::uint64_t plain,
            std::size_t index) noexcept
        {
            const unsigned int rotation = static_cast<unsigned int>(
                ((Seed >> 9U) + (index * 13U)) % 63ULL + 1ULL);
            const std::uint64_t firstKey = KeyWord<Seed>(Seed, index * 2U);
            const std::uint64_t secondKey = KeyWord<Seed>(Seed, index * 2U + 1U);
            const std::uint64_t value = plain ^ firstKey;

            if constexpr (((Seed >> 2U) & 3ULL) == 0ULL)
            {
                return RotateLeft(value, rotation) ^ secondKey;
            }
            else if constexpr (((Seed >> 2U) & 3ULL) == 1ULL)
            {
                return ByteSwap(RotateLeft(value, rotation)) ^ secondKey;
            }
            else if constexpr (((Seed >> 2U) & 3ULL) == 2ULL)
            {
                return RotateRight(value, rotation) ^ secondKey;
            }
            else
            {
                return RotateLeft(ByteSwap(value), rotation) ^ secondKey;
            }
        }

        template <std::uint64_t Seed>
        [[nodiscard]] KROR_FORCEINLINE std::uint64_t DecodeBlock(
            std::uint64_t cipher,
            std::size_t index,
            std::uint64_t runtimeSeed) noexcept
        {
            const unsigned int rotation = static_cast<unsigned int>(
                ((Seed >> 9U) + (index * 13U)) % 63ULL + 1ULL);
            const std::uint64_t firstKey = KeyWord<Seed>(runtimeSeed, index * 2U);
            const std::uint64_t secondKey = KeyWord<Seed>(runtimeSeed, index * 2U + 1U);
            const std::uint64_t value = cipher ^ secondKey;

            if constexpr (((Seed >> 2U) & 3ULL) == 0ULL)
            {
                return RotateRight(value, rotation) ^ firstKey;
            }
            else if constexpr (((Seed >> 2U) & 3ULL) == 1ULL)
            {
                return RotateRight(ByteSwap(value), rotation) ^ firstKey;
            }
            else if constexpr (((Seed >> 2U) & 3ULL) == 2ULL)
            {
                return RotateLeft(value, rotation) ^ firstKey;
            }
            else
            {
                return ByteSwap(RotateRight(value, rotation)) ^ firstKey;
            }
        }

        [[nodiscard]] KROR_FORCEINLINE std::uint64_t RuntimeAnchor() noexcept
        {
            // The volatile zero keeps the compiler from materializing plaintext during optimization.
            static const volatile std::uint64_t kRuntimeZero = 0ULL;
            return kRuntimeZero;
        }

        [[nodiscard]] KROR_FORCEINLINE std::uint64_t LoadFromRegister(std::uint64_t value) noexcept
        {
#if defined(__GNUC__) || defined(__clang__)
            asm("" : "=r"(value) : "0"(value));
            return value;
#else
            volatile std::uint64_t registerValue = value;
            return registerValue;
#endif
        }

        inline void SecureWipe(void* memory, std::size_t bytes) noexcept
        {
            volatile auto* cursor = static_cast<volatile unsigned char*>(memory);
            while (bytes-- != 0U)
            {
                *cursor++ = 0U;
            }
        }

        template <std::uint64_t Seed>
        [[nodiscard]] constexpr std::size_t PhysicalIndex(
            std::size_t logicalIndex,
            std::size_t blockCount) noexcept
        {
            if constexpr (((Seed >> 21U) & 1ULL) != 0ULL)
            {
                return blockCount - logicalIndex - 1U;
            }
            else
            {
                return logicalIndex;
            }
        }

        template <std::uint64_t Seed>
        [[nodiscard]] constexpr std::uint64_t TransportMask(std::size_t physicalIndex) noexcept
        {
            const std::uint64_t step =
                (static_cast<std::uint64_t>(physicalIndex) + 1ULL) * 0xA0761D6478BD642FULL;
            const unsigned int rotation = static_cast<unsigned int>(
                ((Seed >> 37U) + physicalIndex * 17U) % 63ULL + 1ULL);
            return RotateLeft(MixA(Seed ^ step ^ 0xE7037ED1A0B428DBULL), rotation);
        }

        template <typename CharT, std::size_t N, std::uint64_t Seed>
        [[nodiscard]] constexpr std::uint64_t BuildStoredBlock(
            const CharT* literal,
            std::size_t physicalIndex,
            std::size_t blockCount) noexcept
        {
            constexpr std::size_t kCodeUnitBits = sizeof(CharT) * 8U;
            constexpr std::size_t kUnitsPerBlock = sizeof(std::uint64_t) / sizeof(CharT);
            constexpr std::uint64_t kCodeUnitMask = (1ULL << kCodeUnitBits) - 1ULL;
            const std::size_t logicalBlock = PhysicalIndex<Seed>(physicalIndex, blockCount);
            std::uint64_t plain = MixC(Seed ^ (logicalBlock * 0xA0761D6478BD642FULL));

            for (std::size_t unit = 0U; unit < kUnitsPerBlock; ++unit)
            {
                const std::size_t literalIndex = logicalBlock * kUnitsPerBlock + unit;
                if (literalIndex >= N)
                {
                    break;
                }

                const std::size_t shift = unit * kCodeUnitBits;
                using UnsignedCharT = std::make_unsigned_t<CharT>;
                const std::uint64_t value = static_cast<std::uint64_t>(
                    static_cast<UnsignedCharT>(literal[literalIndex]));
                plain &= ~(kCodeUnitMask << shift);
                plain |= (value & kCodeUnitMask) << shift;
            }

            return EncodeBlock<Seed>(plain, logicalBlock) ^ TransportMask<Seed>(physicalIndex);
        }

        template <std::uint64_t Seed, std::size_t BlockCount, std::size_t... Indices>
        KROR_FORCEINLINE void RemoveTransportMask(
            std::array<std::uint64_t, BlockCount>& blocks,
            std::index_sequence<Indices...>) noexcept
        {
            alignas(16) std::array<std::uint64_t, BlockCount> masks = {
                LoadFromRegister(std::integral_constant<
                    std::uint64_t,
                    TransportMask<Seed>(Indices)>::value)... };
            std::size_t index = 0U;

#if defined(KROR_DETAIL_HAS_SSE2)
            for (; index + 1U < BlockCount; index += 2U)
            {
                const __m128i cipher = _mm_load_si128(
                    reinterpret_cast<const __m128i*>(blocks.data() + index));
                const __m128i mask = _mm_load_si128(
                    reinterpret_cast<const __m128i*>(masks.data() + index));
                _mm_store_si128(
                    reinterpret_cast<__m128i*>(blocks.data() + index),
                    _mm_xor_si128(cipher, mask));
            }
#elif defined(KROR_DETAIL_HAS_NEON)
            for (; index + 1U < BlockCount; index += 2U)
            {
                const uint64x2_t cipher = vld1q_u64(blocks.data() + index);
                const uint64x2_t mask = vld1q_u64(masks.data() + index);
                vst1q_u64(blocks.data() + index, veorq_u64(cipher, mask));
            }
#endif

            for (; index < BlockCount; ++index)
            {
                blocks[index] ^= masks[index];
            }
        }
    }

    template <typename CharT, std::size_t N>
    class ScopedString final
    {
        static_assert(N > 0U, "KrOr requires a null-terminated string literal.");

    public:
        using ValueType = CharT;
        using ViewType = std::basic_string_view<CharT>;

        ScopedString(const ScopedString&) = delete;
        ScopedString& operator=(const ScopedString&) = delete;

        ScopedString(ScopedString&& other) noexcept
            : data_(other.data_), wiped_(other.wiped_)
        {
            other.Wipe();
        }

        ScopedString& operator=(ScopedString&& other) noexcept
        {
            if (this != &other)
            {
                Wipe();
                data_ = other.data_;
                wiped_ = other.wiped_;
                other.Wipe();
            }
            return *this;
        }

        ~ScopedString() noexcept
        {
            Wipe();
        }

        [[nodiscard]] static constexpr std::size_t Size() noexcept
        {
            return N - 1U;
        }

        [[nodiscard]] static constexpr bool Empty() noexcept
        {
            return N == 1U;
        }

        [[nodiscard]] const CharT* CStr() const noexcept
        {
            return data_.data();
        }

        [[nodiscard]] const CharT* Data() const noexcept
        {
            return data_.data();
        }

        [[nodiscard]] ViewType View() const noexcept
        {
            return ViewType{ data_.data(), Size() };
        }

        [[nodiscard]] const CharT& operator[](std::size_t index) const noexcept
        {
            return data_[index];
        }

        [[nodiscard]] operator const CharT* () const noexcept
        {
            return CStr();
        }

        [[nodiscard]] explicit operator ViewType() const noexcept
        {
            return View();
        }

        void Wipe() noexcept
        {
            Detail::SecureWipe(data_.data(), sizeof(data_));
            wiped_ = true;
        }

        [[nodiscard]] bool IsWiped() const noexcept
        {
            return wiped_;
        }

    private:
        template <typename, std::size_t, std::uint64_t, typename>
        friend class Detail::EncryptedLiteral;

        template <std::uint64_t Seed, std::size_t BlockCount>
        KROR_FORCEINLINE explicit ScopedString(
            const std::array<std::uint64_t, BlockCount>& blocks,
            std::integral_constant<std::uint64_t, Seed>) noexcept
        {
            constexpr std::size_t kCodeUnitBits = sizeof(CharT) * 8U;
            constexpr std::size_t kUnitsPerBlock = sizeof(std::uint64_t) / sizeof(CharT);
            constexpr std::uint64_t kCodeUnitMask = (1ULL << kCodeUnitBits) - 1ULL;
            const std::uint64_t runtimeSeed = Seed ^ Detail::RuntimeAnchor();

            for (std::size_t logicalBlock = 0U; logicalBlock < BlockCount; ++logicalBlock)
            {
                const std::size_t physicalBlock = Detail::PhysicalIndex<Seed>(logicalBlock, BlockCount);
                const std::uint64_t plain =
                    Detail::DecodeBlock<Seed>(blocks[physicalBlock], logicalBlock, runtimeSeed);

                for (std::size_t unit = 0U; unit < kUnitsPerBlock; ++unit)
                {
                    const std::size_t outputIndex = logicalBlock * kUnitsPerBlock + unit;
                    if (outputIndex >= N)
                    {
                        break;
                    }

                    const std::size_t shift = unit * kCodeUnitBits;
                    using UnsignedCharT = std::make_unsigned_t<CharT>;
                    data_[outputIndex] = static_cast<CharT>(
                        static_cast<UnsignedCharT>((plain >> shift) & kCodeUnitMask));
                }
            }
        }

        std::array<CharT, N> data_ = {};
        bool wiped_ = false;
    };

    namespace Detail
    {
        template <typename CharT, std::size_t N, std::uint64_t Seed, typename Indices>
        class EncryptedLiteral;

        template <typename CharT, std::size_t N, std::uint64_t Seed, std::size_t... Indices>
        class EncryptedLiteral<CharT, N, Seed, std::index_sequence<Indices...>> final
        {
            static_assert(std::is_integral_v<CharT>, "KrOr supports integral character types only.");
            static_assert(sizeof(CharT) == 1U || sizeof(CharT) == 2U || sizeof(CharT) == 4U,
                "KrOr supports 8-bit, 16-bit, and 32-bit character types.");

            static constexpr std::size_t kBlockCount = sizeof...(Indices);

        public:
            template <typename LiteralProvider>
            KROR_FORCEINLINE explicit EncryptedLiteral(LiteralProvider literalProvider) noexcept
                : blocks_{ LoadFromRegister(std::integral_constant<
                    std::uint64_t,
                    BuildStoredBlock<CharT, N, Seed>(
                        literalProvider(),
                        Indices,
                        kBlockCount)>::value)... }
            {
            }

            [[nodiscard]] KROR_FORCEINLINE ScopedString<CharT, N> Decrypt() noexcept
            {
                RemoveTransportMask<Seed>(blocks_, std::index_sequence<Indices...>{});
                return ScopedString<CharT, N>{
                    blocks_,
                        std::integral_constant<std::uint64_t, Seed>{}};
            }

        private:
            alignas(16) std::array<std::uint64_t, kBlockCount> blocks_ = {};
        };

        template <
            std::uint64_t Seed,
            typename LiteralProvider,
            std::size_t N,
            std::size_t... Indices>
        [[nodiscard]] KROR_FORCEINLINE auto Encrypt(
            LiteralProvider literalProvider,
            std::integral_constant<std::size_t, N>,
            std::index_sequence<Indices...>) noexcept
        {
            using LiteralPointer = decltype(literalProvider());
            using CharT = std::remove_cv_t<std::remove_pointer_t<LiteralPointer>>;
            return EncryptedLiteral<
                CharT,
                N,
                Seed,
                std::index_sequence<Indices...>>{literalProvider};
        }

        template <
            std::uint64_t Seed,
            typename LiteralProvider,
            std::size_t N,
            std::size_t... Indices>
        [[nodiscard]] KROR_FORCEINLINE auto DecryptLiteral(
            LiteralProvider literalProvider,
            std::integral_constant<std::size_t, N> size,
            std::index_sequence<Indices...> indices) noexcept
        {
            auto encrypted = Encrypt<Seed>(literalProvider, size, indices);
            return encrypted.Decrypt();
        }
    }
}

// KROR accepts a string literal and returns a move-only ScopedString. Its plaintext
// buffer remains valid until the end of the full expression, or for the lifetime of
// a named ScopedString object.
#define KROR(literal)                                                                                 \
    (::KrOr::Detail::DecryptLiteral<                                                                  \
        ::KrOr::Detail::BuildSeed(                                                                    \
            __FILE__,                                                                                 \
            KROR_DETAIL_BUILD_DATE,                                                                   \
            KROR_DETAIL_BUILD_TIME,                                                                   \
            static_cast<std::uint64_t>(KROR_DETAIL_LINE_U),                                           \
            static_cast<std::uint64_t>(__COUNTER__),                                                  \
            static_cast<std::uint64_t>(sizeof(literal)),                                              \
            KROR_DETAIL_USER_SEED)>(                                                                  \
        []() constexpr noexcept { return literal; },                                                   \
        std::integral_constant<                                                                        \
            std::size_t,                                                                               \
            sizeof(literal) / sizeof((literal)[0])>{},                                                 \
        std::make_index_sequence<                                                                      \
            ::KrOr::Detail::CipherBlockCount<sizeof(literal)>()>{}))
