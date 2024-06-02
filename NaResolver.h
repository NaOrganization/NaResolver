//**************************************//
// Hi NaResolver						//
// Author: NaOrganization				//
// Version: v2.2						//
// Branch: il2cpp						//
//**************************************//

// Change Log (Started since v1.8):
// Release v2.2:
// 1. Re-separate the version of different Game runtime, il2cpp and mono
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

#undef GetObject
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
		VmMethodInvoker(const char* symbol)
		{
			this->content = (R(*)(Args...))GetProcAddress(GetModuleHandleA(TEXT("GameAssembly.dll")), symbol);
		}
	};

	class Object
	{
	public:
		void* object = NULL;
		Object() {}
		Object(void* object) : object(object) {}
		operator void* const () { return object; }
	};

	class Type
	{
	public:
		void* type = NULL;
		Type() {}
		Type(void* type) : type(type) {}
		operator void* const () { return type; }

		std::string GetName() const;

		VmGeneralType::Object GetObject() const;
	};

	class Method
	{
	public:
		void* method = NULL;
		Method() {}
		Method(void* method) : method(method) {}
		operator void* const () { return method; }

		std::string GetName() const;

		Type GetReturnType() const;

		bool IsGeneric() const;

		std::vector<Type> GetParametersType() const;

		void* GetInvokeAddress() const;
	};

	class Field
	{
	public:
		void* fieldInfo = NULL;
		Field() {}
		Field(void* fieldInfo) : fieldInfo(fieldInfo) {}

		void GetValue(Object object, void* value);

		void SetValue(Object object, void* value);

		void GetStaticValue(void* value);

		void SetStaticValue(void* value);
	};

	class Class
	{
	public:
		void* klass = NULL;
		Class() {}
		Class(void* klass) : klass(klass) {}
		operator void* const () { return klass; }

		std::string GetName() const;

		Method GetMethods(void** iter) const;

		Field GetField(std::string name) const;

		Type GetType() const;

		std::vector<Class> GetNestedTypes(std::string className) const;
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
			static auto il2cpp_string_new = VmMethodInvoker<void*, const char*>(TEXT("il2cpp_string_new"));
			address = il2cpp_string_new(string.c_str());
		}
		operator void* const () { return address; }

		std::string ToString() const
		{
			static VmMethodInvoker<wchar_t*, void*> string_chars = TEXT("il2cpp_string_chars");
			if (!string_chars.IsValid()) return "";
			wchar_t* data = string_chars(address);
			if (!data) return "";
			std::wstring wData(data);
			return std::string(wData.begin(), wData.end());
		}

		operator std::string const () { return ToString(); }
	};

	class Array
	{
	public:
		static void* New(Class elementTypeInfo, uint32_t length);
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

	std::string Type::GetName() const
	{
		static VmMethodInvoker<const char*, void*> type_get_name = TEXT("il2cpp_type_get_name");
		return type_get_name(type);
	}

	VmGeneralType::Object Type::GetObject() const
	{
		static VmMethodInvoker<void*, void*> object_new = TEXT("il2cpp_type_get_object");
		return object_new(type);
	}

	std::string Method::GetName() const
	{
		static VmMethodInvoker<const char*, void*> method_get_name = TEXT("il2cpp_method_get_name");
		return method_get_name(method);
	}

	Type Method::GetReturnType() const
	{
		static VmMethodInvoker<void*, void*> method_get_return_type = TEXT("il2cpp_method_get_return_type");
		return method_get_return_type(method);
	}

	bool Method::IsGeneric() const
	{
		static VmMethodInvoker<bool, void*> method_is_generic = TEXT("il2cpp_method_is_generic");
		return method_is_generic(method);
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
		return types;
	}

	void* Method::GetInvokeAddress() const
	{
		return *(void**)method;
	}

	std::string Class::GetName() const
	{
		static VmMethodInvoker<const char*, void*> class_get_name = TEXT("il2cpp_class_get_name");
		return class_get_name(klass);
	}

	Method Class::GetMethods(void** iter) const
	{
		static VmMethodInvoker<void*, void*, void**> class_get_methods = TEXT("il2cpp_class_get_methods");
		return class_get_methods(klass, iter);
	}

	Field Class::GetField(std::string name) const
	{
		static VmMethodInvoker<void*, void*, const char*> class_get_field_from_name = TEXT("il2cpp_class_get_field_from_name");
		return class_get_field_from_name(klass, name.c_str());
	}

	Type Class::GetType() const
	{
		static VmMethodInvoker<void*, void*> class_get_type = TEXT("il2cpp_class_get_type");
		return class_get_type(klass);
	}

	std::vector<Class> VmGeneralType::Class::GetNestedTypes(std::string className) const
	{
		std::vector<Class> classes = {};
		static VmMethodInvoker<void*, void*, void**> class_get_nested_types = TEXT("il2cpp_class_get_nested_types");
		void* iter = NULL;
		void* nestedClass = NULL;
		while ((nestedClass = class_get_nested_types(klass, &iter)) != NULL)
		{
			classes.push_back(nestedClass);
		}
		return classes;
	}

	void Field::GetValue(Object object, void* value)
	{
		static VmMethodInvoker<void, void*, void*, void*> field_get_value = TEXT("il2cpp_field_get_value");
		field_get_value(object, fieldInfo, value);
	}

	void Field::SetValue(Object object, void* value)
	{
		static VmMethodInvoker<void, void*, void*, void*> field_set_value = TEXT("il2cpp_field_set_value");
		field_set_value(object, fieldInfo, value);
	}

	void Field::GetStaticValue(void* value)
	{
		static VmMethodInvoker<void, void*, void*> field_static_get_value = TEXT("il2cpp_field_static_get_value");
		field_static_get_value(fieldInfo, value);
	}

	void Field::SetStaticValue(void* value)
	{
		static VmMethodInvoker<void, void*, void*> field_static_set_value = TEXT("il2cpp_field_static_set_value");
		field_static_set_value(fieldInfo, value);
	}

	std::string Image::GetName() const
	{
		static VmMethodInvoker<const char*, void*> image_get_name = TEXT("il2cpp_image_get_name");
		return image_get_name(image);
	}

	Class Image::GetClassFromName(std::string namespaceName, std::string className) const
	{
		static VmMethodInvoker<void*, void*, const char*, const char*> class_from_name = TEXT("il2cpp_class_from_name");
		return class_from_name(image, namespaceName.c_str(), className.c_str());
	}

	Image Assembly::GetImage() const
	{
		static VmMethodInvoker<void*, void*> assembly_get_image = TEXT("il2cpp_assembly_get_image");
		return assembly_get_image(assembly);
	}

	Domain Domain::Get()
	{
		static VmMethodInvoker<void*> domain_get = TEXT("il2cpp_domain_get");
		return domain_get();
	}

	Assembly Domain::OpenAssembly(std::string name) const
	{
		static VmMethodInvoker<void*, void*, const char*> assembly_get = TEXT("il2cpp_domain_assembly_open");
		return assembly_get(domain, name.c_str());
	}

	void* VmGeneralType::Array::New(Class elementTypeInfo, uint32_t length)
	{
		static VmMethodInvoker<void*, void*, uint32_t> array_new = TEXT("il2cpp_array_new");
		return array_new(elementTypeInfo, length);
	}

	Thread Thread::Attach(Domain domain)
	{
		static VmMethodInvoker<void*, void*>thread_attach = TEXT("il2cpp_thread_attach");
		return alreadyAttach = thread_attach(domain);
	}

	Thread Thread::Current()
	{
		static VmMethodInvoker<void*>thread_current = TEXT("il2cpp_thread_current");
		return thread_current();
	}

	void Thread::Detach() const
	{
		static VmMethodInvoker<void, void*> thread_detach = TEXT("il2cpp_thread_detach");
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
		std::unordered_map<std::string, Class> nestedClasses = {};
		Class() {}
		Class(const Class& klass)
			: assemblyName(klass.assemblyName), namespaceName(klass.namespaceName), className(klass.className), klass(klass.klass), type(klass.type), nestedClasses(klass.nestedClasses) {}
		Class(const std::string& assemblyName, const std::string& namespaceName, const std::string& className, const VmGeneralType::Class& klass, const VmGeneralType::Type& type)
			: assemblyName(assemblyName), namespaceName(namespaceName), className(className), klass(klass), type(type) {}
		operator VmGeneralType::Class() { return klass; }
		operator VmGeneralType::Type() { return type; }
		operator bool() { return klass && type; }

		void AddNestedClass(const std::string name, const Class& klass)
		{
			nestedClasses.insert({ name, klass });
		}

		Class FindNestedClass(const std::string name) const
		{
			auto iter = nestedClasses.find(name);
			if (iter == nestedClasses.end())
				return Class();
			return iter->second;
		}
	};
	class Method
	{
	public:
		std::string returnTypeName = std::string();
		std::string methodName = std::string();
		std::vector<std::string> parametersTypeName = std::vector<std::string>();
		VmGeneralType::Method method = VmGeneralType::Method();
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

		Class& GetClass(std::string assembly, std::string nameSpace, std::string name)
		{
			return classPathMap[assembly][nameSpace][name];
		}

		void Clear();
	};
private:
	VmGeneralType::Domain domain = VmGeneralType::Domain();
	VmGeneralType::Thread thread = VmGeneralType::Thread();
	ContextCache cache = ContextCache();
public:
	bool Setup();
	void Destroy();
	Class GetClass(const std::string& assemblyName, const std::string& namespaceName, const std::string& className);
	Class GetClass(Class parent, std::string className);
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

NaResolver::Class NaResolver::GetClass(const std::string& assemblyName, const std::string& namespaceName, const std::string& className)
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

NaResolver::Class NaResolver::GetClass(Class parent, std::string className)
{
	if (!parent)
	{
		return NaResolver::Class();
	}
	Class& parentReference = cache.GetClass(parent.assemblyName, parent.namespaceName, parent.className);
	if (!parentReference)
	{
		return NaResolver::Class();
	}
	if (parentReference.nestedClasses.empty())
	{
		std::vector<VmGeneralType::Class> nestedClasses = parentReference.klass.GetNestedTypes(className);
		if (nestedClasses.empty())
		{
			return NaResolver::Class();
		}
		for (VmGeneralType::Class nestedClass : nestedClasses)
		{
			Class klass = Class(parentReference.assemblyName, parentReference.namespaceName, nestedClass.GetName(), nestedClass, nestedClass.GetType());
			parentReference.AddNestedClass(klass.className, klass);
		}
	}
	return parentReference.FindNestedClass(className);
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
			if (parameterType != parametersTypeName[i] && parameterType == TEXT("_AUTO_"))
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
#define NESTED_CLASS(parent, className) \
	static NaResolver::Class ThisClass() \
	{ \
		return naResolverInstance.GetClass(parent::ThisClass(), className); \
	}

#define GET_FIELDT(name) ThisClass().klass.GetField(NA_RESOLVER_STRING_XOR(#name))
#define GET_BACKING_FIELD(name) ThisClass().klass.GetField(NA_RESOLVER_STRING_XOR("<" #name ">k__BackingField"))

#define FIELD_GETTER(type, name) \
	type get_##name() \
	{ \
		static VmGeneralType::Field field = GET_FIELDT(name); \
		type value = {}; \
		field.GetValue((VmGeneralType::Object)(void*)this, &value); \
		return value; \
	}
#define STR_FIELD_GETTER(name) \
	std::string get_##name()\
	{\
		static VmGeneralType::Field field = GET_FIELDT(name); \
		void* value; \
		field.GetValue((VmGeneralType::Object)(void*)this, &value); \
		return ((VmGeneralType::String)value);\
	}
#define BACKING_FIELD_GETTER(type, name)\
	type get_backingField_##name()\
	{\
		static VmGeneralType::Field field = GET_BACKING_FIELD(name); \
		type value = {}; \
		field.GetValue((VmGeneralType::Object)(void*)this, &value); \
		return value; \
	}
#define BACKING_STR_FIELD_GETTER(name)\
	std::string get_backingField_##name()\
	{\
		static VmGeneralType::Field field = GET_BACKING_FIELD(name); \
		void* value; \
		field.GetValue((VmGeneralType::Object)(void*)this, &value); \
		return ((VmGeneralType::String)value);\
	}

#define FIELD_SETTER(type, name) \
	void set_##name(type value) \
	{ \
		static VmGeneralType::Field field = GET_FIELDT(name); \
		field.SetValue((VmGeneralType::Object)(void*)this, &value); \
	}
#define STR_FIELD_SETTER(name) \
	void set_##name(VmGeneralType::String value)\
	{\
		static VmGeneralType::Field field = GET_FIELDT(name); \
		field.SetValue((VmGeneralType::Object)(void*)this, value.address);\
	}
#define BACKING_FIELD_SETTER(type, name)\
	void set_backingField_##name(type value)\
	{\
		static VmGeneralType::Field field = GET_BACKING_FIELD(name); \
		field.SetValue((VmGeneralType::Object)(void*)this, &value); \
	}
#define BACKING_STR_FIELD_SETTER(name)\
	void set_backingField_##name(VmGeneralType::String value)\
	{\
		static VmGeneralType::Field field = GET_BACKING_FIELD(name); \
		field.SetValue((VmGeneralType::Object)(void*)this, value.address);\
	}

#define FIELD(type, name) \
	FIELD_GETTER(type, name) \
	FIELD_SETTER(type, name)
#define STR_FIELD(name) \
	STR_FIELD_GETTER(name) \
	STR_FIELD_SETTER(name)
#define BACKING_FIELD(type, name) \
	BACKING_FIELD_GETTER(type, name) \
	BACKING_FIELD_SETTER(type, name)
#define BACKING_STR_FIELD(name) \
	BACKING_STR_FIELD_GETTER(name) \
	BACKING_STR_FIELD_SETTER(name)

#define STATIC_FIELD_GETTER(type, name) \
	static type get_##name() \
	{ \
		static VmGeneralType::Field field = GET_FIELDT(name); \
		type value = {}; \
		field.GetStaticValue(&value); \
		return value; \
	}
#define STATIC_STR_FIELD_GETTER(name) \
	static std::string get_##name()\
	{\
		static VmGeneralType::Field field = GET_FIELDT(name); \
		void* value; \
		field.GetStaticValue(&value); \
		return ((VmGeneralType::String)value);\
	}
#define STATIC_BACKING_FIELD_GETTER(type, name)\
	static type get_backingField_##name()\
	{\
		static VmGeneralType::Field field = GET_BACKING_FIELD(name); \
		type value = {}; \
		field.GetStaticValue(&value); \
		return value; \
	}
#define STATIC_BACKING_STR_FIELD_GETTER(name)\
	static std::string get_backingField_##name()\
	{\
		static VmGeneralType::Field field = GET_BACKING_FIELD(name); \
		void* value; \
		field.GetStaticValue(&value); \
		return ((VmGeneralType::String)value);\
	}
#define STATIC_FIELD_SETTER(type, name) \
	static void set_##name(type value) \
	{ \
		static VmGeneralType::Field field = GET_FIELDT(name); \
		field.SetStaticValue(&value); \
	}
#define STATIC_STR_FIELD_SETTER(name) \
	static void set_##name(VmGeneralType::String value)\
	{\
		static VmGeneralType::Field field = GET_FIELDT(name); \
		field.SetStaticValue(value.address);\
	}
#define STATIC_BACKING_FIELD_SETTER(type, name)\
	static void set_backingField_##name(type value)\
	{\
		static VmGeneralType::Field field = GET_BACKING_FIELD(name); \
		field.SetStaticValue(&value); \
	}
#define STATIC_BACKING_STR_FIELD_SETTER(name)\
	static void set_backingField_##name(VmGeneralType::String value)\
	{\
		static VmGeneralType::Field field = GET_BACKING_FIELD(name); \
		field.SetStaticValue(value.address);\
	}
#define STATIC_FIELD(type, name) \
	STATIC_FIELD_GETTER(type, name) \
	STATIC_FIELD_SETTER(type, name)
#define STATIC_STR_FIELD(name) \
	STATIC_STR_FIELD_GETTER(name) \
	STATIC_STR_FIELD_SETTER(name)
#define STATIC_BACKING_FIELD(type, name) \
	STATIC_BACKING_FIELD_GETTER(type, name) \
	STATIC_BACKING_FIELD_SETTER(type, name)
#define STATIC_BACKING_STR_FIELD(name) \
	STATIC_BACKING_STR_FIELD_GETTER(name) \
	STATIC_BACKING_STR_FIELD_SETTER(name)
#define METHOD_ADDRESS(returnType, methodName, ...) naResolverInstance.GetMethod(ThisClass(), returnType, methodName, { __VA_ARGS__ }).method.GetInvokeAddress()
#define METHOD_ADDRESS_WITH_CLASS(parent, returnType, methodName, ...) naResolverInstance.GetMethod(parent::ThisClass(), returnType, methodName, { __VA_ARGS__ }).method.GetInvokeAddress()

#undef TEXT
#endif // !H_NARESOLVER