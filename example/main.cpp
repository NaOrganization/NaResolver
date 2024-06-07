#include <Windows.h>
#include "../NaResolver.h"

namespace System
{
	// Type: Class
	class Object
	{
	public:
		CLASS("mscorlib", "System", Object);
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
		class LowMemoryCallback
		{
		public:
			NESTED_CLASS(Application, LowMemoryCallback);

			// Flags: Famandassem | Assembly | Family | Famorassem | Public | Virtual | Hidebysig | Newslot
			// Addresss: 00007FF9F7680070
			METHOD_INFO(0, "System.Void", Invoke);
			void Invoke()
			{
				Template::MethodInvoker<void, LowMemoryCallback*>(__Invoke_0_Method_Info__.GetMethodAddress())(this);
			}
		};

		// Name: MemoryUsageChangedCallback
		// ClassType: Normal
		// Flags: Famandassem | Assembly | Public
		// Inheritance: MemoryUsageChangedCallback -> System::MulticastDelegate -> System::Delegate -> System::Object
		class MemoryUsageChangedCallback
		{
		public:
			NESTED_CLASS(Application, MemoryUsageChangedCallback);
		};

		// Name: LogCallback
		// ClassType: Normal
		// Flags: Famandassem | Assembly | Public
		// Inheritance: LogCallback -> System::MulticastDelegate -> System::Delegate -> System::Object
		class LogCallback
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


		// Flags: Famandassem | Assembly | Family | Famorassem | Public | Static | Hidebysig
		// Addresss: 00007FF9F8AF0790
		METHOD_INFO(1, "System.Void", Quit, "System.Int32");
		static void Quit(int32_t exitCode)
		{
			Template::MethodInvoker<void, int32_t>(__Quit_1_Method_Info__.GetMethodAddress())(exitCode);
		}

		// Flags: Famandassem | Assembly | Family | Famorassem | Public | Static | Hidebysig
		// Addresss: 00007FF9F8AF07D0
		METHOD_INFO(0, "System.Void", Quit);
		static void Quit()
		{
			Template::MethodInvoker<void>(__Quit_0_Method_Info__.GetMethodAddress())();
		}

		// Flags: Famandassem | Assembly | Family | Famorassem | Public | Static | Hidebysig | Specialname
		// Addresss: 00007FF9F8AF0E50
		METHOD_INFO(0, "System.Boolean", get_isPlaying);
		static bool get_isPlaying()
		{
			return Template::MethodInvoker<bool>(__get_isPlaying_0_Method_Info__.GetMethodAddress())();
		}

		// Flags: Famandassem | Assembly | Family | Famorassem | Public | Static | Hidebysig | Specialname
		// Addresss: 00007FF9F8AF0D80
		METHOD_INFO(0, "System.Boolean", get_isFocused);
		static bool get_isFocused()
		{
			return Template::MethodInvoker<bool>(__get_isFocused_0_Method_Info__.GetMethodAddress())();
		}

		// Flags: Famandassem | Assembly | Family | Famorassem | Public | Static | Hidebysig | Specialname
		// Addresss: 00007FF9F8AF0BD0
		METHOD_INFO(0, "System.String", get_buildGUID);
		static std::string get_buildGUID()
		{
			return VmGeneralType::String(Template::MethodInvoker<void*>(__get_buildGUID_0_Method_Info__.GetMethodAddress())());
		}

		// Flags: Famandassem | Assembly | Family | Famorassem | Public | Static | Hidebysig | Specialname
		// Addresss: 00007FF9F8AF0F10
		METHOD_INFO(0, "System.Boolean", get_runInBackground);
		static bool get_runInBackground()
		{
			return Template::MethodInvoker<bool>(__get_runInBackground_0_Method_Info__.GetMethodAddress())();
		}

		// Flags: Famandassem | Assembly | Family | Famorassem | Public | Static | Hidebysig | Specialname
		// Addresss: 00007FF9F8AF1350
		METHOD_INFO(1, "System.Void", set_runInBackground, "System.Boolean");
		static void set_runInBackground(bool value)
		{
			Template::MethodInvoker<void, bool>(__set_runInBackground_1_Method_Info__.GetMethodAddress())(value);
		}

