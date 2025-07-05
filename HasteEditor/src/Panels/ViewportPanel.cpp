#define IMGUI_DEFINE_MATH_OPERATORS // for math on vecs

#include "ViewportPanel.h"

#include <Core/Input/Input.h>
#include <Engine/Renderer/RenderCommand.h>
#include <Engine/Renderer/Renderer.h>
#include <Engine/Renderer/API/Framebuffer.h>
#include <Core/Application.h>


namespace HasteEditor {

	ViewportPanel::ViewportPanel()
	{
		m_Name = "Viewport";

		FramebufferSpecification fbSpec =
		{
			.Width = 1280,
			.Height = 720,
			.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth },
		};
		m_Framebuffer = Framebuffer::Create(fbSpec);
	}

	void ViewportPanel::OnUIRender()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		bool bStayOpen = true;
		ImGui::Begin(m_Name.c_str(), &bStayOpen);

		if (ImGui::IsItemClicked(ImGuiMouseButton_Middle) || !bStayOpen)
			m_bWantsToClose = true;

		m_Focused = ImGui::IsWindowFocused();

		UpdateDockedState();
		UpdateFocus();

		// Display render
		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();
		m_Bounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		m_Bounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

		float spaceForResizeCorner = ImGui::IsWindowDocked() ? 0 : 15;

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		m_Size = { viewportPanelSize.x - spaceForResizeCorner, viewportPanelSize.y - spaceForResizeCorner };

		uint64_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
		ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{m_Size.x, m_Size.y}, ImVec2{0, 1}, ImVec2{1, 0});
		m_Hovered = ImGui::IsItemHovered();

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

		ImGui::SetCursorPos(ImVec2(0, viewportMinRegion.y + ImGui::GetStyle().IndentSpacing));
		ImGui::Indent();

		bool bIsPerspective = m_Camera.GetViewMode() == Perspective;
		if (ImGui::Button(bIsPerspective ? "Perspective" : "Orthographic"))
			m_Camera.SetViewMode(bIsPerspective ? Orthographic : Perspective);

		ImGui::Checkbox("Show Render Stats", &bShowRenderStats);
		if (bShowRenderStats)
		{
			ImGui::Text("Fps: %.2f / %.3f ms", ImGui::GetIO().Framerate, 1 / ImGui::GetIO().Framerate);
			ImGui::Text("Draw Calls: %d", Renderer::GetStats().DrawCalls);
			ImGui::Text("Vertices: %d", Renderer::GetStats().GetTotalVertexCount());
			ImGui::Text("Indices: %d", Renderer::GetStats().GetTotalIndexCount());
		}
		ImGui::Unindent();

		ImGui::End();
		ImGui::PopStyleVar();
	}

	void ViewportPanel::OnUpdate(Timestep ts)
	{
		// Update Camera
		m_Camera.OnUpdate(ts);

		// Resize
		if (FramebufferSpecification spec = m_Framebuffer->GetSpecification();
			m_Size.x > 0.0f && m_Size.y > 0.0f && // zero sized framebuffer is invalid
			(spec.Width != m_Size.x || spec.Height != m_Size.y))
		{
			m_Framebuffer->Resize((uint32_t)m_Size.x, (uint32_t)m_Size.y);
			m_Camera.SetViewportSize(m_Size.x, m_Size.y);
		}

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

		// Render
		Renderer::ResetStats();
		m_Framebuffer->Bind();
		RenderCommand::SetClearColor({ 0.1f, 0.105f, 0.11f, 1.0f });
		RenderCommand::Clear();

		m_Framebuffer->ClearAttachment(1, -1);
		
		Renderer::BeginScene(m_Camera);
		Renderer::DrawQuad({ 0.f,0.f }, { 1.f,1.f }, { 0.f,0.f,1.f,1.f }); // Test quad
		Renderer::DrawTriangles({ { -0.5f, 0.5f }, { 0.5f, 0.5f }, { 0.0f,  1.5f } }, {0.f,0.f,1.f,1.f}); // Test tri
		Renderer::EndScene();

		m_Framebuffer->Unbind();
	}

	void ViewportPanel::OnUIRenderProperties()
	{
	}

	void ViewportPanel::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseButtonPressedEvent>(CORE_BIND_EVENT_FN(OnMouseButtonPressed));
		dispatcher.Dispatch<MouseButtonReleasedEvent>(CORE_BIND_EVENT_FN(OnMouseButtonReleased));
		dispatcher.Dispatch<MouseScrolledEvent>(CORE_BIND_EVENT_FN(OnMouseScrolled));
		dispatcher.Dispatch<KeyPressedEvent>(CORE_BIND_EVENT_FN(OnKeyPressed));
	}

	bool ViewportPanel::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() <= Mouse::ButtonMiddle && m_Hovered && !m_DragMode)
		{
			m_MouseDragPos = { Input::GetMouseX(), Input::GetMouseY() };
			m_DragMode = (ViewportDragMode)((uint8_t)e.GetMouseButton() + 1);
			return true;
		}

		return false;
	}

	bool ViewportPanel::OnMouseButtonReleased(MouseButtonReleasedEvent& e)
	{
		if (e.GetMouseButton() <= Mouse::ButtonMiddle && m_DragMode)
		{
			m_DragMode = ViewportDragMode::None;
			m_MouseDragPos = {-1.f, -1.f};
			return true;
		}

		return false;
	}

	bool ViewportPanel::OnMouseScrolled(MouseScrolledEvent& e)
	{
		if (m_Hovered || m_DragMode)
		{
			float delta = e.GetYOffset() * 0.1f;
			m_Camera.MouseZoom(delta);
			return true;
		}

		return false;
	}

	bool ViewportPanel::OnKeyPressed(KeyPressedEvent& e)
	{
		if (m_Focused && e.GetKeyCode() == Key::F)
		{
			m_Camera.ResetView();
			return true;
		}

		return false;
	}
}
