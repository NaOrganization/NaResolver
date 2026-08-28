#pragma once

#include <Windows.h>

#include <cstdint>
#include <limits>
#include <string>
#include <type_traits>
#include <vector>

#include "KrOr.h"

#define VM_API_SYMBOL(value) KROR(value).CStr()

namespace VmGeneralType
{
	struct ObjectStorage;
	struct TypeStorage;
	struct MethodStorage;
	struct FieldStorage;
	struct ClassStorage;
	struct ImageStorage;
	struct AssemblyStorage;
	struct DomainStorage;
	struct StringStorage;
	struct ArrayStorage;
	struct ThreadStorage;

	using Object = ObjectStorage*;
	using Type = TypeStorage*;
	using Method = MethodStorage*;
	using Field = FieldStorage*;
	using Class = ClassStorage*;
	using Image = ImageStorage*;
	using Assembly = AssemblyStorage*;
	using Domain = DomainStorage*;
	using String = StringStorage*;
	using Array = ArrayStorage*;
	using Thread = ThreadStorage*;

	enum class RelativePointerStorage : std::uint8_t
	{
		Direct,
		Indirect
	};

	template<typename Pointer>
	struct RelativePointer
	{
		static_assert(std::is_pointer_v<Pointer>, "RelativePointer requires a pointer type");

		std::uintptr_t rva = 0U;
		RelativePointerStorage storage = RelativePointerStorage::Direct;

		[[nodiscard]] static consteval RelativePointer FromRva(std::uintptr_t value) noexcept
		{
			return { value, RelativePointerStorage::Direct };
		}

		[[nodiscard]] static consteval RelativePointer FromPointerRva(std::uintptr_t value) noexcept
		{
			return { value, RelativePointerStorage::Indirect };
		}
	};

	struct RuntimeInvokeResult
	{
		Object value = nullptr;
		Object exception = nullptr;
		bool dispatched = false;
	};

	class Api final
	{
	private:
		using FreeFn = void(*)(void*);
		using TypeGetNameFn = char*(*)(Type);
		using TypeGetObjectFn = Object(*)(Type);
		using MethodGetNameFn = const char*(*)(Method);
		using MethodGetTokenFn = std::uint32_t(*)(Method);
		using MethodGetReturnTypeFn = Type(*)(Method);
		using MethodIsGenericFn = bool(*)(Method);
		using MethodGetParamCountFn = std::uint32_t(*)(Method);
		using MethodGetParamFn = Type(*)(Method, std::uint32_t);
		using ClassGetNameFn = const char*(*)(Class);
		using ClassGetMethodsFn = Method(*)(Class, void**);
		using ClassGetFieldFn = Field(*)(Class, const char*);
		using ClassGetTypeFn = Type(*)(Class);
		using ClassGetNestedTypesFn = Class(*)(Class, void**);
		using ClassFromTypeFn = Class(*)(Type);
		using FieldValueFn = void(*)(Object, Field, void*);
		using StaticFieldValueFn = void(*)(Field, void*);
		using ImageGetNameFn = const char*(*)(Image);
		using ClassFromNameFn = Class(*)(Image, const char*, const char*);
		using AssemblyGetImageFn = Image(*)(Assembly);
		using DomainGetFn = Domain(*)();
		using AssemblyOpenFn = Assembly(*)(Domain, const char*);
		using StringNewFn = String(*)(const char*);
		using StringNewLenFn = String(*)(const char*, std::uint32_t);
		using StringCharsFn = const char16_t*(*)(String);
		using StringLengthFn = std::int32_t(*)(String);
		using ArrayNewFn = Array(*)(Class, std::uintptr_t);
		using ThreadAttachFn = Thread(*)(Domain);
		using ThreadCurrentFn = Thread(*)();
		using ThreadDetachFn = void(*)(Thread);
		using RuntimeInvokeFn = Object(*)(Method, Object, void**, Object*);
		using ObjectNewFn = Object(*)(Class);
		using ObjectGetVirtualMethodFn = Method(*)(Object, Method);
		using ObjectUnboxFn = void*(*)(Object);
		using GcHandleNewFn = std::uint32_t(*)(Object, bool);
		using GcHandleFreeFn = void(*)(std::uint32_t);
		using GcHandleGetTargetFn = Object(*)(std::uint32_t);

