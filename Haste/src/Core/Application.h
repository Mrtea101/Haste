#pragma once

#include "Core.h"

#include "Window.h"
#include "UI/LayerStack.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"

#include "Timestep.h"
#include "Timer.h"

#include "UI/UILayer.h"

int main(int argc, char** argv);

namespace Core {

	struct ApplicationCommandLineArgs
	{
		int Count = 0;
		char** Args = nullptr;

		const char* operator[](int index) const
		{
			CORE_ASSERT(index < Count);
			return Args[index];
		}
	};

	struct ApplicationSpecification
	{
		ApplicationCommandLineArgs CommandLineArgs;
		std::string Name;
		std::string WorkingDirectory;
		std::string UserSavedDir = "Saved";
		std::string ImGuiIniFile = UserSavedDir + "UI.ini";
		std::string LogFile = UserSavedDir + "Log.txt";
	};

	class Application
	{
	public:
		Application(const ApplicationSpecification& specification);
		virtual ~Application();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		Window& GetWindow() { return *m_Window; }

		void Close();

		UILayer* GetUILayer() { return m_UILayer; }

		void UpdateLayers(Timestep timestep);
		void RenderLayers();

		static Application& Get() { return *s_Instance; }

		const ApplicationSpecification& GetSpecification() const { return m_Specification; }

		void SubmitToMainThread(const std::function<void()>& function);

		const Timer& GetRunTimer() const { return m_RunTimer; }
	private:
		void Run();
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

		void ExecuteMainThreadQueue();
	private:
		ApplicationSpecification m_Specification;
		Scope<Window> m_Window;
		UILayer* m_UILayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;
		Timer m_RunTimer;

		std::vector<std::function<void()>> m_MainThreadQueue;
		std::mutex m_MainThreadQueueMutex;
	private:
		static Application* s_Instance;
		friend int Main(int argc, char** argv);
	};

	// To be defined in CLIENT
	Application* CreateApplication(ApplicationCommandLineArgs args);
}
