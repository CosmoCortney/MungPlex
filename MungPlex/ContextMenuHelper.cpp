#include <boost/asio.hpp>
#include "GLFW/glfw3.h"
#include "Connection.hpp"
#include "ContextMenuHelper.hpp"
#include "imgui.h"
#include <boost/winapi/shell.hpp>
#include <shellapi.h>

void MungPlex::ContextMenuHelper::DrawWindow()
{
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

        GetInstance().drawViewMenuItems();

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

void MungPlex::ContextMenuHelper::openMemoryVierwer()
{
    if (!Connection::IsConnected())
        return;

    if (_memoryViewers.size() >= 16)
        return;

    _memoryViewers.emplace_back(++_memViewerCount);
    UpdateMemoryViewerList();
}

std::vector<MungPlex::MemoryViewer>& MungPlex::ContextMenuHelper::GetMemoryViews()
{
    return GetInstance()._memoryViewers;
}

void MungPlex::ContextMenuHelper::UpdateMemoryViewerList()
{
    for (int i = 0; i < GetInstance()._memoryViewers.size(); ++i)
    {
        if (!GetInstance()._memoryViewers[i].IsOpen())
            GetInstance()._memoryViewers.erase(GetInstance()._memoryViewers.begin() + i);
    }
}

void MungPlex::ContextMenuHelper::drawViewMenuItems()
{
    static bool disableMemViewOtion = false;

    if (ImGui::BeginMenu("View"))
    {
        disableMemViewOtion = !Connection::IsConnected() || _memoryViewers.size() >= 16;
        
        if(disableMemViewOtion) ImGui::BeginDisabled();
        {
            if (ImGui::MenuItem("Memory Viewer"))
                GetInstance().openMemoryVierwer();
        }
        if (disableMemViewOtion) ImGui::EndDisabled();

        ImGui::EndMenu();
    }
}