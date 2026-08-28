//**************************************//
// Hi UniResolver						//
// Author: AlembicOrg					//
// Version: v3.1.3						//
// Branch: il2cpp						//
// License: GPL-3.0 license				//
//**************************************//

// Change Log (Started since v1.8):
// Release v3.1.3:
// 1. Add KrOr.h (compile-time encrypted string / scoped decryption utilities) and wire it into the codebase. 
// 2. Replace raw string-literal templates with EncryptedText/KrOr NTTPs and KROR/ENCRYPTED_STRING usage in NaResolver templates and macros. 
// 3. Add Method::GetToken and NaResolver::GetMethod(Class,int) to support token-based method lookup; update MemberMethodInfo and METHOD_INFO to use tokens. 
// 4. Update example project and main.cpp to include KrOr.h and use the new encrypted/token APIs. Also update .gitignore to ignore /v4.
// Release v3.1.2:
// 1. Normalized the macro METHOD to METHOD_INFO because in essence it actually defines a static method info store
// Release v3.1.1:
// 1. Change the macro CLASS in order to preserve some special assembly names from vs formatting, just like Assembly-CSharp
// Release v3.1:
// 1. Add test engine for finding out the wrong data at setup time
// 2. Change the field backing field name handle logic
// 3. Fixup some bug of unusable format variables
// Release v3.0.1:
// 1. Normalized the code
// 2. Add new features about field and methods
// Release v3.0:
// 1. Normalized the code
// 2. Changed the original mostly macro structure
// Release v2.2:
// 1. Re-separate the runtime-specific versions
// Release v2.1.5:
// 1. Fixup the bug of being unable to get nested class
// 2. Add more new api about getting class, especially nested classes
// Release v2.1.4:
// 1. Fixup some visible bus
// 2. Change the logic of setting and getting the fields
// Release v2.1.3:
// 1. Remove the exception
// 2. Fixup some visible bus
// Release v2.1.2:
// 1. Add runtime invoke method
// Release v2.1.1:
// 1. Add some macros about the static fields
// Release v2.1:
// 1. Rename some unreasonable variables
// 2. Add api about the fields
// Release v2.0:
// 1. Remake most of codes
// 2. Unified the runtime-specific implementations
// 3. Remove that enforce cpp version requirements
// 4. Remove the structure of signature
// Release v1.8:
// 1. Add pre-register mechanism, all class, method and field must register
// 2. Add a exception class
//

#undef GetClassName

#pragma once
#ifndef H_NARESOLVER
#define H_NARESOLVER
#include <Windows.h>
#include <array>
#include <atomic>
#include <concepts>
#include <cstdint>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>
#include <unordered_map>

#include "KrOr.h"
#include "VmApi.h"

#undef GetObject
#undef RegisterClass
#undef TEXT

#ifndef _HAS_CXX20
#error "This library requires C++20 standard"
#endif

#define TEXT(str) KROR((str)).CStr()

class UniResolverContext
{
public:
	class Class
	{
	public:
		std::string assemblyName = std::string();
		std::string namespaceName = std::string();
		std::string className = std::string();
		VmGeneralType::Class klass = nullptr;
		VmGeneralType::Type type = nullptr;
		std::unordered_map<std::string, Class> nestedClasses = {};
		Class() {}
		Class(const Class& klass)
			: assemblyName(klass.assemblyName), namespaceName(klass.namespaceName), className(klass.className), klass(klass.klass), type(klass.type), nestedClasses(klass.nestedClasses) {}
		Class(const std::string& assemblyName, const std::string& namespaceName, const std::string& className, const VmGeneralType::Class& klass, const VmGeneralType::Type& type)
			: assemblyName(assemblyName), namespaceName(namespaceName), className(className), klass(klass), type(type) {}
		operator VmGeneralType::Class() { return klass; }
		operator VmGeneralType::Type() { return type; }
		operator bool() { return klass && type; }

		void AddNestedClass(const std::string name, const Class& nestedClass);

		Class FindNestedClass(const std::string name) const;
	};
	class Method
	{
	public:
		std::string returnTypeName = std::string();
		std::string methodName = std::string();
		std::vector<std::string> parametersTypeName = std::vector<std::string>();
		VmGeneralType::Method method = nullptr;
		Method() {}
		Method(const std::string& returnTypeName, const std::string& methodName, std::vector<std::string> parametersTypeName, VmGeneralType::Method method) : returnTypeName(returnTypeName), methodName(methodName), parametersTypeName(parametersTypeName), method(method) {}
		operator VmGeneralType::Method() { return method; }
		operator bool() { return method; }
	};
	class NameMap
	{
	public:
		using Map = std::unordered_map<std::string, std::string>;

		void MapAssembly(const std::string& name, const std::string& relocatedName);
		void MapNamespace(const std::string& assembly, const std::string& name, const std::string& relocatedName);
		void MapClass(const std::string& assembly, const std::string& nameSpace, const std::string& name, const std::string& relocatedName);
		void MapNestedClass(const std::string& assembly, const std::string& nameSpace, const std::string& parentName, const std::string& name, const std::string& relocatedName);
		void MapMethod(const std::string& assembly, const std::string& nameSpace, const std::string& className, const std::string& name, const std::string& relocatedName);
		void MapMethod(const std::string& assembly, const std::string& nameSpace, const std::string& className, const std::string& returnTypeName, const std::string& name, const std::vector<std::string>& parametersTypeName, const std::string& relocatedName);
		void MapField(const std::string& assembly, const std::string& nameSpace, const std::string& className, const std::string& name, const std::string& relocatedName);
		void MapType(const std::string& name, const std::string& relocatedName);

		[[nodiscard]] std::string RelocateAssembly(const std::string& name) const;
		[[nodiscard]] std::string RelocateNamespace(const std::string& assembly, const std::string& name) const;
		[[nodiscard]] std::string RelocateClass(const std::string& assembly, const std::string& nameSpace, const std::string& name) const;
		[[nodiscard]] std::string RelocateNestedClass(const std::string& assembly, const std::string& nameSpace, const std::string& parentName, const std::string& name) const;
		[[nodiscard]] std::string RelocateMethod(const std::string& assembly, const std::string& nameSpace, const std::string& className, const std::string& returnTypeName, const std::string& name, const std::vector<std::string>& parametersTypeName) const;
		[[nodiscard]] std::string RelocateField(const std::string& assembly, const std::string& nameSpace, const std::string& className, const std::string& name) const;
		[[nodiscard]] std::string RelocateType(const std::string& name) const;

		void Clear();
		[[nodiscard]] bool Empty() const noexcept;

	private:
		Map assemblies = {};
		Map namespaces = {};
		Map classes = {};
		Map nestedClasses = {};
		Map methods = {};
		Map methodSignatures = {};
		Map fields = {};
		Map types = {};

		template<typename... Parts>
		[[nodiscard]] static std::string MakeKey(const Parts&... parts)
		{
			std::string key;
			((key.append(parts), key.push_back('\0')), ...);
			return key;
		}

		[[nodiscard]] static std::string MakeMethodKey(
			const std::string& assembly,
			const std::string& nameSpace,
			const std::string& className,
			const std::string& returnTypeName,
			const std::string& name,
			const std::vector<std::string>& parametersTypeName);
		[[nodiscard]] static std::string Relocate(const Map& map, const std::string& key, const std::string& fallback);
	};
	class ContextCache
	{
	public:
		using AssemblyMap = std::unordered_map<std::string, VmGeneralType::Assembly>;
		using ClassPathMap = std::unordered_map<std::string,	// Assembly
			std::unordered_map<std::string,						// Namespace
			std::unordered_map<std::string, Class>			// Class
			>
		>;
	private:
		AssemblyMap assemblyMap = {};
		ClassPathMap classPathMap = {};
	public:
		ContextCache() {}

		void RegisterAssembly(const std::string& name, VmGeneralType::Assembly assembly);

		Class RegisterClass(const std::string& assembly, const std::string& nameSpace, const std::string& name, VmGeneralType::Class klass, VmGeneralType::Type type);

		VmGeneralType::Assembly GetAssembly(const std::string& name) const;