		// Flags: Famandassem | Assembly | Family | Famorassem | Public | Static | Hidebysig | Specialname
		// Addresss: 00007FF9F8AF0D50
		METHOD_INFO(0, "System.Boolean", get_isBatchMode);
		static bool get_isBatchMode()
		{
			return Template::MethodInvoker<bool>(__get_isBatchMode_0_Method_Info__.GetMethodAddress())();
		}

		// Flags: Famandassem | Assembly | Family | Famorassem | Public | Static | Hidebysig | Specialname
		// Addresss: 00007FF9F8AF0C30
		METHOD_INFO(0, "System.String", get_dataPath);
		static std::string get_dataPath()
		{
			return VmGeneralType::String(Template::MethodInvoker<void*>(__get_dataPath_0_Method_Info__.GetMethodAddress())());
		}

		// Flags: Famandassem | Assembly | Family | Famorassem | Public | Static | Hidebysig | Specialname
		// Addresss: 00007FF9F8AF0F40
		METHOD_INFO(0, "System.String", get_streamingAssetsPath);
		static std::string get_streamingAssetsPath()
		{
			return VmGeneralType::String(Template::MethodInvoker<void*>(__get_streamingAssetsPath_0_Method_Info__.GetMethodAddress())());
		}

		// Flags: Famandassem | Assembly | Family | Famorassem | Public | Static | Hidebysig | Specialname
		// Addresss: 00007FF9F8AF0E80
		METHOD_INFO(0, "System.String", get_persistentDataPath);
		static std::string get_persistentDataPath()
		{
			return VmGeneralType::String(Template::MethodInvoker<void*>(__get_persistentDataPath_0_Method_Info__.GetMethodAddress())());
		}

		// Flags: Famandassem | Assembly | Family | Famorassem | Public | Static | Hidebysig | Specialname
		// Addresss: 00007FF9F8AF0FA0
		METHOD_INFO(0, "System.String", get_temporaryCachePath);
		static std::string get_temporaryCachePath()
		{
			return VmGeneralType::String(Template::MethodInvoker<void*>(__get_temporaryCachePath_0_Method_Info__.GetMethodAddress())());
		}

		// Flags: Famandassem | Assembly | Family | Famorassem | Public | Static | Hidebysig | Specialname
		// Addresss: 00007FF9F8AF0FD0
		METHOD_INFO(0, "System.String", get_unityVersion);
		static std::string get_unityVersion()
		{
			return VmGeneralType::String(Template::MethodInvoker<void*>(__get_unityVersion_0_Method_Info__.GetMethodAddress())());
		}

		// Flags: Famandassem | Assembly | Family | Famorassem | Public | Static | Hidebysig | Specialname
		// Addresss: 00007FF9F8AF1000
		METHOD_INFO(0, "System.String", get_version);
		static std::string get_version()
		{
			return VmGeneralType::String(Template::MethodInvoker<void*>(__get_version_0_Method_Info__.GetMethodAddress())());
		}

		// Flags: Famandassem | Assembly | Family | Famorassem | Public | Static | Hidebysig | Specialname
		// Addresss: 00007FF9F8AF0CC0
		METHOD_INFO(0, "System.String", get_identifier);
		static std::string get_identifier()
		{
			return VmGeneralType::String(Template::MethodInvoker<void*>(__get_identifier_0_Method_Info__.GetMethodAddress())());
		}

		// Flags: Famandassem | Assembly | Family | Famorassem | Public | Static | Hidebysig | Specialname
		// Addresss: 00007FF9F8AF0EE0
		METHOD_INFO(0, "System.String", get_productName);
		static std::string get_productName()
		{
			return VmGeneralType::String(Template::MethodInvoker<void*>(__get_productName_0_Method_Info__.GetMethodAddress())());
		}

		// Flags: Famandassem | Assembly | Family | Famorassem | Public | Static | Hidebysig | Specialname
		// Addresss: 00007FF9F8AF0C00
		METHOD_INFO(0, "System.String", get_cloudProjectId);
		static std::string get_cloudProjectId()
		{
			return VmGeneralType::String(Template::MethodInvoker<void*>(__get_cloudProjectId_0_Method_Info__.GetMethodAddress())());
		}

		// Flags: Famandassem | Assembly | Family | Famorassem | Public | Static | Hidebysig
		// Addresss: 00007FF9F8AF0750
		METHOD_INFO(1, "System.Void", OpenURL, "System.String");
		static void OpenURL(const std::string& url)
		{
			Template::MethodInvoker<void, void*>(__OpenURL_1_Method_Info__.GetMethodAddress())(VmGeneralType::String(url));
		}

