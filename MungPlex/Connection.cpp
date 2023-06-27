#include "Connection.h"

void MungPlex::Connection::DrawWindow()
{
	ImGui::Begin("Connection");
	GetInstance().DrawConnectionSelect();
	ImGui::End();
}

void MungPlex::Connection::DrawConnectionSelect()
{
	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
	{
		std::string emuSelect;
		if (ImGui::BeginTabItem("Emulator"))
		{
			MungPlex::SetUpCombo("Select Emulator", ProcessInformation::GetEmulatorList(), _selectedEmulatorIndex);

			if (ImGui::Button("Connect", ImVec2(200, 50)))
			{
				_connected = ProcessInformation::ConnectToEmulator(_selectedEmulatorIndex);
			}
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Native Application"))
		{
			ImGui::Button("Connect", ImVec2(200, 50));
			ImGui::EndTabItem();
		}
		/*if (ImGui::BeginTabItem("Remote Device"))
		{
			ImGui::Text("Select Console.");
			ImGui::Button("Connect", ImVec2(200, 50));
			ImGui::EndTabItem();
		}*/
		ImGui::EndTabBar();
	}
}

bool MungPlex::Connection::IsConnected()
{
	return GetInstance()._connected;
}