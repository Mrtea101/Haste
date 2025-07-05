#pragma once

#include <Core/UI/Panel.h>

#include <Engine/Renderer/Camera/Camera.h>
#include <Core/Events/KeyEvent.h>
#include <Core/Events/MouseEvent.h>


namespace Core { class Framebuffer; }

enum ViewportDragMode : uint8_t
{
	None,
	Rotate,
	Zoom,
	Pan,
};

namespace HasteEditor
{
	class ViewportPanel : public Panel
	{
	public:
		ViewportPanel();
		void OnUIRender() override;
		void OnUpdate(Timestep ts) override;
		void OnUIRenderProperties() override;
		void OnEvent(Event& e) override;

		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
		bool OnMouseButtonReleased(MouseButtonReleasedEvent& e);
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnKeyPressed(KeyPressedEvent& e);

	private:
		bool m_Focused = false;
		bool m_Hovered = false;

		bool bShowRenderStats = true;

		ViewportDragMode m_DragMode = ViewportDragMode::None;
		glm::vec2 m_MouseDragPos = { 0.0f, 0.0f };

		glm::vec2 m_Size = { 0.0f, 0.0f };
		glm::vec2 m_Bounds[2] = { { 0.0f, 0.0f }, { 0.0f, 0.0f } };
		
		Ref<Framebuffer> m_Framebuffer;
		int m_HoveredIndex = -1;

		Camera m_Camera;
	};
}
