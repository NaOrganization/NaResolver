//**************************************//
// Hi NaResolver						//
// Author: NaOrganization				//
// Version: v3.1.2						//
// Branch: il2cpp						//
//**************************************//

// Change Log (Started since v1.8):
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
#ifndef H_NARESOLVER
#define H_NARESOLVER
#include <Windows.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <codecvt>

#pragma warning(disable:4715)

#ifndef NA_RESOLVER_STRING_XOR
#define NA_RESOLVER_STRING_XOR
#endif

#ifndef NA_RESOLVER_API_DLL_NAME
#define NA_RESOLVER_API_DLL_NAME "GameAssembly.dll"
#endif

#undef GetObject
#undef RegisterClass
#undef TEXT

#ifndef _HAS_CXX20
#error "This library requires C++20 standard"
#endif

#define TEXT NA_RESOLVER_STRING_XOR

namespace VmGeneralType
{
	class Object
	{
	public:
		void* object = nullptr;
		Object() {}
		Object(void* object) : object(object) {}
		operator void* const () { return object; }
	};

	class Type
	{
	public:
		void* type = nullptr;
		Type() {}
		Type(void* type) : type(type) {}
		operator void* const () { return type; }

		std::string GetName() const;

		VmGeneralType::Object GetObject() const;
	};

	class Method
	{
	public:
		void* method = nullptr;
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
		void* fieldInfo = nullptr;
		Field() {}
		Field(void* fieldInfo) : fieldInfo(fieldInfo) {}
		operator void* const () { return fieldInfo; }

		void GetValue(Object object, void* value);

		void SetValue(Object object, void* value);

		void GetStaticValue(void* value);

		void SetStaticValue(void* value);
	};

	class Class
	{
	public:
		void* klass = nullptr;
		Class() {}
		Class(void* klass) : klass(klass) {}
		operator void* const () { return klass; }

		std::string GetName() const;

		Method GetMethods(void** iter) const;

		Field GetField(const std::string& name) const;

		Type GetType() const;

		std::vector<Class> GetNestedTypes() const;
	};

	class Image
	{
	public:
		void* image = nullptr;
		Image() {}
		Image(void* image) : image(image) {}
		operator void* const () { return image; }

		std::string GetName() const;

		Class GetClassFromName(const std::string& namespaceName, const std::string& className) const;
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

		Assembly OpenAssembly(const std::string& name) const;
	};

	class String
	{
	public:
		void* address = nullptr;
		String() {}
		String(void* address) : address(address) {}
		String(const std::string& string) { *this = New(string); }
		operator void* const () { return address; }
		operator std::string const () { return ToString(); }

		static String New(const std::string& string);

		std::string ToString() const;
	};

	class Array
	{
	public:
		void* address = nullptr;
		Array() {}
		Array(void* address) : address(address) {}
		operator void* const () { return address; }

		static Array New(Class elementTypeInfo, uint32_t length);
	};

	class Thread
	{
	public:
		void* thread = nullptr;
		Thread() {}
		Thread(void* thread) : thread(thread) {}
		operator void* const () { return thread; }

		static Thread Attach(Domain domain);

		static Thread Current();

		void Detach() const;
	};
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

		void AddNestedClass(const std::string name, const Class& klass);

		Class FindNestedClass(const std::string name) const;
	};
	class Method
	{
	public:
		std::string returnTypeName = std::string();
		std::string methodName = std::string();
		std::vector<std::string> parametersTypeName = std::vector<std::string>();
		VmGeneralType::Method method = VmGeneralType::Method();
		Method() {}
		Method(const std::string& returnTypeName, const std::string& methodName, std::vector<std::string> parametersTypeName, VmGeneralType::Method method) : returnTypeName(returnTypeName), methodName(methodName), parametersTypeName(parametersTypeName), method(method) {}
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

		void RegisterAssembly(const std::string& name, VmGeneralType::Assembly assembly);

		Class RegisterClass(const std::string& assembly, const std::string& nameSpace, const std::string& name, VmGeneralType::Class klass, VmGeneralType::Type type);

		VmGeneralType::Assembly GetAssembly(const std::string& name) const;

		Class GetClass(const std::string& assembly, const std::string& nameSpace, const std::string& name) const;

		Class& GetClass(const std::string& assembly, const std::string& nameSpace, const std::string& name);

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
	Class GetClass(Class parent, const std::string& className);
	Method GetMethod(Class parent, const std::string& returnTypeName, const std::string& methodName, const std::vector<std::string>& parametersTypeName);
};

