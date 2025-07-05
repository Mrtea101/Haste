#pragma once

#include <Core/Core.h>
#include <Core/UI/Layer.h>
#include <Core/UI/Panel.h>
#include <Core/Events/KeyEvent.h>
#include <Core/Events/MouseEvent.h>

namespace HasteEditor {

	class EditorLayer : public Core::Layer
	{
	public:
		EditorLayer();
		~EditorLayer() = default;

		void OnAttach() override;
		void OnDetach() override;

		void OnUpdate(Timestep ts) override;
		void OnUIRender() override;
		void OnEvent(Event& e) override;

		void OnRenderPanels();

		static EditorLayer* Get() { return m_Inst; };
		unsigned int m_DockspaceID = 0;

		std::vector<Ref<Panel>> Panels;

	private:
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

		static EditorLayer* m_Inst;
		bool m_bNeedSetupDefaultLayout = false;
	};
}
