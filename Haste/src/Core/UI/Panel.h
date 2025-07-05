#pragma once

#include <Core/Core.h>
#include <Core/Timestep.h>
#include <Core/Events/Event.h>
#include <imgui.h>
#include <imgui_internal.h>


namespace HasteEditor
{
	class Panel
	{
	public:
		Panel() = default;
		virtual ~Panel() = default;
		Panel(const Panel&) = delete;
		Panel& operator= (const Panel&) = delete;

		// optional overrides for panels

		virtual void OnUIRender() = 0;
		virtual void OnUpdate(Timestep ts) {};
		virtual void OnUIRenderProperties() {};
		virtual void OnEvent(Event& e) {};
		virtual bool OnClosed() { return true; }

		std::string m_Name = "Panel";
		bool m_bShouldRender = true;
		bool m_bWantsToClose = false;
		bool m_bHasUnsavedData = false;
		bool m_bShouldSetFocused = false;
		bool m_bDocked = false;
		bool m_bNeedSetupDefaultLayout = false;

	protected:

		void UpdateDockedState()
		{
			m_bDocked = false;
			if (auto node = ImGui::GetWindowDockNode())
				if (node->LocalFlags & ImGuiDockNodeFlags_CentralNode)
					m_bDocked = true;
		}

		void UpdateFocus()
		{
			if (m_bShouldSetFocused)
			{
				m_bShouldSetFocused = false;
				ImGui::SetWindowFocus();
			}
		}
	};
}
