#include <Windows.h>
#define __NARESOLVER_ONLY_API
#include "../NaResolver.h"

#define LOGGER_FUNCTION(level) ([](std::string m, ...)->void\
			{\
			m = #level " [NaResolver] " + m + "\n";\
			va_list args;\
			va_start(args, m);\
			vprintf(m.c_str(), args);\
			va_end(args);\
			})

// per-register the class, it will init in init period
// the first parameter: Assembly
// secondly: namespace
// thirdly: class name ( including declarative name of class )
// fourth: inherit ( pay attention: you need have a ":" before your inheritable class )
NA_RESOLVER_REGISTER_CLASS(UnityEngine.CoreModule, UnityEngine, GameObject, : public std::string)
{
public:
	void* get_transform();
};

// in a similar way
// firstly: which class's namespace it comes from
// secondly: which class it comes from
// thirdly: signature
// fourth: return type
// fifth: function name
// ...: parameters (type)
NA_RESOLVER_REGISTER_METHOD(UnityEngine, GameObject, "UnityEngine.Transform get_transform()", void*, get_transform, )
{
	// __{ namespace }_{ class }_Class_{ function }_Method__
	// As<{ return type }, { self class }, { the last parameter must is a MethodInfo }, ...>()
	// Attention: If this function's return type is the void, you need to ues the VoidInvode.
	__UnityEngine_GameObject_Class_get_transform_Method__.As<void*, GameObject*, GeneralMethodInfo*>().
		Invoke(this, nullptr);
}
// Suggestion: add the function in cpp

int WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hinstDLL);
		AllocConsole();
		freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
		try
		{
			NaResolver::Config config;
			config.enableLogger = true;
			config.logger.debug = LOGGER_FUNCTION([Debug]);
			config.logger.info = LOGGER_FUNCTION([Info]);
			// i delete the error and fatal log, there will be export with the NaResolver::Exception

			if (!Il2CppResolver->Setup(config))
				printf("Failed to setup I2Hrame!\n");
		}
		catch (const NaResolver::Exception& e)
		{
			// Everything registered will be handled in Setup, so here is a 'try' to catch the error
			printf("[NaResolver] %s\n", e.message.c_str());
		}

		// content is ptr of it ( all same )
		printf("(Il2CppResolver->GetClass) Class: %p\n", __UnityEngine_GameObject_Class__.content);
		printf("(Il2CppResolver->GetMethod) Method: %p\n", __UnityEngine_GameObject_Class_get_transform_Method__.content);

		Il2CppResolver->Destroy();
		free(Il2CppResolver);
		FreeConsole();
		FreeLibrary(hinstDLL);
	}
	return TRUE; // Successful DLL_PROCESS_ATTACH.
}
