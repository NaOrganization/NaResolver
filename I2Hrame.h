//**************************************//
// Hi I2Hrame - Il2Cpp Hack Framework   //
// Author: MidTerm                   	//
// Version: v1.3.2.1                    //
// License: MIT                         //
//**************************************//

#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <codecvt>

class CI2Hrame
{
public:
	enum class LogLevel
	{
		Error,
		Warnning,
		Info,
		Debug,
		ALL
	};
	struct LogItem
	{
		LogLevel level;
		std::string message;
	};
	enum LogMode
	{
		None,
		CONSOLE,
		MESSAGEBOX,
		ALL
	};
	std::vector<LogItem> m_Logs;
private:
	const bool m_bCloseGC = false;
	const LogLevel m_LogLevel = LogLevel::ALL;
	const LogMode m_LogMode = LogMode::ALL;
	void Log(LogLevel level, const char* message, ...)
	{
		LogItem item;
		item.level = level;
		va_list args;
		va_start(args, message);
		char buffer[1024];
		vsprintf_s(buffer, message, args);
		va_end(args);
		item.message = buffer;
		m_Logs.push_back(item);
		if (m_LogLevel >= level && m_LogMode != LogMode::None)
		{
			switch (level)
			{
			case LogLevel::Error:
				if (m_LogMode == LogMode::CONSOLE || m_LogMode == LogMode::ALL)
					printf("[I2Hrame] [Error] %s", buffer);
				if (m_LogMode == LogMode::MESSAGEBOX || m_LogMode == LogMode::ALL)
					MessageBoxA(NULL, buffer, "[I2Hrame] Error", MB_ICONERROR);
				break;
			case LogLevel::Warnning:
				if (m_LogMode == LogMode::CONSOLE || m_LogMode == LogMode::ALL)
					printf("[I2Hrame] [Warnning] %s", buffer);
				if (m_LogMode == LogMode::MESSAGEBOX || m_LogMode == LogMode::ALL)
					MessageBoxA(NULL, buffer, "[I2Hrame] Warnning", MB_ICONWARNING);
				break;
			case LogLevel::Info:
				if (m_LogMode == LogMode::CONSOLE || m_LogMode == LogMode::ALL)
					printf("[I2Hrame] [Info] %s", buffer);
				if (m_LogMode == LogMode::MESSAGEBOX || m_LogMode == LogMode::ALL)
					MessageBoxA(NULL, buffer, "[I2Hrame] Info", MB_ICONINFORMATION);
				break;
			case LogLevel::Debug:
				if (m_LogMode == LogMode::CONSOLE || m_LogMode == LogMode::ALL)
					printf("[I2Hrame] [Debug] %s", buffer);
				if (m_LogMode == LogMode::MESSAGEBOX || m_LogMode == LogMode::ALL)
					MessageBoxA(NULL, buffer, "[I2Hrame] Debug", MB_ICONINFORMATION);
				break;
			}
		}
	}
public:

	HMODULE m_hModule;
	Il2CppDomain* m_pDomain;
	std::unordered_map<std::string, Il2CppAssembly*> m_mAssemblies;
	std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<std::string, Il2CppClass*>>> m_mClasses;
	std::unordered_map<std::string, Il2CppType*> m_mTypes;

	bool Setup();
	Il2CppClass* GetClassEx(std::string assembly, std::string nameSpace, std::string name);
	Il2CppClass* GetClass(std::string signature);
	Il2CppMethodPointer GetMethod(Il2CppClass* pClass, std::string signature);
	Il2CppType* GetType(std::string signature);
	Il2CppType* GetType(Il2CppClass* klass);
	std::string GetStringByIl2Cpp(Il2CppString* str);
};

inline CI2Hrame* I2Hrame = new CI2Hrame();

Il2CppDomain* (*il2cpp_domain_get)();
Il2CppAssembly* (*il2cpp_domain_assembly_open)(Il2CppDomain* domain, const char* name);
Il2CppClass* (*il2cpp_class_from_name)(Il2CppImage* image, const char* namespaze, const char* name);
char* (*il2cpp_type_get_name)(const Il2CppType* type);
Il2CppThread* (*il2cpp_thread_attach)(Il2CppDomain* domain);
Il2CppString* (*il2cpp_string_new)(const char* str);
void (*il2cpp_gc_disable)();
const MethodInfo* (*il2cpp_class_get_methods)(Il2CppClass* klass, void** iter);

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

bool CI2Hrame::Setup()
{
	m_hModule = GetModuleHandleW(L"GameAssembly.dll");
	if (!m_hModule)
	{
		Log(LogLevel::Error, "[-] GetModuleHandleW failed.\n");
		return false;
	}

	il2cpp_domain_get = ((Il2CppDomain * (*)(void))GetProcAddress(m_hModule, "il2cpp_domain_get"));
	il2cpp_domain_assembly_open = ((Il2CppAssembly * (*)(Il2CppDomain*, const char*))GetProcAddress(m_hModule, "il2cpp_domain_assembly_open"));
	il2cpp_class_from_name = ((Il2CppClass * (*)(Il2CppImage*, const char*, const char*))GetProcAddress(m_hModule, "il2cpp_class_from_name"));
	il2cpp_type_get_name = ((char* (*)(const Il2CppType*))GetProcAddress(m_hModule, "il2cpp_type_get_name"));
	il2cpp_thread_attach = ((Il2CppThread * (*)(Il2CppDomain*))GetProcAddress(m_hModule, "il2cpp_thread_attach"));
	il2cpp_string_new = ((Il2CppString * (*)(const char*))GetProcAddress(m_hModule, "il2cpp_string_new"));
	il2cpp_gc_disable = ((void(*)(void))GetProcAddress(m_hModule, "il2cpp_gc_disable"));
	il2cpp_class_get_methods = ((const MethodInfo * (*)(Il2CppClass*, void**))GetProcAddress(m_hModule, "il2cpp_class_get_methods"));

	if (!il2cpp_domain_get || !il2cpp_domain_assembly_open || !il2cpp_class_from_name || !il2cpp_type_get_name || !il2cpp_thread_attach || !il2cpp_string_new || !il2cpp_gc_disable || !il2cpp_class_get_methods)
	{
		Log(LogLevel::Error, "[-] il2cppApi get failed.\n");
		return false;
	}

	if (!(m_pDomain = il2cpp_domain_get()))
	{
		Log(LogLevel::Error, "[-] Domain get failed.\n");
		return false;
	}

	il2cpp_thread_attach(m_pDomain);
	if (m_bCloseGC) il2cpp_gc_disable();
	Log(LogLevel::Info, "[+] Setup success.\n");
	return true;
}