		// Flags: Famandassem | Assembly | Family | Famorassem | Public | Static | Hidebysig | Specialname
		// Addresss: 00007FF9F8AF0F70
		METHOD_INFO(0, "System.Int32", get_targetFrameRate);
		static int32_t get_targetFrameRate()
		{
			return Template::MethodInvoker<int32_t>(__get_targetFrameRate_0_Method_Info__.GetMethodAddress())();
		}

		// Flags: Famandassem | Assembly | Family | Famorassem | Public | Static | Hidebysig | Specialname
		// Addresss: 00007FF9F8AF1390
		METHOD_INFO(1, "System.Void", set_targetFrameRate, "System.Int32");
		static void set_targetFrameRate(int32_t value)
		{
			Template::MethodInvoker<void, int32_t>(__set_targetFrameRate_1_Method_Info__.GetMethodAddress())(value);
		}

		// Flags: Famandassem | Assembly | Family | Famorassem | Public | Static | Hidebysig | Specialname
		// Addresss: 00007FF9F8AF0C90
		METHOD_INFO(0, "System.Boolean", get_genuine);
		static bool get_genuine()
		{
			return Template::MethodInvoker<bool>(__get_genuine_0_Method_Info__.GetMethodAddress())();
		}

		// Flags: Famandassem | Assembly | Family | Famorassem | Public | Static | Hidebysig | Specialname
		// Addresss: 00007FF9F8AF0C60
		METHOD_INFO(0, "System.Boolean", get_genuineCheckAvailable);
		static bool get_genuineCheckAvailable()
		{
			return Template::MethodInvoker<bool>(__get_genuineCheckAvailable_0_Method_Info__.GetMethodAddress())();
		}

		// Flags: Famandassem | Assembly | Family | Famorassem | Public | Static | Hidebysig | Specialname
		// Addresss: 00007FF9F8AF0DB0
		METHOD_INFO(0, "System.Boolean", get_isMobilePlatform);
		static bool get_isMobilePlatform()
		{
			return Template::MethodInvoker<bool>(__get_isMobilePlatform_0_Method_Info__.GetMethodAddress())();
		}

		// Flags: Private | Famandassem | Assembly | Famorassem | Public | Static | Hidebysig
		// Addresss: 00007FF9F8AF0210
		METHOD_INFO(0, "System.Boolean", HasLogCallback);
		static bool HasLogCallback()
		{
			return Template::MethodInvoker<bool>(__HasLogCallback_0_Method_Info__.GetMethodAddress())();
		}

		// Flags: Famandassem | Assembly | Family | Famorassem | Public | Static | Hidebysig | Specialname
		// Addresss: 00007FF9F8AF08B0
		METHOD_INFO(1, "System.Void", add_focusChanged, "System.Action<System.Boolean>");
		static void add_focusChanged(void* value)
		{
			Template::MethodInvoker<void, void*>(__add_focusChanged_1_Method_Info__.GetMethodAddress())(value);
		}

		// Flags: Famandassem | Assembly | Family | Famorassem | Public | Static | Hidebysig | Specialname
		// Addresss: 00007FF9F8AF1030
		METHOD_INFO(1, "System.Void", remove_focusChanged, "System.Action<System.Boolean>");
		static void remove_focusChanged(void* value)
		{
			Template::MethodInvoker<void, void*>(__remove_focusChanged_1_Method_Info__.GetMethodAddress())(value);
		}

		// Flags: Famandassem | Assembly | Family | Famorassem | Public | Static | Hidebysig | Specialname
		// Addresss: 00007FF9F8AF0AC0
		METHOD_INFO(1, "System.Void", add_wantsToQuit, "System.Func<System.Boolean>");
		static void add_wantsToQuit(void* value)
		{
			Template::MethodInvoker<void, void*>(__add_wantsToQuit_1_Method_Info__.GetMethodAddress())(value);
		}

		// Flags: Famandassem | Assembly | Family | Famorassem | Public | Static | Hidebysig | Specialname
		// Addresss: 00007FF9F8AF1240
		METHOD_INFO(1, "System.Void", remove_wantsToQuit, "System.Func<System.Boolean>");
		static void remove_wantsToQuit(void* value)
		{
			Template::MethodInvoker<void, void*>(__remove_wantsToQuit_1_Method_Info__.GetMethodAddress())(value);
		}