#ifdef NA_RESOLVER_TEST_ENGINE
namespace TestEngine
{
	class Item
	{
	public:
		enum class Type { Class, NestedClass, Field, Method, None };
		Type type = Type::None;
		Item(Type type);
	};
	class ClassItem : public Item
	{
	public:
		std::string assemblyName = "";
		std::string namespaceName = "";
		std::string className = "";

		ClassItem(const std::string& assemblyName, const std::string& namespaceName, const std::string& className) :
			Item(Item::Type::Class), assemblyName(assemblyName), namespaceName(namespaceName), className(className) {}
	};
	class NestedClassItem : public Item
	{
	public:
		std::string assemblyName = "";
		std::string namespaceName = "";
		std::string declaringClass = "";
		std::string className = "";

		NestedClassItem(const std::string& assemblyName, const std::string& namespaceName, const std::string& declaringClass, const std::string& className) :
			Item(Item::Type::NestedClass), assemblyName(assemblyName), namespaceName(namespaceName), declaringClass(declaringClass), className(className) {}
	};
	class FieldItem : public Item
	{
	public:
		std::string assemblyName = "";
		std::string namespaceName = "";
		std::string declaringClass = "";
		std::string className = "";
		std::string fieldName = "";

		FieldItem(const std::string& assemblyName, const std::string& namespaceName, const std::string& declaringClass, const std::string& className, const std::string& fieldName) :
			Item(Item::Type::Field), assemblyName(assemblyName), namespaceName(namespaceName), declaringClass(declaringClass), className(className), fieldName(fieldName) {}
	};
	class MethodItem : public Item
	{
	public:
		std::string assemblyName = "";
		std::string namespaceName = "";
		std::string declaringClass = "";
		std::string className = "";
		std::string returnTypeName = "";
		std::string methodName = "";
		std::vector<std::string> parametersTypeName = {};

		MethodItem(const std::string& assemblyName, const std::string& namespaceName, const std::string& declaringClass, const std::string& className, const std::string& returnTypeName, const std::string& methodName, const std::vector<std::string>& parametersTypeName) :
			Item(Item::Type::Method), assemblyName(assemblyName), namespaceName(namespaceName), declaringClass(declaringClass), className(className), returnTypeName(returnTypeName), methodName(methodName), parametersTypeName(parametersTypeName) {}
	};
	class HandlerDevice
	{
	public:
		using FunctionType = void(*)(Item*);
		FunctionType handler = nullptr;
		
		void Handle(Item* item) { if (handler) handler(item); }

		HandlerDevice(FunctionType handler) : handler(handler) {}
	};
	class HandlerDeviceRegisterer : public HandlerDevice
	{
	public:
		HandlerDeviceRegisterer(FunctionType handler);
	};

	namespace Default
	{
		void DefaultHandler(Item* item);
		HandlerDevice defaultHandlerDevice = HandlerDevice(DefaultHandler);
	}

	class Engine
	{
	public:
		std::vector<Item*> items = {};
		HandlerDevice* handler = &Default::defaultHandlerDevice;

		void TestAllItem();
	};
}

