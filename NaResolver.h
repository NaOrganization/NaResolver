//**************************************//
// Hi NaResolver						//
// Author: MidTerm						//
// Version: v2.1.3						//
//**************************************//

// Change Log (Started since v1.8):
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
// 2. Support the mono and il2cpp in only one version
// 3. Remove that enforce cpp version requirements
// 4. Remove the structure of signature
// Release v1.8:
// 1. Add pre-register mechanism, all class, method and field must register
// 2. Add a exception class
//

#undef GetClassName

#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <codecvt>

#ifndef H_NARESOLVER
#define H_NARESOLVER

#ifndef NA_RESOLVER_STRING_XOR
#define NA_RESOLVER_STRING_XOR
#endif

#undef FIELD_OFFSET
#undef RegisterClass
#undef TEXT

#ifdef _HAS_CXX17
#define CXX17_INLINE inline
#else
#define CXX17_INLINE
#endif

#define TEXT NA_RESOLVER_STRING_XOR

template<typename R, typename ...Args>
class NaMethodInvoker
{
public:
	using functionPtrType = R(*)(Args...);
	functionPtrType content = {};

	NaMethodInvoker() {}
	NaMethodInvoker(void* address) : content((functionPtrType)address) {}

	template<typename T = R,
		typename std::enable_if<!std::is_void<T>::value, int>::type = 0>
	R Invoke(Args ... arg) const
	{
		R result = {};
		if (content != nullptr)
		{
			result = content(arg...);
		}
		return result;
	}

	template<typename T = R,
		typename std::enable_if<std::is_void<T>::value, int>::type = 0>
	void Invoke(Args ... arg) const
	{
		if (content != nullptr)
		{
			content(arg...);
		}
	}

	R operator()(Args ... arg) const
	{
		return Invoke(arg...);
	}

	bool IsValid() const
	{
		return content != nullptr;
	}
};

namespace VmGeneralType
{
	template<typename R, typename ...Args>
	class VmMethodInvoker : public NaMethodInvoker<R, Args...>
	{
	public:
		VmMethodInvoker(std::string symbol)
		{
			this->content = (R(*)(Args...))GetProcAddress(GetModule(), symbol.c_str());
		}
		VmMethodInvoker(std::string il2CppSymbol, std::string monoSymbol)
		{
			this->content = (R(*)(Args...))GetProcAddress(GetModule(), il2CppSymbol.c_str());
			if (this->content == nullptr)
			{
				this->content = (R(*)(Args...))GetProcAddress(GetModule(), monoSymbol.c_str());
			}
		}

		static HMODULE GetModule();
	};

	class Type
	{
	public:
		void* type = NULL;
		Type() {}
		Type(void* type) : type(type) {}
		operator void* const () { return type; }

		std::string GetName() const;
	};

	class Method
	{
	public:
		void* method = NULL;
		Method() {}
		Method(void* method) : method(method) {}
		operator void* const () { return method; }

		void* GetSignatureForMono() const;

		std::string GetName() const;

		Type GetReturnType() const;

		bool IsGeneric() const;

		std::vector<Type> GetParametersType() const;

		void* GetInvokeAddress() const;

		void* RuntimeInvoke(void* obj, void** params, void** exc) const;
	};

	class Field
	{
	public:
		void* fieldInfo = NULL;
		Field() {}
		Field(void* fieldInfo) : fieldInfo(fieldInfo) {}

		uint32_t GetOffset() const;
	};

	class Class
	{
	public:
		void* klass = NULL;
		Class() {}
		Class(void* klass) : klass(klass) {}
		operator void* const () { return klass; }

		Method GetMethods(void** iter) const;

		Field GetField(std::string name) const;

		Type GetType() const;
	};

	class Image
	{
	public:
		void* image = NULL;
		Image() {}
		Image(void* image) : image(image) {}
		operator void* const () { return image; }

		std::string GetName() const;

