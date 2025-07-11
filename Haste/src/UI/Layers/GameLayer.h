#pragma once

#include <Core/Core.h>
#include <Core/UI/Layer.h>
#include <UI/Panels/Panel.h>
#include <Core/Events/KeyEvent.h>
#include <Core/Events/MouseEvent.h>
#include <Engine/Level/Level.h>

namespace Haste {

	class GameLayer : public Layer
	{
	public:
		GameLayer();
		~GameLayer() = default;

		void OnAttach() override;
		void OnDetach() override;

		void OnUpdate(Timestep ts) override;
		void OnUIRender() override;
		void OnEvent(Event& e) override;

		virtual bool OnKeyPressed(KeyPressedEvent& e);
		virtual bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

		virtual void OnRenderPanels();

		template<typename T>
		T* FindPanelByClass();

	private:
		unsigned int m_DockspaceID = 0;
		Ref<Level> m_ActiveLevel;

		std::vector<Scope<Panel>> m_Panels;
	};
}
