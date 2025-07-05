#include "EditorLayer.h"

#include "Panels/ViewportPanel.h"

#include <Core/Application.h>
#include <Core/Input/Input.h>
#include <Core/Platform/PlatformUtils.h>
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

		Panels.push_back(CreateRef<ViewportPanel>());
	}

	void EditorLayer::OnDetach()
	{
		CORE_PROFILE_FUNCTION();
	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
		CORE_PROFILE_FUNCTION();

		for (auto panel : Panels)
			panel->OnUpdate(ts);
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
		
		//ImGui::ShowDemoWindow();
		OnRenderPanels();
		
		ImGui::End();
	}

	void EditorLayer::OnEvent(Event& e)
	{
		for (auto panel : Panels)
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

			ImGui::DockBuilderRemoveNode(m_DockspaceID);
			ImGui::DockBuilderAddNode(m_DockspaceID, ImGuiDockNodeFlags_DockSpace);
			ImGui::DockBuilderSetNodeSize(m_DockspaceID, ImGui::GetWindowSize());

			//// Outliner panel
			//ImGui::DockBuilderDockWindow(Panels[0]->m_Name.c_str(),
			//	ImGui::DockBuilderSplitNode(m_DockspaceID, ImGuiDir_Left, 0.25f, nullptr, &m_DockspaceID));
			//
			//// Log Output panel
			//ImGui::DockBuilderDockWindow(Panels[1]->m_Name.c_str(),
			//	ImGui::DockBuilderSplitNode(m_DockspaceID, ImGuiDir_Down, 0.25f, nullptr, &m_DockspaceID));

			ImGui::DockBuilderFinish(m_DockspaceID);

			for (uint16_t i = 0; i < Panels.size(); i++)
				Panels[i]->m_bNeedSetupDefaultLayout = true;
		}

		bool bUnsavedPanelPopupShown = false;
		for (uint16_t i = 0; i < Panels.size(); i++)
		{
			// Close panels that want to
			if (Panels[i]->m_bWantsToClose && !bUnsavedPanelPopupShown) // only show for one panel at a time
			{
				if (!bUnsavedPanelPopupShown) // skip if another unsaved panel is trying to close
				{
					if (Panels[i]->m_bHasUnsavedData) // skip if this panel is unsaved
						bUnsavedPanelPopupShown = true;
					else if (Panels[i]->OnClosed()) // file closed successfully, remove the panel
					{
						bUnsavedPanelPopupShown = false;
						Panels.erase(Panels.begin() + i);
						i--;
					}
				}
			}

			if (Panels[i]->m_bShouldRender)
			{
				if (Panels[i]->m_bNeedSetupDefaultLayout)
				{
					ImGui::SetNextWindowDockID(m_DockspaceID);
					Panels[i]->m_bNeedSetupDefaultLayout = false;
				}

				Panels[i]->OnUIRender();
			}
		}
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