		Class GetClassFromName(std::string namespaceName, std::string className) const;
	};

	class Assembly
	{
	public:
		void* assembly = nullptr;
		Assembly() {}
		Assembly(void* assembly) : assembly(assembly) {}
		operator void* const () { return assembly; }

		Image GetImage() const;
	};

	class Domain
	{
	public:
		void* domain = nullptr;
		Domain() {}
		Domain(void* domain) : domain(domain) {}
		operator void* const () { return domain; }

		static Domain Get();

		Assembly OpenAssembly(std::string name) const;
	};

	class String
	{
	public:
		void* address = nullptr;
		String() {}
		String(void* address) : address(address) {}
		String(std::string string)
		{
			// il2cpp_string_new(x), mono_string_new(domain, x)
			static auto il2cpp_string_new = VmMethodInvoker<void*, const char*>(TEXT("il2cpp_string_new"));
			if (il2cpp_string_new.IsValid())
			{
				address = il2cpp_string_new(string.c_str());
			}
			else
			{
				static auto mono_string_new = VmMethodInvoker<void*, void*, const char*>(TEXT("mono_string_new"));
				address = mono_string_new(Domain::Get(), string.c_str());
			}
		}
		operator void* const () { return address; }

		std::string ToString() const
		{
			static auto string_chars = VmMethodInvoker<wchar_t*, void*>(TEXT("il2cpp_string_chars"), TEXT("mono_string_chars"));
			if (!string_chars.IsValid()) return "";
			wchar_t* data = string_chars(address);
			if (!data) return "";
			std::wstring wData(data);
			return std::string(wData.begin(), wData.end());
		}

		operator std::string const () { return ToString(); }
	};

	class Thread
	{
	public:
		CXX17_INLINE static void* alreadyAttach;
		void* thread = NULL;
		Thread() {}
		Thread(void* thread) : thread(thread) {}
		operator void* const () { return thread; }

		static Thread Attach(Domain domain);

		static Thread Current();

		void Detach() const;
	};

	template<typename R, typename ...Args>
	HMODULE VmMethodInvoker<R, Args...>::GetModule()
	{
		static HMODULE moduleHandle = NULL;
		if (moduleHandle != NULL)
			return moduleHandle;
		moduleHandle = GetModuleHandleA(TEXT("GameAssembly.dll"));
		if (moduleHandle != NULL)
			return moduleHandle;
		moduleHandle = GetModuleHandleA(TEXT("mono-2.0-bdwgc.dll"));
		if (moduleHandle != NULL)
			return moduleHandle;
		return moduleHandle;
	}

	std::string Type::GetName() const
	{
		static auto type_get_name = VmMethodInvoker<const char*, void*>(TEXT("il2cpp_type_get_name"), TEXT("mono_type_get_name"));
		return type_get_name(type);
	}

	void* Method::GetSignatureForMono() const
	{
		static auto method_get_signature = VmMethodInvoker<void*, void*>(TEXT("mono_method_signature"));
		return method_get_signature(method);
	}

	std::string Method::GetName() const
	{
		static auto method_get_name = VmMethodInvoker<const char*, void*>(TEXT("il2cpp_method_get_name"), TEXT("mono_method_get_name"));
		return method_get_name(method);
	}

	Type Method::GetReturnType() const
	{
		static auto method_get_return_type = VmMethodInvoker<void*, void*>(TEXT("il2cpp_method_get_return_type"));
		if (method_get_return_type.IsValid())
		{
			return method_get_return_type(method);
		}
		static auto mono_signature_get_return_type = VmMethodInvoker<void*, void*>(TEXT("mono_signature_get_return_type"));
		if (mono_signature_get_return_type.IsValid())
		{
			return mono_signature_get_return_type(GetSignatureForMono());
		}
		return Type();
	}

