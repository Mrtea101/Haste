#pragma once

#include <Core/Core.h>
#include <Core/UI/Layer.h>
#include <UI/Panels/Panel.h>
#include <Core/Events/KeyEvent.h>
#include <Core/Events/MouseEvent.h>
#include <Engine/Level/Level.h>

namespace HasteEditor {

	enum LevelState
	{
		Edit = 0, Play = 1, Simulate = 2
	};

	class EditorLayer : public Layer
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
		void OnRenderMenuBar();

		template<typename T>
		T* FindPanelByClass();

		static EditorLayer* Get() { return m_Inst; };
		unsigned int m_DockspaceID = 0;

	private:
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

		static EditorLayer* m_Inst;
		bool m_bNeedSetupDefaultLayout = false;

		std::vector<Scope<Panel>> m_Panels;

		Ref<Level> m_EditorLevel;
		Ref<Level> m_ActiveLevel;

		LevelState m_LevelState = LevelState::Edit;
	};
}
