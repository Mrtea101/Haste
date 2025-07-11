#pragma once

#include "Layers/TestLayer.h"

#if WITH_EDITOR
#include <HasteEditor.cpp>

namespace TestProject {

	class TestProjectApp : public HasteEditor::HasteEditorApp
	{
	public:
		TestProjectApp(const ApplicationSpecification& spec)
			: HasteEditorApp(spec)
		{
			PushLayer(new TestLayer());
		}
	};
}
#else
#include <Core/Application.h>
#include <Project/Project.h>
#include <imgui.h>

namespace TestProject {

	class TestProjectApp : public Core::Application
	{
	public:
		TestProjectApp(const ApplicationSpecification& spec)
			: Core::Application(spec)
		{
			ImGuiIO& io = ImGui::GetIO();
			const float fontSize = 32.f;
			io.FontDefault = io.Fonts->AddFontFromFileTTF(
				Project::GetEngineAsset("Fonts/OpenSans/OpenSans-Regular.ttf").string().c_str(), fontSize);
			io.FontGlobalScale = 0.66666f; // allows non-blurred upscaling (up to 1.0)
			io.ConfigWindowsMoveFromTitleBarOnly = true;

			GetWindow().SetVSync(false);
			GetWindow().SetDarkMode(true, false);

			GetUILayer()->BlockEvents(false);
			PushLayer(new TestLayer());
		}
	};
}
#endif

Application* Core::CreateApplication(ApplicationCommandLineArgs args)
{
	ApplicationSpecification spec
	{
		.CommandLineArgs = args,
		.Name = "Test Project",
		.UserSavedDir = "Saved",
		.ImGuiIniFile = spec.UserSavedDir + "/UI.ini",
		.LogFile = spec.UserSavedDir + "/Log.txt",
	};

	return new TestProject::TestProjectApp(spec);
}