#define NA_RESOLVER_TEST_ENGINE_HANDLER(name) void name(TestEngine::Item* item); TestEngine::HandlerDeviceRegisterer name##HandlerDevice = TestEngine::HandlerDeviceRegisterer(name); void name(TestEngine::Item* item)
#endif

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
	template<typename R, typename ...Args>
	class MethodInvoker
	{
	public:
		using FunctionType = R(*)(Args...);
		FunctionType content = nullptr;

		MethodInvoker() {}
		MethodInvoker(void* address) : content((FunctionType)address) {}

		template<typename T = R,
			typename std::enable_if_t<!std::is_void<T>::value, int> = 0>
		R Invoke(Args ... arg) const
		{
			return content(arg...);
		}

		template<typename T = R,
			typename std::enable_if_t<std::is_void<T>::value, int> = 0>
		void Invoke(Args ... arg) const
		{
			content(arg...);
		}

		R operator()(Args ... arg) const { if (content != nullptr) return Invoke(arg...); }

		bool IsValid() const
		{
			return content != nullptr;
		}
	};

	template<typename R, typename ...Args>
	class VmMethodInvoker : public Template::MethodInvoker<R, Args...>
	{
	public:
		VmMethodInvoker(const char* symbol);
	};

	template<int32_t Len>
	struct StringLiteral
	{
		char Chars[Len];

		consteval StringLiteral(const char(&String)[Len])
		{
			std::copy_n(String, Len, Chars);
		}

		operator std::string() const
		{
			return static_cast<const char*>(Chars);
		}

		template<int32_t OtherLen>
		consteval auto Concat(const StringLiteral<OtherLen>& other) const
		{
			char combined[Len + OtherLen - 1]; // -1 to avoid double null termination
			std::copy_n(Chars, Len - 1, combined);
			std::copy_n(other.Chars, OtherLen, combined + Len - 1);
			return StringLiteral<Len + OtherLen - 1>(combined);
		}
	};

	template<int32_t Len1, int32_t Len2>
	consteval auto operator+(const StringLiteral<Len1>& lhs, const StringLiteral<Len2>& rhs)
	{
		return lhs.Concat(rhs);
	}

	template <StringLiteral Assembly, StringLiteral Namespace, StringLiteral Name>
	class NormalClassInfo
	{
	public:
		static constexpr auto AssemblyName = Assembly;
		static constexpr auto NamespaceName = Namespace;
		static constexpr auto ClassName = Name;
		static constexpr auto DeclaringClassName = StringLiteral("__NONE__");
#ifdef NA_RESOLVER_TEST_ENGINE
		inline static TestEngine::ClassItem TestItem = TestEngine::ClassItem(AssemblyName, NamespaceName, ClassName);
#endif
		inline static NaResolver::Class ClassInfoCache = NaResolver::Class();

		inline static NaResolver::Class Instance();
	};

	template <typename Declaring, StringLiteral Name>
	class NestedClassInfo
	{
	public:
		static constexpr auto DeclaringClass = Declaring::ThisClassInfo;
		static constexpr auto AssemblyName = DeclaringClass.AssemblyName;
		static constexpr auto NamespaceName = DeclaringClass.NamespaceName;
		static constexpr auto DeclaringClassName = DeclaringClass.ClassName;
		static constexpr auto ClassName = Name;
#ifdef NA_RESOLVER_TEST_ENGINE
		inline static TestEngine::NestedClassItem TestItem = TestEngine::NestedClassItem(DeclaringClass.AssemblyName, DeclaringClass.NamespaceName, DeclaringClassName, ClassName);
#endif
		inline static NaResolver::Class ClassInfoCache = NaResolver::Class();

		inline static NaResolver::Class Instance();
	};

	template <typename Declaring, StringLiteral ReturnType, StringLiteral Name, StringLiteral... Args>
	class MemberMethodInfo
	{
	public:
		static constexpr auto DeclaringClass = Declaring::ThisClassInfo;
		static constexpr auto ReturnTypeName = ReturnType;
		static constexpr auto MethodName = Name;
		static constexpr auto Parameters = std::make_tuple(Args...);
#ifdef NA_RESOLVER_TEST_ENGINE
		inline static TestEngine::MethodItem TestItem = TestEngine::MethodItem(DeclaringClass.AssemblyName, DeclaringClass.NamespaceName, DeclaringClass.DeclaringClassName, DeclaringClass.ClassName, ReturnTypeName, MethodName, TupleToVector(Parameters));
#endif
		inline static void* MethodAddressCache = nullptr;

		inline static void* GetMethodAddress();
	};

	template <StringLiteral Name, bool IsBacking>
	struct BackingNameHandler
	{
		static constexpr auto Value = Name;

		static consteval auto Get()
		{
			if constexpr (!IsBacking)
				return Value;
			else
				return StringLiteral("<") + Value + StringLiteral(">k__BackingField");
		}
	};

	template <typename Declaring, StringLiteral Name, bool IsBacking = false>
	class MemberFieldInfo
	{
	public:
		static constexpr auto DeclaringClass = Declaring::ThisClassInfo;
		static constexpr auto FieldName = BackingNameHandler<Name, IsBacking>::Get();
#ifdef NA_RESOLVER_TEST_ENGINE
		inline static TestEngine::FieldItem TestItem = TestEngine::FieldItem(DeclaringClass.AssemblyName, DeclaringClass.NamespaceName, DeclaringClass.DeclaringClassName, DeclaringClass.ClassName, FieldName);
#endif
		inline static VmGeneralType::Field FieldInfoCache = nullptr;

		inline static VmGeneralType::Field GetFieldInfo();
	};

	template <typename Declaring, typename Type, StringLiteral Name, bool IsBacking = false>
	class StaticMemberField
	{
	public:
		MemberFieldInfo<Declaring, Name, IsBacking> field = MemberFieldInfo<Declaring, Name, IsBacking>();

		operator Type();

		const Type& operator=(const Type& value);
	};

	template <typename Type>
	class MemberFieldOperator
	{
	public:
		VmGeneralType::Field fieldInfo = VmGeneralType::Field();
		void* object = nullptr;

		MemberFieldOperator(VmGeneralType::Field fieldInfo, void* object) : fieldInfo(fieldInfo), object(object) {}

		Type Get();

		Type& Set(Type& value);
	};
}