		// Flags: Famandassem | Assembly | Family | Famorassem | Public | Static | Hidebysig | Specialname
		// Addresss: 00007FF9F8AF09C0
		METHOD_INFO(1, "System.Void", add_quitting, "System.Action");
		static void add_quitting(void* value)
		{
			Template::MethodInvoker<void, void*>(__add_quitting_1_Method_Info__.GetMethodAddress())(value);
		}

		// Flags: Famandassem | Assembly | Family | Famorassem | Public | Static | Hidebysig | Specialname
		// Addresss: 00007FF9F8AF1140
		METHOD_INFO(1, "System.Void", remove_quitting, "System.Action");
		static void remove_quitting(void* value)
		{
			Template::MethodInvoker<void, void*>(__remove_quitting_1_Method_Info__.GetMethodAddress())(value);
		}

		// Flags: Private | Assembly | Famorassem | Static | Hidebysig
		// Addresss: 00007FF9F8AF0430
		METHOD_INFO(0, "System.Boolean", Internal_ApplicationWantsToQuit);
		static bool Internal_ApplicationWantsToQuit()
		{
			return Template::MethodInvoker<bool>(__Internal_ApplicationWantsToQuit_0_Method_Info__.GetMethodAddress())();
		}

		// Flags: Private | Assembly | Famorassem | Static | Hidebysig
		// Addresss: 00007FF9F8AF0290
		METHOD_INFO(0, "System.Void", Internal_ApplicationQuit);
		static void Internal_ApplicationQuit()
		{
			Template::MethodInvoker<void>(__Internal_ApplicationQuit_0_Method_Info__.GetMethodAddress())();
		}

		// Flags: Private | Assembly | Famorassem | Static | Hidebysig
		// Addresss: 00007FF9F8AF03A0
		METHOD_INFO(0, "System.Void", Internal_ApplicationUnload);
		static void Internal_ApplicationUnload()
		{
			Template::MethodInvoker<void>(__Internal_ApplicationUnload_0_Method_Info__.GetMethodAddress())();
		}

		// Flags: Private | Famandassem | Assembly | Famorassem | Public | Static | Hidebysig
		// Addresss: 00007FF9F8AF0710
		METHOD_INFO(0, "System.Void", InvokeOnBeforeRender);
		static void InvokeOnBeforeRender()
		{
			Template::MethodInvoker<void>(__InvokeOnBeforeRender_0_Method_Info__.GetMethodAddress())();
		}

		// Flags: Private | Famandassem | Assembly | Famorassem | Public | Static | Hidebysig
		// Addresss: 00007FF9F8AF0670
		METHOD_INFO(1, "System.Void", InvokeFocusChanged, "System.Boolean");
		static void InvokeFocusChanged(bool focus)
		{
			Template::MethodInvoker<void, bool>(__InvokeFocusChanged_1_Method_Info__.GetMethodAddress())(focus);
		}

		// Flags: Private | Famandassem | Assembly | Famorassem | Public | Static | Hidebysig
		// Addresss: 00007FF9F8AF05D0
		METHOD_INFO(1, "System.Void", InvokeDeepLinkActivated, "System.String");
		static void InvokeDeepLinkActivated(const std::string& url)
		{
			Template::MethodInvoker<void, void*>(__InvokeDeepLinkActivated_1_Method_Info__.GetMethodAddress())(VmGeneralType::String(url));
		}

		// Flags: Famandassem | Assembly | Family | Famorassem | Public | Static | Hidebysig | Specialname
		// Addresss: 00007FF9F8382970
		METHOD_INFO(0, "System.Boolean", get_isEditor);
		static bool get_isEditor()
		{
			return Template::MethodInvoker<bool>(__get_isEditor_0_Method_Info__.GetMethodAddress())();
		}

		// Flags: Private | Assembly | Famorassem | Static | Hidebysig | Specialname
		// Addresss: 00007FF9F8AF0830
		METHOD_INFO(0, "System.Void", StaticConstructor);
		static void StaticConstructor()
		{
			Template::MethodInvoker<void>(__StaticConstructor_0_Method_Info__.GetMethodAddress())();
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
	//printf("Game version: %s\n", Application::get_version().c_str());			// some unity versions don't have this method
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