	bool Method::IsGeneric() const
	{
		static auto method_is_generic = VmMethodInvoker<bool, void*>(TEXT("il2cpp_method_is_generic"));
		if (method_is_generic.IsValid())
		{
			return method_is_generic(method);
		}
		static auto mono_signature_is_generic = VmMethodInvoker<bool, void*>(TEXT("mono_signature_is_generic"));
		if (mono_signature_is_generic.IsValid())
		{
			return mono_signature_is_generic(GetSignatureForMono());
		}
		return false;
	}

	std::vector<Type> Method::GetParametersType() const
	{
		std::vector<Type> types = {};
		static auto method_get_param_count = VmMethodInvoker<uint32_t, void*>(TEXT("il2cpp_method_get_param_count"));
		static auto method_get_param = VmMethodInvoker<void*, void*, uint32_t>(TEXT("il2cpp_method_get_param"));
		if (method_get_param_count.IsValid() && method_get_param.IsValid())
		{
			uint32_t count = method_get_param_count(method);
			for (uint32_t i = 0; i < count; i++)
			{
				types.push_back(method_get_param(method, i));
			}
		}
		static auto mono_signature_get_params = VmMethodInvoker<void*, void*, void*>(TEXT("mono_signature_get_params"));
		if (mono_signature_get_params.IsValid())
		{
			void* iter = NULL;
			void* param = NULL;
			while ((param = mono_signature_get_params(GetSignatureForMono(), &iter)) != NULL)
			{
				types.push_back(param);
			}
		}
		return types;
	}

	void* Method::GetInvokeAddress() const
	{
		static auto mono_compile_method = VmMethodInvoker<void*, void*>(TEXT("mono_compile_method"));
		try
		{
			if (mono_compile_method.IsValid())
				return mono_compile_method(method);
		}
		catch (...)
		{
			return 0;
		}
		return *(void**)method;
	}

	void* Method::RuntimeInvoke(void* obj, void** params, void** exc) const
	{
		static auto mono_runtime_invoke = VmMethodInvoker<void*, void*, void*, void**, void**>(TEXT("mono_runtime_invoke"));
		return mono_runtime_invoke(method, obj, params, exc);
	}

	Method Class::GetMethods(void** iter) const
	{
		static auto class_get_methods = VmMethodInvoker<void*, void*, void**>(TEXT("il2cpp_class_get_methods"), TEXT("mono_class_get_methods"));
		return class_get_methods(klass, iter);
	}

	Field Class::GetField(std::string name) const
	{
		static auto class_get_field_from_name = VmMethodInvoker<void*, void*, const char*>(TEXT("il2cpp_class_get_field_from_name"), TEXT("mono_class_get_field_from_name"));
		return class_get_field_from_name(klass, name.c_str());
	}

	Type Class::GetType() const
	{
		static auto class_get_type = VmMethodInvoker<void*, void*>(TEXT("il2cpp_class_get_type"), TEXT("mono_class_get_type"));
		return class_get_type(klass);
	}

	uint32_t Field::GetOffset() const
	{
		static auto field_get_offset = VmMethodInvoker<uint32_t, void*>(TEXT("il2cpp_field_get_offset"), TEXT("mono_field_get_offset"));
		return field_get_offset(fieldInfo);
	}

	std::string Image::GetName() const
	{
		static auto image_get_name = VmMethodInvoker<const char*, void*>(TEXT("il2cpp_image_get_name"), TEXT("mono_image_get_name"));
		return image_get_name(image);
	}

	Class Image::GetClassFromName(std::string namespaceName, std::string className) const
	{
		static auto class_from_name = VmMethodInvoker<void*, void*, const char*, const char*>(TEXT("il2cpp_class_from_name"), TEXT("mono_class_from_name"));
		return class_from_name(image, namespaceName.c_str(), className.c_str());
	}

	Image Assembly::GetImage() const
	{
		static auto assembly_get_image = VmMethodInvoker<void*, void*>(TEXT("il2cpp_assembly_get_image"), TEXT("mono_assembly_get_image"));
		return assembly_get_image(assembly);
	}