		HMODULE module = nullptr;

		FreeFn freeMemory = nullptr;
		TypeGetNameFn typeGetName = nullptr;
		TypeGetObjectFn typeGetObject = nullptr;
		MethodGetNameFn methodGetName = nullptr;
		MethodGetTokenFn methodGetToken = nullptr;
		MethodGetReturnTypeFn methodGetReturnType = nullptr;
		MethodIsGenericFn methodIsGeneric = nullptr;
		MethodGetParamCountFn methodGetParamCount = nullptr;
		MethodGetParamFn methodGetParam = nullptr;
		ClassGetNameFn classGetName = nullptr;
		ClassGetMethodsFn classGetMethods = nullptr;
		ClassGetFieldFn classGetField = nullptr;
		ClassGetTypeFn classGetType = nullptr;
		ClassGetNestedTypesFn classGetNestedTypes = nullptr;
		ClassFromTypeFn classFromType = nullptr;
		FieldValueFn fieldGetValue = nullptr;
		FieldValueFn fieldSetValue = nullptr;
		StaticFieldValueFn staticFieldGetValue = nullptr;
		StaticFieldValueFn staticFieldSetValue = nullptr;
		ImageGetNameFn imageGetName = nullptr;
		ClassFromNameFn classFromName = nullptr;
		AssemblyGetImageFn assemblyGetImage = nullptr;
		DomainGetFn domainGet = nullptr;
		AssemblyOpenFn assemblyOpen = nullptr;
		StringNewFn stringNew = nullptr;
		StringNewLenFn stringNewLen = nullptr;
		StringCharsFn stringChars = nullptr;
		StringLengthFn stringLength = nullptr;
		ArrayNewFn arrayNew = nullptr;
		ThreadAttachFn threadAttach = nullptr;
		ThreadCurrentFn threadCurrent = nullptr;
		ThreadDetachFn threadDetach = nullptr;
		RuntimeInvokeFn runtimeInvoke = nullptr;
		ObjectNewFn objectNew = nullptr;
		ObjectGetVirtualMethodFn objectGetVirtualMethod = nullptr;
		ObjectUnboxFn objectUnbox = nullptr;
		GcHandleNewFn gcHandleNew = nullptr;
		GcHandleFreeFn gcHandleFree = nullptr;
		GcHandleGetTargetFn gcHandleGetTarget = nullptr;

		template<typename Function>
		[[nodiscard]] Function Resolve(const char* symbol) const noexcept
		{
			if (module == nullptr || symbol == nullptr)
			{
				return nullptr;
			}

			return reinterpret_cast<Function>(GetProcAddress(module, symbol));
		}

		[[nodiscard]] bool HasRequiredBindings() const noexcept
		{
			return domainGet != nullptr
				&& assemblyOpen != nullptr
				&& assemblyGetImage != nullptr
				&& classFromName != nullptr
				&& classGetMethods != nullptr
				&& classGetField != nullptr
				&& classGetType != nullptr
				&& classFromType != nullptr
				&& fieldGetValue != nullptr
				&& fieldSetValue != nullptr
				&& staticFieldGetValue != nullptr
				&& staticFieldSetValue != nullptr
				&& methodGetName != nullptr
				&& methodGetToken != nullptr
				&& methodGetReturnType != nullptr
				&& methodGetParamCount != nullptr
				&& methodGetParam != nullptr
				&& typeGetName != nullptr
				&& (stringNewLen != nullptr || stringNew != nullptr)
				&& stringChars != nullptr
				&& stringLength != nullptr
				&& threadAttach != nullptr
				&& threadCurrent != nullptr
				&& threadDetach != nullptr
				&& runtimeInvoke != nullptr
				&& objectUnbox != nullptr
				&& gcHandleNew != nullptr
				&& gcHandleFree != nullptr
				&& gcHandleGetTarget != nullptr
				&& objectNew != nullptr
				&& objectGetVirtualMethod != nullptr;
		}