inline Il2CppClass* CI2Hrame::GetClassEx(std::string _assembly, std::string _nameSpace, std::string _name)
{
	std::string assembly = _assembly, nameSpace = _nameSpace, name = _name;
	std::string signature = Signature::Class::Create(assembly, nameSpace, name);
	if (nameSpace.compare("") == 0) nameSpace = "__NO_NAMESPACE__";
	if (m_mClasses.find(assembly) != m_mClasses.end())
		if (m_mClasses[assembly].find(nameSpace) != m_mClasses[assembly].end())
			if (m_mClasses[assembly][nameSpace].find(signature) != m_mClasses[assembly][nameSpace].end())
				return m_mClasses[assembly][nameSpace][signature];
	Signature::Class::Analysis(ConfusedTranslate::RestoreKlass(signature), &assembly, &nameSpace, &name);

	Il2CppAssembly* pAssembly = nullptr;
	if (m_mAssemblies.find(assembly) == m_mAssemblies.end())
	{
		pAssembly = il2cpp_domain_assembly_open(m_pDomain, assembly.c_str());
		if (!pAssembly)
			return nullptr;
		m_mAssemblies[assembly] = pAssembly;
	}
	pAssembly = m_mAssemblies[assembly];

	Il2CppImage* pImage = pAssembly->image;
	if (!pImage)
	{
		Log(LogLevel::Error, "[-] pImage is null for %s.\n", signature.c_str());
		return nullptr;
	}

	Il2CppClass* pClass = il2cpp_class_from_name(pImage, nameSpace.c_str(), name.c_str());
	if (!pClass)
	{
		Log(LogLevel::Error, "[-] pClass is null for %s.\n", signature.c_str());
		return nullptr;
	}
	if (m_mClasses.find(assembly) == m_mClasses.end())
	{
		m_mClasses.insert(std::make_pair(assembly, std::unordered_map<std::string, std::unordered_map<std::string, Il2CppClass*>>()));
	}
	if (nameSpace.compare("") == 0) nameSpace = "__NO_NAMESPACE__";
	if (m_mClasses[assembly].find(nameSpace) == m_mClasses[assembly].end())
	{
		m_mClasses[assembly].insert(std::make_pair(nameSpace, std::unordered_map<std::string, Il2CppClass*>()));
	}
	m_mClasses[assembly][nameSpace].insert(std::make_pair(signature, pClass));
	Log(LogLevel::Info, "[+] Class: %s\n", signature.c_str());
	return pClass;
}

inline Il2CppClass* CI2Hrame::GetClass(std::string signature)
{
	std::string assembly, nameSpace, name;
	Signature::Class::Analysis(ConfusedTranslate::RestoreKlass(signature), &assembly, &nameSpace, &name);
	return GetClassEx(assembly, nameSpace, name);
}

inline Il2CppMethodPointer CI2Hrame::GetMethod(Il2CppClass* pClass, std::string signature)
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
		Log(LogLevel::Info, "[+] Method: %s\n", signature.c_str());
		return pMethod->methodPointer;
	}
	Log(LogLevel::Error, "[-] Could not find the method: %s\n", signature.c_str());
	return nullptr;
}

inline Il2CppType* CI2Hrame::GetType(std::string signature)
{
	if (m_mTypes.find(signature) != m_mTypes.end())
		return m_mTypes[signature];
	static auto func = ((Il2CppType * (*)(Il2CppString * typeName, MethodInfo * method))
		GetMethod(GetClassEx("mscorlib", "System", "Type"), "System.Type GetType(System.String)"));
	Il2CppType* result = func(il2cpp_string_new(signature.c_str()), nullptr);
	if (!result)
	{
		Log(LogLevel::Error, "[-] Could not find the type: %s\n", signature.c_str());
		return nullptr;
	}
	m_mTypes[signature] = result;
	return result;
}

inline Il2CppType* CI2Hrame::GetType(Il2CppClass* klass)
{
	if (!klass) return nullptr;
	std::string signature = ConfusedTranslate::RestoreKlass(Signature::Class::Create(klass));
	std::string assembly, nameSpace, name;
	Signature::Class::Analysis(signature, &assembly, &nameSpace, &name);
	std::string typeSignature = name + ", " + assembly;
	if (nameSpace.size() > 0)
		typeSignature = nameSpace + "." + typeSignature;
	return GetType(typeSignature);
}

inline std::string CI2Hrame::GetStringByIl2Cpp(Il2CppString* str)
{
	if (!str) return "";
	std::u16string u16str = std::u16string(reinterpret_cast<char16_t*>(str->chars));
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
	return convert.to_bytes(u16str);
}
