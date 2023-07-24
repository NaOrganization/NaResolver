#include <Windows.h>
#define __NARESOLVER_ONLY_API
#include "../NaResolver.h"

#define LOGGER_FUNCTION(level) ([](std::string m, ...)->void\
			{\
			m = #level " " + m + "\n";\
			va_list args;\
			va_start(args, m);\
			vprintf(m.c_str(), args);\
			va_end(args);\
			})

int WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		AllocConsole();
		freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
		NaResolver::Config config;
		config.enableLogger = true;
		config.logger.debug = LOGGER_FUNCTION([DEBUG]);
		config.logger.info = LOGGER_FUNCTION([INFO]);
		config.logger.error = LOGGER_FUNCTION([ERROR]);
		config.logger.fatal = LOGGER_FUNCTION([FATAL]);

		if (!Il2CppResolver->Setup(config))
			printf("Failed to setup I2Hrame!\n");
		Il2CppClass* klass = Il2CppResolver->GetClassEx("UnityEngine.CoreModule", "UnityEngine", "GameObject");
		printf("(Il2CppResolver->GetClassEx) Class: %p\n", klass);
		klass = Il2CppResolver->GetClass("(UnityEngine.CoreModule)UnityEngine.GameObject");
		printf("(Il2CppResolver->GetClass) Class: %p\n", klass);
		Il2CppMethodPointer method = Il2CppResolver->GetMethod(klass, "UnityEngine.Transform get_transform()");
		printf("(Il2CppResolver->GetMethod) Method: %p\n", method);

		klass = Il2CppResolver->GetClass("(UnityEngine.CoreModule)UnityEngine.GameObjct");
		printf("(Error Demonstration) Class: %p\n", klass);
	}
	return TRUE; // Successful DLL_PROCESS_ATTACH.
}