	Domain Domain::Get()
	{
		static auto domain_get = VmMethodInvoker<void*>(TEXT("il2cpp_domain_get"), TEXT("mono_get_root_domain"));
		return domain_get();
	}

	Assembly Domain::OpenAssembly(std::string name) const
	{
		static auto assembly_get = VmMethodInvoker<void*, void*, const char*>(TEXT("il2cpp_domain_assembly_open"), TEXT("mono_domain_assembly_open"));
		return assembly_get(domain, name.c_str());
	}

	Thread Thread::Attach(Domain domain)
	{
		static auto thread_attach = VmMethodInvoker<void*, void*>(TEXT("il2cpp_thread_attach"), TEXT("mono_thread_attach"));
		return alreadyAttach = thread_attach(domain);
	}

	Thread Thread::Current()
	{
		static auto thread_current = VmMethodInvoker<void*>(TEXT("il2cpp_thread_current"), TEXT("mono_thread_current"));
		return thread_current();
	}

	void Thread::Detach() const
	{
		static auto thread_detach = VmMethodInvoker<void, void*>(TEXT("il2cpp_thread_detach"), TEXT("mono_thread_detach"));
		thread_detach(thread);
	}
}

class NaResolver
{
public:
	class Class
	{
	public:
		std::string assemblyName = std::string();
		std::string namespaceName = std::string();
		std::string className = std::string();
		VmGeneralType::Class klass = VmGeneralType::Class();
		VmGeneralType::Type type = VmGeneralType::Type();
		Class() {}
		Class(std::string assemblyName, std::string namespaceName, std::string className, VmGeneralType::Class klass, VmGeneralType::Type type) : assemblyName(assemblyName), namespaceName(namespaceName), className(className), klass(klass), type(type) {}
		operator VmGeneralType::Class() { return klass; }
		operator VmGeneralType::Type() { return type; }
		operator bool() { return klass && type; }
	};
	class Method
	{
	public:
		std::string returnTypeName = std::string();
		std::string methodName = std::string();
		std::vector<std::string> parametersTypeName = std::vector<std::string>();
		VmGeneralType::Method method = VmGeneralType::Method();
		template<typename ...Args>
		void* PackArgs(Args ...args)
		{
			const int argsSize = sizeof...(Args);
			void* argsArray[argsSize] = { &args... };
			if (argsSize != 0)
			{
				int index = 0;
				std::vector<void*> argsVector = { &args... };
				for (int i = 0; i < argsSize; i++)
				{
					if (argsVector[i] == nullptr)
						continue;
					if (argsVector[i] == nullptr)
						continue;
					if (parametersTypeName[i] == "System.String")
					{
						argsArray[i] = VmGeneralType::String(*(std::string*)argsVector[i]).address;
						continue;
					}
					argsArray[i] = argsVector[i];
				}
			}
			return argsArray;
		}
		Method() {}
		Method(std::string returnTypeName, std::string methodName, std::vector<std::string> parametersTypeName, VmGeneralType::Method method) : returnTypeName(returnTypeName), methodName(methodName), parametersTypeName(parametersTypeName), method(method) {}
		operator VmGeneralType::Method() { return method; }
		operator bool() { return method; }
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

		void RegisterAssembly(std::string name, VmGeneralType::Assembly assembly);

		Class RegisterClass(std::string assembly, std::string nameSpace, std::string name, VmGeneralType::Class klass, VmGeneralType::Type type);

		VmGeneralType::Assembly GetAssembly(std::string name) const;

		Class GetClass(std::string assembly, std::string nameSpace, std::string name) const;

		void Clear();
	};
private:
	VmGeneralType::Domain domain = VmGeneralType::Domain();
	VmGeneralType::Thread thread = VmGeneralType::Thread();
	ContextCache cache = ContextCache();
public:
	bool Setup();
	void Destroy();
	Class GetClass(std::string assemblyName, std::string namespaceName, std::string className);
	Method GetMethod(Class parent, std::string returnTypeName, std::string methodName, std::vector<std::string> parametersTypeName);
};

