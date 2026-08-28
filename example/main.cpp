#include <Windows.h>

#include <cstdio>
#include <exception>

#include "UniDumperSdk.h"

namespace
{
	HMODULE gModule = nullptr;

	DWORD WINAPI RunExample(LPVOID)
	{
		AllocConsole();
		FILE* consoleStream = nullptr;
		freopen_s(&consoleStream, "CONOUT$", "w", stdout);

		if (!UniResolver.Setup())
		{
			std::printf("UniResolver setup failed.\n");
		}
		else
		{
			try
			{
				std::printf(
					"Unity version: %s\n",
					UnityEngine::Application::get_unityVersion().c_str());
				std::printf(
					"Game version: %s\n",
					UnityEngine::Application::get_version().c_str());
			}
			catch (const std::exception& exception)
			{
				std::printf("Managed call failed: %s\n", exception.what());
			}

			UniResolver.Destroy();
		}

		if (consoleStream != nullptr)
		{
			std::fclose(consoleStream);
		}
		FreeConsole();
		FreeLibraryAndExitThread(gModule, 0U);
	}
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		gModule = module;
		DisableThreadLibraryCalls(module);

		if (HANDLE thread = CreateThread(nullptr, 0U, RunExample, nullptr, 0U, nullptr);
			thread != nullptr)
		{
			CloseHandle(thread);
		}
	}

	return TRUE;
}
