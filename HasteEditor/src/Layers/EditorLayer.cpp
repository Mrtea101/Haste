#define IMGUI_DEFINE_MATH_OPERATORS
#include "EditorLayer.h"

#include "Panels/EditorViewportPanel.h"
#include "Panels/LevelOutlinerPanel.h"

#include <Core/Application.h>
#include <Core/Input/Input.h>
#include <Core/Platform/PlatformUtils.h>
#include <Engine/Renderer/Renderer.h>
#include <Project/Project.h>
#include <imgui.h>


namespace HasteEditor {

	EditorLayer* EditorLayer::m_Inst = nullptr;

	EditorLayer::EditorLayer()
		: Layer("EditorLayer")
	{
		m_Inst = this;

		Application::Get().GetWindow().SetDarkMode(true, false);

		// Testing until there is some kind of data persistence
		m_bNeedSetupDefaultLayout = true;
	}

	void EditorLayer::OnAttach()
	{
		CORE_PROFILE_FUNCTION();

		// Command line
		auto commandLineArgs = Application::Get().GetSpecification().CommandLineArgs;
		if (commandLineArgs.Count > 1)
		{
			
		}

#if WITH_PROJECT
		if (!Project::Load(std::filesystem::current_path() / "TestProject.hasteproj"))
		{
			auto prj = Project::New();
			auto& cfg = prj->GetConfig();
			cfg.Name = "TestProject";
			cfg.ProjectDirectory = std::filesystem::current_path();
			Project::SaveActive();
			CORE_INFO("Created new project file");
		}
#endif

		m_EditorLevel = CreateRef<DemoLevel>();
		m_ActiveLevel = m_EditorLevel;
		Renderer::SetLineWidth(4.0f);

		m_Panels.push_back(CreateScope<EditorViewportPanel>());
		m_Panels.push_back(CreateScope<LevelOutlinerPanel>(m_ActiveLevel));
	}

	void EditorLayer::OnDetach()
	{
		CORE_PROFILE_FUNCTION();
	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
		CORE_PROFILE_FUNCTION();

		for (auto& panel : m_Panels)
			panel->OnUpdate(ts);

		if (EditorViewportPanel* viewport = FindPanelByClass<EditorViewportPanel>())
		{
			switch (m_LevelState)
			{
			case LevelState::Edit:
				m_ActiveLevel->OnUpdateEditor(ts, viewport->GetCamera());
				break;
			case LevelState::Simulate:
				m_ActiveLevel->OnUpdateSimulation(ts, viewport->GetCamera());
				break;
			case LevelState::Play:
				m_ActiveLevel->OnUpdateRuntime(ts);
				break;
			}

			viewport->PostUpdate();
		}
	}

	void EditorLayer::OnUIRender()
	{
		CORE_PROFILE_FUNCTION();

		static bool bFirstRender = true; // showing window from now prevents flashbang
		if (bFirstRender)
		{
			bFirstRender = false;

			bool StartMaximized = false;
			if (StartMaximized)
				Application::Get().GetWindow().MaximizeWindow(); // will also make window visible
			else
				Application::Get().GetWindow().SetShouldShow(true);
		}

		// Setup Dockspace
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		
		const char* dockspaceName = "EditorLayer.Dockspace";
		ImGui::Begin(dockspaceName, 0, window_flags);
		ImGui::PopStyleVar(3);
		
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 370.0f;
		m_DockspaceID = ImGui::GetID(dockspaceName);
		ImGui::DockSpace(m_DockspaceID);
		style.WindowMinSize.x = minWinSizeX;
		
		OnRenderMenuBar();
		OnRenderPanels();
		
		ImGui::End();
	}