inline NaResolver naResolverInstance = NaResolver();

#ifdef NA_RESOLVER_TEST_ENGINE
inline TestEngine::Engine naResolverTestEngineInstance = TestEngine::Engine();

TestEngine::Item::Item(TestEngine::Item::Type type) : type(type)
{
	naResolverTestEngineInstance.items.push_back(this);
}

TestEngine::HandlerDeviceRegisterer::HandlerDeviceRegisterer(FunctionType handler)
	: HandlerDevice(handler)
{
	naResolverTestEngineInstance.handler = this;
}

void TestEngine::Default::DefaultHandler(Item* item)
{
	if (item->type == TestEngine::Item::Type::Class)
	{
		TestEngine::ClassItem* classItem = (TestEngine::ClassItem*)item;
		NaResolver::Class classInstance = naResolverInstance.GetClass(classItem->assemblyName, classItem->namespaceName, classItem->className);
		printf(TEXT("[Test Engine] [Assembly: %s] [Namespace: %s] [Class: %s] [Result: %s]\n"), classItem->assemblyName.c_str(), classItem->namespaceName.c_str(), classItem->className.c_str(), classInstance ? "Success" : "Failed");
	}
	else if (item->type == TestEngine::Item::Type::NestedClass)
	{
		TestEngine::NestedClassItem* nestedClassItem = (TestEngine::NestedClassItem*)item;
		NaResolver::Class classInstance = naResolverInstance.GetClass(nestedClassItem->assemblyName, nestedClassItem->namespaceName, nestedClassItem->declaringClass);
		NaResolver::Class nestedClassInstance = naResolverInstance.GetClass(classInstance, nestedClassItem->className);
		printf(TEXT("[Test Engine] [Assembly: %s] [Namespace: %s] [Declaring Class: %s] [Nested Class: %s] [Result: %s]\n"), nestedClassItem->assemblyName.c_str(), nestedClassItem->namespaceName.c_str(), nestedClassItem->declaringClass.c_str(), nestedClassItem->className.c_str(), nestedClassInstance ? "Success" : "Failed");
	}
	else if (item->type == TestEngine::Item::Type::Field)
	{
		TestEngine::FieldItem* fieldItem = (TestEngine::FieldItem*)item;
		NaResolver::Class classInstance = NaResolver::Class();
		if (fieldItem->declaringClass == "__NONE__")
			classInstance = naResolverInstance.GetClass(fieldItem->assemblyName, fieldItem->namespaceName, fieldItem->className);
		else
			classInstance = naResolverInstance.GetClass(naResolverInstance.GetClass(fieldItem->assemblyName, fieldItem->namespaceName, fieldItem->declaringClass), fieldItem->className);
		VmGeneralType::Field fieldInstance = classInstance.klass.GetField(fieldItem->fieldName);
		printf(TEXT("[Test Engine] [Assembly: %s] [Namespace: %s] [Class: %s] [Field: %s] [Result: %s]\n"), fieldItem->assemblyName.c_str(), fieldItem->namespaceName.c_str(), fieldItem->className.c_str(), fieldItem->fieldName.c_str(), fieldInstance ? "Success" : "Failed");
	}
	else if (item->type == TestEngine::Item::Type::Method)
	{
		TestEngine::MethodItem* methodItem = (TestEngine::MethodItem*)item;
		NaResolver::Class classInstance = NaResolver::Class();
		if (methodItem->declaringClass == "__NONE__")
			classInstance = naResolverInstance.GetClass(methodItem->assemblyName, methodItem->namespaceName, methodItem->className);
		else
			classInstance = naResolverInstance.GetClass(naResolverInstance.GetClass(methodItem->assemblyName, methodItem->namespaceName, methodItem->declaringClass), methodItem->className);
		VmGeneralType::Method methodInstance = naResolverInstance.GetMethod(classInstance, methodItem->returnTypeName, methodItem->methodName, methodItem->parametersTypeName);
		printf(TEXT("[Test Engine] [Assembly: %s] [Namespace: %s] [Class: %s] [Method: %s] [Result: %s]\n"), methodItem->assemblyName.c_str(), methodItem->namespaceName.c_str(), methodItem->className.c_str(), methodItem->methodName.c_str(), methodInstance ? "Success" : "Failed");
	}
}

