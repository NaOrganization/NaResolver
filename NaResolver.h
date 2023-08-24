//**************************************//
// Hi NaResolver			//
// Author: MidTerm                   	//
// Version: v1.7			//
// License: MIT                         //
//**************************************//

#if !_HAS_CXX17
#error "The contents of NaResolver are available only with C++17 or later."
#else

#undef GetClassName

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
typedef void *Il2CppMethodPointer;
typedef wchar_t Il2CppChar;
#endif

class Il2CppManager
{
public:
	HMODULE assemblyModule = 0;
	std::unordered_map<std::string, void *> il2cppMethodMap =
		{
			{"il2cpp_domain_get", nullptr},
			{"il2cpp_domain_assembly_open", nullptr},
			{"il2cpp_domain_get_assemblies", nullptr},

			{"il2cpp_type_get_name", nullptr},
			{"il2cpp_type_is_byref", nullptr},
			{"il2cpp_type_get_attrs", nullptr},
			{"il2cpp_type_get_class_or_element_class", nullptr},

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
			{"il2cpp_class_is_generic", nullptr},
			{"il2cpp_class_is_enum", nullptr},
			{"il2cpp_class_is_valuetype", nullptr},
			{"il2cpp_class_from_il2cpp_type", nullptr},
			{"il2cpp_class_get_type", nullptr},
			{"il2cpp_class_get_element_class", nullptr},

			{"il2cpp_assembly_get_image", nullptr},

			{"il2cpp_method_get_name", nullptr},
			{"il2cpp_method_get_return_type", nullptr},
			{"il2cpp_method_get_param_count", nullptr},
			{"il2cpp_method_get_param", nullptr},
			{"il2cpp_method_get_flags", nullptr},
			{"il2cpp_method_get_param_name", nullptr},

			{"il2cpp_image_get_assembly", nullptr},
			{"il2cpp_image_get_class_count", nullptr},
			{"il2cpp_image_get_class", nullptr},

			{"il2cpp_field_get_name", nullptr},
			{"il2cpp_field_get_flags", nullptr},
			{"il2cpp_field_get_type", nullptr},
			{"il2cpp_field_get_offset", nullptr},
			{"il2cpp_field_get_value", nullptr},
			{"il2cpp_field_set_value", nullptr},
			{"il2cpp_field_static_get_value", nullptr},
			{"il2cpp_field_static_set_value", nullptr},
	};

	inline void ImportMethods()
	{
		assemblyModule = GetModuleHandleW(L"GameAssembly.dll");
		if (!assemblyModule)
		{
			throw std::exception("Failed to get GameAssembly.dll module handle");
		}
		for (auto &m : il2cppMethodMap)
		{
			m.second = GetProcAddress(assemblyModule, m.first.c_str());
			if (!m.second)
			{
				throw std::exception(("Failed to get " + m.first + " function address").c_str());
			}
		}
	}

	inline void SetMapMethodAddress(std::string method, void *newAddress)
	{
		if (il2cppMethodMap.find(method) == il2cppMethodMap.end())
			return;
		il2cppMethodMap[method] = newAddress;
	}

	inline Il2CppDomain *GetDomain() { return ((Il2CppDomain * (*)(void)) il2cppMethodMap["il2cpp_domain_get"])(); }
	inline const Il2CppAssembly *OpenDomainAssembly(Il2CppDomain *domain, const char *name) { return ((const Il2CppAssembly *(*)(Il2CppDomain *, const char *))il2cppMethodMap["il2cpp_domain_assembly_open"])(domain, name); }
	inline const Il2CppAssembly **GetAssemblies(const Il2CppDomain *domain, size_t *size) { return ((const Il2CppAssembly **(*)(const Il2CppDomain *, size_t *))il2cppMethodMap["il2cpp_domain_get_assemblies"])(domain, size); }

	inline char *GetTypeName(const Il2CppType *type) { return ((char *(*)(const Il2CppType *))il2cppMethodMap["il2cpp_type_get_name"])(type); }
	inline bool TypeIsByref(const Il2CppType *type) { return ((bool (*)(const Il2CppType *))il2cppMethodMap["il2cpp_type_is_byref"])(type); }
	inline uint32_t GetTypeAttrs(const Il2CppType *type) { return ((uint32_t(*)(const Il2CppType *))il2cppMethodMap["il2cpp_type_get_attrs"])(type); }
	inline Il2CppClass *GetTypeClassOrElementClass(const Il2CppType *type) { return ((Il2CppClass * (*)(const Il2CppType *)) il2cppMethodMap["il2cpp_type_get_class_or_element_class"])(type); }

