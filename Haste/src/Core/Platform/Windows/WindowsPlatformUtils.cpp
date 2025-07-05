#include "Core/Platform/PlatformUtils.h"
#include "Core/Application.h"

#include "Windows.h"
#include <shlobj.h>
#include <shlwapi.h>
#include <commdlg.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <fileapi.h>

namespace Core {

	float Time::GetTime()
	{
		return glfwGetTime();
	}

	std::string FileDialogs::OpenFile(const char* filter)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		CHAR currentDir[256] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		if (GetCurrentDirectoryA(256, currentDir))
			ofn.lpstrInitialDir = currentDir;
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetOpenFileNameA(&ofn) == TRUE)
			return ofn.lpstrFile;

		return std::string();
	}

	std::string FileDialogs::SaveFile(const char* filter)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		CHAR currentDir[256] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		if (GetCurrentDirectoryA(256, currentDir))
			ofn.lpstrInitialDir = currentDir;
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

		// Sets the default extension by extracting it from the filter
		ofn.lpstrDefExt = strchr(filter, '\0') + 1;

		if (GetSaveFileNameA(&ofn) == TRUE)
			return ofn.lpstrFile;
		
		return std::string();
	}

	std::string FileDialogs::OpenDirectory()
	{
		TCHAR path[MAX_PATH];
		BROWSEINFO bi = { 0 };
		bi.lpszTitle = L"Browse for folder...";
		bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

		if (LPITEMIDLIST pidl = SHBrowseForFolder(&bi))
		{
			SHGetPathFromIDList(pidl, path);

			IMalloc* imalloc = 0;
			if (SUCCEEDED(SHGetMalloc(&imalloc)))
			{
				imalloc->Free(pidl);
				imalloc->Release();
			}

			std::wstring wret(&path[0]);
			std::string ret(wret.begin(), wret.end());
			return ret;
		}

		return "";
	}

	bool FileAttributes::IsFileHidden(const std::string& path)
	{
		std::wstring stemp = std::wstring(path.begin(), path.end());
		DWORD dw = GetFileAttributes(stemp.c_str());

		if (dw == 0xFFFFFFFF)
			return false;

		return !!(dw & FILE_ATTRIBUTE_HIDDEN);
	}

	void System::ExecuteCommand(const char* command)
	{
		system(command);
	}

	void System::ExecuteCommand(const char* command, const std::string& args)
	{
		std::string cmd = command + args;
		system(cmd.c_str());
	}

	void System::StartProcess(const std::string& name, const std::string& args)
	{
		wchar_t* wName = new wchar_t[name.size() + 1];
		mbstowcs(wName, name.data(), name.size() + 1);

		const std::string argsFixed("args " + args);
		wchar_t* wArgs = new wchar_t[argsFixed.size() + 1];
		mbstowcs(wArgs, argsFixed.data(), argsFixed.size() + 1);

		STARTUPINFO startupInfo = { sizeof(STARTUPINFO) };
		PROCESS_INFORMATION processInfo = {};
		if (CreateProcess(wName, wArgs, NULL, NULL, TRUE, 0, NULL, NULL, &startupInfo, &processInfo))
		{
			CloseHandle(processInfo.hThread);
			CloseHandle(processInfo.hProcess);
		}

		delete[] wName, wArgs;
	}

	void FilePaths::AddVirtualPath(const std::string& alias, const std::string& path)
	{
		System::StartProcess("C:/Windows/System32/subst.exe", alias + " \"" + path + "\"");
	}

	void FilePaths::RemoveVirtualPath(const std::string& alias)
	{
		System::StartProcess("C:/Windows/System32/subst.exe", alias + " /D");
	}

	std::string FilePaths::NormalizePath(const char* path, char preferedSeparator)
	{
		std::string ret = "";

		bool sep = false;
		size_t pos = 0;
		size_t posLastSep = 0;
		size_t posLastSepEnd = 0;

		for (pos = 0; path[pos] != '\0'; pos++)
		{
			if (path[pos] == '/' || path[pos] == '\\')
			{
				if (!sep)
				{
					ret.append(path + posLastSepEnd, pos - posLastSepEnd);
					ret.append("/");
					posLastSep = pos;
					sep = true;
				}

				posLastSepEnd = pos + 1;
			}
			else
			{
				sep = false;
			}
		}

		if (posLastSepEnd < pos)
		{
			ret.append(path + posLastSepEnd);
		}

		return ret;
	}

	std::string FilePaths::GetDocumentsDir()
	{
		char szPath[MAX_PATH + 1] = {};
		if (SHGetFolderPathA(NULL, CSIDL_MYDOCUMENTS, NULL, SHGFP_TYPE_CURRENT, szPath) == S_OK)
			return std::string(szPath) + "\\";
		return std::string();
	}

	uint16_t FilePaths::GetPathMax()
	{
		return MAX_PATH;
	}
}
