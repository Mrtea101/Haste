
#include "Layers/EditorLayer.h"

#include <Core/Application.h>
#include <Project/Project.h>
#include <imgui.h>

using namespace Core;

namespace HasteEditor {

	class HasteEditorApp : public Core::Application
	{
	public:
		HasteEditorApp(const ApplicationSpecification& spec)
			: Application(spec)
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
			PushLayer(new EditorLayer());
		}
	};
}

#if !WITH_PROJECT
Application* Core::CreateApplication(ApplicationCommandLineArgs args)
{
	ApplicationSpecification spec
	{
		.CommandLineArgs = args,
		.Name = "Haste Editor",
		.UserSavedDir = "Saved",
		.ImGuiIniFile = spec.UserSavedDir + "/UI.ini",
		.LogFile = spec.UserSavedDir + "/Log.txt",
	};

	return new HasteEditor::HasteEditorApp(spec);
}
#endif
