//**************************************//
// Hi NaResolver			//
// Author: MidTerm                   	//
// Version: v1.6	                //
// License: MIT                         //
//**************************************//

#if !_HAS_CXX17
#error "The contents of NaResolver are available only with C++17 or later."
#endif

#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <codecvt>

#if defined(__NARESOLVER_ONLY_API)
typedef void Il2CppDomain;
typedef void Il2CppAssembly;
typedef void Il2CppClass;
typedef void Il2CppThread;
typedef void Il2CppImage;
typedef void Il2CppString;
typedef void MethodInfo;
typedef void Il2CppType;
typedef void FieldInfo;
typedef void* Il2CppMethodPointer;
typedef wchar_t Il2CppChar;
#endif

class Il2CppManager
{
public:
	HMODULE assemblyModule;
	std::unordered_map<std::string, void*> il2cppMethodMap =
	{
		{"il2cpp_domain_get", nullptr},
		{"il2cpp_domain_assembly_open", nullptr},

		{"il2cpp_type_get_name", nullptr},

		{"il2cpp_thread_attach", nullptr},
		{"il2cpp_thread_detach", nullptr},

		{"il2cpp_string_new", nullptr},
		{"il2cpp_string_chars", nullptr},
		{"il2cpp_string_length", nullptr},

		{"il2cpp_gc_disable", nullptr},

		{"il2cpp_class_get_methods", nullptr},
		{"il2cpp_class_from_name", nullptr},
		{"il2cpp_class_get_name", nullptr},
		{"il2cpp_class_get_namespace", nullptr},
		{"il2cpp_class_get_image", nullptr},
		{"il2cpp_class_get_fields", nullptr},
		{"il2cpp_class_get_parent", nullptr},
		{"il2cpp_class_get_nested_types", nullptr},
		{"il2cpp_class_get_flags", nullptr},

		{"il2cpp_assembly_get_image", nullptr},

		{"il2cpp_method_get_name", nullptr},
		{"il2cpp_method_get_return_type", nullptr},
		{"il2cpp_method_get_param_count", nullptr},
		{"il2cpp_method_get_param", nullptr},
		{"il2cpp_method_get_flags", nullptr},
		{"il2cpp_method_get_param_name", nullptr},

		{"il2cpp_image_get_assembly", nullptr},

		{"il2cpp_field_get_name", nullptr},
		{"il2cpp_field_get_flags", nullptr},
		{"il2cpp_field_get_type", nullptr},
		{"il2cpp_field_get_offset", nullptr},

		{"il2cpp_type_get_class_or_element_class", nullptr},
	};

	inline void ImportMethods()
	{
		assemblyModule = GetModuleHandleW(L"GameAssembly.dll");
		if (!assemblyModule)
		{
			throw std::exception("Failed to get GameAssembly.dll module handle");
		}
		for (auto& m : il2cppMethodMap)
		{
			m.second = GetProcAddress(assemblyModule, m.first.c_str());
			if (!m.second)
			{
				throw std::exception(("Failed to get " + m.first + " function address").c_str());
			}
		}
	}

	inline Il2CppDomain* GetDomain() { return ((Il2CppDomain * (*)(void)) il2cppMethodMap["il2cpp_domain_get"])(); }
	inline Il2CppAssembly* OpenDomainAssembly(Il2CppDomain* domain, const char* name) { return ((Il2CppAssembly * (*)(Il2CppDomain*, const char*)) il2cppMethodMap["il2cpp_domain_assembly_open"])(domain, name); }

	inline char* GetTypeName(const Il2CppType* type) { return ((char* (*)(const Il2CppType*))il2cppMethodMap["il2cpp_type_get_name"])(type); }

	inline Il2CppThread* AttachThread(Il2CppDomain* domain) { return ((Il2CppThread * (*)(Il2CppDomain*)) il2cppMethodMap["il2cpp_thread_attach"])(domain); }
	inline void DetachThread(Il2CppThread* thread) { ((void (*)(Il2CppThread*))il2cppMethodMap["il2cpp_thread_detach"])(thread); }

	inline Il2CppString* NewString(const char* string) { return ((Il2CppString * (*)(const char*)) il2cppMethodMap["il2cpp_string_new"])(string); }
	inline Il2CppChar* GetChars(Il2CppString* string) { return ((Il2CppChar * (*)(Il2CppString*)) il2cppMethodMap["il2cpp_string_chars"])(string); }
	inline int32_t GetStringLength(Il2CppString* string) { return ((int32_t(*)(Il2CppString*))il2cppMethodMap["il2cpp_string_length"])(string); }

