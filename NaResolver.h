//**************************************//
// Hi NaResolver						//
// Author: MidTerm                   	//
// Version: v1.5                        //
// License: MIT                         //
//**************************************//

#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <codecvt>

Il2CppDomain *(*il2cpp_domain_get)();
Il2CppAssembly *(*il2cpp_domain_assembly_open)(Il2CppDomain *domain, const char *name);
Il2CppClass *(*il2cpp_class_from_name)(Il2CppImage *image, const char *namespaze, const char *name);
char *(*il2cpp_type_get_name)(const Il2CppType *type);
Il2CppThread *(*il2cpp_thread_attach)(Il2CppDomain *domain);
Il2CppString *(*il2cpp_string_new)(const char *str);
void (*il2cpp_gc_disable)();
const MethodInfo *(*il2cpp_class_get_methods)(Il2CppClass *klass, void **iter);

#define CATCH_IL2CPP_FUNCTION(name)                                               \
	name = decltype(name)(GetProcAddress(assemblyModule, #name));                 \
	if (name == nullptr)                                                          \
	{                                                                             \
		LogFatal("[NaResolver] il2cppApi get failed (" + std::string(#name) + ")."); \
		return false;                                                             \
	}

struct NaRConfig
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

class NaResolver
{
public:
	HMODULE assemblyModule;
	Il2CppDomain *domain;
	std::unordered_map<std::string, Il2CppAssembly *> assemblies;
	std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<std::string, Il2CppClass *>>> classes;
	std::unordered_map<std::string, Il2CppType *> types;
	
	NaResolver();
	bool Setup(NaRConfig config);
	void Destroy();
	Il2CppClass *GetClassEx(std::string assembly, std::string nameSpace, std::string name);
	Il2CppClass *GetClass(std::string signature);
	Il2CppMethodPointer GetMethod(Il2CppClass *klass, std::string signature);
	Il2CppType *GetType(std::string signature);
	Il2CppType *GetType(Il2CppClass *klass);
	std::string GetStringByIl2Cpp(Il2CppString *string);

private:
	bool ClassExistsInCache(std::string assembly, std::string nameSpace, std::string name);
	Il2CppAssembly *GetAssembly(std::string name);
	bool MethodVerifyParams(const MethodInfo *method, std::vector<std::string> parameters);
	void (*LogFatal)(std::string, ...);
	void (*LogInfo)(std::string, ...);
	void (*LogDebug)(std::string, ...);
	void (*LogError)(std::string, ...);
};

inline NaResolver* Il2CppResolver = new NaResolver();

namespace Signature
{
	namespace Class
	{
		std::string Create(std::string assembly, std::string nameSpace, std::string name)
		{
			return std::string("(") + assembly + ")" + nameSpace + (nameSpace.empty() ? "" : ".") + name;
		}

		std::string Create(Il2CppClass *klass)
		{
			return Create(klass->image->assembly->aname.name, klass->namespaze, klass->name);
		}

		void Analysis(std::string signature, std::string *assembly, std::string *nameSpace, std::string *name)
		{
			*assembly = signature.substr(signature.find("(") + 1, signature.find(")") - signature.find("(") - 1);
			signature = signature.substr(signature.find(")") + 1);
			*nameSpace = signature.find(".") == std::string::npos ? "" : signature.substr(0, signature.rfind("."));
			*name = signature.substr(signature.rfind(".") + 1);
		}
	}

	namespace Method
	{
		void Analysis(std::string signature, std::string *returnKlass, std::string *name, std::vector<std::string> *parameters)
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

		std::string Create(const MethodInfo *method)
		{
			std::string signature = il2cpp_type_get_name(method->return_type) + std::string(" ") + method->name + "(";

			for (int i = 0; i < method->parameters_count; i++)
			{
				signature += il2cpp_type_get_name(method->parameters[i].parameter_type) + std::string(", ");
			}

			if (method->parameters_count > 0)
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

	std::vector<Klass> klass = std::vector<Klass>();

	std::vector<Method> method = std::vector<Method>();

	std::string RestoreKlass(std::string signature)
	{
		std::string assembly, nameSpace, name;
		Signature::Class::Analysis(signature, &assembly, &nameSpace, &name);
		for (auto &k : klass)
		{
			if (k.assembly.compare(assembly) == 0 && k.nameSpace.compare(nameSpace) == 0 && k.originalName.compare(name) == 0)
				return Signature::Class::Create(k.assembly, k.nameSpace, k.confusedName);
		}
		return signature;
	}

	std::string RestoreMethod(std::string klassSignature, std::string methodName)
	{
		std::string assembly, nameSpace, name;
		Signature::Class::Analysis(klassSignature, &assembly, &nameSpace, &name);
		for (auto &m : method)
		{
			if (m.klass.assembly.compare(assembly) == 0 && m.klass.nameSpace.compare(nameSpace) == 0 && (m.klass.confusedName.compare(name) == 0 || m.klass.originalName.compare(name) == 0) && m.originalName.compare(methodName) == 0)
				return m.confusedName;
		}
		return methodName;
	}

	std::string ConvertKlass(std::string signature)
	{
		std::string assembly, nameSpace, name;
		Signature::Class::Analysis(signature, &assembly, &nameSpace, &name);
		for (auto &k : klass)
		{
			bool allMatch = k.assembly.compare(assembly) == 0 && k.nameSpace.compare(nameSpace) == 0 && k.confusedName.compare(name) == 0;
			if (allMatch)
				return Signature::Class::Create(k.assembly, k.nameSpace, k.originalName);
		}
		return signature;
	}
}

NaResolver::NaResolver()
{
	domain = nullptr;
	assemblies = std::unordered_map<std::string, Il2CppAssembly*>();
	classes = std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<std::string, Il2CppClass*>>>();
	types = std::unordered_map<std::string, Il2CppType*>();
}

bool NaResolver::ClassExistsInCache(std::string assembly, std::string nameSpace, std::string name)
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

Il2CppAssembly *NaResolver::GetAssembly(std::string name)
{
	if (assemblies.find(name) != assemblies.end())
		return assemblies[name];
	Il2CppAssembly *assembly = il2cpp_domain_assembly_open(domain, name.c_str());
	if (!assembly)
		return nullptr;
	assemblies[name] = assembly;
	return assembly;
}

bool NaResolver::MethodVerifyParams(const MethodInfo *method, std::vector<std::string> parameters)
{
	if (method->parameters_count != parameters.size())
		return false;
	for (uint32_t j = 0; j < method->parameters_count; j++)
	{
		if (parameters[j].compare("AUTO") == 0)
			continue;
		std::string parameterName = il2cpp_type_get_name(method->parameters[j].parameter_type);
		if (parameterName.compare(parameters[j]) != 0)
			return false;
	}
	return true;
}

bool NaResolver::Setup(NaRConfig config)
{
	LogFatal = config.logger.fatal;
	LogError = config.logger.error;
	LogInfo = config.logger.info;
	LogDebug = config.logger.debug;
	if (!config.enableLogger)
	{
		LogFatal = LogInfo = LogDebug = LogError = (decltype(LogFatal))([](std::string, ...) -> void {});
	}

	assemblyModule = GetModuleHandleW(L"GameAssembly.dll");
	if (!assemblyModule)
	{
		LogFatal("[NaResolver] GetModuleHandleW failed.");
		return false;
	}

	CATCH_IL2CPP_FUNCTION(il2cpp_domain_get);
	CATCH_IL2CPP_FUNCTION(il2cpp_domain_assembly_open);
	CATCH_IL2CPP_FUNCTION(il2cpp_class_from_name);
	CATCH_IL2CPP_FUNCTION(il2cpp_type_get_name);
	CATCH_IL2CPP_FUNCTION(il2cpp_thread_attach);
	CATCH_IL2CPP_FUNCTION(il2cpp_string_new);
	CATCH_IL2CPP_FUNCTION(il2cpp_gc_disable);
	CATCH_IL2CPP_FUNCTION(il2cpp_class_get_methods);

	if (domain = il2cpp_domain_get(), domain == nullptr)
	{
		LogFatal("[NaResolver] Domain get failed.");
		return false;
	}

	il2cpp_thread_attach(domain);
	if (config.disableGC)
		il2cpp_gc_disable();

	LogInfo("[NaResolver] Setup success.");
	return true;
}

void NaResolver::Destroy()
{
	domain = nullptr;
	assemblies.clear();
	classes.clear();
	types.clear();
	
	LogInfo("[NaResolver] Destroy success.");
}

inline Il2CppClass *NaResolver::GetClassEx(std::string _assembly, std::string _nameSpace, std::string _name)
{
	std::string assembly = _assembly, nameSpace = _nameSpace, name = _name;
	std::string signature = Signature::Class::Create(assembly, nameSpace, name);
	if (nameSpace.compare("") == 0)
		nameSpace = "__NO_NAMESPACE__";

	if (ClassExistsInCache(assembly, nameSpace, name))
		return classes[assembly][nameSpace][signature];

	Signature::Class::Analysis(ConfusedTranslate::RestoreKlass(signature), &assembly, &nameSpace, &name);

	Il2CppAssembly *pAssembly = GetAssembly(assembly);
	if (pAssembly == nullptr)
	{
		LogError("[NaResolver] Get assembly failed (%s).", assembly.c_str());
		return nullptr;
	}

	Il2CppImage *pImage = pAssembly->image;
	if (!pImage)
	{
		LogError("[NaResolver] pImage is null for %s.", signature.c_str());
		return nullptr;
	}

	Il2CppClass *pClass = il2cpp_class_from_name(pImage, nameSpace.c_str(), name.c_str());
	if (!pClass)
	{
		LogError("[NaResolver] pClass is null for %s.", signature.c_str());
		return nullptr;
	}
	if (classes.find(assembly) == classes.end())
	{
		classes.insert(std::make_pair(assembly, std::unordered_map<std::string, std::unordered_map<std::string, Il2CppClass *>>()));
	}
	if (nameSpace.compare("") == 0)
		nameSpace = "__NO_NAMESPACE__";
	if (classes[assembly].find(nameSpace) == classes[assembly].end())
	{
		classes[assembly].insert(std::make_pair(nameSpace, std::unordered_map<std::string, Il2CppClass *>()));
	}
	classes[assembly][nameSpace].insert(std::make_pair(signature, pClass));
	LogInfo("[NaResolver] Find class: %s", signature.c_str());
	return pClass;
}

inline Il2CppClass *NaResolver::GetClass(std::string signature)
{
	std::string assembly, nameSpace, name;
	Signature::Class::Analysis(ConfusedTranslate::RestoreKlass(signature), &assembly, &nameSpace, &name);
	return GetClassEx(assembly, nameSpace, name);
}

inline Il2CppMethodPointer NaResolver::GetMethod(Il2CppClass *klass, std::string signature)
{
	if (klass == nullptr)
		return nullptr;

	std::string name = "";
	std::string returnType = "";
	std::vector<std::string> parameters = std::vector<std::string>();
	Signature::Method::Analysis(signature, &returnType, &name, &parameters);
	name = ConfusedTranslate::RestoreMethod(Signature::Class::Create(klass), name);

	void *iterator = nullptr;
	const MethodInfo *method = nullptr;

	while ((method = il2cpp_class_get_methods(klass, &iterator)) != nullptr)
	{
		std::string methodName = method->name;
		if (methodName.compare(name) != 0 && name.compare("AUTO") != 0)
			continue;
		std::string returnTypeName = il2cpp_type_get_name(method->return_type);
		if (returnTypeName.compare(returnType) != 0 && returnType.compare("AUTO") != 0)
			continue;
		if (!MethodVerifyParams(method, parameters))
			continue;
		LogInfo("[NaResolver] Find method: %s", signature.c_str());
		return method->methodPointer;
	}
	LogFatal("[NaResolver] Could not find the method: %s", signature.c_str());
	return nullptr;
}

inline Il2CppType *NaResolver::GetType(std::string signature)
{
	if (types.find(signature) != types.end())
		return types[signature];
	static auto func = ((Il2CppType * (*)(Il2CppString * typeName, MethodInfo * method))
							GetMethod(GetClassEx("mscorlib", "System", "Type"), "System.Type GetType(System.String)"));
	Il2CppType *result = func(il2cpp_string_new(signature.c_str()), nullptr);
	if (!result)
	{
		LogError("[NaResolver] Could not find the type: %s", signature.c_str());
		return nullptr;
	}
	types[signature] = result;
	return result;
}

inline Il2CppType *NaResolver::GetType(Il2CppClass *klass)
{
	if (klass == nullptr)
		return nullptr;
	std::string signature = ConfusedTranslate::RestoreKlass(Signature::Class::Create(klass));
	std::string assembly, nameSpace, name;
	Signature::Class::Analysis(signature, &assembly, &nameSpace, &name);
	std::string typeSignature = name + ", " + assembly;
	if (nameSpace.size() > 0)
		typeSignature = nameSpace + "." + typeSignature;
	return GetType(typeSignature);
}

inline std::string NaResolver::GetStringByIl2Cpp(Il2CppString *string)
{
	if (!string)
	{
		return std::string();
	}

	return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>().to_bytes(std::u16string(reinterpret_cast<char16_t *>(string->chars)));
}
