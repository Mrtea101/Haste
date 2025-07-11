#pragma once
#include "Panel.h"

#include "Engine/Renderer/Camera/Camera.h"

namespace Core { class Framebuffer; }

namespace Haste {

	class ViewportPanel : public Panel
	{
	public:
		ViewportPanel();
		virtual void OnUIRender() override;
		virtual void OnUpdate(Timestep ts) override;

		virtual void PostUpdate();

		Camera& GetCamera() { return m_Camera; }

	protected:
		bool m_Focused = false;
		bool m_Hovered = false;

		glm::vec2 m_Size = { 0.0f, 0.0f };
		glm::vec2 m_Bounds[2] = { { 0.0f, 0.0f }, { 0.0f, 0.0f } };

		Ref<Framebuffer> m_Framebuffer;
		Camera m_Camera;
	};
}
