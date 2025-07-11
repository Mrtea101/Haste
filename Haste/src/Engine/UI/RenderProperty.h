#pragma once

#include "Core/Core.h"
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

namespace Haste {

	namespace RenderPropertyUtils
	{
		inline static float GetDragSpeed(float value)
		{
			return glm::max(0.001f, value * 0.001f);
		}
	}

	class RenderProperty
	{
	public:
		static void Render(const std::string& name, std::string& prop)
		{
			ImGui::TextUnformatted(name.c_str());
			ImGui::InputText(("##" + name).c_str(), &prop);
		}

		static void Render(const std::string& name, int& prop, int min = INT_MIN, int max = INT_MAX)
		{
			ImGui::TextUnformatted(name.c_str());
			ImGui::DragInt(("##" + name).c_str(), &prop, RenderPropertyUtils::GetDragSpeed(prop), min, max);
		}

		static void Render(const std::string& name, float& prop, float min = -FLT_MAX / 2.f, float max = FLT_MAX / 2.f)
		{
			ImGui::TextUnformatted(name.c_str());
			ImGui::DragFloat(("##" + name).c_str(), &prop, RenderPropertyUtils::GetDragSpeed(prop), min, max);
		}

		static void Render(const std::string& name, glm::vec3& prop, float min = -FLT_MAX / 2.f, float max = FLT_MAX / 2.f)
		{
			ImGui::TextUnformatted(name.c_str());
			ImGui::DragFloat3(("##" + name).c_str(), &prop.x, RenderPropertyUtils::GetDragSpeed(prop.length()), min, max);
		}

		static void RenderColor(const std::string& name, glm::vec3& prop)
		{
			ImGui::TextUnformatted(name.c_str());
			ImGui::ColorPicker3(("##" + name).c_str(), &prop.r);
		}

		static void RenderColor(const std::string& name, glm::vec4& prop)
		{
			ImGui::TextUnformatted(name.c_str());
			ImGui::ColorPicker4(("##" + name).c_str(), &prop.r);
		}

		static void Render(const std::string& name, std::vector<std::string>& propArr)
		{
			ImGui::TextUnformatted(name.c_str());
			ImGui::SameLine();
			if (ImGui::Button(("+##+" + name).c_str()))
			{
				propArr.emplace_back();
			}
			ImGui::Indent();
			for (size_t i = 0; i < propArr.size(); i++)
			{
				const std::string& label = FormatString("[{}]", i);
				ImGui::TextUnformatted(label.c_str());
				ImGui::InputText(("##" + name + label).c_str(), &propArr[i]);
			}
			ImGui::Unindent();
		}

		static void Render(const std::string& name, std::vector<int>& propArr, int min = INT_MIN, int max = INT_MAX)
		{
			ImGui::TextUnformatted(name.c_str());
			ImGui::SameLine();
			if (ImGui::Button(("+##+" + name).c_str()))
			{
				propArr.emplace_back();
			}
			ImGui::Indent();
			for (size_t i = 0; i < propArr.size(); i++)
			{
				const std::string& label = FormatString("[{}]", i);
				ImGui::TextUnformatted(label.c_str());
				ImGui::SliderInt(("##" + name + label).c_str(), &propArr[i], min, max);
			}
			ImGui::Unindent();
		}

		static void Render(const std::string& name, std::vector<float>& propArr, int min = INT_MIN, int max = INT_MAX)
		{
			ImGui::TextUnformatted(name.c_str());
			ImGui::SameLine();
			if (ImGui::Button(("+##+" + name).c_str()))
			{
				propArr.emplace_back();
			}
			ImGui::Indent();
			for (size_t i = 0; i < propArr.size(); i++)
			{
				const std::string& label = FormatString("[{}]", i);
				ImGui::TextUnformatted(label.c_str());
				ImGui::SliderFloat(("##" + name + label).c_str(), &propArr[i], min, max);
			}
			ImGui::Unindent();
		}
	};
}
