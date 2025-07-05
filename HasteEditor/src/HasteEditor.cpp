#include "Core/Application.h"
#include "Layers/EditorLayer.h"
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
			io.FontDefault = io.Fonts->AddFontFromFileTTF("Content/Fonts/OpenSans/OpenSans-Regular.ttf", fontSize);
			io.FontGlobalScale = 0.66666f; // allows non-blurred upscaling (up to 1.0)
			io.ConfigWindowsMoveFromTitleBarOnly = true;

			GetWindow().SetVSync(false);

			GetUILayer()->BlockEvents(false);
			PushLayer(new EditorLayer());
		}
	};
}

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