	inline void DisableGC() { ((void (*)(void))il2cppMethodMap["il2cpp_gc_disable"])(); }

	inline const MethodInfo* GetClassMethods(Il2CppClass* klass, void** iter) { return ((const MethodInfo * (*)(Il2CppClass*, void**))il2cppMethodMap["il2cpp_class_get_methods"])(klass, iter); }
	inline Il2CppClass* GetClassFromName(const Il2CppImage* image, const char* nameSpace, const char* name) { return ((Il2CppClass * (*)(const Il2CppImage*, const char*, const char*)) il2cppMethodMap["il2cpp_class_from_name"])(image, nameSpace, name); }
	inline const char* GetClassName(Il2CppClass* klass) { return ((const char* (*)(Il2CppClass*))il2cppMethodMap["il2cpp_class_get_name"])(klass); }
	inline const char* GetClassNamespace(Il2CppClass* klass) { return ((const char* (*)(Il2CppClass*))il2cppMethodMap["il2cpp_class_get_namespace"])(klass); }
	inline const Il2CppImage* GetClassImage(Il2CppClass* klass) { return ((const Il2CppImage * (*)(Il2CppClass*)) il2cppMethodMap["il2cpp_class_get_image"])(klass); }
	inline FieldInfo* GetClassFields(Il2CppClass* klass, void** iter) { return ((FieldInfo * (*)(Il2CppClass*, void**))il2cppMethodMap["il2cpp_class_get_fields"])(klass, iter); }
	inline Il2CppClass* GetClassParent(Il2CppClass* klass) { return ((Il2CppClass * (*)(Il2CppClass*))il2cppMethodMap["il2cpp_class_get_parent"])(klass); }
	inline Il2CppType* GetClassNestedTypes(Il2CppClass* klass, void** iter) { return ((Il2CppType * (*)(Il2CppClass*, void**))il2cppMethodMap["il2cpp_class_get_nested_types"])(klass, iter); }
	inline int GetClassFlags(Il2CppClass* klass) { return ((int (*)(Il2CppClass*))il2cppMethodMap["il2cpp_class_get_flags"])(klass); }

	inline const Il2CppImage* GetAssemblyImage(const Il2CppAssembly* assembly) { return ((const Il2CppImage * (*)(const Il2CppAssembly*)) il2cppMethodMap["il2cpp_assembly_get_image"])(assembly); }

	inline const char* GetMethodName(const MethodInfo* method) { return ((const char* (*)(const MethodInfo*))il2cppMethodMap["il2cpp_method_get_name"])(method); }
	inline const Il2CppType* GetMethodReturnType(const MethodInfo* method) { return ((const Il2CppType * (*)(const MethodInfo*))il2cppMethodMap["il2cpp_method_get_return_type"])(method); }
	inline uint32_t GetMethodParamCount(const MethodInfo* method) { return ((uint32_t(*)(const MethodInfo*))il2cppMethodMap["il2cpp_method_get_param_count"])(method); }
	inline const Il2CppType* GetMethodParam(const MethodInfo* method, uint32_t index) { return ((const Il2CppType * (*)(const MethodInfo*, uint32_t))il2cppMethodMap["il2cpp_method_get_param"])(method, index); }
	inline uint32_t GetMethodFlags(const MethodInfo* method, uint32_t* iflags) { return ((uint32_t(*)(const MethodInfo*, uint32_t*))il2cppMethodMap["il2cpp_method_get_flags"])(method, iflags); }
	inline const char* GetMethodParamName(const MethodInfo* method, uint32_t index) { return ((const char* (*)(const MethodInfo*, uint32_t))il2cppMethodMap["il2cpp_method_get_param_name"])(method, index); }

	inline const Il2CppAssembly* GetImageAssembly(const Il2CppImage* image) { return ((const Il2CppAssembly * (*)(const Il2CppImage*)) il2cppMethodMap["il2cpp_image_get_assembly"])(image); }