		Class GetClass(const std::string& assembly, const std::string& nameSpace, const std::string& name) const;

		Class& GetClass(const std::string& assembly, const std::string& nameSpace, const std::string& name);

		void Clear();
	};
private:
	VmGeneralType::Domain domain = nullptr;
	VmGeneralType::Thread thread = nullptr;
	ContextCache cache = ContextCache();
	NameMap nameMap = NameMap();
	bool ownsThread = false;
	mutable std::recursive_mutex stateMutex;
public:
	bool Setup();
	void Destroy();
	void SetNameMap(NameMap map);
	[[nodiscard]] const NameMap& GetNameMap() const noexcept;
	void ClearNameMap();
	Class GetClass(const std::string& assemblyName, const std::string& namespaceName, const std::string& className);
	Class GetClass(Class parent, const std::string& className);
	Method GetMethod(Class parent, const std::string& returnTypeName, const std::string& methodName, const std::vector<std::string>& parametersTypeName);
	Method GetMethod(Class parent, int token);
	VmGeneralType::Field GetField(Class parent, const std::string& fieldName);
};

template <typename Tuple, std::size_t... I>
auto TupleToVectorImpl(const Tuple& t, std::index_sequence<I...>)
{
	return std::vector<std::string>{std::string(std::get<I>(t))...};
}

template <typename... Args>
auto TupleToVector(const std::tuple<Args...>& t)
{
	return TupleToVectorImpl(t, std::index_sequence_for<Args...>{});
}

namespace Template
{
	namespace Detail
	{
		class RuntimeThreadAttachmentGuard final
		{
		public:
			RuntimeThreadAttachmentGuard() noexcept
			{
				thread = VmGeneralType::api.GetCurrentThread();
				if (thread != nullptr) return;
				const VmGeneralType::Domain domain = VmGeneralType::api.GetDomain();
				if (domain == nullptr) return;
				thread = VmGeneralType::api.AttachThread(domain);
				ownsAttachment = thread != nullptr;
			}

			~RuntimeThreadAttachmentGuard()
			{
				if (ownsAttachment && thread != nullptr)
					VmGeneralType::api.DetachThread(thread);
			}

			RuntimeThreadAttachmentGuard(const RuntimeThreadAttachmentGuard&) = delete;
			RuntimeThreadAttachmentGuard& operator=(
				const RuntimeThreadAttachmentGuard&) = delete;

			[[nodiscard]] bool IsAttached() const noexcept { return thread != nullptr; }

		private:
			VmGeneralType::Thread thread = nullptr;
			bool ownsAttachment = false;
		};

		template<typename Value>
		consteval void AppendTypeIdValue(std::uint64_t& state, Value value) noexcept
		{
			using UnsignedValue = std::make_unsigned_t<Value>;
			UnsignedValue unsignedValue = static_cast<UnsignedValue>(value);
			for (std::size_t byteIndex = 0U; byteIndex < sizeof(Value); ++byteIndex)
			{
				state ^= static_cast<std::uint64_t>(unsignedValue & static_cast<UnsignedValue>(0xFFU));
				state *= 0x100000001B3ULL;
				unsignedValue >>= 8U;
			}
		}

		template<typename Encrypted>
		consteval void AppendTypeIdPart(std::uint64_t& state, const Encrypted& value) noexcept
		{
			using CharType = typename Encrypted::ValueType;
			using UnsignedCharType = std::make_unsigned_t<CharType>;
			constexpr std::size_t kMaximumSize = Encrypted::kBlockCount * Encrypted::kUnitsPerBlock;

			std::size_t size = 0U;
			while (size < kMaximumSize && EncryptedText::Detail::ReadCodeUnit(value, size) != CharType{})
			{
				++size;
			}

			AppendTypeIdValue(state, static_cast<std::uint64_t>(size));
			AppendTypeIdValue(state, static_cast<std::uint8_t>(sizeof(CharType)));
			for (std::size_t index = 0U; index < size; ++index)
			{
				UnsignedCharType codeUnit = static_cast<UnsignedCharType>(
					EncryptedText::Detail::ReadCodeUnit(value, index));
				AppendTypeIdValue(state, codeUnit);
			}
		}

		template<typename... EncryptedParts>
		[[nodiscard]] consteval std::uint64_t MakeTypeId(const EncryptedParts&... parts) noexcept
		{
			std::uint64_t state = 0xCBF29CE484222325ULL;
			(AppendTypeIdPart(state, parts), ...);
			return EncryptedText::Detail::NormalizeSeed(EncryptedText::Detail::Mix(state));
		}

		template<typename EncryptedName>
		[[nodiscard]] consteval std::uint64_t MakeNestedTypeId(
			std::uint64_t declaringTypeId,
			const EncryptedName& name) noexcept
		{
			std::uint64_t state = 0xCBF29CE484222325ULL;
			AppendTypeIdValue(state, declaringTypeId);
			AppendTypeIdPart(state, name);
			return EncryptedText::Detail::NormalizeSeed(EncryptedText::Detail::Mix(state));
		}

		template<typename... ArgumentTypeIds>
		[[nodiscard]] consteval std::uint64_t MakeClosedGenericTypeId(
			std::uint64_t definitionTypeId,
			ArgumentTypeIds... argumentTypeIds) noexcept
		{
			std::uint64_t state = 0xCBF29CE484222325ULL;
			AppendTypeIdValue(state, definitionTypeId);
			AppendTypeIdValue(state, static_cast<std::uint64_t>(sizeof...(ArgumentTypeIds)));
			(AppendTypeIdValue(state, argumentTypeIds), ...);
			return EncryptedText::Detail::NormalizeSeed(EncryptedText::Detail::Mix(state));
		}

		[[nodiscard]] consteval std::uint64_t MakeArrayTypeId(
			const std::uint64_t elementTypeId,
			const std::size_t rank) noexcept
		{
			std::uint64_t state = 0xCBF29CE484222325ULL;
			constexpr std::uint64_t kArrayDomain = 0x4172726179547970ULL;
			AppendTypeIdValue(state, kArrayDomain);
			AppendTypeIdValue(state, elementTypeId);
			AppendTypeIdValue(state, static_cast<std::uint64_t>(rank));
			return EncryptedText::Detail::NormalizeSeed(EncryptedText::Detail::Mix(state));
		}

		template<typename T>
		inline constexpr bool IsStdString = std::is_same_v<std::remove_cvref_t<T>, std::string>;

		template<typename T>
		using DirectArgument = std::conditional_t<IsStdString<T>, VmGeneralType::String, std::decay_t<T>>;

		template<typename T>
		DirectArgument<T> ConvertArgument(T&& value)
		{
			if constexpr (IsStdString<T>)
			{
				return VmGeneralType::api.NewString(value);
			}
			else
			{
				return std::forward<T>(value);
			}
		}

		template<typename T>
		void* GetRuntimeParameter(T& value) noexcept
		{
			if constexpr (std::is_pointer_v<T>)
			{
				return const_cast<void*>(reinterpret_cast<const void*>(value));
			}
			else
			{
				return std::addressof(value);
			}
		}

		template<typename>
		inline constexpr bool AlwaysFalse = false;

		template<typename ClassInfo>
		[[nodiscard]] consteval std::uint64_t GetClassInfoTypeId(
			const ClassInfo& classInfo)
		{
			if constexpr (requires { classInfo.TypeId; })
				return classInfo.TypeId;
			else
				return std::remove_cvref_t<ClassInfo>::GetTypeId();
		}
	}

	template<typename Identity, typename = void>
	struct ManagedStorageTraits
	{
	};

	template<typename Identity>
	struct ManagedStorageTraits<Identity,
		std::void_t<typename Identity::ManagedStorageType>>
	{
		using Type = typename Identity::ManagedStorageType;
	};

	template<typename Type, bool IsManagedIdentity = std::is_class_v<Type>>
	struct ManagedTypeTraitsBase
	{
		using StorageType = typename ManagedStorageTraits<Type>::Type;
		static constexpr std::uint64_t TypeId =
			Detail::GetClassInfoTypeId(Type::ThisClassInfo);
	};

