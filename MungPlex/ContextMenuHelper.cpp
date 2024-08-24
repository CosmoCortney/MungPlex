#include <windows.h>
#include <shellapi.h>
#include "ContextMenuHelper.hpp"
#include "imgui.h"
#include "GLFW/glfw3.h"


void MungPlex::ContextMenuHelper::DrawWindow() {
	ImGui::Begin("ContextMenu", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Placeholder", "Something")) {  }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Placeholder", "")) {  }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::MenuItem("Memory Viewer")) {}
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("Wiki")) { GetInstance().OpenGithubLink(); }
            if (ImGui::MenuItem("Github")) { GetInstance().OpenGithubLink(); }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

	ImGui::End();
}

MungPlex::ContextMenuHelper::ContextMenuHelper()
{
}

MungPlex::ContextMenuHelper::~ContextMenuHelper()
{
}

void MungPlex::ContextMenuHelper::OpenGithubLink()
{
    ShellExecute(0, 0, "https://github.com/CosmoCortney/MungPlex", 0, 0, SW_SHOW);
}
void MungPlex::ContextMenuHelper::OpenGithubWikiLink()
{
    ShellExecute(0, 0, "https://github.com/CosmoCortney/MungPlex/wiki", 0, 0, SW_SHOW);
}