	inline const char* GetFieldName(FieldInfo* field) { return ((const char* (*)(FieldInfo*))il2cppMethodMap["il2cpp_field_get_name"])(field); }
	inline int GetFieldFlags(FieldInfo* field) { return ((int(*)(FieldInfo*))il2cppMethodMap["il2cpp_field_get_flags"])(field); }
	inline const Il2CppType* GetFieldType(FieldInfo* field) { return ((const Il2CppType * (*)(FieldInfo*))il2cppMethodMap["il2cpp_field_get_type"])(field); }
	inline size_t GetFieldOffset(FieldInfo* field) { return ((size_t(*)(FieldInfo*))il2cppMethodMap["il2cpp_field_get_offset"])(field); }

	inline Il2CppClass* GetTypeClassOrElementClass(const Il2CppType* type) { return ((Il2CppClass * (*)(const Il2CppType*))il2cppMethodMap["il2cpp_type_get_class_or_element_class"])(type); }
};

namespace Signature
{
	namespace Class
	{
		inline std::string Create(std::string assembly, std::string nameSpace, std::string name)
		{
			return std::string("(") + assembly + ")" + nameSpace + (nameSpace.empty() ? "" : ".") + name;
		}

		inline std::string Create(Il2CppClass* klass, Il2CppManager il2CppManager)
		{
			struct AssemblyStructure
			{
				char __pad__[sizeof(void*) + sizeof(uint32_t) + sizeof(int32_t) + sizeof(int32_t)];
				const char* name;
			};
			const Il2CppImage* image = il2CppManager.GetClassImage(klass);
			if (!image)
				return std::string();
			const AssemblyStructure* assembly = (AssemblyStructure*)il2CppManager.GetImageAssembly(image);
			if (!assembly)
				return std::string();
			return Create(assembly->name, il2CppManager.GetClassNamespace(klass), il2CppManager.GetClassName(klass));
		}

		inline void Analysis(std::string signature, std::string* assembly, std::string* nameSpace, std::string* name)
		{
			*assembly = signature.substr(signature.find("(") + 1, signature.find(")") - signature.find("(") - 1);
			signature = signature.substr(signature.find(")") + 1);
			*nameSpace = signature.find(".") == std::string::npos ? "" : signature.substr(0, signature.rfind("."));
			*name = signature.substr(signature.rfind(".") + 1);
		}
	}

	namespace Method
	{
		inline void Analysis(std::string signature, std::string* returnKlass, std::string* name, std::vector<std::string>* parameters)
		{
			*returnKlass = signature.substr(0, signature.find(" "));
			signature = signature.substr(signature.find(" ") + 1);
			*name = signature.substr(0, signature.find("("));
			signature = signature.substr(signature.find("(") + 1);
			if (signature.find(")") == std::string::npos)
				return;
			signature = signature.substr(0, signature.find(")"));

			if (signature.find("MPA_") != std::string::npos)
			{
				int index = std::stoi(signature.substr(signature.find("MPA_") + 4));
				for (int i = 0; i < index; i++)
				{
					parameters->push_back("AUTO");
				}
				return;
			}

			if (signature.size() <= 0)
			{
				parameters = new std::vector<std::string>();
				return;
			}

			while (signature.find(",") != std::string::npos)
			{
				parameters->push_back(signature.substr(0, signature.find(",")));
				signature = signature.substr(signature.find(",") + 2);
			}
			parameters->push_back(signature);
		}

		inline std::string Create(const MethodInfo* method, Il2CppManager il2CppManager)
		{
			std::string signature = il2CppManager.GetTypeName(il2CppManager.GetMethodReturnType(method)) + std::string(" ") + il2CppManager.GetMethodName(method) + "(";

			int paramCount = il2CppManager.GetMethodParamCount(method);
			for (int i = 0; i < paramCount; i++)
			{
				signature += il2CppManager.GetTypeName(il2CppManager.GetMethodParam(method, i)) + std::string(", ");
			}

			if (paramCount > 0)
			{
				signature = signature.substr(0, signature.size() - 2);
			}
			return signature + ")";
		}
	}
}

namespace ConfusedTranslate
{
	struct Klass
	{
		std::string assembly;
		std::string nameSpace;
		std::string originalName;
		std::string confusedName;
	};

	struct Method
	{
		Klass klass;
		std::string originalName;
		std::string confusedName;
	};

	inline std::vector<Klass> klass = std::vector<Klass>();

	inline std::vector<Method> method = std::vector<Method>();