	inline Il2CppThread *AttachThread(Il2CppDomain *domain) { return ((Il2CppThread * (*)(Il2CppDomain *)) il2cppMethodMap["il2cpp_thread_attach"])(domain); }
	inline void DetachThread(Il2CppThread *thread) { ((void (*)(Il2CppThread *))il2cppMethodMap["il2cpp_thread_detach"])(thread); }

	inline Il2CppString *NewString(const char *string) { return ((Il2CppString * (*)(const char *)) il2cppMethodMap["il2cpp_string_new"])(string); }
	inline Il2CppChar *GetChars(Il2CppString *string) { return ((Il2CppChar * (*)(Il2CppString *)) il2cppMethodMap["il2cpp_string_chars"])(string); }
	inline int32_t GetStringLength(Il2CppString *string) { return ((int32_t(*)(Il2CppString *))il2cppMethodMap["il2cpp_string_length"])(string); }

	inline void DisableGC() { ((void (*)(void))il2cppMethodMap["il2cpp_gc_disable"])(); }

	inline const MethodInfo *GetClassMethods(Il2CppClass *klass, void **iter) { return ((const MethodInfo *(*)(Il2CppClass *, void **))il2cppMethodMap["il2cpp_class_get_methods"])(klass, iter); }
	inline Il2CppClass *GetClassFromName(const Il2CppImage *image, const char *nameSpace, const char *name) { return ((Il2CppClass * (*)(const Il2CppImage *, const char *, const char *)) il2cppMethodMap["il2cpp_class_from_name"])(image, nameSpace, name); }
	inline const char *GetClassName(Il2CppClass *klass) { return ((const char *(*)(Il2CppClass *))il2cppMethodMap["il2cpp_class_get_name"])(klass); }
	inline const char *GetClassNamespace(Il2CppClass *klass) { return ((const char *(*)(Il2CppClass *))il2cppMethodMap["il2cpp_class_get_namespace"])(klass); }
	inline const Il2CppImage *GetClassImage(Il2CppClass *klass) { return ((const Il2CppImage *(*)(Il2CppClass *))il2cppMethodMap["il2cpp_class_get_image"])(klass); }
	inline FieldInfo *GetClassFields(Il2CppClass *klass, void **iter) { return ((FieldInfo * (*)(Il2CppClass *, void **)) il2cppMethodMap["il2cpp_class_get_fields"])(klass, iter); }
	inline Il2CppClass *GetClassParent(Il2CppClass *klass) { return ((Il2CppClass * (*)(Il2CppClass *)) il2cppMethodMap["il2cpp_class_get_parent"])(klass); }
	inline Il2CppType *GetClassNestedTypes(Il2CppClass *klass, void **iter) { return ((Il2CppType * (*)(Il2CppClass *, void **)) il2cppMethodMap["il2cpp_class_get_nested_types"])(klass, iter); }
	inline int GetClassFlags(const Il2CppClass *klass) { return ((int (*)(const Il2CppClass *))il2cppMethodMap["il2cpp_class_get_flags"])(klass); }
	inline bool ClassIsGeneric(const Il2CppClass *klass) { return ((bool (*)(const Il2CppClass *))il2cppMethodMap["il2cpp_class_is_generic"])(klass); }
	inline bool ClassIsEnum(const Il2CppClass *klass) { return ((bool (*)(const Il2CppClass *))il2cppMethodMap["il2cpp_class_is_enum"])(klass); }
	inline bool ClassIsValueType(const Il2CppClass *klass) { return ((bool (*)(const Il2CppClass *))il2cppMethodMap["il2cpp_class_is_valuetype"])(klass); }
	inline Il2CppClass *GetClassFromType(const Il2CppType *type) { return ((Il2CppClass * (*)(const Il2CppType *)) il2cppMethodMap["il2cpp_class_from_il2cpp_type"])(type); }
	inline const Il2CppType *GetClassType(const Il2CppClass *klass) { return ((const Il2CppType *(*)(const Il2CppClass *))il2cppMethodMap["il2cpp_class_get_type"])(klass); }
	inline Il2CppClass *GetClassElementClass(Il2CppClass *klass) { return ((Il2CppClass * (*)(Il2CppClass *)) il2cppMethodMap["il2cpp_class_get_element_class"])(klass); }

