#pragma once

#include "Core/Core.h"
#include "Core/Timestep.h"
#include "Engine/Renderer/Camera/Camera.h"

namespace HasteEditor { class LevelOutlinerPanel; }

namespace Haste
{
	class Actor;

	class Level
	{
	public:
		Level();
		~Level();

		template<typename T = Actor>
		T* CreateActor(const std::string& name = std::string());
		void DestroyActor(Actor* actor);

		void OnRuntimeStart();
		void OnRuntimeStop();

		void OnSimulationStart();
		void OnSimulationStop();

		void OnUpdateRuntime(Timestep ts);
		void OnUpdateSimulation(Timestep ts, Camera& camera);
		void OnUpdateEditor(Timestep ts, Camera& camera);

		template<typename T>
		T* FindActorByClass();
		Actor* FindActorByName(std::string_view name);
		Actor* FindActorByID(uint32_t id);

		std::string& GetName() { return m_Name; }

		bool IsRunning() const { return m_IsRunning; }
		bool IsPaused() const { return m_IsPaused; }

		void SetPaused(bool paused) { m_IsPaused = paused; }

		void Step(int frames = 1);

		uint32_t GetNextID() { return m_NextID++; }

	protected:

		void RenderScene(const Camera& camera = Camera(), const glm::mat4& transform = glm::mat4(1.f));

	protected:
		bool m_IsRunning = false;
		bool m_IsPaused = false;
		int m_StepFrames = 0;

		std::string m_Name;
		std::unordered_map<uint32_t, Scope<Actor>> m_ActorMap;
		uint32_t m_NextID = 1;

		friend class HasteEditor::LevelOutlinerPanel;
	};

	class DemoLevel : public Level
	{
	public:
		DemoLevel();
		~DemoLevel() {};
	};
}