		bool BindIl2Cpp(HMODULE runtimeModule) noexcept
		{
			module = runtimeModule;
			freeMemory = Resolve<FreeFn>(VM_API_SYMBOL("il2cpp_free"));
			typeGetName = Resolve<TypeGetNameFn>(VM_API_SYMBOL("il2cpp_type_get_name"));
			typeGetObject = Resolve<TypeGetObjectFn>(VM_API_SYMBOL("il2cpp_type_get_object"));
			methodGetName = Resolve<MethodGetNameFn>(VM_API_SYMBOL("il2cpp_method_get_name"));
			methodGetToken = Resolve<MethodGetTokenFn>(VM_API_SYMBOL("il2cpp_method_get_token"));
			methodGetReturnType = Resolve<MethodGetReturnTypeFn>(VM_API_SYMBOL("il2cpp_method_get_return_type"));
			methodIsGeneric = Resolve<MethodIsGenericFn>(VM_API_SYMBOL("il2cpp_method_is_generic"));
			methodGetParamCount = Resolve<MethodGetParamCountFn>(VM_API_SYMBOL("il2cpp_method_get_param_count"));
			methodGetParam = Resolve<MethodGetParamFn>(VM_API_SYMBOL("il2cpp_method_get_param"));
			classGetName = Resolve<ClassGetNameFn>(VM_API_SYMBOL("il2cpp_class_get_name"));
			classGetMethods = Resolve<ClassGetMethodsFn>(VM_API_SYMBOL("il2cpp_class_get_methods"));
			classGetField = Resolve<ClassGetFieldFn>(VM_API_SYMBOL("il2cpp_class_get_field_from_name"));
			classGetType = Resolve<ClassGetTypeFn>(VM_API_SYMBOL("il2cpp_class_get_type"));
			classGetNestedTypes = Resolve<ClassGetNestedTypesFn>(VM_API_SYMBOL("il2cpp_class_get_nested_types"));
			classFromType = Resolve<ClassFromTypeFn>(VM_API_SYMBOL("il2cpp_class_from_type"));
			fieldGetValue = Resolve<FieldValueFn>(VM_API_SYMBOL("il2cpp_field_get_value"));
			fieldSetValue = Resolve<FieldValueFn>(VM_API_SYMBOL("il2cpp_field_set_value"));
			staticFieldGetValue = Resolve<StaticFieldValueFn>(VM_API_SYMBOL("il2cpp_field_static_get_value"));
			staticFieldSetValue = Resolve<StaticFieldValueFn>(VM_API_SYMBOL("il2cpp_field_static_set_value"));
			imageGetName = Resolve<ImageGetNameFn>(VM_API_SYMBOL("il2cpp_image_get_name"));
			classFromName = Resolve<ClassFromNameFn>(VM_API_SYMBOL("il2cpp_class_from_name"));
			assemblyGetImage = Resolve<AssemblyGetImageFn>(VM_API_SYMBOL("il2cpp_assembly_get_image"));
			domainGet = Resolve<DomainGetFn>(VM_API_SYMBOL("il2cpp_domain_get"));
			assemblyOpen = Resolve<AssemblyOpenFn>(VM_API_SYMBOL("il2cpp_domain_assembly_open"));
			stringNew = Resolve<StringNewFn>(VM_API_SYMBOL("il2cpp_string_new"));
			stringNewLen = Resolve<StringNewLenFn>(VM_API_SYMBOL("il2cpp_string_new_len"));
			stringChars = Resolve<StringCharsFn>(VM_API_SYMBOL("il2cpp_string_chars"));
			stringLength = Resolve<StringLengthFn>(VM_API_SYMBOL("il2cpp_string_length"));
			arrayNew = Resolve<ArrayNewFn>(VM_API_SYMBOL("il2cpp_array_new"));
			threadAttach = Resolve<ThreadAttachFn>(VM_API_SYMBOL("il2cpp_thread_attach"));
			threadCurrent = Resolve<ThreadCurrentFn>(VM_API_SYMBOL("il2cpp_thread_current"));
			threadDetach = Resolve<ThreadDetachFn>(VM_API_SYMBOL("il2cpp_thread_detach"));
			runtimeInvoke = Resolve<RuntimeInvokeFn>(VM_API_SYMBOL("il2cpp_runtime_invoke"));
			objectNew = Resolve<ObjectNewFn>(VM_API_SYMBOL("il2cpp_object_new"));
			objectGetVirtualMethod = Resolve<ObjectGetVirtualMethodFn>(
				VM_API_SYMBOL("il2cpp_object_get_virtual_method"));
			objectUnbox = Resolve<ObjectUnboxFn>(VM_API_SYMBOL("il2cpp_object_unbox"));
			gcHandleNew = Resolve<GcHandleNewFn>(VM_API_SYMBOL("il2cpp_gchandle_new"));
			gcHandleFree = Resolve<GcHandleFreeFn>(VM_API_SYMBOL("il2cpp_gchandle_free"));
			gcHandleGetTarget = Resolve<GcHandleGetTargetFn>(
				VM_API_SYMBOL("il2cpp_gchandle_get_target"));

			return HasRequiredBindings();
		}