	inline const Il2CppImage *GetAssemblyImage(const Il2CppAssembly *assembly) { return ((const Il2CppImage *(*)(const Il2CppAssembly *))il2cppMethodMap["il2cpp_assembly_get_image"])(assembly); }

	inline const char *GetMethodName(const MethodInfo *method) { return ((const char *(*)(const MethodInfo *))il2cppMethodMap["il2cpp_method_get_name"])(method); }
	inline const Il2CppType *GetMethodReturnType(const MethodInfo *method) { return ((const Il2CppType *(*)(const MethodInfo *))il2cppMethodMap["il2cpp_method_get_return_type"])(method); }
	inline uint32_t GetMethodParamCount(const MethodInfo *method) { return ((uint32_t(*)(const MethodInfo *))il2cppMethodMap["il2cpp_method_get_param_count"])(method); }
	inline const Il2CppType *GetMethodParam(const MethodInfo *method, uint32_t index) { return ((const Il2CppType *(*)(const MethodInfo *, uint32_t))il2cppMethodMap["il2cpp_method_get_param"])(method, index); }
	inline uint32_t GetMethodFlags(const MethodInfo *method, uint32_t *iflags) { return ((uint32_t(*)(const MethodInfo *, uint32_t *))il2cppMethodMap["il2cpp_method_get_flags"])(method, iflags); }
	inline const char *GetMethodParamName(const MethodInfo *method, uint32_t index) { return ((const char *(*)(const MethodInfo *, uint32_t))il2cppMethodMap["il2cpp_method_get_param_name"])(method, index); }

	inline const Il2CppAssembly *GetImageAssembly(const Il2CppImage *image) { return ((const Il2CppAssembly *(*)(const Il2CppImage *))il2cppMethodMap["il2cpp_image_get_assembly"])(image); }
	inline size_t GetImageClassCount(const Il2CppImage *image) { return ((size_t(*)(const Il2CppImage *))il2cppMethodMap["il2cpp_image_get_class_count"])(image); }
	inline const Il2CppClass *GetImageClass(const Il2CppImage *image, size_t index) { return ((const Il2CppClass *(*)(const Il2CppImage *, size_t))il2cppMethodMap["il2cpp_image_get_class"])(image, index); }

	inline const char *GetFieldName(FieldInfo *field) { return ((const char *(*)(FieldInfo *))il2cppMethodMap["il2cpp_field_get_name"])(field); }
	inline int GetFieldFlags(FieldInfo *field) { return ((int (*)(FieldInfo *))il2cppMethodMap["il2cpp_field_get_flags"])(field); }
	inline const Il2CppType *GetFieldType(FieldInfo *field) { return ((const Il2CppType *(*)(FieldInfo *))il2cppMethodMap["il2cpp_field_get_type"])(field); }
	inline size_t GetFieldOffset(FieldInfo *field) { return ((size_t(*)(FieldInfo *))il2cppMethodMap["il2cpp_field_get_offset"])(field); }
	inline void GetFieldValue(void* obj, FieldInfo* field, void* value) { return ((void (*)(void*, FieldInfo*, void*))il2cppMethodMap["il2cpp_field_get_value"])(obj, field, value); }
	inline void SetFieldValue(void* obj, FieldInfo* field, void* value) { return ((void (*)(void*, FieldInfo *, void *))il2cppMethodMap["il2cpp_field_set_value"])(obj, field, value); }
	inline void GetStaticFieldValue(FieldInfo *field, void *value) { ((void (*)(FieldInfo *, void *))il2cppMethodMap["il2cpp_field_static_get_value"])(field, value); }
	inline void SetStaticFieldValue(FieldInfo* field, void* value) { ((void (*)(FieldInfo*, void*))il2cppMethodMap["il2cpp_field_static_set_value"])(field, value); }
};

namespace Signature
{
	namespace Class
	{
		inline std::string Create(std::string assembly, std::string nameSpace, std::string name)
		{
			return std::string("(") + assembly + ")" + nameSpace + (nameSpace.empty() ? "" : ".") + name;
		}