	inline std::string RestoreKlass(std::string signature)
	{
		std::string assembly, nameSpace, name;
		Signature::Class::Analysis(signature, &assembly, &nameSpace, &name);
		for (auto& k : klass)
		{
			if (k.assembly.compare(assembly) == 0 && k.nameSpace.compare(nameSpace) == 0 && k.originalName.compare(name) == 0)
				return Signature::Class::Create(k.assembly, k.nameSpace, k.confusedName);
		}
		return signature;
	}

	inline std::string RestoreMethod(std::string klassSignature, std::string methodName)
	{
		std::string assembly, nameSpace, name;
		Signature::Class::Analysis(klassSignature, &assembly, &nameSpace, &name);
		for (auto& m : method)
		{
			if (m.klass.assembly.compare(assembly) == 0 && m.klass.nameSpace.compare(nameSpace) == 0 && (m.klass.confusedName.compare(name) == 0 || m.klass.originalName.compare(name) == 0) && m.originalName.compare(methodName) == 0)
				return m.confusedName;
		}
		return methodName;
	}

	inline std::string ConvertKlass(std::string signature)
	{
		std::string assembly, nameSpace, name;
		Signature::Class::Analysis(signature, &assembly, &nameSpace, &name);
		for (auto& k : klass)
		{
			bool allMatch = k.assembly.compare(assembly) == 0 && k.nameSpace.compare(nameSpace) == 0 && k.confusedName.compare(name) == 0;
			if (allMatch)
				return Signature::Class::Create(k.assembly, k.nameSpace, k.originalName);
		}
		return signature;
	}
}

class NaResolver
{
public:
	struct Config
	{
		bool disableGC = false;
		bool enableLogger = false;

		struct LoggerConfig
		{
			void (*fatal)(std::string, ...);
			void (*info)(std::string, ...);
			void (*debug)(std::string, ...);
			void (*error)(std::string, ...);
		} logger;
	};
	Il2CppManager il2CppManager;
	Il2CppDomain* domain;
	Il2CppThread* attachedThread;
	std::unordered_map<std::string, Il2CppAssembly*> assemblies;
	std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<std::string, Il2CppClass*>>> classes;
	std::unordered_map<std::string, Il2CppType*> types;

	inline NaResolver();
	inline bool Setup(Config config);
	inline void Destroy();
	inline Il2CppClass* GetClassEx(std::string assembly, std::string nameSpace, std::string name);
	inline Il2CppClass* GetClass(std::string signature);
	inline Il2CppMethodPointer GetMethod(Il2CppClass* klass, std::string signature);
	inline FieldInfo* GetField(Il2CppClass* klass, std::string name);
	inline Il2CppType* GetType(std::string signature);
	inline Il2CppType* GetType(Il2CppClass* klass);
	inline std::string GetStringByIl2Cpp(Il2CppString* string);

private:
	inline bool ClassExistsInCache(std::string assembly, std::string nameSpace, std::string name);
	inline Il2CppAssembly* GetAssembly(std::string name);
	inline bool MethodVerifyParams(const MethodInfo* method, std::vector<std::string> parameters);
	void (*LogFatal)(std::string, ...);
	void (*LogInfo)(std::string, ...);
	void (*LogDebug)(std::string, ...);
	void (*LogError)(std::string, ...);
};

inline NaResolver* Il2CppResolver = new NaResolver();

NaResolver::NaResolver()
{
	domain = nullptr;
	assemblies = std::unordered_map<std::string, Il2CppAssembly*>();
	classes = std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<std::string, Il2CppClass*>>>();
	types = std::unordered_map<std::string, Il2CppType*>();
}

inline bool NaResolver::Setup(Config config)
{
	LogFatal = config.logger.fatal;
	LogError = config.logger.error;
	LogInfo = config.logger.info;
	LogDebug = config.logger.debug;
	if (!config.enableLogger)
	{
		LogFatal = LogInfo = LogDebug = LogError = (decltype(LogFatal))([](std::string, ...) -> void {});
	}

	try
	{
		il2CppManager.ImportMethods();
	}
	catch (const std::exception& e)
	{
		LogFatal("[NaResolver] Failed to import il2cpp methods: %s", e.what());
		return false;
	}

	if (domain = il2CppManager.GetDomain(), domain == nullptr)
	{
		LogFatal("[NaResolver] Domain get failed.");
		return false;
	}

	attachedThread = il2CppManager.AttachThread(domain);
	if (config.disableGC)
		il2CppManager.DisableGC();

	LogInfo("[NaResolver] Setup success.");
	return true;
}