	template<typename Type>
	struct ManagedTypeTraitsBase<Type, false>
	{
		static_assert(
			Detail::AlwaysFalse<Type>,
			"ManagedTypeTraits expects a managed type identity declared by a UniResolver type macro");

		using StorageType = void;
		static constexpr std::uint64_t TypeId = 0U;
	};

	template<typename Type>
	struct ManagedTypeTraits : ManagedTypeTraitsBase<std::remove_cvref_t<Type>>
	{
	};

	template<typename Type>
	using ManagedStorage = typename ManagedStorageTraits<
		std::remove_cvref_t<Type>>::Type;

	template<typename T>
	concept ManagedType = std::is_class_v<std::remove_cvref_t<T>> && requires
	{
		{ std::remove_cvref_t<T>::ThisClassInfo.Instance() } -> std::same_as<UniResolverContext::Class>;
	};

	template<typename T>
	struct BoxedValue final
	{
		void* klass = nullptr;
		void* monitor = nullptr;
		alignas(std::remove_cvref_t<T>) std::remove_cvref_t<T> value;

		[[nodiscard]] std::remove_cvref_t<T>& Value() noexcept { return value; }
		[[nodiscard]] const std::remove_cvref_t<T>& Value() const noexcept { return value; }
	};

	template<typename T>
	using NewObjectResult = std::conditional_t<
		std::is_same_v<ManagedStorage<T>, std::remove_cvref_t<T>>,
		BoxedValue<std::remove_cvref_t<T>>*, std::remove_cvref_t<T>*>;

	template<typename T>
	requires ManagedType<T>
	[[nodiscard]] NewObjectResult<T> NewObject()
	{
		using ObjectType = std::remove_cvref_t<T>;
		Detail::RuntimeThreadAttachmentGuard attachment;
		if (!attachment.IsAttached())
		{
			throw std::runtime_error(
				"UniResolver could not attach the current thread while allocating a managed object.");
		}
		UniResolverContext::Class classInfo = ObjectType::ThisClassInfo.Instance();
		if (!classInfo)
		{
			throw std::runtime_error(
				"UniResolver could not resolve the managed class while allocating an object.");
		}

		VmGeneralType::Object object = VmGeneralType::api.NewObject(classInfo.klass);
		if (object == nullptr)
		{
			throw std::runtime_error("UniResolver could not allocate the managed object.");
		}

		return reinterpret_cast<NewObjectResult<T>>(object);
	}

	class ManagedInvocationException final : public std::runtime_error
	{
	public:
		explicit ManagedInvocationException(VmGeneralType::Object exception)
			: std::runtime_error("A managed method invocation raised an exception.")
		{
			Detail::RuntimeThreadAttachmentGuard attachment;
			if (!attachment.IsAttached())
				throw std::runtime_error(
					"UniResolver could not attach the current thread while preserving a managed exception.");
			handle = std::make_shared<Handle>(VmGeneralType::api.NewGcHandle(exception));
		}

		[[nodiscard]] VmGeneralType::Object Exception() const noexcept
		{
			Detail::RuntimeThreadAttachmentGuard attachment;
			return attachment.IsAttached() && handle != nullptr ?
				VmGeneralType::api.GetGcHandleTarget(handle->value) : nullptr;
		}

	private:
		struct Handle final
		{
			explicit Handle(const std::uint32_t value) noexcept : value(value) {}
			~Handle()
			{
				Detail::RuntimeThreadAttachmentGuard attachment;
				if (attachment.IsAttached()) VmGeneralType::api.FreeGcHandle(value);
			}
			std::uint32_t value = 0U;
		};
		std::shared_ptr<Handle> handle;
	};

	template<typename R, typename... Args>
	class MethodInvoker
	{
		static_assert(!std::is_reference_v<R>, "MethodInvoker does not support reference return values");

	public:
		using DirectReturnType = std::conditional_t<Detail::IsStdString<R>, VmGeneralType::String, R>;
		using DirectFunctionType = DirectReturnType(*)(Detail::DirectArgument<Args>...);
		using DirectInstanceFunctionType = DirectReturnType(*)(VmGeneralType::Object, Detail::DirectArgument<Args>...);
		using Il2CppDirectFunctionType = DirectReturnType(*)(Detail::DirectArgument<Args>..., VmGeneralType::Method);
		using Il2CppDirectInstanceFunctionType = DirectReturnType(*)(VmGeneralType::Object, Detail::DirectArgument<Args>..., VmGeneralType::Method);
		using Dispatcher = R(*)(const MethodInvoker&, VmGeneralType::Method,
			VmGeneralType::Object, bool, Args...);

		MethodInvoker() noexcept = default;
		MethodInvoker(void* address) noexcept
			: directAddress(address), dispatcher(&InvokeDirectDispatcher)
		{
		}
		MethodInvoker(VmGeneralType::Method method) noexcept
			: method(method), dispatcher(&InvokeRuntimeDispatcher)
		{
		}

		R Invoke(Args... args) const
		{
			return Dispatch(method, nullptr, false, std::forward<Args>(args)...);
		}

		R InvokeInstance(VmGeneralType::Object instance, Args... args) const
		{
			return Dispatch(method, instance, true, std::forward<Args>(args)...);
		}

		template<typename Instance>
		R InvokeInstance(Instance* instance, Args... args) const
		{
			return InvokeInstance(
				reinterpret_cast<VmGeneralType::Object>(const_cast<std::remove_const_t<Instance>*>(instance)),
				std::forward<Args>(args)...);
		}

		R InvokeVirtual(VmGeneralType::Object instance, Args... args) const
		{
			Detail::RuntimeThreadAttachmentGuard attachment;
			if (!attachment.IsAttached())
				throw std::runtime_error(
					"UniResolver could not attach the current thread to the managed runtime.");
			std::atomic_ref<VmGeneralType::Object>(lastException).store(nullptr, std::memory_order_release);
			VmGeneralType::Method virtualMethod = VmGeneralType::api.GetVirtualMethod(instance, method);
			if (virtualMethod == nullptr)
			{
				throw std::runtime_error(
					"UniResolver could not resolve the virtual MethodInfo.");
			}

			return Dispatch(virtualMethod, instance, true, std::forward<Args>(args)...);
		}

		template<typename Instance>
		R InvokeVirtual(Instance* instance, Args... args) const
		{
			return InvokeVirtual(
				reinterpret_cast<VmGeneralType::Object>(const_cast<std::remove_const_t<Instance>*>(instance)),
				std::forward<Args>(args)...);
		}

		R operator()(Args... args) const
		{
			return Invoke(std::forward<Args>(args)...);
		}

		[[nodiscard]] bool IsValid() const noexcept
		{
			return method != nullptr || directAddress != nullptr;
		}

		[[nodiscard]] VmGeneralType::Object GetLastException() const noexcept
		{
			return std::atomic_ref<VmGeneralType::Object>(lastException).load(std::memory_order_acquire);
		}

	private:
		using ConvertedArguments = std::tuple<Detail::DirectArgument<Args>...>;

		VmGeneralType::Method method = nullptr;
		mutable void* directAddress = nullptr;
		mutable VmGeneralType::Object lastException = nullptr;
		Dispatcher dispatcher = nullptr;

		R Dispatch(VmGeneralType::Method invokeMethod,
			VmGeneralType::Object instance, bool instanceCall, Args... args) const
		{
			Detail::RuntimeThreadAttachmentGuard attachment;
			constexpr bool convertsManagedStrings = Detail::IsStdString<R> ||
				(Detail::IsStdString<Args> || ...);
			if ((invokeMethod != nullptr || convertsManagedStrings) &&
				!attachment.IsAttached())
				throw std::runtime_error(
					"UniResolver could not attach the current thread to the managed runtime.");
			if (dispatcher == nullptr)
			{
				throw std::runtime_error("UniResolver has no method invocation dispatcher.");
			}
			return dispatcher(*this, invokeMethod, instance, instanceCall,
				std::forward<Args>(args)...);
		}

