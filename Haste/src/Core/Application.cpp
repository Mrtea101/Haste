#include "Application.h"

#include "Engine/Renderer/Renderer.h"

#include "Core/Input/Input.h"
#include "Core/Platform/PlatformUtils.h"

namespace Core {

	Application* Application::s_Instance = nullptr;

	Application::Application(const ApplicationSpecification& specification)
		: m_Specification(specification)
	{
		std::string userSavedDir = m_Specification.UserSavedDir;

		// Set working directory here
		if (!m_Specification.WorkingDirectory.empty())
		{
			std::filesystem::current_path(m_Specification.WorkingDirectory);
			userSavedDir = m_Specification.WorkingDirectory + userSavedDir;
		}

		std::filesystem::create_directories(userSavedDir);

		Log::Init(specification.LogFile.c_str());

		CORE_PROFILE_BEGIN_SESSION("Startup", "CoreProfile-Startup.json");

		CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_Window = Window::Create(WindowProps(m_Specification.Name, 1600, 900, true));
		m_Window->SetEventCallback(CORE_BIND_EVENT_FN(Application::OnEvent));

		Renderer::Init();

		m_UILayer = new UILayer();
		PushOverlay(m_UILayer);
	}

	Application::~Application()
	{
		CORE_PROFILE_FUNCTION();

		Renderer::Shutdown();
	}

	void Application::PushLayer(Layer* layer)
	{
		CORE_PROFILE_FUNCTION();

		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{
		CORE_PROFILE_FUNCTION();

		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	void Application::Close()
	{
		m_Running = false;
	}

	void Application::SubmitToMainThread(const std::function<void()>& function)
	{
		std::scoped_lock<std::mutex> lock(m_MainThreadQueueMutex);

		m_MainThreadQueue.emplace_back(function);
	}

	void Application::OnEvent(Event& e)
	{
		CORE_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(CORE_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(CORE_BIND_EVENT_FN(Application::OnWindowResize));

		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
		{
			if (e.Handled) 
				break;
			(*it)->OnEvent(e);
		}
	}

	void Application::Run()
	{
		CORE_PROFILE_FUNCTION();

		while (m_Running)
		{
			CORE_PROFILE_SCOPE("RunLoop");

			float time = Time::GetTime();
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			ExecuteMainThreadQueue();

			if (!m_Minimized)
			{
				UpdateLayers(timestep);
				RenderLayers();
			}

			m_Window->OnUpdate();
		}
	}

	void Application::UpdateLayers(Timestep timestep)
	{
		CORE_PROFILE_FUNCTION();

		for (Layer* layer : m_LayerStack)
			layer->OnUpdate(timestep);
	}

	void Application::RenderLayers()
	{
		CORE_PROFILE_FUNCTION();

		m_UILayer->Begin();
		{
			CORE_PROFILE_SCOPE("LayerStack OnImGuiRender");

			for (Layer* layer : m_LayerStack)
				layer->OnUIRender();
		}
		m_UILayer->End();
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		CORE_PROFILE_FUNCTION();

		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}

		m_Minimized = false;
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

		return false;
	}

	void Application::ExecuteMainThreadQueue()
	{
		std::scoped_lock<std::mutex> lock(m_MainThreadQueueMutex);

		for (auto& func : m_MainThreadQueue)
			func();

		m_MainThreadQueue.clear();
	}
}