inline void NaResolver::Destroy()
{
	if (attachedThread != nullptr)
	{
		il2CppManager.DetachThread(attachedThread);
		attachedThread = nullptr;
	}
	domain = nullptr;
	assemblies.clear();
	classes.clear();
	types.clear();

	LogInfo("[NaResolver] Destroy success.");
}

inline Il2CppClass* NaResolver::GetClassEx(std::string _assembly, std::string _nameSpace, std::string _name)
{
	std::string assembly = _assembly, nameSpace = _nameSpace, name = _name;
	std::string signature = Signature::Class::Create(assembly, nameSpace, name);
	if (nameSpace.compare("") == 0)
		nameSpace = "__NO_NAMESPACE__";

	if (ClassExistsInCache(assembly, nameSpace, name))
		return classes[assembly][nameSpace][signature];

	Signature::Class::Analysis(ConfusedTranslate::RestoreKlass(signature), &assembly, &nameSpace, &name);

	Il2CppAssembly* pAssembly = GetAssembly(assembly);
	if (pAssembly == nullptr)
	{
		LogError("[NaResolver] Get assembly failed (%s).", assembly.c_str());
		return nullptr;
	}

	const Il2CppImage* pImage = il2CppManager.GetAssemblyImage(pAssembly);
	if (!pImage)
	{
		LogError("[NaResolver] pImage is null for %s.", signature.c_str());
		return nullptr;
	}

	Il2CppClass* pClass = il2CppManager.GetClassFromName(pImage, nameSpace.c_str(), name.c_str());
	if (!pClass)
	{
		LogError("[NaResolver] pClass is null for %s.", signature.c_str());
		return nullptr;
	}
	if (classes.find(assembly) == classes.end())
	{
		classes.insert(std::make_pair(assembly, std::unordered_map<std::string, std::unordered_map<std::string, Il2CppClass*>>()));
	}
	if (nameSpace.compare("") == 0)
		nameSpace = "__NO_NAMESPACE__";
	if (classes[assembly].find(nameSpace) == classes[assembly].end())
	{
		classes[assembly].insert(std::make_pair(nameSpace, std::unordered_map<std::string, Il2CppClass*>()));
	}
	classes[assembly][nameSpace].insert(std::make_pair(signature, pClass));
	LogInfo("[NaResolver] Find class: %s", signature.c_str());
	return pClass;
}

inline Il2CppClass* NaResolver::GetClass(std::string signature)
{
	std::string assembly, nameSpace, name;
	Signature::Class::Analysis(ConfusedTranslate::RestoreKlass(signature), &assembly, &nameSpace, &name);
	return GetClassEx(assembly, nameSpace, name);
}

inline Il2CppMethodPointer NaResolver::GetMethod(Il2CppClass* klass, std::string signature)
{
	if (klass == nullptr)
		return nullptr;

	std::string name = "";
	std::string returnType = "";
	std::vector<std::string> parameters = std::vector<std::string>();
	Signature::Method::Analysis(signature, &returnType, &name, &parameters);
	name = ConfusedTranslate::RestoreMethod(Signature::Class::Create(klass, il2CppManager), name);

	void* iterator = nullptr;
	const MethodInfo* method = nullptr;

	while ((method = il2CppManager.GetClassMethods(klass, &iterator)) != nullptr)
	{
		std::string methodName = il2CppManager.GetMethodName(method);
		if (methodName.compare(name) != 0 && name.compare("AUTO") != 0)
			continue;
		std::string returnTypeName = il2CppManager.GetTypeName(il2CppManager.GetMethodReturnType(method));
		if (returnTypeName.compare(returnType) != 0 && returnType.compare("AUTO") != 0)
			continue;
		if (!MethodVerifyParams(method, parameters))
			continue;
		LogInfo("[NaResolver] Find method: %s", signature.c_str());
		return *(void**)method;
	}
	LogFatal("[NaResolver] Could not find the method: %s", signature.c_str());
	return nullptr;
}

inline FieldInfo* NaResolver::GetField(Il2CppClass* klass, std::string name)
{
	if (klass == nullptr)
		return nullptr;

	void* iterator = nullptr;
	FieldInfo* field = nullptr;

	while ((field = il2CppManager.GetClassFields(klass, &iterator)) != nullptr)
	{
		std::string fieldName = il2CppManager.GetFieldName(field);
		if (fieldName.compare(name) != 0)
			continue;
		LogInfo("[NaResolver] Find field: %s", name.c_str());
		return const_cast<FieldInfo*>(field);
	}
}

