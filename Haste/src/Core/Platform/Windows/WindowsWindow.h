#pragma once

#include "Core/Window.h"
#include "Engine/Renderer/API/GraphicsContext.h"

#include <GLFW/glfw3.h>

namespace Core {

	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void OnUpdate() override;

		unsigned int GetWidth() const override { return m_Data.Width; }
		unsigned int GetHeight() const override { return m_Data.Height; }

		// Window attributes
		void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled) const override;
		void SetDarkMode(bool enabled, bool bUpdateTitleBar) const override;
		void SetShouldShow(bool visible) const override;
		void MaximizeWindow() const override;
		void UpdateTitleBar() const override;
		void GetPos(int& x, int& y) const override;
		void SetPos(int x, int y) const override;

		virtual void* GetNativeWindow() const { return m_Window; }
	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();
	private:
		GLFWwindow* m_Window;
		Scope<GraphicsContext> m_Context;

		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
	public:
		WindowData& GetData() { return m_Data; }
	};

}