		[[nodiscard]] void* GetDirectAddress(VmGeneralType::Method invokeMethod) const noexcept
		{
			if (invokeMethod != method)
			{
				return VmGeneralType::api.GetMethodPointer(invokeMethod);
			}

			std::atomic_ref<void*> addressReference(directAddress);
			void* address = addressReference.load(std::memory_order_acquire);
			if (address == nullptr && invokeMethod != nullptr)
			{
				void* resolvedAddress = VmGeneralType::api.GetMethodPointer(invokeMethod);
				addressReference.compare_exchange_strong(
					address,
					resolvedAddress,
					std::memory_order_release,
					std::memory_order_acquire);
				address = addressReference.load(std::memory_order_acquire);
			}
			return address;
		}

		static R DefaultResult()
		{
			if constexpr (std::is_void_v<R>)
			{
				return;
			}
			else
			{
				return R{};
			}
		}

		template<typename RawResult>
		static R ConvertDirectResult(RawResult result)
		{
			if constexpr (Detail::IsStdString<R>)
			{
				return VmGeneralType::api.StringToUtf8(result);
			}
			else
			{
				return result;
			}
		}

		R InvokeDirect(
			VmGeneralType::Method invokeMethod,
			VmGeneralType::Object instance,
			bool instanceCall,
			ConvertedArguments& arguments) const
		{
			void* address = GetDirectAddress(invokeMethod);
			if (address == nullptr)
			{
				return DefaultResult();
			}

			if (instanceCall)
			{
				if (invokeMethod != nullptr)
				{
					auto function = reinterpret_cast<Il2CppDirectInstanceFunctionType>(address);
					if constexpr (std::is_void_v<R>)
					{
						std::apply([&](auto&... values) { function(instance, values..., invokeMethod); }, arguments);
						return;
					}
					else
					{
						return std::apply([&](auto&... values) { return ConvertDirectResult(function(instance, values..., invokeMethod)); }, arguments);
					}
				}

				auto function = reinterpret_cast<DirectInstanceFunctionType>(address);
				if constexpr (std::is_void_v<R>)
				{
					std::apply([&](auto&... values) { function(instance, values...); }, arguments);
					return;
				}
				else
				{
					return std::apply([&](auto&... values) { return ConvertDirectResult(function(instance, values...)); }, arguments);
				}
			}

			if (invokeMethod != nullptr)
			{
				auto function = reinterpret_cast<Il2CppDirectFunctionType>(address);
				if constexpr (std::is_void_v<R>)
				{
					std::apply([&](auto&... values) { function(values..., invokeMethod); }, arguments);
					return;
				}
				else
				{
					return std::apply([&](auto&... values) { return ConvertDirectResult(function(values..., invokeMethod)); }, arguments);
				}
			}

			auto function = reinterpret_cast<DirectFunctionType>(address);
			if constexpr (std::is_void_v<R>)
			{
				std::apply([&](auto&... values) { function(values...); }, arguments);
				return;
			}
			else
			{
				return std::apply([&](auto&... values) { return ConvertDirectResult(function(values...)); }, arguments);
			}
		}

		static R InvokeDirectDispatcher(const MethodInvoker& self,
			VmGeneralType::Method invokeMethod,
			VmGeneralType::Object instance,
			bool instanceCall,
			Args... args)
		{
			ConvertedArguments convertedArguments(
				Detail::ConvertArgument<Args>(std::forward<Args>(args))...);
			return self.InvokeDirect(invokeMethod, instance, instanceCall,
				convertedArguments);
		}

		static R InvokeRuntimeDispatcher(
			const MethodInvoker& self,
			VmGeneralType::Method invokeMethod,
			VmGeneralType::Object instance,
			bool instanceCall,
			Args... args)
		{
			(void)instanceCall;
			Detail::RuntimeThreadAttachmentGuard attachment;
			if (!attachment.IsAttached())
				throw std::runtime_error(
					"UniResolver could not attach the current thread to the managed runtime.");
			std::array<VmGeneralType::String, sizeof...(Args)> runtimeStrings = {};
			std::size_t runtimeStringIndex = 0U;
			auto getRuntimeParameter = [&]<typename Argument>(Argument& value) -> void*
			{
				if constexpr (Detail::IsStdString<Argument>)
				{
					VmGeneralType::String& runtimeString =
						runtimeStrings[runtimeStringIndex++];
					runtimeString = VmGeneralType::api.NewString(value);
					return runtimeString;
				}
				else if constexpr (std::is_pointer_v<std::remove_reference_t<Argument>>)
				{
					return const_cast<void*>(reinterpret_cast<const void*>(value));
				}
				else
				{
					return const_cast<void*>(static_cast<const void*>(std::addressof(value)));
				}
			};
			std::array<void*, sizeof...(Args)> runtimeParameters = {};
			std::size_t parameterIndex = 0U;
			((runtimeParameters[parameterIndex++] = getRuntimeParameter(args)), ...);

			VmGeneralType::RuntimeInvokeResult invokeResult = VmGeneralType::api.Invoke(
				invokeMethod,
				instance,
				sizeof...(Args) == 0U ? nullptr : runtimeParameters.data());
			std::atomic_ref<VmGeneralType::Object>(self.lastException).store(
				invokeResult.exception, std::memory_order_release);

			if (!invokeResult.dispatched)
			{
				throw std::runtime_error(
					"UniResolver could not dispatch MethodInfo through il2cpp_runtime_invoke.");
			}

			if (invokeResult.exception != nullptr)
			{
				throw ManagedInvocationException(invokeResult.exception);
			}

			if constexpr (std::is_void_v<R>)
			{
				return;
			}
			else
			{
				if constexpr (Detail::IsStdString<R>)
				{
					return VmGeneralType::api.StringToUtf8(
						reinterpret_cast<VmGeneralType::String>(invokeResult.value));
				}
				else if constexpr (std::is_pointer_v<R>)
				{
					return reinterpret_cast<R>(invokeResult.value);
				}
				else if constexpr (std::is_trivially_copyable_v<R>)
				{
					if (void* value = VmGeneralType::api.Unbox(invokeResult.value);
						value != nullptr)
					{
						return *reinterpret_cast<const R*>(value);
					}

					return DefaultResult();
				}
				else
				{
					return DefaultResult();
				}
			}
		}
	};

	template <EncryptedText::EncryptedString Assembly, EncryptedText::EncryptedString Namespace, EncryptedText::EncryptedString Name>
	class NormalClassInfo
	{
	public:
		static constexpr auto AssemblyName = Assembly;
		static constexpr auto NamespaceName = Namespace;
		static constexpr auto ClassName = Name;
		static constexpr auto DeclaringClassName = EncryptedText::EncryptedString("__NONE__");
		static constexpr std::uint64_t TypeId = Detail::MakeTypeId(AssemblyName, NamespaceName, ClassName);
		inline static UniResolverContext::Class ClassInfoCache = UniResolverContext::Class();
		inline static std::mutex ClassInfoMutex;

		inline static UniResolverContext::Class Instance();
	};

	template <typename Declaring, EncryptedText::EncryptedString Name>
	class NestedClassInfo
	{
	public:
		static constexpr auto DeclaringClass = Declaring::ThisClassInfo;
		static constexpr auto AssemblyName = DeclaringClass.AssemblyName;
		static constexpr auto NamespaceName = DeclaringClass.NamespaceName;
		static constexpr auto DeclaringClassName = DeclaringClass.ClassName;
		static constexpr auto ClassName = Name;
		static constexpr std::uint64_t TypeId = Detail::MakeNestedTypeId(
			Detail::GetClassInfoTypeId(DeclaringClass), ClassName);
		inline static UniResolverContext::Class ClassInfoCache = UniResolverContext::Class();
		inline static std::mutex ClassInfoMutex;

		inline static UniResolverContext::Class Instance();
	};

	template<typename Definition, typename... Arguments>
	class ClosedGenericClassInfo;

	template<typename Definition, typename... Arguments>
	class ClosedGenericClassLocator;

	template<typename Derived, typename Definition, typename... Arguments>
	class ClosedGenericClassInfoBase
	{
		static_assert(sizeof...(Arguments) > 0U, "A closed generic class requires at least one type argument");

