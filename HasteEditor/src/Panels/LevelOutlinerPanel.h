#pragma once
#include <UI/Panels/Panel.h>

#include <Engine/Level/Level.h>

namespace HasteEditor {

	class LevelOutlinerPanel : public Panel
	{
	public:
		LevelOutlinerPanel(const Ref<Level>& level);

		void SetContext(const Ref<Level>& level);

		void OnUIRender() override;

		Actor* GetSelectedActor() const { return m_Selection; }
		void SetSelectedActor(Actor* actor);

	private:
		template<typename T>
		void RenderAddComponentItem(const std::string& name);

		void RenderActor(Actor* actor);
		void RenderComponents(Actor* actor);

	private:
		Ref<Level> m_Context;
		Actor* m_Selection;
	};
}