void TestEngine::Engine::TestAllItem()
{
	for (const auto& item : items)
	{
		handler->Handle(item);
	}
}
#endif

template<typename R, typename ...Args>
Template::VmMethodInvoker<R, Args...>::VmMethodInvoker(const char* symbol)
{
	this->content = (R(*)(Args...))GetProcAddress(GetModuleHandleA(TEXT(NA_RESOLVER_API_DLL_NAME)), symbol);
}

template<Template::StringLiteral Assembly, Template::StringLiteral Namespace, Template::StringLiteral Name>
NaResolver::Class Template::NormalClassInfo<Assembly, Namespace, Name>::Instance()
{
	if (ClassInfoCache.klass != nullptr)
		return ClassInfoCache;
	return ClassInfoCache = naResolverInstance.GetClass(AssemblyName, NamespaceName, ClassName);
}

template<typename Declaring, Template::StringLiteral Name>
NaResolver::Class Template::NestedClassInfo<Declaring, Name>::Instance()
{
	if (ClassInfoCache.klass != nullptr)
		return ClassInfoCache;
	return ClassInfoCache = naResolverInstance.GetClass(DeclaringClass.Instance(), ClassName);
}

template<typename Declaring, Template::StringLiteral Name, bool IsBacking>
VmGeneralType::Field Template::MemberFieldInfo<Declaring, Name, IsBacking>::GetFieldInfo()
{
	if (FieldInfoCache.fieldInfo == nullptr)
		return FieldInfoCache;
	return FieldInfoCache = DeclaringClass.Instance().klass.GetField(FieldName);
}

template<typename Type>
Type Template::MemberFieldOperator<Type>::Get()
{
	if constexpr (std::is_same_v<Type, std::string>)
	{
		void* result = nullptr;
		fieldInfo.GetValue(object, &result);
		return VmGeneralType::String(result);
	}
	else
	{
		Type result = Type();
		fieldInfo.GetValue(object, &result);
		return result;
	}
}

template<typename Type>
Type& Template::MemberFieldOperator<Type>::Set(Type& value)
{
	if constexpr (std::is_same_v<Type, std::string>)
	{
		fieldInfo.SetValue(object, VmGeneralType::String(value).address);
		return value;
	}
	else
	{
		fieldInfo.SetValue(object, &value);
		return value;
	}
}


