#include "GameLayer.h"
#include <Core/Application.h>
#include <imgui.h>


GameLayer::GameLayer()
	: Layer("GameLayer")
{
}

void GameLayer::OnAttach()
{
	CORE_PROFILE_FUNCTION();
}

void GameLayer::OnDetach()
{
	CORE_PROFILE_FUNCTION();
}

void GameLayer::OnUIRender()
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

	const char* dockspaceName = "GameLayer.Dockspace";
	ImGui::Begin(dockspaceName, 0, window_flags);
	ImGui::PopStyleVar(3);

	ImGuiStyle& style = ImGui::GetStyle();
	float minWinSizeX = style.WindowMinSize.x;
	style.WindowMinSize.x = 370.0f;
	m_DockspaceID = ImGui::GetID(dockspaceName);
	ImGui::DockSpace(m_DockspaceID);
	style.WindowMinSize.x = minWinSizeX;

	ImGui::ShowDemoWindow();

	ImGui::End();
}
