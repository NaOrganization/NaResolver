#include <Windows.h>
#include "../../I2Hrame.h"

int WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		AllocConsole();
		freopen("CONOUT$", "w", stdout);
		freopen("CONIN$", "r", stdin);
		
		if(!I2Hrame->Setup())
			printf("Failed to setup I2Hrame!\n");
		Il2CppClass* klass = I2Hrame->GetClassEx("UnityEngine.CoreModule", "UnityEngine", "GameObject");
		printf("(1) Class: %p\n", klass);
		klass = I2Hrame->GetClass("(UnityEngine.CoreModule)UnityEngine.GameObject");
		printf("(2) Class: %p\n", klass);
		Il2CppMethodPointer method = I2Hrame->GetMethod(klass, "UnityEngine.Transform get_transform()");
		printf("(1) Method: %p\n", method);
	}
	return TRUE; // Successful DLL_PROCESS_ATTACH.
}