CXX17_INLINE NaResolver naResolverInstance = NaResolver();

void NaResolver::ContextCache::RegisterAssembly(std::string name, VmGeneralType::Assembly assembly)
{
	assemblyMap[name] = assembly;
}

NaResolver::Class NaResolver::ContextCache::RegisterClass(std::string assembly, std::string nameSpace, std::string name, VmGeneralType::Class klass, VmGeneralType::Type type)
{
	return classPathMap[assembly][nameSpace][name] = Class(assembly, nameSpace, name, klass, type);
}

VmGeneralType::Assembly NaResolver::ContextCache::GetAssembly(std::string name) const
{
	auto iter = assemblyMap.find(name);
	if (iter == assemblyMap.end())
		return VmGeneralType::Assembly();
	return iter->second;
}

NaResolver::Class NaResolver::ContextCache::GetClass(std::string assembly, std::string nameSpace, std::string name) const
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

void NaResolver::ContextCache::Clear()
{
	assemblyMap.clear();
	classPathMap.clear();
}

bool NaResolver::Setup()
{
	domain = VmGeneralType::Domain::Get();
	if (domain == nullptr)
	{
		return false;
	}
	thread = VmGeneralType::Thread::Attach(domain);
	if (thread == nullptr)
	{
		return false;
	}
	return true;
}

void NaResolver::Destroy()
{
	thread.Detach();
	cache.Clear();
}

NaResolver::Class NaResolver::GetClass(std::string assemblyName, std::string namespaceName, std::string className)
{
	Class result = cache.GetClass(assemblyName, namespaceName, className);
	if (result)
		return result;
	VmGeneralType::Assembly assembly = cache.GetAssembly(assemblyName);
	if (!assembly)
	{
		assembly = domain.OpenAssembly(assemblyName);
		if (!assembly)
		{
			return Class();
		}
		cache.RegisterAssembly(assemblyName, assembly);
	}
	VmGeneralType::Image image = assembly.GetImage();
	if (!image)
	{
		return Class();
	}
	VmGeneralType::Class klass = image.GetClassFromName(namespaceName, className);
	if (!klass)
	{
		return Class();
	}
	return result = cache.RegisterClass(assemblyName, namespaceName, className, klass, klass.GetType());
}

NaResolver::Method NaResolver::GetMethod(Class parent, std::string returnTypeName, std::string methodName, std::vector<std::string> parametersTypeName)
{
	if (!parent)
	{
		return NaResolver::Method();
	}
	void* iterator = nullptr;
	VmGeneralType::Method method = nullptr;
	while ((method = parent.klass.GetMethods(&iterator)) != nullptr)
	{
		std::string name = method.GetName();
		if (name != methodName && name != TEXT("_AUTO_"))
			continue;
		std::string returnType = method.GetReturnType().GetName();
		if (returnType != returnTypeName && returnType != TEXT("_AUTO_"))
			continue;
		std::vector<VmGeneralType::Type> parametersType = method.GetParametersType();
		if (parametersType.size() != parametersTypeName.size())
			continue;
		bool isMatch = true;
		for (size_t i = 0; i < parametersType.size(); i++)
		{
			std::string parameterType = parametersType[i].GetName();
			if (parameterType != parametersTypeName[i] && parameterType != TEXT("_AUTO_"))
			{
				isMatch = false;
				break;
			}
		}
		if (!isMatch)
			continue;
		return Method(returnType, name, parametersTypeName, method);
	}
	return NaResolver::Method();
}

#define CLASS(assemblyName,namespaceName,className) \
	static NaResolver::Class ThisClass() \
	{ \
		return naResolverInstance.GetClass(assemblyName, namespaceName, className); \
	}

