#include "OpenGLContext.h"

#include "Core/Core.h"
#include <GLFW/glfw3.h>
#include <glad/gl.h>

namespace Core {

	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
	{
		CORE_ASSERT(windowHandle, "Window handle is null!")
	}

	void OpenGLContext::Init()
	{
		CORE_PROFILE_FUNCTION();

		glfwMakeContextCurrent(m_WindowHandle);
		int version = gladLoadGL(glfwGetProcAddress);
		CORE_ASSERT(version, "Failed to initialize OpenGL");

		CORE_INFO("OpenGL Info:");
		CORE_INFO("  Vendor: {0}", (char*)glGetString(GL_VENDOR));
		CORE_INFO("  Renderer: {0}", (char*)glGetString(GL_RENDERER));
		CORE_INFO("  Version: {0}", (char*)glGetString(GL_VERSION));
		CORE_ASSERT(GLAD_VERSION_MAJOR(version) > 4 || (GLAD_VERSION_MAJOR(version) == 4 && GLAD_VERSION_MINOR(version) >= 5), "Core requires at least OpenGL version 4.5!");
	}

	void OpenGLContext::SwapBuffers()
	{
		CORE_PROFILE_FUNCTION();

		glfwSwapBuffers(m_WindowHandle);
	}

}
