#pragma once

#include "Core/Core.h"
#include "ActorComponent.h"

#if WITH_EDITOR
#include "Engine/UI/RenderProperty.h"
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


namespace HasteEditor { class LevelOutlinerPanel; }

namespace Haste {

	class Actor
	{
	public:
		Actor(const Actor& other) = default;
		Actor(uint32_t id = 0, const std::string& name = std::string())
			: m_ID(id)
			, m_Name(name.empty() ? "Actor" : name)
		{};

		uint32_t GetID() const { return m_ID; }
		void SetID(uint32_t id) { m_ID = id; }

		const std::string& GetName() { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }

		glm::mat4 GetTransform() const
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(m_Rotation));

			return glm::translate(glm::mat4(1.0f), m_Translation)
				* rotation
				* glm::scale(glm::mat4(1.0f), m_Scale);
		}

		template<typename T = ActorComponent>
		T* AddNewComponent(uint32_t id, const std::string& name = std::string())
		{
			static_assert(std::is_base_of<ActorComponent, T>(), "AddNewComponent called with non-ActorComponent template param");
			T* component = new T(id, name);
			m_Components.emplace_back(Scope<T>(component));
			return component;
		}

		bool operator==(const Actor& other) const
		{
			return m_ID == other.m_ID;
		}

		bool operator!=(const Actor& other) const
		{
			return !(*this == other);
		}

		virtual void OnRender()
		{
			for (auto& component : m_Components)
			{
				component->OnRender(GetTransform());
			}
		}

#if WITH_EDITOR
		virtual void OnRenderEditorProperties()
		{
			RenderProperty::Render("Translation", m_Translation);
			RenderProperty::Render("Rotation", m_Rotation);
			RenderProperty::Render("Scale", m_Scale);
			RenderProperty::Render("Tags", m_Tags);
		}

		virtual void OnRenderEditorComponents()
		{
			ImGui::TextUnformatted("Components:");
			ImGui::Indent();
			for (auto& component : m_Components)
			{
				component->OnRenderEditorProperties();
			}
			ImGui::Unindent();
		}
#endif

	private:
		uint32_t m_ID;
		std::string m_Name;
		std::vector<Scope<ActorComponent>> m_Components;

		glm::vec3 m_Translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_Scale = { 1.0f, 1.0f, 1.0f };
		std::vector<std::string> m_Tags;

		friend class HasteEditor::LevelOutlinerPanel;
	};

	class CameraActor : public Actor
	{
	public:
		Camera& GetCamera() { return m_Camera; }
	private:
		Camera m_Camera;
	};
}
