#include <Windows.h>
#include "../../Types/29-2021.2.0-.h"
#include "../../I2Hrame.h"

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
		freopen("CONOUT$", "w", stdout);
		freopen("CONIN$", "r", stdin);
		I2HConfig config;
		config.enableLogger = true;
		config.logger.debug = LOGGER_FUNCTION([DEBUG]);
		config.logger.info = LOGGER_FUNCTION([INFO]);
		config.logger.error = LOGGER_FUNCTION([ERROR]);
		config.logger.fatal = LOGGER_FUNCTION([FATAL]);

		if(!i2Hrame->Setup(config))
			printf("Failed to setup I2Hrame!\n");
		Il2CppClass* klass = i2Hrame->GetClassEx("UnityEngine.CoreModule", "UnityEngine", "GameObject");
		printf("(i2Hrame->GetClassEx) Class: %p\n", klass);
		klass = i2Hrame->GetClass("(UnityEngine.CoreModule)UnityEngine.GameObject");
		printf("(i2Hrame->GetClass) Class: %p\n", klass);
		Il2CppMethodPointer method = i2Hrame->GetMethod(klass, "UnityEngine.Transform get_transform()");
		printf("(i2Hrame->GetMethod) Method: %p\n", method);

		klass = i2Hrame->GetClass("(UnityEngine.CoreModule)UnityEngine.GameObjct");
		printf("(Error Demonstration) Class: %p\n", klass);
	}
	return TRUE; // Successful DLL_PROCESS_ATTACH.
}