	public:
		bool Bind() noexcept
		{
			Reset();

			HMODULE il2CppModule = GetModuleHandleA(VM_API_SYMBOL("GameAssembly.dll"));
			if (il2CppModule == nullptr)
			{
				return false;
			}

			if (!BindIl2Cpp(il2CppModule))
			{
				Reset();
				return false;
			}

			return true;
		}

		void Reset() noexcept
		{
			*this = Api{};
		}

		[[nodiscard]] bool IsBound() const noexcept
		{
			return module != nullptr;
		}

		template<typename Pointer>
		[[nodiscard]] Pointer ResolveRelativePointer(RelativePointer<Pointer> locator) const noexcept
		{
			if (module == nullptr || locator.rva == 0U)
			{
				return nullptr;
			}

			const std::uintptr_t moduleAddress = reinterpret_cast<std::uintptr_t>(module);
			if (locator.rva > (std::numeric_limits<std::uintptr_t>::max)() - moduleAddress)
			{
				return nullptr;
			}

			const std::uintptr_t targetAddress = moduleAddress + locator.rva;
			if (locator.storage == RelativePointerStorage::Direct)
			{
				return reinterpret_cast<Pointer>(targetAddress);
			}

			return *reinterpret_cast<Pointer*>(targetAddress);
		}

		[[nodiscard]] Class ResolveRuntimeClass(
			RelativePointer<Class> locator) const noexcept
		{
			return ResolveRelativePointer(locator);
		}

		[[nodiscard]] Class ResolveRuntimeClass(
			RelativePointer<Type> locator) const noexcept
		{
			const Type type = ResolveRelativePointer(locator);
			return type != nullptr && classFromType != nullptr ? classFromType(type) : nullptr;
		}

		[[nodiscard]] Domain GetDomain() const noexcept
		{
			return domainGet != nullptr ? domainGet() : nullptr;
		}

		[[nodiscard]] Assembly OpenAssembly(Domain domain, const std::string& name) const
		{
			if (assemblyOpen == nullptr)
			{
				return nullptr;
			}

			return assemblyOpen(domain, name.c_str());
		}

		[[nodiscard]] Image GetAssemblyImage(Assembly assembly) const noexcept
		{
			return assemblyGetImage != nullptr ? assemblyGetImage(assembly) : nullptr;
		}

		[[nodiscard]] std::string GetImageName(Image image) const
		{
			const char* name = imageGetName != nullptr ? imageGetName(image) : nullptr;
			return name != nullptr ? name : "";
		}

		[[nodiscard]] Class FindClass(Image image, const std::string& namespaceName, const std::string& className) const noexcept
		{
			return classFromName != nullptr ? classFromName(image, namespaceName.c_str(), className.c_str()) : nullptr;
		}

