#pragma once

#include <string>
#include <filesystem>

#include "Core/Core.h"

namespace Haste {

	struct ProjectConfig
	{
		std::string Name;
		std::filesystem::path ProjectDirectory;
		std::filesystem::path EngineDirectory = "../..";
		std::filesystem::path StartupLevel;
	};

	class Project
	{
	public:
		static const std::filesystem::path& GetProjectDirectory()
		{
			CORE_ASSERT(s_ActiveProject);
			return s_ActiveProject->m_Config.ProjectDirectory;
		}

		static std::filesystem::path GetProjectContentDirectory()
		{
			CORE_ASSERT(s_ActiveProject);
			return GetProjectDirectory() / "Content";
		}

		static std::filesystem::path GetEngineDirectory()
		{
			return s_ActiveProject
				? s_ActiveProject->m_Config.EngineDirectory
				: "../..";
		}

		static std::filesystem::path GetEngineContentDirectory()
		{
			return GetEngineDirectory() / "Haste/Content";
		}

		static std::filesystem::path GetEngineAsset(const std::string& path)
		{
			return GetEngineContentDirectory() / path;
		}

#if WITH_EDITOR
		static std::filesystem::path GetEditorContentDirectory()
		{
			CORE_ASSERT(s_ActiveProject);
			return s_ActiveProject->m_Config.EngineDirectory / "HasteEditor/Content";
		}
#endif

		ProjectConfig& GetConfig() { return m_Config; }

		static Ref<Project> GetActive() { return s_ActiveProject; }

		static Ref<Project> New();
		static Ref<Project> Load(const std::filesystem::path& path);
		static bool SaveActive();

		bool Serialize(const std::filesystem::path& filepath);
		bool Deserialize(const std::filesystem::path& filepath);

	private:
		ProjectConfig m_Config;
		inline static Ref<Project> s_ActiveProject;
	};

}
