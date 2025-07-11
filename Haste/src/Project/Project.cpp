#include "Project.h"

#include <yaml-cpp/yaml.h>

namespace Haste {

	Ref<Project> Project::New()
	{
		s_ActiveProject = CreateRef<Project>();
		return s_ActiveProject;
	}
	
	Ref<Project> Project::Load(const std::filesystem::path& path)
	{
		Ref<Project> project = CreateRef<Project>();
		if (project->Deserialize(path))
		{
			s_ActiveProject = project;
			return s_ActiveProject;
		}

		return nullptr;
	}
	
	bool Project::SaveActive()
	{
		const std::filesystem::path path = GetProjectDirectory() / (s_ActiveProject->m_Config.Name + ".hasteproj");
		return s_ActiveProject->Serialize(path);
	}

	bool Project::Serialize(const std::filesystem::path& filepath)
	{
		const auto& config = s_ActiveProject->GetConfig();

		YAML::Emitter out;
		{
			out << YAML::BeginMap;
			out << YAML::Key << "Project" << YAML::Value;
			{
				out << YAML::BeginMap;
				out << YAML::Key << "EngineDirectory" << YAML::Value << config.EngineDirectory.string();
				out << YAML::Key << "StartupLevel" << YAML::Value << config.StartupLevel.string();
				out << YAML::EndMap;
			}
			out << YAML::EndMap;
		}

		std::ofstream fout(filepath);
		fout << out.c_str();

		return true;
	}

	bool Project::Deserialize(const std::filesystem::path& filepath)
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filepath.string());
		}
		catch (YAML::Exception e)
		{
			CORE_ERROR("Failed to load project file '{0}'\n     {1}", filepath.string(), e.what());
			return false;
		}

		if (auto projectNode = data["Project"])
		{
			m_Config.Name = filepath.stem().string();
			m_Config.ProjectDirectory = filepath.parent_path().string();
			m_Config.EngineDirectory = projectNode["EngineDirectory"].as<std::string>();
			m_Config.StartupLevel = projectNode["StartupLevel"].as<std::string>();

			if (m_Config.EngineDirectory.empty())
				m_Config.EngineDirectory = "../..";

			return true;
		}

		return false;
	}
}
