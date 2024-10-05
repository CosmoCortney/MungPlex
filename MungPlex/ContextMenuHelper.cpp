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
        /*
        
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Placeholder", "Something")) {  }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Placeholder", "")) {  }
            ImGui::EndMenu();
        }*/

        GetInstance().drawViewMenuItems();
        GetInstance().drawHelpMenuItems();

        ImGui::EndMainMenuBar();
    }
	ImGui::End();
}

void MungPlex::ContextMenuHelper::openWebPage(const std::string& url)
{
    ShellExecute(0, 0, url.c_str(), 0, 0, SW_SHOW);
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

void MungPlex::ContextMenuHelper::drawHelpMenuItems()
{
    if (ImGui::BeginMenu("Help"))
    {
        if (ImGui::MenuItem("Wiki"))
            GetInstance().openWebPage("https://github.com/CosmoCortney/MungPlex/wiki");

        if (ImGui::MenuItem("GitHub"))
            GetInstance().openWebPage("https://github.com/CosmoCortney/MungPlex");

        if (ImGui::MenuItem("Discord Server"))
            GetInstance().openWebPage("https://discord.gg/wU62ZTvQRj");

        if (ImGui::MenuItem("Cheat Code Database"))
            GetInstance().openWebPage("https://lawnmeower.de");

        if (ImGui::MenuItem("Get your Lovense token"))
            GetInstance().openWebPage("https://en.lovense.com/user/developer/info");

        ImGui::EndMenu();
    }
}