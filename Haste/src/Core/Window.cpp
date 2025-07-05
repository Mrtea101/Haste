#include "Window.h"

#ifdef IS_PLATFORM_WINDOWS
	#include "Core/Platform/Windows/WindowsWindow.h"
#endif

namespace Core
{
	Scope<Window> Window::Create(const WindowProps& props)
	{
	#ifdef IS_PLATFORM_WINDOWS
		return CreateScope<WindowsWindow>(props);
	#else
		CORE_ASSERT(false, "Unknown platform!");
		return nullptr;
	#endif
	}

}