		inline std::string Create(Il2CppClass *klass, Il2CppManager il2CppManager)
		{
			struct AssemblyStructure
			{
				char __pad__[sizeof(void *) + sizeof(uint32_t) + sizeof(int32_t) + sizeof(int32_t)];
				const char *name;
			};
			const Il2CppImage *image = il2CppManager.GetClassImage(klass);
			if (!image)
				return std::string();
			const AssemblyStructure *assembly = (AssemblyStructure *)il2CppManager.GetImageAssembly(image);
			if (!assembly)
				return std::string();
			return Create(assembly->name, il2CppManager.GetClassNamespace(klass), il2CppManager.GetClassName(klass));
		}

		inline void Analysis(std::string signature, std::string &assembly, std::string &nameSpace, std::string &name)
		{
			assembly = signature.substr(signature.find("(") + 1, signature.find(")") - signature.find("(") - 1);
			signature = signature.substr(signature.find(")") + 1);
			nameSpace = signature.find(".") == std::string::npos ? "" : signature.substr(0, signature.rfind("."));
			name = signature.substr(signature.rfind(".") + 1);
		}
	}

	namespace Method
	{
		inline void Analysis(std::string signature, std::string &returnKlass, std::string &name, std::vector<std::string> &parameters)
		{
			returnKlass = signature.substr(0, signature.find(" "));
			signature = signature.substr(signature.find(" ") + 1);
			name = signature.substr(0, signature.find("("));
			signature = signature.substr(signature.find("(") + 1);
			if (signature.find(")") == std::string::npos)
				return;
			signature = signature.substr(0, signature.find(")"));

			if (signature.find("MPA_") != std::string::npos)
			{
				int index = std::stoi(signature.substr(signature.find("MPA_") + 4));
				for (int i = 0; i < index; i++)
				{
					parameters.push_back("AUTO");
				}
				return;
			}

			if (signature.size() <= 0)
			{
				parameters = std::vector<std::string>();
				return;
			}

			while (signature.find(",") != std::string::npos)
			{
				parameters.push_back(signature.substr(0, signature.find(",")));
				signature = signature.substr(signature.find(",") + 2);
			}
			parameters.push_back(signature);
		}

		inline std::string Create(const MethodInfo *method, Il2CppManager il2CppManager)
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
	
	namespace Field
	{
		inline std::string Create(FieldInfo* field, Il2CppManager il2CppManager)
		{
			return il2CppManager.GetTypeName(il2CppManager.GetFieldType(field)) + std::string(" ") + il2CppManager.GetFieldName(field);
		}

		inline std::string Create(std::string type, std::string name)
		{
			return type + std::string(" ") + name;
		}

		inline void Analysis(std::string signature, std::string& type, std::string& name)
		{
			type = signature.substr(0, signature.find(" "));
			name = signature.substr(signature.find(" ") + 1);
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

	inline void RestoreKlass(std::string& assembly, std::string& nameSpace, std::string& name)
	{
		for (const auto &k : klass)
		{
			if (k.assembly.compare(assembly) == 0 && k.nameSpace.compare(nameSpace) == 0 && k.originalName.compare(name) == 0)
			{
				name = k.confusedName;
				nameSpace = k.nameSpace;
				assembly = k.assembly;
				return;
			}
		}
	}

	inline std::string RestoreMethod(std::string klassSignature, std::string methodName)
	{
		std::string assembly, nameSpace, name;
		Signature::Class::Analysis(klassSignature, assembly, nameSpace, name);
		for (auto &m : method)
		{
			if (m.klass.assembly.compare(assembly) == 0 && m.klass.nameSpace.compare(nameSpace) == 0 && (m.klass.confusedName.compare(name) == 0 || m.klass.originalName.compare(name) == 0) && m.originalName.compare(methodName) == 0)
				return m.confusedName;
		}
		return methodName;
	}
}

class NaResolver
{
public:
	struct Config
	{
		bool enableLogger = false;
		bool attachedThread = true;
		
		struct LoggerConfig
		{
			void (*fatal)(std::string, ...) = NULL;
			void (*info)(std::string, ...) = NULL;
			void (*debug)(std::string, ...) = NULL;
			void (*error)(std::string, ...) = NULL;
		} logger;
	};

