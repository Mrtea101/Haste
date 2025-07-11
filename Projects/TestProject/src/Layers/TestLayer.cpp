#include "TestLayer.h"
#include <Core/Application.h>
#include <imgui.h>


TestLayer::TestLayer()
	: Layer("TestLayer")
{
	Application::Get().GetWindow().SetDarkMode(true, false);
}

void TestLayer::OnAttach()
{
	CORE_PROFILE_FUNCTION();

	// Command line
	auto commandLineArgs = Application::Get().GetSpecification().CommandLineArgs;
	if (commandLineArgs.Count > 1)
	{

	}
}

void TestLayer::OnDetach()
{
	CORE_PROFILE_FUNCTION();
}

void TestLayer::OnUIRender()
{
	CORE_PROFILE_FUNCTION();

	static bool bFirstRender = true; // showing window from now prevents flashbang
	if (bFirstRender)
	{
		bFirstRender = false;

		bool StartMaximized = false;
		if (StartMaximized)
			Application::Get().GetWindow().MaximizeWindow(); // will also make window visible
		else
			Application::Get().GetWindow().SetShouldShow(true);
	}

	// Setup Dockspace
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	const char* dockspaceName = "TestLayer.Dockspace";
	ImGui::Begin(dockspaceName, 0, window_flags);
	ImGui::PopStyleVar(3);

	ImGuiStyle& style = ImGui::GetStyle();
	float minWinSizeX = style.WindowMinSize.x;
	style.WindowMinSize.x = 370.0f;
	m_DockspaceID = ImGui::GetID(dockspaceName);
	ImGui::DockSpace(m_DockspaceID);
	style.WindowMinSize.x = minWinSizeX;

	// Render panels
	
	ImGui::End();
}