template<typename Declaring, typename Type, Template::StringLiteral Name, bool IsBacking>
Template::StaticMemberField<Declaring, Type, Name, IsBacking>::operator Type()
{
	if constexpr (std::is_same_v<Type, std::string>)
	{
		void* result = nullptr;
		field.GetFieldInfo().GetStaticValue(&result);
		return VmGeneralType::String(result);
	}
	else
	{
		Type result = Type();
		field.GetFieldInfo().GetStaticValue(&result);
		return result;
	}
}

template<typename Declaring, typename Type, Template::StringLiteral Name, bool IsBacking>
const Type& Template::StaticMemberField<Declaring, Type, Name, IsBacking>::operator=(const Type& value)
{
	if constexpr (std::is_same_v<Type, std::string>)
	{
		field.GetFieldInfo().SetStaticValue(VmGeneralType::String(value).address);
		return value;
	}
	else
	{
		field.GetFieldInfo().SetStaticValue(&value);
		return value;
	}
}

template<typename Declaring, Template::StringLiteral ReturnType, Template::StringLiteral MethodName, Template::StringLiteral ...Args>
void* Template::MemberMethodInfo<Declaring, ReturnType, MethodName, Args...>::GetMethodAddress()
{
	if (MethodAddressCache != nullptr)
		return MethodAddressCache;
	return MethodAddressCache = naResolverInstance.GetMethod(DeclaringClass.Instance(), ReturnTypeName, MethodName, TupleToVector(Parameters))
		.method.GetInvokeAddress();
}

std::string VmGeneralType::Type::GetName() const
{
	static Template::VmMethodInvoker<const char*, void*> type_get_name = TEXT("il2cpp_type_get_name");
	return type_get_name(type);
}

VmGeneralType::Object VmGeneralType::Type::GetObject() const
{
	static Template::VmMethodInvoker<void*, void*> object_new = TEXT("il2cpp_type_get_object");
	return object_new(type);
}

std::string VmGeneralType::Method::GetName() const
{
	static Template::VmMethodInvoker<const char*, void*> method_get_name = TEXT("il2cpp_method_get_name");
	return method_get_name(method);
}

VmGeneralType::Type VmGeneralType::Method::GetReturnType() const
{
	static auto method_get_return_type = Template::VmMethodInvoker<void*, void*>(TEXT("il2cpp_method_get_return_type"));
	return method_get_return_type(method);
}

bool VmGeneralType::Method::IsGeneric() const
{
	static auto method_is_generic = Template::VmMethodInvoker<bool, void*>(TEXT("il2cpp_method_is_generic"));
	return method_is_generic(method);
}