	using AssemblyMap = std::unordered_map<std::string, const Il2CppAssembly*>;
	using ClassPair = std::pair<Il2CppClass*, const Il2CppType*>;
	using ClassPathMap = std::unordered_map<std::string,	// Assembly
			std::unordered_map<std::string,					// Namespace
			std::unordered_map<std::string, ClassPair>		// Class
		>
	>;
	using FieldMap = std::unordered_map<std::string, std::unordered_map<std::string, FieldInfo*>>;
	
	Il2CppDomain* domain = nullptr;
	Il2CppThread* attachedThread = nullptr;
	Il2CppManager il2CppManager;
	AssemblyMap assemblies;
	ClassPathMap classes;
	FieldMap fields;
	
	inline NaResolver();
	inline bool Setup(Config config = NaResolver::Config());
	inline void Destroy();
	inline Il2CppClass *GetClass(std::string assembly, std::string nameSpace, std::string name);
	inline Il2CppMethodPointer GetMethod(Il2CppClass *klass, std::string signature);
	inline FieldInfo *GetField(Il2CppClass *klass, std::string type, std::string name);
	inline const Il2CppType *GetType(std::string signature);
	inline const Il2CppType *GetType(Il2CppClass *klass);
	inline std::string StringConvert(Il2CppString *string);
private:
	inline bool CheckClassExistsInCache(std::string assembly, std::string nameSpace, std::string signature);
	inline bool CheckFieldExistsInCache(std::string klassSignature, std::string fieldSignature);
	inline void InsertClassToCache(Il2CppClass* klass, std::string signature, std::string assembly, std::string nameSpace);
	inline void InsertFieldToCache(FieldInfo* field, std::string signature);
	inline const Il2CppAssembly *GetAssembly(std::string name);
	inline bool MethodVerifyParams(const MethodInfo *method, std::vector<std::string> parameters);