		[[nodiscard]] std::string GetClassName(Class klass) const
		{
			const char* name = classGetName != nullptr ? classGetName(klass) : nullptr;
			return name != nullptr ? name : "";
		}

		[[nodiscard]] Type GetClassType(Class klass) const noexcept
		{
			return classGetType != nullptr ? classGetType(klass) : nullptr;
		}

		[[nodiscard]] Method GetNextMethod(Class klass, void** iterator) const noexcept
		{
			return classGetMethods != nullptr ? classGetMethods(klass, iterator) : nullptr;
		}

		[[nodiscard]] Field FindField(Class klass, const std::string& name) const noexcept
		{
			return classGetField != nullptr ? classGetField(klass, name.c_str()) : nullptr;
		}

		[[nodiscard]] std::vector<Class> GetNestedTypes(Class klass) const
		{
			std::vector<Class> result = {};
			if (classGetNestedTypes == nullptr)
			{
				return result;
			}

			void* iterator = nullptr;
			while (Class nestedClass = classGetNestedTypes(klass, &iterator))
			{
				result.push_back(nestedClass);
			}
			return result;
		}

		[[nodiscard]] std::string GetTypeName(Type type) const
		{
			char* name = typeGetName != nullptr ? typeGetName(type) : nullptr;
			if (name == nullptr)
			{
				return "";
			}

			std::string result = name;
			if (freeMemory != nullptr)
			{
				freeMemory(name);
			}
			return result;
		}

		[[nodiscard]] Object GetTypeObject(Type type) const noexcept
		{
			return typeGetObject != nullptr ? typeGetObject(type) : nullptr;
		}

		[[nodiscard]] std::string GetMethodName(Method method) const
		{
			const char* name = methodGetName != nullptr ? methodGetName(method) : nullptr;
			return name != nullptr ? name : "";
		}

		[[nodiscard]] std::uint32_t GetMethodToken(Method method) const noexcept
		{
			return methodGetToken != nullptr ? methodGetToken(method) : 0U;
		}

		[[nodiscard]] Type GetMethodReturnType(Method method) const noexcept
		{
			return methodGetReturnType != nullptr ? methodGetReturnType(method) : nullptr;
		}

		[[nodiscard]] bool IsMethodGeneric(Method method) const noexcept
		{
			return methodIsGeneric != nullptr && methodIsGeneric(method);
		}

		[[nodiscard]] std::vector<Type> GetMethodParameterTypes(Method method) const
		{
			std::vector<Type> result = {};
			if (methodGetParamCount == nullptr || methodGetParam == nullptr)
			{
				return result;
			}

			const std::uint32_t count = methodGetParamCount(method);
			result.reserve(count);
			for (std::uint32_t i = 0U; i < count; ++i)
			{
				result.push_back(methodGetParam(method, i));
			}
			return result;
		}

		[[nodiscard]] void* GetMethodPointer(Method method) const noexcept
		{
			if (method == nullptr)
			{
				return nullptr;
			}
			return *reinterpret_cast<void**>(method);
		}

		[[nodiscard]] Method GetVirtualMethod(Object object, Method method) const noexcept
		{
			if (objectGetVirtualMethod == nullptr || object == nullptr || method == nullptr)
			{
				return nullptr;
			}

			return objectGetVirtualMethod(object, method);
		}

		[[nodiscard]] RuntimeInvokeResult Invoke(Method method, Object instance, void** parameters) const noexcept
		{
			RuntimeInvokeResult result = {};
			if (runtimeInvoke == nullptr || method == nullptr)
			{
				return result;
			}

			result.dispatched = true;
			result.value = runtimeInvoke(method, instance, parameters, &result.exception);
			return result;
		}

		[[nodiscard]] void* Unbox(Object object) const noexcept
		{
			return objectUnbox != nullptr && object != nullptr ? objectUnbox(object) : nullptr;
		}

		[[nodiscard]] Object NewObject(Class klass) const noexcept
		{
			return objectNew != nullptr && klass != nullptr ? objectNew(klass) : nullptr;
		}

