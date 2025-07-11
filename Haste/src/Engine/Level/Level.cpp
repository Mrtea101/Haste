#include "Level.h"

#include "Engine/Actor/Actor.h"
#include "Engine/Actor/ActorComponent.h"
#include "Engine/Renderer/Renderer.h"

#include <glm/glm.hpp>

namespace Haste {

	Level::Level()
	{
	}

	Level::~Level()
	{
	}

	template<typename T>
	T* Level::CreateActor(const std::string& name)
	{
		static_assert(std::is_base_of<Actor, T>(), "CreateActor called with non-actor template param");
		T* actor = new T(GetNextID(), name);
		m_ActorMap[actor->GetID()] = Scope<Actor>(actor);
		return actor;
	}

	void Level::DestroyActor(Actor* actor)
	{
		m_ActorMap.erase(actor->GetID());
	}

	void Level::OnRuntimeStart()
	{
		m_IsRunning = true;
	}

	void Level::OnRuntimeStop()
	{
		m_IsRunning = false;
	}

	void Level::OnSimulationStart()
	{
	}

	void Level::OnSimulationStop()
	{
	}

	void Level::OnUpdateRuntime(Timestep ts)
	{
		// Render
		Camera* cam = nullptr;
		glm::mat4 camTransform(1.0);

		if (CameraActor* camActor = FindActorByClass<CameraActor>())
		{
			cam = &camActor->GetCamera();
			camTransform = camActor->GetTransform();
		}

		RenderScene(cam ? *cam : Camera(), camTransform);
	}

	void Level::RenderScene(const Camera& camera, const glm::mat4& transform)
	{
		Renderer::BeginScene(camera, transform);

		for (auto&& [uuid, actor] : m_ActorMap)
			actor->OnRender();

		Renderer::EndScene();
	}

	void Level::OnUpdateSimulation(Timestep ts, Camera& camera)
	{
		// Render
		RenderScene(camera);
	}

	void Level::OnUpdateEditor(Timestep ts, Camera& camera)
	{
		// Render
		RenderScene(camera);
	}

	void Level::Step(int frames)
	{
		m_StepFrames = frames;
	}

	template<typename T>
	T* Level::FindActorByClass()
	{
		static_assert(std::is_base_of<Actor, T>(), "FindActorByClass called with non-actor template param");
		for (auto&& [uuid, actor] : m_ActorMap)
		{
			if (T* actorCasted = static_cast<T*>(actor.get()))
				return actorCasted;
		}
		return nullptr;
	}

	Actor* Level::FindActorByName(std::string_view name)
	{
		for (auto&& [uuid, actor] : m_ActorMap)
		{
			if (actor->GetName() == name)
				return actor.get();
		}
		return nullptr;
	}

	Actor* Level::FindActorByID(uint32_t id)
	{
		const auto found = m_ActorMap.find(id);
		if (found != m_ActorMap.end())
			return found->second.get();
		return nullptr;
	}

	DemoLevel::DemoLevel()
	{
		m_Name = "DemoLevel";
		Actor* actor = CreateActor("TestActor");
		actor->AddNewComponent<Shape2DComponent>(GetNextID(), "TestComponent");
	}
}