	void EditorLayer::OnEvent(Event& e)
	{
		for (auto& panel : m_Panels)
		{
			panel->OnEvent(e);
			if (e.Handled) return;
		}

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(CORE_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(CORE_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
	}

	void EditorLayer::OnRenderPanels()
	{
		if (m_bNeedSetupDefaultLayout)
		{
			m_bNeedSetupDefaultLayout = false;

			for (auto& panel : m_Panels)
				panel->m_bNeedSetupDefaultLayout = true;

			ImGui::DockBuilderRemoveNode(m_DockspaceID);
			ImGui::DockBuilderAddNode(m_DockspaceID, ImGuiDockNodeFlags_DockSpace);
			ImGui::DockBuilderSetNodeSize(m_DockspaceID, ImGui::GetWindowSize());

			auto outliner = FindPanelByClass<LevelOutlinerPanel>();
			outliner->m_bNeedSetupDefaultLayout = false;
			auto outlinerDockId = ImGui::DockBuilderSplitNode(m_DockspaceID, ImGuiDir_Right, 0.25f, nullptr, &m_DockspaceID);
			ImGui::DockBuilderDockWindow(outliner->m_Name.c_str(), outlinerDockId);
			ImGui::DockBuilderDockWindow("Properties", // part of the outliner
				ImGui::DockBuilderSplitNode(outlinerDockId, ImGuiDir_Down, 0.5f, nullptr, &outlinerDockId));

			ImGui::DockBuilderFinish(m_DockspaceID);
		}

		bool bUnsavedPanelPopupShown = false;
		for (uint16_t i = 0; i < m_Panels.size(); i++)
		{
			// Close panels that want to
			if (m_Panels[i]->m_bWantsToClose && !bUnsavedPanelPopupShown) // only show for one panel at a time
			{
				if (!bUnsavedPanelPopupShown) // skip if another unsaved panel is trying to close
				{
					if (m_Panels[i]->m_bHasUnsavedData) // skip if this panel is unsaved
						bUnsavedPanelPopupShown = true;
					else if (m_Panels[i]->OnClosed()) // file closed successfully, remove the panel
					{
						bUnsavedPanelPopupShown = false;
						m_Panels.erase(m_Panels.begin() + i);
						i--;
						continue;
					}
				}
			}

			if (m_Panels[i]->m_bShouldRender)
			{
				if (m_Panels[i]->m_bNeedSetupDefaultLayout)
				{
					m_Panels[i]->m_bNeedSetupDefaultLayout = false;
					ImGui::SetNextWindowDockID(m_DockspaceID);
				}

				m_Panels[i]->OnUIRender();
			}
		}

	}

	void EditorLayer::OnRenderMenuBar()
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Exit", "Alt+F4"))
					Application::Get().Close();

				ImGui::EndMenu();
			}

#if WITH_PROJECT
			if (ImGui::BeginMenu("Project"))
			{
				if (ImGui::MenuItem("Save", "Ctrl+S"))
					Project::SaveActive();

				ImGui::EndMenu();
			}
#endif

#ifdef IS_DEBUG
			if (ImGui::BeginMenu("Debug"))
			{
				if (ImGui::MenuItem("Test")) // For quick testing
				{
				}

				ImGui::EndMenu();
			}
#endif

			ImGui::BeginHorizontal("EditorInfoBar",
				ImVec2(ImGui::GetIO().DisplaySize.x - ImGui::GetCursorPosX(), ImGui::GetItemRectSize().y));

			//if (m_ActiveLevel != m_EditorLevel)
			{
				ImGui::Spring();
				ImGui::TextUnformatted(m_ActiveLevel->GetName().c_str());
			}

			ImGui::Spring();
			ImGui::Text("%.2f fps / %.3f ms", ImGui::GetIO().Framerate, 1 / ImGui::GetIO().Framerate);
#if WITH_PROJECT
			ImGui::Separator();
			ImGui::TextUnformatted(Project::GetActive()->GetConfig().Name.c_str());
#endif

			ImGui::Spring(0.f, 8.f);
			ImGui::EndHorizontal();

			ImGui::EndMenuBar();
		}
	}

	template<typename T>
	T* EditorLayer::FindPanelByClass()
	{
		static_assert(std::is_base_of<Panel, T>(), "FindPanelByClass called with non-panel template param");
		for (auto& panel : m_Panels)
		{
			if (T* panelCasted = dynamic_cast<T*>(panel.get()))
				return panelCasted;
		}
		return nullptr;
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		if (e.IsRepeat())
			return false;

		return false;
	}

	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		return false;
	}
}