	public:
		static constexpr auto DefinitionClass = Definition::ThisClassInfo;
		static constexpr auto AssemblyName = DefinitionClass.AssemblyName;
		static constexpr auto NamespaceName = DefinitionClass.NamespaceName;
		static constexpr auto DeclaringClassName = DefinitionClass.DeclaringClassName;
		static constexpr auto ClassName = DefinitionClass.ClassName;
		static consteval std::uint64_t GetTypeId()
		{
			return Detail::MakeClosedGenericTypeId(
				DefinitionClass.TypeId,
				ManagedTypeTraits<Arguments>::TypeId...);
		}
		inline static UniResolverContext::Class ClassInfoCache = UniResolverContext::Class();
		inline static std::mutex ClassInfoMutex;

		inline static UniResolverContext::Class Instance()
		{
			Detail::RuntimeThreadAttachmentGuard attachment;
			if (!attachment.IsAttached()) return UniResolverContext::Class();
			std::lock_guard lock(ClassInfoMutex);
			if (ClassInfoCache.klass != nullptr)
			{
				return ClassInfoCache;
			}

			VmGeneralType::Class runtimeClass = VmGeneralType::api.ResolveRuntimeClass(
				ClosedGenericClassLocator<Definition, Arguments...>::Locator);
			if (runtimeClass == nullptr)
			{
				return UniResolverContext::Class();
			}

			ClassInfoCache = UniResolverContext::Class(
				AssemblyName.Decrypt().CStr(),
				NamespaceName.Decrypt().CStr(),
				ClassName.Decrypt().CStr(),
				runtimeClass,
				VmGeneralType::api.GetClassType(runtimeClass));
			return ClassInfoCache;
		}
	};

	template<typename Definition, typename... Arguments>
	class ClosedGenericClassInfo final
		: public ClosedGenericClassInfoBase<ClosedGenericClassInfo<Definition, Arguments...>,
			Definition, Arguments...>
	{
	};

	template <typename Declaring, int MethodToken>
	class MemberMethodInfo
	{
	public:
		static constexpr auto DeclaringClass = Declaring::ThisClassInfo;
		static constexpr auto Token = MethodToken;
		inline static std::atomic<VmGeneralType::Method> MethodCache = nullptr;
		inline static std::atomic<void*> MethodAddressCache = nullptr;

		inline static VmGeneralType::Method GetMethod();
		inline static void* GetMethodAddress();

		[[nodiscard]] inline static VmGeneralType::Method GetVirtualMethod(
			VmGeneralType::Object instance) noexcept
		{
			Detail::RuntimeThreadAttachmentGuard attachment;
			if (!attachment.IsAttached()) return nullptr;
			return VmGeneralType::api.GetVirtualMethod(instance, GetMethod());
		}

		template<typename Instance>
		[[nodiscard]] inline static VmGeneralType::Method GetVirtualMethod(Instance* instance) noexcept
		{
			return GetVirtualMethod(
				reinterpret_cast<VmGeneralType::Object>(const_cast<std::remove_const_t<Instance>*>(instance)));
		}
	};

	template<typename Declaring, int MethodToken, typename... Arguments>
	class ClosedGenericMethodInfo;

	template<typename Derived, typename Declaring, int MethodToken, typename... Arguments>
	class ClosedGenericMethodInfoBase
	{
		static_assert(sizeof...(Arguments) > 0U, "A closed generic method requires at least one type argument");

	public:
		static constexpr auto DeclaringClass = Declaring::ThisClassInfo;
		static constexpr auto Token = MethodToken;
		inline static std::atomic<VmGeneralType::Method> MethodCache = nullptr;
		inline static std::atomic<void*> MethodAddressCache = nullptr;

		inline static VmGeneralType::Method GetMethod()
		{
			Detail::RuntimeThreadAttachmentGuard attachment;
			if (!attachment.IsAttached()) return nullptr;
			if (MethodCache != nullptr)
			{
				return MethodCache;
			}
			if (!DeclaringClass.Instance())
			{
				return nullptr;
			}

			return MethodCache = VmGeneralType::api.ResolveRelativePointer(Derived::Locator);
		}

		inline static void* GetMethodAddress()
		{
			Detail::RuntimeThreadAttachmentGuard attachment;
			if (!attachment.IsAttached()) return nullptr;
			if (MethodAddressCache != nullptr)
			{
				return MethodAddressCache;
			}

			VmGeneralType::Method method = GetMethod();
			if (method == nullptr)
			{
				return nullptr;
			}

			return MethodAddressCache = VmGeneralType::api.GetMethodPointer(method);
		}

		[[nodiscard]] inline static VmGeneralType::Method GetVirtualMethod(
			VmGeneralType::Object instance) noexcept
		{
			Detail::RuntimeThreadAttachmentGuard attachment;
			if (!attachment.IsAttached()) return nullptr;
			return VmGeneralType::api.GetVirtualMethod(instance, GetMethod());
		}

		template<typename Instance>
		[[nodiscard]] inline static VmGeneralType::Method GetVirtualMethod(Instance* instance) noexcept
		{
			return GetVirtualMethod(
				reinterpret_cast<VmGeneralType::Object>(const_cast<std::remove_const_t<Instance>*>(instance)));
		}
	};

	template <EncryptedText::EncryptedString Name, bool IsBacking>
	struct BackingNameHandler
	{
		static constexpr auto Value = Name;

		static consteval auto Get()
		{
			if constexpr (!IsBacking)
				return Value;
			else
				return EncryptedText::EncryptedString("<").Concat(Value).Concat(
					EncryptedText::EncryptedString(">k__BackingField"));
		}
	};

	template <typename Declaring, EncryptedText::EncryptedString Name, bool IsBacking = false>
	class MemberFieldInfo
	{
	public:
		static constexpr auto DeclaringClass = Declaring::ThisClassInfo;
		static constexpr auto FieldName = BackingNameHandler<Name, IsBacking>::Get();
		inline static std::atomic<VmGeneralType::Field> FieldInfoCache = nullptr;

		inline static VmGeneralType::Field GetFieldInfo();
	};

	template <typename Declaring, typename Type, EncryptedText::EncryptedString Name, bool IsBacking = false>
	class StaticMemberField
	{
	public:
		MemberFieldInfo<Declaring, Name, IsBacking> field = MemberFieldInfo<Declaring, Name, IsBacking>();

		operator Type();

		Type operator->() requires std::is_pointer_v<Type>;

		const Type& operator=(const Type& value);
	};
}

inline UniResolverContext UniResolver = UniResolverContext();

template<EncryptedText::EncryptedString Assembly, EncryptedText::EncryptedString Namespace, EncryptedText::EncryptedString Name>
UniResolverContext::Class Template::NormalClassInfo<Assembly, Namespace, Name>::Instance()
{
	Detail::RuntimeThreadAttachmentGuard attachment;
	if (!attachment.IsAttached()) return UniResolverContext::Class();
	std::lock_guard lock(ClassInfoMutex);
	if (ClassInfoCache.klass != nullptr)
		return ClassInfoCache;
	return ClassInfoCache = UniResolver.GetClass(AssemblyName.Decrypt().CStr(), NamespaceName.Decrypt().CStr(), ClassName.Decrypt().CStr());
}

template<typename Declaring, EncryptedText::EncryptedString Name>
UniResolverContext::Class Template::NestedClassInfo<Declaring, Name>::Instance()
{
	Detail::RuntimeThreadAttachmentGuard attachment;
	if (!attachment.IsAttached()) return UniResolverContext::Class();
	std::lock_guard lock(ClassInfoMutex);
	if (ClassInfoCache.klass != nullptr)
		return ClassInfoCache;
	return ClassInfoCache = UniResolver.GetClass(DeclaringClass.Instance(), ClassName.Decrypt().CStr());
}

template<typename Declaring, EncryptedText::EncryptedString Name, bool IsBacking>
VmGeneralType::Field Template::MemberFieldInfo<Declaring, Name, IsBacking>::GetFieldInfo()
{
	Detail::RuntimeThreadAttachmentGuard attachment;
	if (!attachment.IsAttached()) return nullptr;
	if (FieldInfoCache != nullptr)
		return FieldInfoCache;
	return FieldInfoCache = UniResolver.GetField(DeclaringClass.Instance(), FieldName.Decrypt().CStr());
}

