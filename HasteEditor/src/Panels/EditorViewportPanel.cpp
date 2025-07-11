#define IMGUI_DEFINE_MATH_OPERATORS // for math on vecs

#include "EditorViewportPanel.h"

#include <Core/Input/Input.h>
#include <Engine/Renderer/RenderCommand.h>
#include <Engine/Renderer/Renderer.h>
#include <Engine/Renderer/API/Framebuffer.h>
#include <Core/Application.h>


namespace HasteEditor {

	void EditorViewportPanel::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseButtonPressedEvent>(CORE_BIND_EVENT_FN(OnMouseButtonPressed));
		dispatcher.Dispatch<MouseButtonReleasedEvent>(CORE_BIND_EVENT_FN(OnMouseButtonReleased));
		dispatcher.Dispatch<MouseScrolledEvent>(CORE_BIND_EVENT_FN(OnMouseScrolled));
		dispatcher.Dispatch<KeyPressedEvent>(CORE_BIND_EVENT_FN(OnKeyPressed));
	}

	bool EditorViewportPanel::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() <= Mouse::ButtonMiddle && m_Hovered && !m_DragMode)
		{
			m_MouseDragPos = { Input::GetMouseX(), Input::GetMouseY() };
			m_DragMode = (ViewportDragMode)((uint8_t)e.GetMouseButton() + 1);
			return true;
		}

		return false;
	}

	bool EditorViewportPanel::OnMouseButtonReleased(MouseButtonReleasedEvent& e)
	{
		if (e.GetMouseButton() <= Mouse::ButtonMiddle && m_DragMode)
		{
			m_DragMode = ViewportDragMode::None;
			m_MouseDragPos = {-1.f, -1.f};
			return true;
		}

		return false;
	}

	bool EditorViewportPanel::OnMouseScrolled(MouseScrolledEvent& e)
	{
		if (m_Hovered || m_DragMode)
		{
			float delta = e.GetYOffset() * 0.1f;
			m_Camera.MouseZoom(delta);
			return true;
		}

		return false;
	}

	bool EditorViewportPanel::OnKeyPressed(KeyPressedEvent& e)
	{
		if (m_Focused && e.GetKeyCode() == Key::F)
		{
			m_Camera.ResetView();
			return true;
		}

		return false;
	}

	void EditorViewportPanel::OnUIRender()
	{
		ViewportPanel::OnUIRender();

		ImGui::Begin(m_Name.c_str());

		// Visual for the cursor
		if (m_DragMode)
		{
			int windowPosX, windowPosY;
			Application::Get().GetWindow().GetPos(windowPosX, windowPosY);
			auto pos = ImVec2(windowPosX, windowPosY) + ImVec2(m_MouseDragPos.x, m_MouseDragPos.y);
			const auto min = pos - ImVec2(5, 5);
			const auto max = pos + ImVec2(5, 5);
			ImGui::GetWindowDrawList()->AddRectFilled(min, max, ImColor(1.f, 1.f, 1.f, 1.f), 3.f);
		}
		
		ImGui::SetCursorPos(ImVec2(0, ImGui::GetWindowContentRegionMin().y + ImGui::GetStyle().IndentSpacing));
		ImGui::Indent();
		
		bool bIsPerspective = m_Camera.GetViewMode() == Perspective;
		if (ImGui::Button(bIsPerspective ? "Perspective" : "Orthographic"))
			m_Camera.SetViewMode(bIsPerspective ? Orthographic : Perspective);
		
		ImGui::Checkbox("Show Render Stats", &m_bShowRenderStats);
		if (m_bShowRenderStats)
		{
			ImGui::Text("Draw Calls: %d", Renderer::GetStats().DrawCalls);
			ImGui::Text("Vertices: %d", Renderer::GetStats().GetTotalVertexCount());
			ImGui::Text("Indices: %d", Renderer::GetStats().GetTotalIndexCount());
		}
		ImGui::Unindent();

		ImGui::End();
	}

	void EditorViewportPanel::OnUpdate(Timestep ts)
	{
		ViewportPanel::OnUpdate(ts);

		// Drag controls
		if (m_DragMode)
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_None);
		
			const glm::vec2& mouse = Input::GetMousePosition();
			glm::vec2 delta = (mouse - m_MouseDragPos) * 0.003f;
			Input::SetMousePosition(m_MouseDragPos);
		
			if (m_DragMode == ViewportDragMode::Rotate)
				m_Camera.MouseRotate(delta);
			else if (m_DragMode == ViewportDragMode::Zoom)
				m_Camera.MouseZoom(delta.y);
			else if (m_DragMode == ViewportDragMode::Pan)
				m_Camera.MousePan(delta);
		}
	}
}
