#pragma once

#include <string>

namespace Core {

	class FileDialogs
	{
	public:
		// These return empty strings if cancelled
		static std::string OpenFile(const char* filter);
		static std::string SaveFile(const char* filter);
		static std::string OpenDirectory();
	};

	class Time
	{
	public:
		static float GetTime();
	};

	class FileAttributes
	{
	public:
		static bool IsFileHidden(const std::string& path);
	};

	class System
	{
	public:
		static void ExecuteCommand(const char* command);
		static void ExecuteCommand(const char* command, const std::string& args);
		static void StartProcess(const std::string& name, const std::string& args = "");
	};

	class FilePaths
	{
	public:
		static void AddVirtualPath(const std::string& alias, const std::string& path);
		static void RemoveVirtualPath(const std::string& alias);
		static std::string NormalizePath(const char* path, char preferedSeparator = '/');
		static std::string GetDocumentsDir();
		static uint16_t GetPathMax();
	};
}