		void GetFieldValue(Object object, Field field, void* value) const noexcept
		{
			if (fieldGetValue != nullptr)
			{
				fieldGetValue(object, field, value);
			}
		}

		void SetFieldValue(Object object, Field field, void* value) const noexcept
		{
			if (fieldSetValue != nullptr)
			{
				fieldSetValue(object, field, value);
			}
		}

		void GetStaticFieldValue(Field field, void* value) const noexcept
		{
			if (staticFieldGetValue != nullptr)
			{
				staticFieldGetValue(field, value);
			}
		}

		void SetStaticFieldValue(Field field, void* value) const noexcept
		{
			if (staticFieldSetValue != nullptr)
			{
				staticFieldSetValue(field, value);
			}
		}

		[[nodiscard]] String NewString(const std::string& value) const noexcept
		{
			if (value.size() > (std::numeric_limits<std::uint32_t>::max)())
			{
				return nullptr;
			}

			const auto length = static_cast<std::uint32_t>(value.size());
			if (stringNewLen != nullptr)
			{
				return stringNewLen(value.data(), length);
			}
			return stringNew != nullptr ? stringNew(value.c_str()) : nullptr;
		}

		[[nodiscard]] std::string StringToUtf8(String value) const
		{
			if (value == nullptr || stringChars == nullptr || stringLength == nullptr)
			{
				return "";
			}

			const char16_t* characters = stringChars(value);
			const std::int32_t length = stringLength(value);
			if (characters == nullptr || length <= 0)
			{
				return "";
			}

			const wchar_t* wideCharacters = reinterpret_cast<const wchar_t*>(characters);
			static_assert(sizeof(wchar_t) == sizeof(char16_t));
			int conversionFlags = WC_ERR_INVALID_CHARS;
			int requiredBytes = WideCharToMultiByte(
				CP_UTF8,
				conversionFlags,
				wideCharacters,
				length,
				nullptr,
				0,
				nullptr,
				nullptr);
			if (requiredBytes <= 0)
			{
				conversionFlags = 0;
				requiredBytes = WideCharToMultiByte(
					CP_UTF8,
					conversionFlags,
					wideCharacters,
					length,
					nullptr,
					0,
					nullptr,
					nullptr);
				if (requiredBytes <= 0)
				{
					return "";
				}
			}

			std::string result(static_cast<std::size_t>(requiredBytes), '\0');
			WideCharToMultiByte(
				CP_UTF8,
				conversionFlags,
				wideCharacters,
				length,
				result.data(),
				requiredBytes,
				nullptr,
				nullptr);
			return result;
		}

		[[nodiscard]] Array NewArray(Class elementClass, std::uintptr_t length) const noexcept
		{
			return arrayNew != nullptr ? arrayNew(elementClass, length) : nullptr;
		}

		[[nodiscard]] Thread AttachThread(Domain domain) const noexcept
		{
			return threadAttach != nullptr ? threadAttach(domain) : nullptr;
		}

		[[nodiscard]] Thread GetCurrentThread() const noexcept
		{
			return threadCurrent != nullptr ? threadCurrent() : nullptr;
		}

		void DetachThread(Thread thread) const noexcept
		{
			if (threadDetach != nullptr && thread != nullptr)
			{
				threadDetach(thread);
			}
		}

		[[nodiscard]] std::uint32_t NewGcHandle(Object object) const noexcept
		{
			return gcHandleNew != nullptr && object != nullptr ? gcHandleNew(object, false) : 0U;
		}

		void FreeGcHandle(const std::uint32_t handle) const noexcept
		{
			if (gcHandleFree != nullptr && handle != 0U) gcHandleFree(handle);
		}

		[[nodiscard]] Object GetGcHandleTarget(const std::uint32_t handle) const noexcept
		{
			return gcHandleGetTarget != nullptr && handle != 0U ?
				gcHandleGetTarget(handle) : nullptr;
		}
	};

	inline Api api = {};
}

#undef VM_API_SYMBOL