std::vector<VmGeneralType::Type> VmGeneralType::Method::GetParametersType() const
{
	std::vector<Type> types = {};
	static auto method_get_param_count = Template::VmMethodInvoker<uint32_t, void*>(TEXT("il2cpp_method_get_param_count"));
	static auto method_get_param = Template::VmMethodInvoker<void*, void*, uint32_t>(TEXT("il2cpp_method_get_param"));
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

void* VmGeneralType::Method::GetInvokeAddress() const
{
	return *(void**)method;
}

std::string VmGeneralType::Class::GetName() const
{
	static Template::VmMethodInvoker<const char*, void*> class_get_name = TEXT("il2cpp_class_get_name");
	return class_get_name(klass);
}

VmGeneralType::Method VmGeneralType::Class::GetMethods(void** iter) const
{
	static Template::VmMethodInvoker<void*, void*, void**> class_get_methods = TEXT("il2cpp_class_get_methods");
	return class_get_methods(klass, iter);
}

VmGeneralType::Field VmGeneralType::Class::GetField(const std::string& name) const
{
	static Template::VmMethodInvoker<void*, void*, const char*> class_get_field_from_name = TEXT("il2cpp_class_get_field_from_name");
	return class_get_field_from_name(klass, name.c_str());
}

VmGeneralType::Type VmGeneralType::Class::GetType() const
{
	static Template::VmMethodInvoker<void*, void*> class_get_type = TEXT("il2cpp_class_get_type");
	return class_get_type(klass);
}

std::vector<VmGeneralType::Class> VmGeneralType::Class::GetNestedTypes() const
{
	std::vector<Class> classes = {};
	static Template::VmMethodInvoker<void*, void*, void**> class_get_nested_types = TEXT("il2cpp_class_get_nested_types");
	void* iter = NULL;
	void* nestedClass = NULL;
	while ((nestedClass = class_get_nested_types(klass, &iter)) != NULL)
	{
		classes.push_back(nestedClass);
	}
	return classes;
}

void VmGeneralType::Field::GetValue(Object object, void* value)
{
	static Template::VmMethodInvoker<void, void*, void*, void*> field_get_value = TEXT("il2cpp_field_get_value");
	field_get_value(object, fieldInfo, value);
}

void VmGeneralType::Field::SetValue(Object object, void* value)
{
	static Template::VmMethodInvoker<void, void*, void*, void*> field_set_value = TEXT("il2cpp_field_set_value");
	field_set_value(object, fieldInfo, value);
}

void VmGeneralType::Field::GetStaticValue(void* value)
{
	static auto field_static_get_value = Template::VmMethodInvoker<void, void*, void*>(TEXT("il2cpp_field_static_get_value"));
	field_static_get_value(fieldInfo, value);
}

void VmGeneralType::Field::SetStaticValue(void* value)
{
	static auto field_static_set_value = Template::VmMethodInvoker<void, void*, void*>(TEXT("il2cpp_field_static_set_value"));
	field_static_set_value(fieldInfo, value);
}

std::string VmGeneralType::Image::GetName() const
{
	static Template::VmMethodInvoker<const char*, void*> image_get_name = TEXT("il2cpp_image_get_name");
	return image_get_name(image);
}

VmGeneralType::Class VmGeneralType::Image::GetClassFromName(const std::string& namespaceName, const std::string& className) const
{
	static Template::VmMethodInvoker<void*, void*, const char*, const char*> class_from_name = TEXT("il2cpp_class_from_name");
	return class_from_name(image, namespaceName.c_str(), className.c_str());
}

VmGeneralType::Image VmGeneralType::Assembly::GetImage() const
{
	static Template::VmMethodInvoker<void*, void*> assembly_get_image = TEXT("il2cpp_assembly_get_image");
	return assembly_get_image(assembly);
}

VmGeneralType::Domain VmGeneralType::Domain::Get()
{
	static Template::VmMethodInvoker<void*> domain_get = TEXT("il2cpp_domain_get");
	return domain_get();
}

VmGeneralType::Assembly VmGeneralType::Domain::OpenAssembly(const std::string& name) const
{
	static Template::VmMethodInvoker<void*, void*, const char*> assembly_get = TEXT("il2cpp_domain_assembly_open");
	return assembly_get(domain, name.c_str());
}

VmGeneralType::String VmGeneralType::String::New(const std::string& string)
{
	static auto il2cpp_string_new = Template::VmMethodInvoker<void*, const char*>(TEXT("il2cpp_string_new"));
	return il2cpp_string_new(string.c_str());
}

std::string VmGeneralType::String::ToString() const
{
	static Template::VmMethodInvoker<wchar_t*, void*> string_chars = TEXT("il2cpp_string_chars");
	if (!string_chars.IsValid()) return "";
	wchar_t* data = string_chars(address);
	if (!data) return "";
	std::wstring wData(data);
	return std::string(wData.begin(), wData.end());
}

VmGeneralType::Array VmGeneralType::Array::New(Class elementTypeInfo, uint32_t length)
{
	static Template::VmMethodInvoker<void*, void*, uint32_t> array_new = TEXT("il2cpp_array_new");
	return array_new(elementTypeInfo, length);
}

VmGeneralType::Thread VmGeneralType::Thread::Attach(Domain domain)
{
	static Template::VmMethodInvoker<void*, void*> thread_attach = TEXT("il2cpp_thread_attach");
	return thread_attach(domain);
}

VmGeneralType::Thread VmGeneralType::Thread::Current()
{
	static Template::VmMethodInvoker<void*> thread_current = TEXT("il2cpp_thread_current");
	return thread_current();
}

void VmGeneralType::Thread::Detach() const
{
	static Template::VmMethodInvoker<void, void*> thread_detach = TEXT("il2cpp_thread_detach");
	thread_detach(thread);
}

void NaResolver::Class::AddNestedClass(const std::string name, const Class& klass)
{
	nestedClasses.insert({ name, klass });
}

NaResolver::Class NaResolver::Class::FindNestedClass(const std::string name) const
{
	auto iter = nestedClasses.find(name);
	if (iter == nestedClasses.end())
		return NaResolver::Class();
	return iter->second;
}

void NaResolver::ContextCache::RegisterAssembly(const std::string& name, VmGeneralType::Assembly assembly)
{
	assemblyMap[name] = assembly;
}

NaResolver::Class NaResolver::ContextCache::RegisterClass(const std::string& assembly, const std::string& nameSpace, const std::string& name, VmGeneralType::Class klass, VmGeneralType::Type type)
{
	return classPathMap[assembly][nameSpace][name] = Class(assembly, nameSpace, name, klass, type);
}

VmGeneralType::Assembly NaResolver::ContextCache::GetAssembly(const std::string& name) const
{
	auto iter = assemblyMap.find(name);
	if (iter == assemblyMap.end())
		return VmGeneralType::Assembly();
	return iter->second;
}

NaResolver::Class NaResolver::ContextCache::GetClass(const std::string& assembly, const std::string& nameSpace, const std::string& name) const
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

NaResolver::Class& NaResolver::ContextCache::GetClass(const std::string& assembly, const std::string& nameSpace, const std::string& name)
{
	return classPathMap[assembly][nameSpace][name];
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
#ifdef NA_RESOLVER_TEST_ENGINE
	naResolverTestEngineInstance.TestAllItem();
#endif
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

NaResolver::Class NaResolver::GetClass(Class parent, const std::string& className)
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
		std::vector<VmGeneralType::Class> nestedClasses = parentReference.klass.GetNestedTypes();
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

NaResolver::Method NaResolver::GetMethod(Class parent, const std::string& returnTypeName, const std::string& methodName, const std::vector<std::string>& parametersTypeName)
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
	using __This_Class_Type__ = className; \
	inline static constexpr Template::NormalClassInfo<assemblyName, namespaceName, #className> ThisClassInfo = {}
#define NESTED_CLASS(declaring, className) \
	using __This_Class_Type__ = className; \
	inline static constexpr Template::NestedClassInfo<declaring, #className> ThisClassInfo = {}

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

#define FIELD_INFO(name, backing) inline static constexpr Template::MemberFieldInfo<__This_Class_Type__, #name, backing> __##name##_Field_Info__ = {};
#define FIELD_BODY(type, name) \
	type get_##name##() { return Template::MemberFieldOperator<type>(__##name##_Field_Info__.GetFieldInfo(), this).Get(); } \
	type set_##name##(type value) { return Template::MemberFieldOperator<type>(__##name##_Field_Info__.GetFieldInfo(), this).Set(value); }
#define FIELD(type, name) \
	FIELD_INFO(name, false)\
	FIELD_BODY(type, name)
#define BACKING_FIELD(type, name) \
	FIELD_INFO(name, true)\
	FIELD_BODY(type, name)

#define STATIC_FIELD(type, name) inline static Template::StaticMemberField<__This_Class_Type__, type, #name> name = {}
#define STATIC_BACKING_FIELD(type, name) inline static Template::StaticMemberField<__This_Class_Type__, type, #name, true> name = {}

#define METHOD_INFO(parameterCount, returnType, methodName, ...) inline static constexpr Template::MemberMethodInfo<__This_Class_Type__, returnType, #methodName, __VA_ARGS__> __##methodName##_##parameterCount##_Method_Info__ = {}
#undef TEXT
#endif // !H_NARESOLVER