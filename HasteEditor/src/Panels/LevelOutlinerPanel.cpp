#include "LevelOutlinerPanel.h"

#include <Engine/Actor/Actor.h>

namespace HasteEditor {

	LevelOutlinerPanel::LevelOutlinerPanel(const Ref<Level>& context)
	{
		m_Name = "Outliner";
		SetContext(context);
	}

	void LevelOutlinerPanel::SetContext(const Ref<Level>& context)
	{
		m_Context = context;
		m_Selection = nullptr;
	}

	void LevelOutlinerPanel::OnUIRender()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		bool bStayOpen = true;
		ImGui::Begin(m_Name.c_str(), &bStayOpen);

		if (ImGui::IsItemClicked(ImGuiMouseButton_Middle) || !bStayOpen)
			m_bWantsToClose = true;

		UpdateDockedState();
		UpdateFocus();

		if (m_Context)
		{
			for (auto&& [id, actor] : m_Context->m_ActorMap)
			{
				RenderActor(actor.get());
			}

			if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered())
				m_Selection = nullptr;

			// Right-click on blank space
			if (ImGui::BeginPopupContextWindow())
			{
				if (ImGui::MenuItem("Create Actor"))
					m_Context->CreateActor("Test1");

				ImGui::EndPopup();
			}

		}
		ImGui::End();

		ImGui::Begin("Properties");
		if (m_Selection)
		{
			RenderComponents(m_Selection);
		}

		ImGui::End();
		ImGui::PopStyleVar();
	}

	void LevelOutlinerPanel::SetSelectedActor(Actor* actor)
	{
		m_Selection = actor;
	}

	void LevelOutlinerPanel::RenderActor(Actor* actor)
	{
		ImGuiTreeNodeFlags flags = ((m_Selection == actor) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		bool bExpanded = ImGui::TreeNodeEx(actor, flags, actor->GetName().c_str());
		if (ImGui::IsItemClicked())
		{
			m_Selection = actor;
		}

		bool bDeleteActor = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete"))
				bDeleteActor = true;

			ImGui::EndPopup();
		}

		if (bExpanded)
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
			if (ImGui::TreeNodeEx((char*)actor + 1, flags, actor->GetName().c_str()))
				ImGui::TreePop();
			ImGui::TreePop();
		}

		if (bDeleteActor)
		{
			m_Context->DestroyActor(actor);
			if (m_Selection == actor)
				m_Selection = nullptr;
		}
	}

	static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = ImGui::GetFontBaked()->Size + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
			values.x = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
			values.y = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
			values.z = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}

	/*template<typename T, typename UIFunction>
	static void DrawComponent(const std::string& name, Actor actor, UIFunction uiFunction)
	{
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
		if (entity.HasComponent<T>())
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
			ImGui::PopStyleVar(
			);
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - lineHeight * 0.5f);
			if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
			{
				ImGui::OpenPopup("ComponentSettings");
			}

			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Remove component"))
					removeComponent = true;

				ImGui::EndPopup();
			}

			if (open)
			{
				uiFunction(component);
				ImGui::TreePop();
			}

			if (removeComponent)
				entity.RemoveComponent<T>();
		}
	}*/

	static bool RenderComponent(ActorComponent* component)
	{
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
		ImGui::Separator();
		bool bExpanded = ImGui::TreeNodeEx(component, treeNodeFlags, component->Name.c_str());
		ImGui::PopStyleVar();

		const float lineHeight = ImGui::GetFontBaked()->Size + GImGui->Style.FramePadding.y * 2.0f;
		ImGui::SameLine(ImGui::GetContentRegionAvail().x - lineHeight * 0.5f);
		if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
		{
			ImGui::OpenPopup("ComponentSettings");
		}

		bool bDelete = false;
		if (ImGui::BeginPopup("ComponentSettings"))
		{
			if (ImGui::MenuItem("Remove component"))
				bDelete = true;

			ImGui::EndPopup();
		}

		if (bExpanded)
		{
			component->OnRenderEditorProperties();
			ImGui::TreePop();
		}

		if (bDelete)
			return false;

		ImGui::Indent();
		for (size_t i = 0; i < component->Children.size(); i++)
		{
			if (!RenderComponent(component->Children[i].get()))
				i--;
		}
		ImGui::Unindent();

		return true;
	}

	void LevelOutlinerPanel::RenderComponents(Actor* actor)
	{
		actor->OnRenderEditorProperties();

		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent"))
		{
			RenderAddComponentItem<Shape2DComponent>("Shape2DComponent");

			ImGui::EndPopup();
		}

		ImGui::TextUnformatted("Components:");
		ImGui::Indent();
		for (size_t i = 0; i < actor->m_Components.size(); i++)
		{
			if (!RenderComponent(actor->m_Components[i].get()))
				i--;
		}
		ImGui::Unindent();
	}

	template<typename T>
	void LevelOutlinerPanel::RenderAddComponentItem(const std::string& name)
	{
		if (ImGui::MenuItem(name.c_str()))
		{
			m_Selection->AddNewComponent(m_Context->GetNextID(), name);
			ImGui::CloseCurrentPopup();
		}
	}
}
