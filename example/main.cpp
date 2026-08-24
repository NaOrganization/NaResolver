#include <Windows.h>
#include "../UniResolver.h"

namespace System
{
	// Type: Class
	class Object
	{
	public:
		CLASS("mscorlib", "System", Object);

		void* klass;
		void* monitor;
	};

	class MulticastDelegate
	{

	};
}

namespace UnityEngine
{
	using namespace Template;

	// Type: Class
	class Object : public System::Object
	{
	public:
		CLASS("UnityEngine.CoreModule", "UnityEngine", Object);

		std::intptr_t m_CachedPtr;
	};
	// Name: UnityEngine::Application
	// ClassType: Normal
	// Flags: Private | Assembly | Famorassem
	// Inheritance: UnityEngine::Application -> System::Object
	class Application
	{
	public:
		CLASS("UnityEngine.CoreModule", "UnityEngine", Application);

	public:
		// Name: LowMemoryCallback
		// ClassType: Normal
		// Flags: Famandassem | Assembly | Public
		// Inheritance: LowMemoryCallback -> System::MulticastDelegate -> System::Delegate -> System::Object
		class LowMemoryCallback : System::MulticastDelegate
		{
		public:
			NESTED_CLASS(Application, LowMemoryCallback);
		};

		// Name: MemoryUsageChangedCallback
		// ClassType: Normal
		// Flags: Famandassem | Assembly | Public
		// Inheritance: MemoryUsageChangedCallback -> System::MulticastDelegate -> System::Delegate -> System::Object
		class MemoryUsageChangedCallback : System::MulticastDelegate
		{
		public:
			NESTED_CLASS(Application, MemoryUsageChangedCallback);
		};

		// Name: LogCallback
		// ClassType: Normal
		// Flags: Famandassem | Assembly | Public
		// Inheritance: LogCallback -> System::MulticastDelegate -> System::Delegate -> System::Object
		class LogCallback : System::MulticastDelegate
		{
		public:
			NESTED_CLASS(Application, LogCallback);
		};

		STATIC_FIELD(UnityEngine::Application::LowMemoryCallback*, lowMemory);
		STATIC_FIELD(UnityEngine::Application::MemoryUsageChangedCallback*, memoryUsageChanged);
		STATIC_FIELD(UnityEngine::Application::LogCallback*, s_LogCallbackHandler);
		STATIC_FIELD(UnityEngine::Application::LogCallback*, s_LogCallbackHandlerThreaded);
		STATIC_FIELD(void*, focusChanged);
		STATIC_FIELD(void*, deepLinkActivated);
		STATIC_FIELD(void*, wantsToQuit);
		STATIC_FIELD(void*, quitting);
		STATIC_FIELD(void*, unloading);

		// Flags: Famandassem | Assembly | Family | Famorassem | Public | Static | Hidebysig | Specialname
		// RVA: 0x1259B50
		METHOD_INFO(0x600006D);
		static std::string get_unityVersion()
		{
			return VmGeneralType::String(Template::MethodInvoker<void*>(__0x600006D_Method_Info__.GetMethodAddress())());
		}

		// Flags: Famandassem | Assembly | Family | Famorassem | Public | Static | Hidebysig | Specialname
		// Addresss: 0x1259BA0
		METHOD_INFO(0x600006E);
		static std::string get_version()
		{
			return VmGeneralType::String(Template::MethodInvoker<void*>(__0x600006E_Method_Info__.GetMethodAddress())());
		}
	};
	// Type: Class
	class Debug : public System::Object
	{
	public:
		CLASS("UnityEngine.CoreModule", "UnityEngine", Debug);
	};
}

using namespace UnityEngine;

HINSTANCE module = NULL;

void Example()
{
	AllocConsole();
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);

	if (!naResolverInstance.Setup())
		printf("Failed to setup NaResolver!\n");

	printf("Unity version: %s\n", Application::get_unityVersion().c_str());	// some unity versions don't have this method
	printf("Game version: %s\n", Application::get_version().c_str());			// some unity versions don't have this method
	//printf("%p\n", UnityEngine::Application::__get_isMobilePlatform_Method_Info__.GetMethodAddress());
	//printf("Is MobilePlatform: %d\n", Application::get_isMobilePlatform());

	FreeConsole();
	FreeLibraryAndExitThread(module, 0);
}


int WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hinstDLL);
		module = hinstDLL;
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Example, NULL, NULL, NULL);
	}
	return TRUE; // Successful DLL_PROCESS_ATTACH.
}