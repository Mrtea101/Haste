#pragma once
#include "Core.h"
#include "Application.h"

#ifdef IS_PLATFORM_WINDOWS

extern Core::Application* Core::CreateApplication(ApplicationCommandLineArgs args);

namespace Core
{
	int Main(int argc, char** argv)
	{
		// Begin profile session in Application::Application
		// because app spec logname is needed to startup the logger
		auto app = CreateApplication({ argc, argv });
		CORE_PROFILE_END_SESSION(); // End "Startup", "CoreProfile-Startup.json"

		CORE_PROFILE_BEGIN_SESSION("Runtime", "CoreProfile-Runtime.json");
		app->Run();
		CORE_PROFILE_END_SESSION();

		CORE_PROFILE_BEGIN_SESSION("Shutdown", "CoreProfile-Shutdown.json");
		delete app;
		CORE_PROFILE_END_SESSION();

		return 0;
	}
}

#ifdef IS_RELEASE

#include <Windows.h>

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
	return Core::Main(__argc, __argv);
}

#else

int main(int argc, char** argv)
{
	return Core::Main(argc, argv);
}

#endif // IS_RELEASE

#endif // IS_PLATFORM_WINDOWS
