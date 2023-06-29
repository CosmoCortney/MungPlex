#include "Connection.h"

void MungPlex::Connection::DrawWindow()
{
	ImGui::Begin("Connection");
	GetInstance().DrawConnectionSelect();
	ImGui::End();
}

void MungPlex::Connection::DrawConnectionSelect()
{
	static ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
	{
		std::string emuSelect;
		if (ImGui::BeginTabItem("Emulator"))
		{

			ImGui::Dummy(ImVec2(0.0f, 5.0f));
			ImGui::Text("Emulator:");
			ImGui::SameLine();
			MungPlex::SetUpCombo("##EmulatorSelect", ProcessInformation::GetEmulatorList(), _selectedEmulatorIndex);

			ImGui::Dummy(ImVec2(0.0f, 5.0f));

			if (ImGui::Button("Connect", ImVec2(200, 50)))
			{
				_connected = ProcessInformation::ConnectToEmulator(_selectedEmulatorIndex);

				if (_connected)
				{
					strcpy(_connectionMessage, "Connected to emulator: ");
					strcat(_connectionMessage, MorphText::Utf16LE_To_Utf8(ProcessInformation::GetEmulatorList()[_selectedEmulatorIndex].first).c_str());
				}
			}
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Native Application"))
		{
			ImGui::Dummy(ImVec2(0.0f, 5.0f));
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

	ImGui::Dummy(ImVec2(0.0f, 5.0f));
	ImGui::Text(_connectionMessage);
}

bool MungPlex::Connection::IsConnected()
{
	return GetInstance()._connected;
}