template<typename Declaring, typename Type, EncryptedText::EncryptedString Name, bool IsBacking>
Template::StaticMemberField<Declaring, Type, Name, IsBacking>::operator Type()
{
	Detail::RuntimeThreadAttachmentGuard attachment;
	if (!attachment.IsAttached())
		throw std::runtime_error("UniResolver could not attach the current thread to the managed runtime.");
	if constexpr (std::is_same_v<Type, std::string>)
	{
		VmGeneralType::String result = nullptr;
		VmGeneralType::api.GetStaticFieldValue(field.GetFieldInfo(), &result);
		return VmGeneralType::api.StringToUtf8(result);
	}
	else
	{
		Type result = Type();
		VmGeneralType::api.GetStaticFieldValue(field.GetFieldInfo(), &result);
		return result;
	}
}

template<typename Declaring, typename Type, EncryptedText::EncryptedString Name, bool IsBacking>
Type Template::StaticMemberField<Declaring, Type, Name, IsBacking>::operator->() requires std::is_pointer_v<Type>
{
	return static_cast<Type>(*this);
}

template<typename Declaring, typename Type, EncryptedText::EncryptedString Name, bool IsBacking>
const Type& Template::StaticMemberField<Declaring, Type, Name, IsBacking>::operator=(const Type& value)
{
	Detail::RuntimeThreadAttachmentGuard attachment;
	if (!attachment.IsAttached())
		throw std::runtime_error("UniResolver could not attach the current thread to the managed runtime.");
	if constexpr (std::is_same_v<Type, std::string>)
	{
		VmGeneralType::String runtimeString = VmGeneralType::api.NewString(value);
		VmGeneralType::api.SetStaticFieldValue(field.GetFieldInfo(), &runtimeString);
		return value;
	}
	else
	{
		VmGeneralType::api.SetStaticFieldValue(field.GetFieldInfo(), const_cast<Type*>(&value));
		return value;
	}
}

template <typename Declaring, int MethodToken>
VmGeneralType::Method Template::MemberMethodInfo<Declaring, MethodToken>::GetMethod()
{
	Detail::RuntimeThreadAttachmentGuard attachment;
	if (!attachment.IsAttached()) return nullptr;
	if (MethodCache != nullptr)
	{
		return MethodCache;
	}

	return MethodCache = UniResolver.GetMethod(DeclaringClass.Instance(), MethodToken).method;
}

template <typename Declaring, int MethodToken>
void* Template::MemberMethodInfo<Declaring, MethodToken>::GetMethodAddress()
{
	Detail::RuntimeThreadAttachmentGuard attachment;
	if (!attachment.IsAttached()) return nullptr;
	if (MethodAddressCache != nullptr)
	{
		return MethodAddressCache;
	}

	VmGeneralType::Method method = GetMethod();
	if (method == nullptr)
	{
		return nullptr;
	}

	return MethodAddressCache = VmGeneralType::api.GetMethodPointer(method);
}

inline std::string UniResolverContext::NameMap::MakeMethodKey(
	const std::string& assembly,
	const std::string& nameSpace,
	const std::string& className,
	const std::string& returnTypeName,
	const std::string& name,
	const std::vector<std::string>& parametersTypeName)
{
	std::string key = MakeKey(assembly, nameSpace, className, returnTypeName, name);
	for (const std::string& parameterTypeName : parametersTypeName)
	{
		key.append(parameterTypeName);
		key.push_back('\0');
	}
	return key;
}

inline std::string UniResolverContext::NameMap::Relocate(const Map& map, const std::string& key, const std::string& fallback)
{
	auto iterator = map.find(key);
	return iterator != map.end() ? iterator->second : fallback;
}

inline void UniResolverContext::NameMap::MapAssembly(const std::string& name, const std::string& relocatedName)
{
	assemblies[name] = relocatedName;
}

inline void UniResolverContext::NameMap::MapNamespace(const std::string& assembly, const std::string& name, const std::string& relocatedName)
{
	namespaces[MakeKey(assembly, name)] = relocatedName;
}

inline void UniResolverContext::NameMap::MapClass(const std::string& assembly, const std::string& nameSpace, const std::string& name, const std::string& relocatedName)
{
	classes[MakeKey(assembly, nameSpace, name)] = relocatedName;
}

inline void UniResolverContext::NameMap::MapNestedClass(const std::string& assembly, const std::string& nameSpace, const std::string& parentName, const std::string& name, const std::string& relocatedName)
{
	nestedClasses[MakeKey(assembly, nameSpace, parentName, name)] = relocatedName;
}

inline void UniResolverContext::NameMap::MapMethod(const std::string& assembly, const std::string& nameSpace, const std::string& className, const std::string& name, const std::string& relocatedName)
{
	methods[MakeKey(assembly, nameSpace, className, name)] = relocatedName;
}

inline void UniResolverContext::NameMap::MapMethod(
	const std::string& assembly,
	const std::string& nameSpace,
	const std::string& className,
	const std::string& returnTypeName,
	const std::string& name,
	const std::vector<std::string>& parametersTypeName,
	const std::string& relocatedName)
{
	methodSignatures[MakeMethodKey(
		assembly,
		nameSpace,
		className,
		returnTypeName,
		name,
		parametersTypeName)] = relocatedName;
}

inline void UniResolverContext::NameMap::MapField(const std::string& assembly, const std::string& nameSpace, const std::string& className, const std::string& name, const std::string& relocatedName)
{
	fields[MakeKey(assembly, nameSpace, className, name)] = relocatedName;
}

inline void UniResolverContext::NameMap::MapType(const std::string& name, const std::string& relocatedName)
{
	types[name] = relocatedName;
}

inline std::string UniResolverContext::NameMap::RelocateAssembly(const std::string& name) const
{
	return Relocate(assemblies, name, name);
}

inline std::string UniResolverContext::NameMap::RelocateNamespace(const std::string& assembly, const std::string& name) const
{
	return Relocate(namespaces, MakeKey(assembly, name), name);
}

inline std::string UniResolverContext::NameMap::RelocateClass(const std::string& assembly, const std::string& nameSpace, const std::string& name) const
{
	return Relocate(classes, MakeKey(assembly, nameSpace, name), name);
}

inline std::string UniResolverContext::NameMap::RelocateNestedClass(const std::string& assembly, const std::string& nameSpace, const std::string& parentName, const std::string& name) const
{
	return Relocate(nestedClasses, MakeKey(assembly, nameSpace, parentName, name), name);
}

inline std::string UniResolverContext::NameMap::RelocateMethod(
	const std::string& assembly,
	const std::string& nameSpace,
	const std::string& className,
	const std::string& returnTypeName,
	const std::string& name,
	const std::vector<std::string>& parametersTypeName) const
{
	const std::string signatureKey = MakeMethodKey(
		assembly,
		nameSpace,
		className,
		returnTypeName,
		name,
		parametersTypeName);
	auto signatureIterator = methodSignatures.find(signatureKey);
	if (signatureIterator != methodSignatures.end())
	{
		return signatureIterator->second;
	}
	return Relocate(methods, MakeKey(assembly, nameSpace, className, name), name);
}

inline std::string UniResolverContext::NameMap::RelocateField(const std::string& assembly, const std::string& nameSpace, const std::string& className, const std::string& name) const
{
	return Relocate(fields, MakeKey(assembly, nameSpace, className, name), name);
}

inline std::string UniResolverContext::NameMap::RelocateType(const std::string& name) const
{
	return Relocate(types, name, name);
}

inline void UniResolverContext::NameMap::Clear()
{
	assemblies.clear();
	namespaces.clear();
	classes.clear();
	nestedClasses.clear();
	methods.clear();
	methodSignatures.clear();
	fields.clear();
	types.clear();
}