inline Il2CppType* NaResolver::GetType(std::string signature)
{
	if (types.find(signature) != types.end())
		return types[signature];
	static auto func = ((Il2CppType * (*)(Il2CppString * typeName, MethodInfo * method))
		GetMethod(GetClassEx("mscorlib", "System", "Type"), "System.Type GetType(System.String)"));
	Il2CppType* result = func(il2CppManager.NewString(signature.c_str()), nullptr);
	if (!result)
	{
		LogError("[NaResolver] Could not find the type: %s", signature.c_str());
		return nullptr;
	}
	types[signature] = result;
	return result;
}

inline Il2CppType* NaResolver::GetType(Il2CppClass* klass)
{
	if (klass == nullptr)
		return nullptr;
	std::string signature = ConfusedTranslate::RestoreKlass(Signature::Class::Create(klass, il2CppManager));
	std::string assembly, nameSpace, name;
	Signature::Class::Analysis(signature, &assembly, &nameSpace, &name);
	std::string typeSignature = name + ", " + assembly;
	if (nameSpace.size() > 0)
		typeSignature = nameSpace + "." + typeSignature;
	return GetType(typeSignature);
}

inline std::string NaResolver::GetStringByIl2Cpp(Il2CppString* string)
{
	if (!string)
	{
		return std::string();
	}
	char16_t* chars = (char16_t*)il2CppManager.GetChars(string);
	int len = il2CppManager.GetStringLength(string);
	int size = WideCharToMultiByte(CP_UTF8, 0, (LPCWCH)chars, len, NULL, 0, NULL, NULL);
	char* buffer = new char[size + 1];
	WideCharToMultiByte(CP_UTF8, 0, (LPCWCH)chars, len, buffer, size, NULL, NULL);
	buffer[size] = '\0';
	std::string result = std::string(buffer);
	delete[] buffer;
	return result;
}

inline bool NaResolver::ClassExistsInCache(std::string assembly, std::string nameSpace, std::string name)
{
	if (classes.find(assembly) == classes.end())
	{
		return false;
	}
	if (classes[assembly].find(nameSpace) == classes[assembly].end())
	{
		return false;
	}
	return classes[assembly][nameSpace].find(name) != classes[assembly][nameSpace].end();
}

inline Il2CppAssembly* NaResolver::GetAssembly(std::string name)
{
	if (assemblies.find(name) != assemblies.end())
		return assemblies[name];
	Il2CppAssembly* assembly = il2CppManager.OpenDomainAssembly(domain, name.c_str());
	if (!assembly)
		return nullptr;
	assemblies[name] = assembly;
	return assembly;
}

inline bool NaResolver::MethodVerifyParams(const MethodInfo* method, std::vector<std::string> parameters)
{
	uint32_t methodParamCount = il2CppManager.GetMethodParamCount(method);
	if (methodParamCount != parameters.size())
		return false;
	for (uint32_t j = 0; j < methodParamCount; j++)
	{
		if (parameters[j].compare("AUTO") == 0)
			continue;
		std::string parameterName = il2CppManager.GetTypeName(il2CppManager.GetMethodParam(method, j));
		if (parameterName.compare(parameters[j]) != 0)
			return false;
	}
	return true;
}

#define STATIC_AREA_OFFSET (sizeof(void *) == 8 ? 0xB8 : 0xC5)
#define CLASS(assembly, namespaze, klass) \
	static Il2CppClass *ThisClass() { return Il2CppResolver->GetClassEx(assembly, namespaze, klass); }
#define MEMBER(klass, name, offset) \
	struct                          \
	{                               \
		char __pad__##name[offset]; \
		klass name;                 \
	}
#define STATIC_MEMBER(klass, name, offset)                                                                                                                 \
	static klass get_##name() { return *reinterpret_cast<klass *>(*reinterpret_cast<uintptr_t *>((uintptr_t)ThisClass() + STATIC_AREA_OFFSET) + offset); } \
	static void set_##name(klass value) { *reinterpret_cast<klass *>(*reinterpret_cast<uintptr_t *>((uintptr_t)ThisClass() + STATIC_AREA_OFFSET) + offset) = value; }
#define METHOD(returnType, parameters, signature) static auto __fn = (returnType(*) parameters)(Il2CppResolver->GetMethod(ThisClass(), signature));
