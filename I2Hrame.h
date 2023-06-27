//**************************************//
// Hi I2Hrame - Il2Cpp Hack Framework   //
// Author: MidTerm                   	//
// Version: v1.4.1                      //
// License: MIT                         //
//**************************************//

#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <codecvt>
#include <NaLibrary/NaLogger/NaLogger.h>

Il2CppDomain* (*il2cpp_domain_get)();
Il2CppAssembly* (*il2cpp_domain_assembly_open)(Il2CppDomain* domain, const char* name);
Il2CppClass* (*il2cpp_class_from_name)(Il2CppImage* image, const char* namespaze, const char* name);
char* (*il2cpp_type_get_name)(const Il2CppType* type);
Il2CppThread* (*il2cpp_thread_attach)(Il2CppDomain* domain);
Il2CppString* (*il2cpp_string_new)(const char* str);
void (*il2cpp_gc_disable)();
const MethodInfo* (*il2cpp_class_get_methods)(Il2CppClass* klass, void** iter);

class I2Hrame
{
public:
	bool m_closeGC;
	NaLogger* m_logger;
	HMODULE m_module;
	Il2CppDomain* m_domain;
	std::unordered_map<std::string, Il2CppAssembly*> m_assemblies;
	std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<std::string, Il2CppClass*>>> m_classes;
	std::unordered_map<std::string, Il2CppType*> m_types;

	bool Setup(bool closeGC, NaLogger* logger);
	Il2CppClass* GetClassEx(std::string assembly, std::string nameSpace, std::string name);
	Il2CppClass* GetClass(std::string signature);
	Il2CppMethodPointer GetMethod(Il2CppClass* pClass, std::string signature);
	Il2CppType* GetType(std::string signature);
	Il2CppType* GetType(Il2CppClass* klass);
	std::string GetStringByIl2Cpp(Il2CppString* str);
};

inline I2Hrame* g_I2Hrame = new I2Hrame();

namespace Signature
{
	namespace Class
	{
		std::string Create(std::string assembly, std::string nameSpace, std::string name)
		{
			return std::string("(") + assembly + ")" + nameSpace + (nameSpace.empty() ? "" : ".") + name;
		}

		std::string Create(Il2CppClass* klass)
		{
			return Create(klass->image->assembly->aname.name, klass->namespaze, klass->name);
		}

		void Analysis(std::string signature, std::string* assembly, std::string* nameSpace, std::string* name)
		{
			*assembly = signature.substr(signature.find("(") + 1, signature.find(")") - signature.find("(") - 1);
			signature = signature.substr(signature.find(")") + 1);
			*nameSpace = signature.find(".") == std::string::npos ? "" : signature.substr(0, signature.rfind("."));
			*name = signature.substr(signature.rfind(".") + 1);
		}
	}

	namespace Method
	{
		void Analysis(std::string signature, std::string* returnKlass, std::string* name, std::vector<std::string>* parameters)
		{
			// format: returntype methodname(parameter1, parameter2, ...)
			// example: Void MethodName(System.Int32, System.String)
			// example: Void MethodName()
			// example: Void MethodName(System.Int32)
			*returnKlass = signature.substr(0, signature.find(" "));
			signature = signature.substr(signature.find(" ") + 1);
			*name = signature.substr(0, signature.find("("));
			signature = signature.substr(signature.find("(") + 1);
			if (signature.find(")") != std::string::npos)
			{
				signature = signature.substr(0, signature.find(")"));
				if (signature.find("MPA_") != std::string::npos)
				{
					int index = std::stoi(signature.substr(signature.find("MPA_") + 4));
					for (int i = 0; i < index; i++)
					{
						parameters->push_back("AUTO");
					}
				}
				else
				{
					if (signature.size() > 0)
					{
						while (signature.find(",") != std::string::npos)
						{
							parameters->push_back(signature.substr(0, signature.find(",")));
							signature = signature.substr(signature.find(",") + 2);
						}
						parameters->push_back(signature);
					}
					else
					{
						parameters = new std::vector<std::string>();
					}
				}
			}
		}

