//**************************************//
// Hi I2Hrame - Il2Cpp Plug Framework   //
// Author: MidTerm                   	//
// Version: v1.0.1                      //
// License: MIT                         //
//**************************************//

#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "Types/2021.1.0+.h"

class CI2Hrame
{
public:
	Il2CppDomain* m_pDomain;
	std::unordered_map<std::string, Il2CppClass*> m_mClasses;
	std::unordered_map<std::string, Il2CppAssembly*> m_mAssemblies;
	bool Setup();
	Il2CppClass* GetClassEx(std::string assembly, std::string nameSpace, std::string name);
	Il2CppClass* GetClass(std::string signature);
	Il2CppMethodPointer GetMethod(Il2CppClass* pClass, std::string signature);
	Il2CppType* GetType(std::string signature);
};

inline CI2Hrame* I2Hrame = new CI2Hrame();
