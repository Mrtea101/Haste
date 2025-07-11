#define IMGUI_DEFINE_MATH_OPERATORS // for math on vecs

#include "ViewportPanel.h"

#include "Core/Input/Input.h"
#include "Core/Application.h"
#include "Engine/Renderer/RenderCommand.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/API/Framebuffer.h"


namespace Haste {

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
		ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ m_Size.x, m_Size.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		m_Hovered = ImGui::IsItemHovered();

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

		// Render
		Renderer::ResetStats();
		m_Framebuffer->Bind();
		RenderCommand::SetClearColor({ 0.1f, 0.105f, 0.11f, 1.0f });
		RenderCommand::Clear();

		m_Framebuffer->ClearAttachment(1, -1);

		// After this, don't forget to call PostUpdate
	}

	void ViewportPanel::PostUpdate()
	{
		m_Framebuffer->Unbind();
	}
}