inline bool UniResolverContext::NameMap::Empty() const noexcept
{
	return assemblies.empty()
		&& namespaces.empty()
		&& classes.empty()
		&& nestedClasses.empty()
		&& methods.empty()
		&& methodSignatures.empty()
		&& fields.empty()
		&& types.empty();
}

inline void UniResolverContext::Class::AddNestedClass(const std::string name, const Class& nestedClass)
{
	nestedClasses.insert({ name, nestedClass });
}

inline UniResolverContext::Class UniResolverContext::Class::FindNestedClass(const std::string name) const
{
	auto iter = nestedClasses.find(name);
	if (iter == nestedClasses.end())
		return UniResolverContext::Class();
	return iter->second;
}

inline void UniResolverContext::ContextCache::RegisterAssembly(const std::string& name, VmGeneralType::Assembly assembly)
{
	assemblyMap[name] = assembly;
}

inline UniResolverContext::Class UniResolverContext::ContextCache::RegisterClass(const std::string& assembly, const std::string& nameSpace, const std::string& name, VmGeneralType::Class klass, VmGeneralType::Type type)
{
	return classPathMap[assembly][nameSpace][name] = Class(assembly, nameSpace, name, klass, type);
}

inline VmGeneralType::Assembly UniResolverContext::ContextCache::GetAssembly(const std::string& name) const
{
	auto iter = assemblyMap.find(name);
	if (iter == assemblyMap.end())
		return nullptr;
	return iter->second;
}

inline UniResolverContext::Class UniResolverContext::ContextCache::GetClass(const std::string& assembly, const std::string& nameSpace, const std::string& name) const
{
	auto assemblyIter = classPathMap.find(assembly);
	if (assemblyIter == classPathMap.end())
		return Class();
	auto nameSpaceIter = assemblyIter->second.find(nameSpace);
	if (nameSpaceIter == assemblyIter->second.end())
		return Class();
	auto nameIter = nameSpaceIter->second.find(name);
	if (nameIter == nameSpaceIter->second.end())
		return Class();
	return nameIter->second;
}

inline UniResolverContext::Class& UniResolverContext::ContextCache::GetClass(const std::string& assembly, const std::string& nameSpace, const std::string& name)
{
	return classPathMap[assembly][nameSpace][name];
}

inline void UniResolverContext::ContextCache::Clear()
{
	assemblyMap.clear();
	classPathMap.clear();
}

inline bool UniResolverContext::Setup()
{
	std::lock_guard lock(stateMutex);
	if (thread != nullptr && domain != nullptr)
	{
		return true;
	}

	if (!VmGeneralType::api.IsBound() && !VmGeneralType::api.Bind())
	{
		return false;
	}

	domain = VmGeneralType::api.GetDomain();
	if (domain == nullptr)
	{
		return false;
	}

	thread = VmGeneralType::api.GetCurrentThread();
	if (thread == nullptr)
	{
		thread = VmGeneralType::api.AttachThread(domain);
		ownsThread = thread != nullptr;
	}
	else
	{
		ownsThread = false;
	}

	if (thread == nullptr)
	{
		domain = nullptr;
		return false;
	}
	return true;
}

inline void UniResolverContext::Destroy()
{
	std::lock_guard lock(stateMutex);
	if (ownsThread && thread != nullptr)
	{
		VmGeneralType::api.DetachThread(thread);
	}

	thread = nullptr;
	domain = nullptr;
	ownsThread = false;
	cache.Clear();
}

inline void UniResolverContext::SetNameMap(NameMap map)
{
	std::lock_guard lock(stateMutex);
	nameMap = std::move(map);
	cache.Clear();
}

inline const UniResolverContext::NameMap& UniResolverContext::GetNameMap() const noexcept
{
	return nameMap;
}

inline void UniResolverContext::ClearNameMap()
{
	std::lock_guard lock(stateMutex);
	nameMap.Clear();
	cache.Clear();
}

inline UniResolverContext::Class UniResolverContext::GetClass(const std::string& assemblyName, const std::string& namespaceName, const std::string& className)
{
	Template::Detail::RuntimeThreadAttachmentGuard attachment;
	if (!attachment.IsAttached()) return Class();
	std::lock_guard lock(stateMutex);
	Class result = cache.GetClass(assemblyName, namespaceName, className);
	if (result)
		return result;
	const std::string relocatedAssemblyName = nameMap.RelocateAssembly(assemblyName);
	const std::string relocatedNamespaceName = nameMap.RelocateNamespace(assemblyName, namespaceName);
	const std::string relocatedClassName = nameMap.RelocateClass(assemblyName, namespaceName, className);
	VmGeneralType::Assembly assembly = cache.GetAssembly(assemblyName);
	if (!assembly)
	{
		assembly = VmGeneralType::api.OpenAssembly(domain, relocatedAssemblyName);
		if (!assembly)
		{
			return Class();
		}
		cache.RegisterAssembly(assemblyName, assembly);
	}
	VmGeneralType::Image image = VmGeneralType::api.GetAssemblyImage(assembly);
	if (!image)
	{
		return Class();
	}
	VmGeneralType::Class klass = VmGeneralType::api.FindClass(image, relocatedNamespaceName, relocatedClassName);
	if (!klass)
	{
		return Class();
	}
	return result = cache.RegisterClass(assemblyName, namespaceName, className, klass, VmGeneralType::api.GetClassType(klass));
}

inline UniResolverContext::Class UniResolverContext::GetClass(Class parent, const std::string& className)
{
	Template::Detail::RuntimeThreadAttachmentGuard attachment;
	if (!attachment.IsAttached()) return Class();
	std::lock_guard lock(stateMutex);
	if (!parent)
	{
		return UniResolverContext::Class();
	}
	Class& parentReference = cache.GetClass(parent.assemblyName, parent.namespaceName, parent.className);
	if (!parentReference)
	{
		return UniResolverContext::Class();
	}
	Class cachedClass = parentReference.FindNestedClass(className);
	if (cachedClass)
	{
		return cachedClass;
	}
	const std::string relocatedClassName = nameMap.RelocateNestedClass(
		parent.assemblyName,
		parent.namespaceName,
		parent.className,
		className);
	std::vector<VmGeneralType::Class> nestedClasses = VmGeneralType::api.GetNestedTypes(parentReference.klass);
	for (VmGeneralType::Class nestedClass : nestedClasses)
	{
		if (VmGeneralType::api.GetClassName(nestedClass) == relocatedClassName)
		{
			Class klass = Class(
				parentReference.assemblyName,
				parentReference.namespaceName,
				className,
				nestedClass,
				VmGeneralType::api.GetClassType(nestedClass));
			parentReference.AddNestedClass(className, klass);
			return klass;
		}
	}
	return UniResolverContext::Class();
}

inline UniResolverContext::Method UniResolverContext::GetMethod(Class parent, const std::string& returnTypeName, const std::string& methodName, const std::vector<std::string>& parametersTypeName)
{
	Template::Detail::RuntimeThreadAttachmentGuard attachment;
	if (!attachment.IsAttached()) return Method();
	std::lock_guard lock(stateMutex);
	if (!parent)
	{
		return UniResolverContext::Method();
	}

	const std::string automaticMatch = TEXT("_AUTO_");
	const std::string relocatedMethodName = methodName == automaticMatch
		? automaticMatch
		: nameMap.RelocateMethod(
			parent.assemblyName,
			parent.namespaceName,
			parent.className,
			returnTypeName,
			methodName,
			parametersTypeName);
	const std::string relocatedReturnTypeName = returnTypeName == automaticMatch
		? automaticMatch
		: nameMap.RelocateType(returnTypeName);
	std::vector<std::string> relocatedParametersTypeName = parametersTypeName;
	for (std::string& parameterTypeName : relocatedParametersTypeName)
	{
		if (parameterTypeName != automaticMatch)
		{
			parameterTypeName = nameMap.RelocateType(parameterTypeName);
		}
	}
	void* iterator = nullptr;
	VmGeneralType::Method method = nullptr;
	while ((method = VmGeneralType::api.GetNextMethod(parent.klass, &iterator)) != nullptr)
	{
		std::string name = VmGeneralType::api.GetMethodName(method);
		if (relocatedMethodName != automaticMatch && name != relocatedMethodName)
			continue;
		std::string returnType = VmGeneralType::api.GetTypeName(VmGeneralType::api.GetMethodReturnType(method));
		if (relocatedReturnTypeName != automaticMatch && returnType != relocatedReturnTypeName)
			continue;
		std::vector<VmGeneralType::Type> parametersType = VmGeneralType::api.GetMethodParameterTypes(method);
		if (parametersType.size() != parametersTypeName.size())
			continue;
		std::vector<std::string> actualParametersTypeName = {};
		actualParametersTypeName.reserve(parametersType.size());
		bool isMatch = true;
		for (size_t i = 0; i < parametersType.size(); i++)
		{
			std::string parameterType = VmGeneralType::api.GetTypeName(parametersType[i]);
			actualParametersTypeName.push_back(parameterType);
			if (relocatedParametersTypeName[i] != automaticMatch && parameterType != relocatedParametersTypeName[i])
			{
				isMatch = false;
				break;
			}
		}
		if (!isMatch)
			continue;
		return Method(returnType, name, actualParametersTypeName, method);
	}
	return UniResolverContext::Method();
}