#define FIELD_OFFSET(name) ThisClass().klass.GetField(#name).GetOffset()
#define BACKING_FIELD_OFFSET(name) ThisClass().klass.GetField(NA_RESOLVER_STRING_XOR("<" #name ">k__BackingField")).GetOffset()

#define FIELD_GET(type, name) \
	type get_##name() \
	{ \
		static int32_t memberOffset = FIELD_OFFSET(name); \
		return *(type*)((uint8_t*)this + memberOffset); \
	}
#define STR_FIELD_GET(name) \
	std::string get_##name()\
	{\
		static int32_t memberOffset = FIELD_OFFSET(name); \
		return VmGeneralType::String(*(void**)((uint8_t*)this + memberOffset));\
	}
#define BACKING_FIELD_GET(type, name)\
	type get_backingField_##name()\
	{\
		static int32_t memberOffset = BACKING_FIELD_OFFSET(name); \
		return *(type*)((uint8_t*)this + memberOffset); \
	}
#define BACKING_STR_FIELD_GET(name)\
	std::string get_backingField_##name()\
	{\
		static int32_t memberOffset = BACKING_FIELD_OFFSET(name); \
		return VmGeneralType::String(*(void**)((uint8_t*)this + memberOffset));\
	}

#define FIELD_SET(type, name) \
	void set_##name(type value) \
	{ \
		static int32_t memberOffset = FIELD_OFFSET(name); \
		*(type*)((uint8_t*)this + memberOffset) = value; \
	}
#define STR_FIELD_SET(name) \
	void set_##name(VmGeneralType::String value)\
	{\
		static int32_t memberOffset = FIELD_OFFSET(name); \
		*(void**)((uint8_t*)this + memberOffset) = value;\
	}
#define BACKING_FIELD_SET(type, name)\
	void set_backingField_##name(type value)\
	{\
		static int32_t memberOffset = BACKING_FIELD_OFFSET(name); \
		*(type*)((uint8_t*)this + memberOffset) = value; \
	}
#define BACKING_STR_FIELD_SET(name)\
	void set_backingField_##name(VmGeneralType::String value)\
	{\
		static int32_t memberOffset = BACKING_FIELD_OFFSET(name); \
		*(void**)((uint8_t*)this + memberOffset) = value;\
	}

#define FIELD(type, name) \
	FIELD_GET(type, name) \
	FIELD_SET(type, name)
#define STR_FIELD(name) \
	STR_FIELD_GET(name) \
	STR_FIELD_SET(name)
#define BACKING_FIELD(type, name) \
	BACKING_FIELD_GET(type, name) \
	BACKING_FIELD_SET(type, name)
#define BACKING_STR_FIELD(name) \
	BACKING_STR_FIELD_GET(name) \
	BACKING_STR_FIELD_SET(name)

#define STATIC_AREA_OFFSET (sizeof(void *) * 0x17)

#define STATIC_FIELD_GET(type, name) \
	static type get_##name() \
	{ \
		static int32_t memberOffset = FIELD_OFFSET(name); \
		uintptr_t staticAreaAddress = *(uintptr_t*)((uint8_t*)ThisClass().klass.klass + STATIC_AREA_OFFSET); \
		if (staticAreaAddress == NULL) \
			return {}; \
		return *(type*)(staticAreaAddress + memberOffset); \
	}
#define STATIC_STR_FIELD_GET(name) \
	static std::string get_##name()\
	{\
		static int32_t memberOffset = FIELD_OFFSET(name); \
		uintptr_t staticAreaAddress = *(uintptr_t*)((uint8_t*)ThisClass().klass.klass + STATIC_AREA_OFFSET); \
		if (staticAreaAddress == NULL) \
			return {}; \
		return VmGeneralType::String(*(void**)(staticAreaAddress + memberOffset));\
	}
