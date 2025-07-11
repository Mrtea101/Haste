#pragma once
#include <UI/Panels/ViewportPanel.h>

#include <Core/Events/KeyEvent.h>
#include <Core/Events/MouseEvent.h>


enum ViewportDragMode : uint8_t
{
	None,
	Rotate,
	Zoom,
	Pan,
};

namespace HasteEditor {

	class EditorViewportPanel : public ViewportPanel
	{
	public:
		EditorViewportPanel() = default;
		virtual void OnUIRender() override;
		virtual void OnUpdate(Timestep ts) override;
		virtual void OnEvent(Event& e) override;

		virtual bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
		virtual bool OnMouseButtonReleased(MouseButtonReleasedEvent& e);
		virtual bool OnMouseScrolled(MouseScrolledEvent& e);
		virtual bool OnKeyPressed(KeyPressedEvent& e);

	private:
		bool m_bShowRenderStats = false;

		ViewportDragMode m_DragMode = ViewportDragMode::None;
		glm::vec2 m_MouseDragPos = { 0.0f, 0.0f };
		int m_HoveredIndex = -1;
	};
}