	void (*LogFatal)(std::string, ...) = (decltype(LogFatal))([](std::string, ...) -> void {});
	void (*LogInfo)(std::string, ...) = (decltype(LogFatal))([](std::string, ...) -> void {});
	void (*LogDebug)(std::string, ...) = (decltype(LogFatal))([](std::string, ...) -> void {});
	void (*LogError)(std::string, ...) = (decltype(LogFatal))([](std::string, ...) -> void {});
};

inline NaResolver *Il2CppResolver = new NaResolver();

NaResolver::NaResolver()
{
	domain = nullptr;
	assemblies = AssemblyMap();
	classes = ClassPathMap();
	fields = FieldMap();
}

inline bool NaResolver::Setup(Config config)
{
	if (config.enableLogger)
	{
		if (config.logger.fatal != NULL)
			LogFatal = config.logger.fatal;
		if (config.logger.error != NULL)
			LogError = config.logger.error;
		if (config.logger.info != NULL)
			LogInfo = config.logger.info;
		if (config.logger.debug != NULL)
			LogDebug = config.logger.debug;
	}

	try
	{
		il2CppManager.ImportMethods();
	}
	catch (const std::exception &e)
	{
		LogFatal("[NaResolver] Failed to import il2cpp methods: %s", e.what());
		return false;
	}

	if (domain = il2CppManager.GetDomain(), domain == nullptr)
	{
		LogFatal("[NaResolver] Domain get failed.");
		return false;
	}

	if (config.attachedThread)
	{
		attachedThread = il2CppManager.AttachThread(domain);
	}
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

	LogInfo("[NaResolver] Destroy success.");
}

inline Il2CppClass *NaResolver::GetClass(std::string assembly, std::string nameSpace, std::string name)
{
	std::string signature = Signature::Class::Create(assembly, nameSpace, name);

	if (CheckClassExistsInCache(assembly, nameSpace, signature))
	{
		if (nameSpace.compare("") == 0)
		{
			nameSpace = "__NO_NAMESPACE__";
		}
		return classes[assembly][nameSpace][signature].first;
	}

	ConfusedTranslate::RestoreKlass(assembly, nameSpace, name);

	const Il2CppAssembly *il2cppAssembly = GetAssembly(assembly);
	if (il2cppAssembly == nullptr)
	{
		LogError("[NaResolver] Get assembly failed (%s).", assembly.c_str());
		return nullptr;
	}

	const Il2CppImage *il2CppImage = il2CppManager.GetAssemblyImage(il2cppAssembly);
	if (il2CppImage == nullptr)
	{
		LogError("[NaResolver] pImage is null for %s.", signature.c_str());
		return nullptr;
	}

	Il2CppClass *il2CppClass = il2CppManager.GetClassFromName(il2CppImage, nameSpace.c_str(), name.c_str());
	if (il2CppClass == nullptr)
	{
		LogError("[NaResolver] pClass is null for %s.", signature.c_str());
		return nullptr;
	}
	
	InsertClassToCache(il2CppClass, signature, assembly, nameSpace);
	
	LogInfo("[NaResolver] Find class: %s", signature.c_str());
	return il2CppClass;
}

inline Il2CppMethodPointer NaResolver::GetMethod(Il2CppClass *klass, std::string signature)
{
	if (klass == nullptr)
	{
		return nullptr;
	}
	std::string name = "";
	std::string returnType = "";
	std::vector<std::string> parameters = std::vector<std::string>();
	Signature::Method::Analysis(signature, returnType, name, parameters);
	name = ConfusedTranslate::RestoreMethod(Signature::Class::Create(klass, il2CppManager), name);

	void *iterator = nullptr;
	const MethodInfo *method = nullptr;

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
#if defined(__NARESOLVER_ONLY_API)
		return *(void **)method;
#else
		return method->methodPointer;
#endif
	}
	LogFatal("[NaResolver] Could not find the method: %s", signature.c_str());
	return nullptr;
}

inline FieldInfo *NaResolver::GetField(Il2CppClass *klass, std::string type, std::string name)
{
	if (klass == nullptr)
	{
		return nullptr;
	}
	std::string klassSignature = Signature::Class::Create(klass, il2CppManager);
	std::string fieldSignature = Signature::Field::Create(type, name);
	if (CheckFieldExistsInCache(klassSignature, fieldSignature))
	{
		return fields[klassSignature][fieldSignature];
	}

	void *iterator = nullptr;
	FieldInfo *field = nullptr;

	while ((field = il2CppManager.GetClassFields(klass, &iterator)) != nullptr)
	{
		std::string fieldName = il2CppManager.GetFieldName(field);
		if (fieldName.compare(name) != 0)
			continue;
		std::string fieldTypeName = il2CppManager.GetTypeName(il2CppManager.GetFieldType(field));
		if (fieldTypeName.compare(type) != 0)
			continue;
		InsertFieldToCache(field, fieldSignature);
		LogInfo("[NaResolver] Find field: %s", name.c_str());
		return field;
	}
	LogFatal("[NaResolver] Could not find the field: %s", name.c_str());
	return nullptr;
}

inline const Il2CppType *NaResolver::GetType(std::string signature)
{
	std::string assembly, nameSpace, name;
	Signature::Class::Analysis(signature, assembly, nameSpace, name);
	Il2CppClass *klass = GetClass(assembly, nameSpace, name);
	if (klass == nullptr)
	{
		return nullptr;
	}
	const Il2CppType* type = il2CppManager.GetClassType(klass);
	if (type == nullptr)
	{
		return nullptr;
	}
	
	return type;
}

inline const Il2CppType *NaResolver::GetType(Il2CppClass *klass)
{
	if (klass == nullptr)
	{
		return nullptr;
	}
	return il2CppManager.GetClassType(klass);
}

inline std::string NaResolver::StringConvert(Il2CppString *string)
{
	if (!string)
	{
		return std::string();
	}
	char16_t *chars = (char16_t *)il2CppManager.GetChars(string);
	int len = (int)il2CppManager.GetStringLength(string);
	int size = WideCharToMultiByte(CP_UTF8, 0, (LPCWCH)chars, len, NULL, 0, NULL, NULL);
	char *buffer = new char[size + 1];
	WideCharToMultiByte(CP_UTF8, 0, (LPCWCH)chars, len, buffer, size, NULL, NULL);
	buffer[size] = '\0';
	std::string result = std::string(buffer);
	delete[] buffer;
	return result;
}

inline void NaResolver::InsertClassToCache(Il2CppClass* klass, std::string signature, std::string assembly, std::string nameSpace)
{
	if (nameSpace.compare("") == 0)
	{
		nameSpace = "__NO_NAMESPACE__";
	}
	if (classes.find(assembly) == classes.end())
	{
		classes.insert(std::make_pair(assembly, std::unordered_map<std::string, std::unordered_map<std::string, ClassPair>>()));
	}
	if (classes[assembly].find(nameSpace) == classes[assembly].end())
	{
		classes[assembly].insert(std::make_pair(nameSpace, std::unordered_map<std::string, ClassPair>()));
	}
	classes[assembly][nameSpace].insert(std::make_pair(signature, std::make_pair(klass, GetType(klass))));
}

inline void NaResolver::InsertFieldToCache(FieldInfo* field, std::string signature)
{
	if (fields.find(signature) == fields.end())
	{
		fields.insert(std::make_pair(signature, std::unordered_map<std::string, FieldInfo*>()));
	}
	if (fields[signature].find(signature) == fields[signature].end())
	{
		std::string fieldSignature = Signature::Field::Create(field, il2CppManager);
		fields[signature].insert(std::make_pair(fieldSignature, field));
	}
}

inline bool NaResolver::CheckClassExistsInCache(std::string assembly, std::string nameSpace, std::string signature)
{
	if (nameSpace.compare("") == 0)
	{
		nameSpace = "__NO_NAMESPACE__";
	}
	if (classes.find(assembly) == classes.end())
	{
		return false;
	}
	if (classes[assembly].find(nameSpace) == classes[assembly].end())
	{
		return false;
	}
	return classes[assembly][nameSpace].find(signature) != classes[assembly][nameSpace].end();
}

inline bool NaResolver::CheckFieldExistsInCache(std::string klassSignature, std::string fieldSignature)
{
	if (fields.find(klassSignature) == fields.end())
	{
		return false;
	}
	return fields[klassSignature].find(fieldSignature) != fields[klassSignature].end();
}

inline const Il2CppAssembly *NaResolver::GetAssembly(std::string name)
{
	if (assemblies.find(name) != assemblies.end())
	{
		return assemblies[name];
	}
	const Il2CppAssembly *assembly = il2CppManager.OpenDomainAssembly(domain, name.c_str());
	if (!assembly)
	{
		return nullptr;
	}
	assemblies[name] = assembly;
	return assembly;
}

inline bool NaResolver::MethodVerifyParams(const MethodInfo *method, std::vector<std::string> parameters)
{
	uint32_t methodParamCount = il2CppManager.GetMethodParamCount(method);
	if (methodParamCount != parameters.size())
	{
		return false;
	}
	for (uint32_t j = 0; j < methodParamCount; j++)
	{
		if (parameters[j].compare("AUTO") == 0)
		{
			continue;
		}
		std::string parameterName = il2CppManager.GetTypeName(il2CppManager.GetMethodParam(method, j));
		if (parameterName.compare(parameters[j]) != 0)
		{
			return false;
		}
	}
	return true;
}

#pragma warning(disable:4715)


#define STATIC_AREA_OFFSET (sizeof(void *) * 0x17)
#define CLASS(assembly, namespaze, klass) \
	static struct Il2CppClass *ThisClass() { return Il2CppResolver->GetClass(assembly, namespaze, klass); }\
	static uint64_t ThisClassStaticArea() { return *(uint64_t*)((uint64_t)ThisClass() + STATIC_AREA_OFFSET); }
#define STATIC_MEMBER(klass, name, offset)\
	static klass get_##name() { auto ptr = *(uintptr_t*)(((uintptr_t)ThisClass()) + STATIC_AREA_OFFSET); if (ptr != NULL) return *(klass*)(ptr + offset); } \
	static void set_##name(klass value) { *reinterpret_cast<klass *>(*reinterpret_cast<uintptr_t *>((uintptr_t)ThisClass() + STATIC_AREA_OFFSET) + offset) = value; }
#define METHOD(returnType, parameters, signature) static auto function = (returnType(*) parameters)(Il2CppResolver->GetMethod(ThisClass(), signature));
#define MEMBER(klass, name, offset) \
	struct                          \
	{                               \
		char __pad__##name[offset]; \
		klass name;                 \
	}
#define NewStdString(str) Il2CppResolver->StringConvert(str)
#define NewIl2CppString(str) Il2CppResolver->il2CppManager.NewString(str)

#endif
