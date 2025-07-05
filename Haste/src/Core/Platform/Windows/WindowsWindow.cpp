#include "WindowsWindow.h"

#include "Core/Input/Input.h"
#include "Core/Application.h"
#include "Core/Events/ApplicationEvent.h"
#include "Core/Events/MouseEvent.h"
#include "Core/Events/KeyEvent.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/API/OpenGL/OpenGLContext.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <dwmapi.h>

namespace Core {
	
	static uint8_t s_GLFWWindowCount = 0;

	static void GLFWErrorCallback(int error, const char* description)
	{
		CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		CORE_PROFILE_FUNCTION();

		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		CORE_PROFILE_FUNCTION();

		Shutdown();
	}

	void WindowsWindow::Init(const WindowProps& props)
	{
		CORE_PROFILE_FUNCTION();

		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

		if (s_GLFWWindowCount == 0)
		{
			CORE_PROFILE_SCOPE("glfwInit");
			int success = glfwInit();
			CORE_ASSERT(success, "Could not initialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);
		}

		{
			CORE_PROFILE_SCOPE("glfwCreateWindow");
		#if defined(CORE_DEBUG)
			if (Renderer3D::GetAPI() == RendererAPI::API::OpenGL)
				glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
		#endif
			if (props.StartHidden)
				glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // Hide window temporarily to prevent flashbang
			m_Window = glfwCreateWindow(m_Data.Width, m_Data.Height, m_Data.Title.c_str(), nullptr, nullptr);
			++s_GLFWWindowCount;
		}

		m_Context = GraphicsContext::Create(m_Window);
		m_Context->Init();

		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);

		// Set GLFW callbacks
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.Width = width;
			data.Height = height;
			
			WindowResizeEvent event(width, height);
			data.EventCallback(event);
		});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.EventCallback(event);
		});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(key, 0);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					data.EventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(key, true);
					data.EventCallback(event);
					break;
				}
			}
		});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			KeyTypedEvent event(keycode);
			data.EventCallback(event);
		});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					data.EventCallback(event);
					break;
				}
			}
		});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			data.EventCallback(event);
		});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent event((float)xPos, (float)yPos);
			data.EventCallback(event);
		});

		glfwSetWindowRefreshCallback(m_Window, [](GLFWwindow* window)
		{
			Application::Get().UpdateLayers(Timestep());
			Application::Get().RenderLayers();
			glfwSwapBuffers(window);
		});
	}

	void WindowsWindow::Shutdown()
	{
		CORE_PROFILE_FUNCTION();

		glfwDestroyWindow(m_Window);
		--s_GLFWWindowCount;

		if (s_GLFWWindowCount == 0)
		{
			glfwTerminate();
		}
	}

	void WindowsWindow::OnUpdate()
	{
		CORE_PROFILE_FUNCTION();

		glfwPollEvents();
		m_Context->SwapBuffers();
	}

	void WindowsWindow::SetVSync(bool enabled) const
	{
		CORE_PROFILE_FUNCTION();

		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);
	}

	void WindowsWindow::SetDarkMode(bool enabled, bool bUpdateTitleBar) const
	{
		CORE_PROFILE_FUNCTION();

		HWND WINhWnd = glfwGetWin32Window(m_Window);
		BOOL USE_DARK_MODE = enabled;
		BOOL SET_IMMERSIVE_DARK_MOCORE_SUCCESS = SUCCEEDED(DwmSetWindowAttribute(
			WINhWnd, DWMWINDOWATTRIBUTE::DWMWA_USE_IMMERSIVE_DARK_MODE,
			&USE_DARK_MODE, sizeof(USE_DARK_MODE)));

		if (bUpdateTitleBar)
			UpdateTitleBar();
	}

	void WindowsWindow::SetShouldShow(bool visible) const
	{
		if (visible)
			glfwShowWindow(m_Window);
		else
			glfwHideWindow(m_Window);
	}

	void WindowsWindow::MaximizeWindow() const
	{
		glfwMaximizeWindow(m_Window);
		glfwGetWindowSize(m_Window, (int*)&m_Data.Width, (int*)&m_Data.Height);
	}

	void WindowsWindow::UpdateTitleBar() const
	{
		if (glfwGetWindowAttrib(m_Window, GLFW_MAXIMIZED))
		{
			glfwRestoreWindow(m_Window);
			glfwMaximizeWindow(m_Window);
		}
		else
		{
			glfwMaximizeWindow(m_Window);
			glfwRestoreWindow(m_Window);
		}
	}

	void WindowsWindow::GetPos(int& x, int& y) const
	{
		glfwGetWindowPos(m_Window, &x, &y);
	}

	void WindowsWindow::SetPos(int x, int y) const
	{
		glfwSetWindowPos(m_Window, x, y);
	}
}