		std::string Create(const MethodInfo* method)
		{
			// format: returntype methodname(parameter1, parameter2, ...)
			// example: System.Void MethodName(System.Int32, System.String)
			// example: Void MethodName()
			// example: Void MethodName(System.Int32)
			std::string signature = il2cpp_type_get_name(method->return_type) + std::string(" ") + method->name + "(";
			for (int i = 0; i < method->parameters_count; i++)
			{
				signature += il2cpp_type_get_name(method->parameters[i].parameter_type) + std::string(", ");
			}
			if (method->parameters_count > 0)
			{
				signature = signature.substr(0, signature.size() - 2);
			}
			signature += ")";
			return signature;
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

	std::vector<Klass> klass =
	{

	};

	std::vector<Method> method =
	{

	};

	std::string RestoreKlass(std::string signature)
	{
		std::string assembly;
		std::string nameSpace;
		std::string name;
		Signature::Class::Analysis(signature, &assembly, &nameSpace, &name);
		for (auto& k : klass)
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
		for (auto& m : method)
		{
			if (m.klass.assembly.compare(assembly) == 0 && m.klass.nameSpace.compare(nameSpace) == 0 && (m.klass.confusedName.compare(name) == 0 || m.klass.originalName.compare(name) == 0) && m.originalName.compare(methodName) == 0)
				return m.confusedName;
		}
		return methodName;
	}

	std::string ConvertKlass(std::string signature)
	{
		std::string assembly;
		std::string nameSpace;
		std::string name;
		Signature::Class::Analysis(signature, &assembly, &nameSpace, &name);
		for (auto& k : klass)
		{
			if (k.assembly.compare(assembly) == 0 && k.nameSpace.compare(nameSpace) == 0 && k.confusedName.compare(name) == 0)
				return Signature::Class::Create(k.assembly, k.nameSpace, k.originalName);
		}
		return signature;
	}
}

bool I2Hrame::Setup(bool closeGC, NaLogger* logger)
{
	m_closeGC = closeGC;
	m_logger = logger;
	if (!logger)
	{
		throw std::exception("Logger is null!");
		return false;
	}
	m_module = GetModuleHandleW(L"GameAssembly.dll");
	if (!m_module)
	{
		m_logger->LogFatal("[I2Hrame] GetModuleHandleW failed.");
		return false;
	}

	il2cpp_domain_get = ((Il2CppDomain * (*)(void)) GetProcAddress(m_module, "il2cpp_domain_get"));
	il2cpp_domain_assembly_open = ((Il2CppAssembly * (*)(Il2CppDomain*, const char*)) GetProcAddress(m_module, "il2cpp_domain_assembly_open"));
	il2cpp_class_from_name = ((Il2CppClass * (*)(Il2CppImage*, const char*, const char*)) GetProcAddress(m_module, "il2cpp_class_from_name"));
	il2cpp_type_get_name = ((char* (*)(const Il2CppType*))GetProcAddress(m_module, "il2cpp_type_get_name"));
	il2cpp_thread_attach = ((Il2CppThread * (*)(Il2CppDomain*)) GetProcAddress(m_module, "il2cpp_thread_attach"));
	il2cpp_string_new = ((Il2CppString * (*)(const char*)) GetProcAddress(m_module, "il2cpp_string_new"));
	il2cpp_gc_disable = ((void (*)(void))GetProcAddress(m_module, "il2cpp_gc_disable"));
	il2cpp_class_get_methods = ((const MethodInfo * (*)(Il2CppClass*, void**))GetProcAddress(m_module, "il2cpp_class_get_methods"));

	if (!il2cpp_domain_get || !il2cpp_domain_assembly_open || !il2cpp_class_from_name || !il2cpp_type_get_name || !il2cpp_thread_attach || !il2cpp_string_new || !il2cpp_gc_disable || !il2cpp_class_get_methods)
	{
		m_logger->LogFatal("[I2Hrame] il2cppApi get failed.");
		return false;
	}

	if (!(m_domain = il2cpp_domain_get()))
	{
		m_logger->LogFatal("[I2Hrame] Domain get failed.");
		return false;
	}

	il2cpp_thread_attach(m_domain);
	if (m_closeGC)
		il2cpp_gc_disable();
	m_logger->LogInfo("[I2Hrame] Setup success.");
	return true;
}

inline Il2CppClass* I2Hrame::GetClassEx(std::string _assembly, std::string _nameSpace, std::string _name)
{
	std::string assembly = _assembly, nameSpace = _nameSpace, name = _name;
	std::string signature = Signature::Class::Create(assembly, nameSpace, name);
	if (nameSpace.compare("") == 0)
		nameSpace = "__NO_NAMESPACE__";
	if (m_classes.find(assembly) != m_classes.end())
		if (m_classes[assembly].find(nameSpace) != m_classes[assembly].end())
			if (m_classes[assembly][nameSpace].find(signature) != m_classes[assembly][nameSpace].end())
				return m_classes[assembly][nameSpace][signature];
	Signature::Class::Analysis(ConfusedTranslate::RestoreKlass(signature), &assembly, &nameSpace, &name);

	Il2CppAssembly* pAssembly = nullptr;
	if (m_assemblies.find(assembly) == m_assemblies.end())
	{
		pAssembly = il2cpp_domain_assembly_open(m_domain, assembly.c_str());
		if (!pAssembly)
			return nullptr;
		m_assemblies[assembly] = pAssembly;
	}
	pAssembly = m_assemblies[assembly];

	Il2CppImage* pImage = pAssembly->image;
	if (!pImage)
	{
		m_logger->LogError("[I2Hrame] pImage is null for %s.", signature.c_str());
		return nullptr;
	}

	Il2CppClass* pClass = il2cpp_class_from_name(pImage, nameSpace.c_str(), name.c_str());
	if (!pClass)
	{
		m_logger->LogError("[I2Hrame] pClass is null for %s.", signature.c_str());
		return nullptr;
	}
	if (m_classes.find(assembly) == m_classes.end())
	{
		m_classes.insert(std::make_pair(assembly, std::unordered_map<std::string, std::unordered_map<std::string, Il2CppClass*>>()));
	}
	if (nameSpace.compare("") == 0)
		nameSpace = "__NO_NAMESPACE__";
	if (m_classes[assembly].find(nameSpace) == m_classes[assembly].end())
	{
		m_classes[assembly].insert(std::make_pair(nameSpace, std::unordered_map<std::string, Il2CppClass*>()));
	}
	m_classes[assembly][nameSpace].insert(std::make_pair(signature, pClass));
	m_logger->LogInfo("[I2Hrame] Find class: %s", signature.c_str());
	return pClass;
}

inline Il2CppClass* I2Hrame::GetClass(std::string signature)
{
	std::string assembly, nameSpace, name;
	Signature::Class::Analysis(ConfusedTranslate::RestoreKlass(signature), &assembly, &nameSpace, &name);
	return GetClassEx(assembly, nameSpace, name);
}

inline Il2CppMethodPointer I2Hrame::GetMethod(Il2CppClass* pClass, std::string signature)
{
	if (!pClass)
		return nullptr;

	std::string name = "";
	std::string returnType = "";
	std::vector<std::string> parameters = std::vector<std::string>();
	Signature::Method::Analysis(signature, &returnType, &name, &parameters);
	name = ConfusedTranslate::RestoreMethod(Signature::Class::Create(pClass), name);

	void* iterator = NULL;
	const MethodInfo* pMethod = NULL;

	while ((pMethod = il2cpp_class_get_methods(pClass, &iterator)) != NULL)
	{
		if (std::string(pMethod->name).compare(name) != 0 && name.compare("AUTO") != 0)
			continue;
		if (std::string(il2cpp_type_get_name(pMethod->return_type)).compare(returnType) != 0 && returnType.compare("AUTO") != 0)
			continue;
		if (pMethod->parameters_count != parameters.size())
			continue;
		bool bParameters = true;
		for (uint32_t j = 0; j < pMethod->parameters_count; j++)
		{
			if (std::string(il2cpp_type_get_name(pMethod->parameters[j].parameter_type)).compare(parameters[j]) != 0 && parameters[j].compare("AUTO") != 0)
			{
				bParameters = false;
				break;
			}
		}
		if (!bParameters)
			continue;
		m_logger->LogInfo("[I2Hrame] Find method: %s", signature.c_str());
		return pMethod->methodPointer;
	}
	m_logger->LogFatal("[I2Hrame] Could not find the method: %s", signature.c_str());
	return nullptr;
}

inline Il2CppType* I2Hrame::GetType(std::string signature)
{
	if (m_types.find(signature) != m_types.end())
		return m_types[signature];
	static auto func = ((Il2CppType * (*)(Il2CppString * typeName, MethodInfo * method))
		GetMethod(GetClassEx("mscorlib", "System", "Type"), "System.Type GetType(System.String)"));
	Il2CppType* result = func(il2cpp_string_new(signature.c_str()), nullptr);
	if (!result)
	{
		m_logger->LogError("[I2Hrame] Could not find the type: %s", signature.c_str());
		return nullptr;
	}
	m_types[signature] = result;
	return result;
}

inline Il2CppType* I2Hrame::GetType(Il2CppClass* klass)
{
	if (!klass)
		return nullptr;
	std::string signature = ConfusedTranslate::RestoreKlass(Signature::Class::Create(klass));
	std::string assembly, nameSpace, name;
	Signature::Class::Analysis(signature, &assembly, &nameSpace, &name);
	std::string typeSignature = name + ", " + assembly;
	if (nameSpace.size() > 0)
		typeSignature = nameSpace + "." + typeSignature;
	return GetType(typeSignature);
}

inline std::string I2Hrame::GetStringByIl2Cpp(Il2CppString* str)
{
	if (!str)
		return "";
	std::u16string u16str = std::u16string(reinterpret_cast<char16_t*>(str->chars));
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
	return convert.to_bytes(u16str);
}