#define STATIC_BACKING_FIELD_GET(type, name)\
	static type get_backingField_##name()\
	{\
		static int32_t memberOffset = BACKING_FIELD_OFFSET(name); \
		uintptr_t staticAreaAddress = *(uintptr_t*)((uint8_t*)ThisClass().klass.klass + STATIC_AREA_OFFSET); \
		if (staticAreaAddress == NULL) \
			return {}; \
		return *(type*)(staticAreaAddress + memberOffset); \
	}
#define STATIC_BACKING_STR_FIELD_GET(name)\
	static std::string get_backingField_##name()\
	{\
		static int32_t memberOffset = BACKING_FIELD_OFFSET(name); \
		uintptr_t staticAreaAddress = *(uintptr_t*)((uint8_t*)ThisClass().klass.klass + STATIC_AREA_OFFSET); \
		if (staticAreaAddress == NULL) \
			return {}; \
		return VmGeneralType::String(*(void**)(staticAreaAddress + memberOffset));\
	}
#define STATIC_FIELD_SET(type, name) \
	static void set_##name(type value) \
	{ \
		static int32_t memberOffset = FIELD_OFFSET(name); \
		uintptr_t staticAreaAddress = *(uintptr_t*)((uint8_t*)ThisClass().klass .klass+ STATIC_AREA_OFFSET); \
		if (staticAreaAddress == NULL) \
			return; \
		*(type*)(staticAreaAddress + memberOffset) = value; \
	}
#define STATIC_STR_FIELD_SET(name) \
	static void set_##name(VmGeneralType::String value)\
	{\
		static int32_t memberOffset = FIELD_OFFSET(name); \
		uintptr_t staticAreaAddress = *(uintptr_t*)((uint8_t*)ThisClass().klass.klass + STATIC_AREA_OFFSET); \
		if (staticAreaAddress == NULL) \
			return; \
		*(void**)(staticAreaAddress + memberOffset) = value;\
	}
#define STATIC_BACKING_FIELD_SET(type, name)\
	static void set_backingField_##name(type value)\
	{\
		static int32_t memberOffset = BACKING_FIELD_OFFSET(name); \
		uintptr_t staticAreaAddress = *(uintptr_t*)((uint8_t*)ThisClass().klass.klass + STATIC_AREA_OFFSET); \
		if (staticAreaAddress == NULL) \
			return; \
		*(type*)(staticAreaAddress + memberOffset) = value; \
	}
#define STATIC_BACKING_STR_FIELD_SET(name)\
	static void set_backingField_##name(VmGeneralType::String value)\
	{\
		static int32_t memberOffset = BACKING_FIELD_OFFSET(name); \
		uintptr_t staticAreaAddress = *(uintptr_t*)((uint8_t*)ThisClass().klass.klass + STATIC_AREA_OFFSET); \
		if (staticAreaAddress == NULL) \
			return; \
		*(void**)(staticAreaAddress + memberOffset) = value;\
	}
#define STATIC_FIELD(type, name) \
	STATIC_FIELD_GET(type, name) \
	STATIC_FIELD_SET(type, name)
#define STATIC_STR_FIELD(name) \
	STATIC_STR_FIELD_GET(name) \
	STATIC_STR_FIELD_SET(name)
#define STATIC_BACKING_FIELD(type, name) \
	STATIC_BACKING_FIELD_GET(type, name) \
	STATIC_BACKING_FIELD_SET(type, name)
#define STATIC_BACKING_STR_FIELD(name) \
	STATIC_BACKING_STR_FIELD_GET(name) \
	STATIC_BACKING_STR_FIELD_SET(name)
#define METHOD_ADDRESS(returnType, methodName, ...) naResolverInstance.GetMethod(ThisClass(), returnType, methodName, { __VA_ARGS__ }).method.GetInvokeAddress()
#define METHOD_ADDRESS_WITH_CLASS(parent, returnType, methodName, ...) naResolverInstance.GetMethod(parent::ThisClass(), returnType, methodName, { __VA_ARGS__ }).method.GetInvokeAddress()

#undef TEXT
#endif // !H_NARESOLVER