inline UniResolverContext::Method UniResolverContext::GetMethod(Class parent, int token)
{
	Template::Detail::RuntimeThreadAttachmentGuard attachment;
	if (!attachment.IsAttached()) return Method();
	std::lock_guard lock(stateMutex);
	if (!parent)
	{
		return UniResolverContext::Method();
	}
	void* iterator = nullptr;
	VmGeneralType::Method method = nullptr;
	while ((method = VmGeneralType::api.GetNextMethod(parent.klass, &iterator)) != nullptr)
	{
		if (VmGeneralType::api.GetMethodToken(method) != static_cast<std::uint32_t>(token))
			continue;
		std::string name = VmGeneralType::api.GetMethodName(method);
		std::string returnType = VmGeneralType::api.GetTypeName(VmGeneralType::api.GetMethodReturnType(method));
		std::vector<VmGeneralType::Type> parametersType = VmGeneralType::api.GetMethodParameterTypes(method);
		std::vector<std::string> parametersTypeName(parametersType.size());
		for (size_t i = 0; i < parametersType.size(); i++)
		{
			parametersTypeName[i] = VmGeneralType::api.GetTypeName(parametersType[i]);
		}
		return Method(returnType, name, parametersTypeName, method);
	}
	return UniResolverContext::Method();
}

inline VmGeneralType::Field UniResolverContext::GetField(Class parent, const std::string& fieldName)
{
	Template::Detail::RuntimeThreadAttachmentGuard attachment;
	if (!attachment.IsAttached()) return nullptr;
	std::lock_guard lock(stateMutex);
	if (!parent)
	{
		return nullptr;
	}
	const std::string relocatedFieldName = nameMap.RelocateField(
		parent.assemblyName,
		parent.namespaceName,
		parent.className,
		fieldName);
	return VmGeneralType::api.FindField(parent.klass, relocatedFieldName);
}

#define MANAGED_TYPE_NAMED(assemblyName, namespaceName, className, metadataName, storageType) \
	using __This_Class_Type__ = className; \
	using ManagedStorageType = storageType; \
	inline static constexpr Template::NormalClassInfo<ENCRYPTED_STRING(assemblyName), ENCRYPTED_STRING(namespaceName), ENCRYPTED_STRING(metadataName)> ThisClassInfo = {}
#define CLASS_NAMED(assemblyName, namespaceName, className, metadataName) MANAGED_TYPE_NAMED(assemblyName, namespaceName, className, metadataName, className*)
#define CLASS(assemblyName, namespaceName, className) CLASS_NAMED(assemblyName, namespaceName, className, #className)
#define VALUE_CLASS_NAMED(assemblyName, namespaceName, className, metadataName) MANAGED_TYPE_NAMED(assemblyName, namespaceName, className, metadataName, className)
#define VALUE_CLASS(assemblyName, namespaceName, className) VALUE_CLASS_NAMED(assemblyName, namespaceName, className, #className)
#define VALUE_CLASS_STORAGE_NAMED(assemblyName, namespaceName, className, metadataName, storageType) MANAGED_TYPE_NAMED(assemblyName, namespaceName, className, metadataName, storageType)
#define VALUE_CLASS_STORAGE(assemblyName, namespaceName, className, storageType) VALUE_CLASS_STORAGE_NAMED(assemblyName, namespaceName, className, #className, storageType)

#define GENERIC_CLASS_DEFINITION(assemblyName, namespaceName, metadataName) \
	inline static constexpr Template::NormalClassInfo<ENCRYPTED_STRING(assemblyName), ENCRYPTED_STRING(namespaceName), ENCRYPTED_STRING(metadataName)> ThisClassInfo = {}
#define GENERIC_CLASS(className, definition, ...) \
	using __This_Class_Type__ = className; \
	using ManagedStorageType = className*; \
	inline static constexpr Template::ClosedGenericClassInfo<definition, __VA_ARGS__> ThisClassInfo = {}
#define GENERIC_VALUE_CLASS(className, definition, ...) \
	using __This_Class_Type__ = className; \
	using ManagedStorageType = className; \
	inline static constexpr Template::ClosedGenericClassInfo<definition, __VA_ARGS__> ThisClassInfo = {}

#define CLOSED_GENERIC_CLASS_INFO(definition, locator, ...) \
	template<> \
	class Template::ClosedGenericClassLocator<definition, __VA_ARGS__> final \
	{ \
	public: \
		inline static constexpr auto Locator = locator; \
	}

#define CLOSED_GENERIC_METHOD_INFO(declaring, token, locator, ...) \
	template<> \
	class Template::ClosedGenericMethodInfo<declaring, token, __VA_ARGS__> final \
		: public Template::ClosedGenericMethodInfoBase<Template::ClosedGenericMethodInfo<declaring, token, __VA_ARGS__>, declaring, token, __VA_ARGS__> \
	{ \
	public: \
		inline static constexpr auto Locator = locator; \
	}

#define NESTED_MANAGED_TYPE_NAMED(declaring, className, metadataName, storageType) \
	using __This_Class_Type__ = className; \
	using ManagedStorageType = storageType; \
	inline static constexpr Template::NestedClassInfo<declaring, ENCRYPTED_STRING(metadataName)> ThisClassInfo = {}
#define NESTED_CLASS_NAMED(declaring, className, metadataName) NESTED_MANAGED_TYPE_NAMED(declaring, className, metadataName, className*)
#define NESTED_CLASS(declaring, className) NESTED_CLASS_NAMED(declaring, className, #className)
#define NESTED_VALUE_CLASS_NAMED(declaring, className, metadataName) NESTED_MANAGED_TYPE_NAMED(declaring, className, metadataName, className)
#define NESTED_VALUE_CLASS(declaring, className) NESTED_VALUE_CLASS_NAMED(declaring, className, #className)
#define NESTED_VALUE_CLASS_STORAGE_NAMED(declaring, className, metadataName, storageType) NESTED_MANAGED_TYPE_NAMED(declaring, className, metadataName, storageType)
#define NESTED_VALUE_CLASS_STORAGE(declaring, className, storageType) NESTED_VALUE_CLASS_STORAGE_NAMED(declaring, className, #className, storageType)

#define STATIC_FIELD_NAMED(type, name, metadataName) inline static Template::StaticMemberField<__This_Class_Type__, type, ENCRYPTED_STRING(metadataName)> name = {}
#define STATIC_FIELD(type, name) STATIC_FIELD_NAMED(type, name, #name)
#define STATIC_BACKING_FIELD_NAMED(type, name, metadataName) inline static Template::StaticMemberField<__This_Class_Type__, type, ENCRYPTED_STRING(metadataName), true> name = {}
#define STATIC_BACKING_FIELD(type, name) STATIC_BACKING_FIELD_NAMED(type, name, #name)

#define METHOD_INFO(token) inline static constexpr Template::MemberMethodInfo<__This_Class_Type__, token> __##token##_Method_Info__ = {}
#undef TEXT
#endif // !H_NARESOLVER
