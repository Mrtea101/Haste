#pragma once

#include "Core/Core.h"
#include "Engine/Renderer/API/Texture.h"
#include "Engine/Renderer/Camera/Camera.h"
#include "Engine/Renderer/Renderer.h"

#if WITH_EDITOR == true
#include "Engine/UI/RenderProperty.h"
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Haste {

	struct ActorComponent
	{
		ActorComponent(uint32_t id, const std::string& name)
			: ID(id)
			, Name(name)
		{};

		virtual void OnRender(const glm::mat4& transformParent)
		{
			for (auto& child : Children)
			{
				child->OnRender(transformParent);
			}
		};

#if WITH_EDITOR
		inline virtual void OnRenderEditorProperties() {}
#endif
		uint32_t ID;
		std::string Name;
		std::vector<std::string> Tags;
		std::vector<Scope<ActorComponent>> Children;
	};

	struct SceneComponent : ActorComponent
	{
		SceneComponent(uint32_t id, const std::string& name, const glm::vec3& translation = glm::vec3())
			: ActorComponent(id, name)
			, Translation(translation)
		{}
		
		glm::mat4 GetTransform() const
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

			return glm::translate(glm::mat4(1.0f), Translation)
				* rotation
				* glm::scale(glm::mat4(1.0f), Scale);
		}

		virtual void OnRender(const glm::mat4& transformParent) override
		{
			for (auto& child : Children)
			{
				child->OnRender(GetTransform() * transformParent);
			}
		};

#if WITH_EDITOR
		inline virtual void OnRenderEditorProperties() override
		{
			RenderProperty::Render("Translation", Translation);
			RenderProperty::Render("Rotation", Rotation);
			RenderProperty::Render("Scale", Scale);
		}
#endif

		glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };
	};

	struct SpriteComponent : SceneComponent
	{
		SpriteComponent(uint32_t id, const std::string& name, const glm::vec3& translation = glm::vec3(), const glm::vec4& color = { 1.0f, 1.0f, 1.0f, 1.0f })
			: SceneComponent(id, name, translation)
			, Color(color)
		{}

		virtual void OnRender(const glm::mat4& transformParent) override
		{
			SceneComponent::OnRender(transformParent);

			const auto& transform = GetTransform() * transformParent;
			Renderer::DrawQuad(transform, Texture, TilingFactor, Color, (int)ID);
		};

#if WITH_EDITOR
		inline virtual void OnRenderEditorProperties() override
		{
			SceneComponent::OnRenderEditorProperties();
			RenderProperty::Render("Translation", Translation);
			RenderProperty::Render("Rotation", Rotation);
			RenderProperty::Render("Scale", Scale);
		}
#endif

		glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
		Ref<Texture2D> Texture;
		float TilingFactor = 1.0f;
	};

	struct Shape2DComponent : SceneComponent
	{
		Shape2DComponent(uint32_t id, const std::string& name, const glm::vec3& translation = glm::vec3(), const glm::vec4& color = { 1.0f, 1.0f, 1.0f, 1.0f })
			: SceneComponent(id, name, translation)
			, Color(color)
		{}

		virtual void OnRender(const glm::mat4& transformParent) override
		{
			SceneComponent::OnRender(transformParent);

			const auto& transform = GetTransform() * transformParent;
			Renderer::DrawCircle(transform, Color, Thickness, Fade, (int)ID);
		};

#if WITH_EDITOR
		inline virtual void OnRenderEditorProperties() override
		{
			SceneComponent::OnRenderEditorProperties();
			RenderProperty::RenderColor("Color", Color);
			RenderProperty::Render("Thickness", Thickness);
			RenderProperty::Render("Fade", Fade);
		}
#endif

		glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
		float Thickness = 1.0f;
		float Fade = 0.005f;
	};

	//struct CameraComponent : SceneComponent
	//{
	//	Camera Cam;
	//	bool FixedAspectRatio = false;
	//};
}
