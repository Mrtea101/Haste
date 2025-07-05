#pragma once

#include "Core.h"
#include "Events/Event.h"

namespace Core {

	struct WindowProps
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;
		bool StartHidden;

		WindowProps(const std::string& title = "Title",
			        uint32_t width = 1600,
			        uint32_t height = 900,
					bool bStartHidden = false)
			: Title(title), Width(width), Height(height), StartHidden(bStartHidden)
		{
		}
	};

	// Interface representing a desktop system based Window
	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() = default;

		virtual void OnUpdate() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		// Window attributes
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) const = 0;
		virtual void SetDarkMode(bool enabled, bool bUpdateTitleBar = true) const = 0;
		virtual void SetShouldShow(bool visible) const = 0;
		virtual void MaximizeWindow() const = 0;
		virtual void UpdateTitleBar() const = 0;
		virtual void GetPos(int& x, int& y) const = 0;
		virtual void SetPos(int x, int y) const = 0;

		virtual void* GetNativeWindow() const = 0;

		static Scope<Window> Create(const WindowProps& props = WindowProps());
	};

}
