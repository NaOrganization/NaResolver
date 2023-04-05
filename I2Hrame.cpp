#include "I2Hrame.h"
#include <wtypes.h>
#include <string>

Il2CppDomain* (*il2cpp_domain_get)();
Il2CppAssembly* (*il2cpp_domain_assembly_open)(Il2CppDomain* domain, const char* name);
Il2CppClass* (*il2cpp_class_from_name)(Il2CppImage* image, const char* namespaze, const char* name);
char* (*il2cpp_type_get_name)(const Il2CppType* type);
uint32_t(*il2cpp_method_get_param_count)(const MethodInfo* method);
Il2CppThread* (*il2cpp_thread_attach)(Il2CppDomain* domain);

struct WString
{
	void* klass;
	void* monitor;
	size_t length;
	wchar_t string[1024];

	WString()
	{
		klass = nullptr;
		monitor = nullptr;
		length = 0;
		string[0] = 0;
	}

	WString(std::string str)
	{
		length = str.length();
		for (size_t i = 0; i < length; i++)
			string[i] = str[i];
	}
	std::string ToStr()
	{
		std::wstring wstr = std::wstring(string);
		return std::string(wstr.begin(), wstr.end());
	}
};

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
}

bool CI2Hrame::Setup()
{
	HMODULE moduleHandle = GetModuleHandleW(L"GameAssembly.dll");
	if (!moduleHandle)
		return false;
	if (!(il2cpp_domain_get = (Il2CppDomain * (*)(void))GetProcAddress(moduleHandle, "il2cpp_domain_get")))
		return false;
	if (!(il2cpp_domain_assembly_open = (Il2CppAssembly * (*)(Il2CppDomain*, const char*))GetProcAddress(moduleHandle, "il2cpp_domain_assembly_open")))
		return false;
	if (!(il2cpp_class_from_name = (Il2CppClass * (*)(Il2CppImage*, const char*, const char*))GetProcAddress(moduleHandle, "il2cpp_class_from_name")))
		return false;
	if (!(il2cpp_type_get_name = (char* (*)(const Il2CppType*))GetProcAddress(moduleHandle, "il2cpp_type_get_name")))
		return false;
	if (!(il2cpp_method_get_param_count = (uint32_t(*)(const MethodInfo*))GetProcAddress(moduleHandle, "il2cpp_method_get_param_count")))
		return false;
	if (!(il2cpp_thread_attach = (Il2CppThread*(*)(Il2CppDomain*))GetProcAddress(moduleHandle, "il2cpp_thread_attach")))
		return false;
	
	m_pDomain = il2cpp_domain_get();
	if (!m_pDomain)
		return false;
	il2cpp_thread_attach(m_pDomain);

	if (!I2Hrame->GetMethod(I2Hrame->GetClassEx("UnityEngine.CoreModule", "UnityEngine", "GameObject"), "UnityEngine.Transform get_transform()"))
	{
		printf("[-] Maybe you using a wrong version of il2cppType.\n");
		return false;
	}
	return true;
}

Il2CppClass* CI2Hrame::GetClassEx(std::string _assembly, std::string _nameSpace, std::string _name)
{
	std::string assembly = _assembly, nameSpace = _nameSpace, name = _name;
	std::string signature = Signature::Class::Create(assembly, nameSpace, name);
	if (m_mClasses.find(signature) != m_mClasses.end())
		return m_mClasses[signature];
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
		printf("[*] [-] (pImage is null) Class: %s\n", signature.c_str());
		return nullptr;
	}

	Il2CppClass* pClass = il2cpp_class_from_name(pImage, nameSpace.c_str(), name.c_str());
	if (!pClass)
	{
		printf("[*] [-] (pClass is null) Class: %s\n", signature.c_str());
		return nullptr;
	}
	
	m_mClasses[signature] = pClass;
	printf("[*] Class: %s\n", signature.c_str());
	return pClass;
}

Il2CppClass* CI2Hrame::GetClass(std::string signature)
{
	std::string assembly, nameSpace, name;
	Signature::Class::Analysis(ConfusedTranslate::RestoreKlass(signature), &assembly, &nameSpace, &name);
	return GetClassEx(assembly, nameSpace, name);
}

Il2CppMethodPointer CI2Hrame::GetMethod(Il2CppClass* pClass, std::string signature)
{
	if (!pClass)
		return nullptr;

	std::string name = "";
	std::string returnType = "";
	std::vector<std::string> parameters = std::vector<std::string>();
	Signature::Method::Analysis(signature, &returnType, &name, &parameters);
	name = ConfusedTranslate::RestoreMethod(Signature::Class::Create(pClass), name);

	for (uint32_t i = 0; i < pClass->method_count; i++)
	{
		const MethodInfo* pMethod = pClass->methods[i];
		if (!pMethod)
			continue;
		if (std::string(pMethod->name).compare(name) != 0 && returnType.compare("AUTO") != 0)
			continue;
		if (std::string(il2cpp_type_get_name(pMethod->return_type)).compare(returnType) != 0 && returnType.compare("AUTO") != 0)
			continue;
		bool bParameters = true;
		for (uint32_t j = 0; j < il2cpp_method_get_param_count(pMethod); j++)
		{
			if (std::string(il2cpp_type_get_name(pMethod->parameters[j].parameter_type)).compare(parameters[j]) != 0 && parameters[j].compare("AUTO") != 0)
			{
				bParameters = false;
				break;
			}
		}
		if (!bParameters)
			continue;
		printf("[*] Method: %s\n", signature.c_str());
		return pMethod->methodPointer;
	}
	printf("[*] [-] (Could not find the method) Method: %s\n", signature.c_str());
	return nullptr;
}

Il2CppType* CI2Hrame::GetType(std::string signature)
{
	static auto func = ((Il2CppType * (*)(WString * typeName, MethodInfo * method))
		GetMethod(GetClassEx("mscorlib", "System", "Type"), "System.Type GetType(System.String)"));
	if (!func)
		return nullptr;
	return func(new WString(signature), nullptr);
}
