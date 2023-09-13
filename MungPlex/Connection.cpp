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
			SetUpCombo("Emulator:", ProcessInformation::GetEmulatorList(), _selectedEmulatorIndex, 0.75f, 0.4f);

			ImGui::Dummy(ImVec2(0.0f, 5.0f));

			if (ImGui::Button("Connect"))
			{
				_connected = ProcessInformation::ConnectToEmulator(_selectedEmulatorIndex);

				if (_connected)
				{
					strcpy_s(_connectionMessage, "Connected to emulator: ");
					strcat_s(_connectionMessage, MorphText::Utf16LE_To_Utf8(ProcessInformation::GetEmulatorList()[_selectedEmulatorIndex].first).c_str());
				}
			}
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Native Application"))
		{
			ImGui::Dummy(ImVec2(0.0f, 5.0f));
			static bool app = true;

			if (ImGui::BeginTabBar("ProcessesTab", tab_bar_flags))
			{
				if (Xertz::SystemInfo::GetProcessInfoList().size() == 0)
					Xertz::SystemInfo::RefreshProcessInfoList();

				if (Xertz::SystemInfo::GetApplicationProcessInfoList().size() == 0)
					Xertz::SystemInfo::RefreshApplicationProcessInfoList();

				if (ImGui::BeginTabItem("Applications"))
				{
					app = true;
					SetUpCombo("Application:", Xertz::SystemInfo::GetApplicationProcessInfoList(), _selectedApplicationProcessIndex, 0.75f, 0.4f);
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Processes"))
				{
					app = false;
					SetUpCombo("Process:", Xertz::SystemInfo::GetProcessInfoList(), _selectedProcessIndex, 0.75f, 0.4f);
					ImGui::EndTabItem();
				}

				ImGui::EndTabBar();
			}

			if (ImGui::Button("Connect"))
			{
				if (app)
					_connected = MungPlex::ProcessInformation::ConnectToApplicationProcess(_selectedApplicationProcessIndex);
				else
					_connected = MungPlex::ProcessInformation::ConnectToProcess(_selectedProcessIndex);
			}

			ImGui::SameLine();

			if (ImGui::Button("Refresh List"))
			{
				Xertz::SystemInfo::RefreshProcessInfoList();
				Xertz::SystemInfo::RefreshApplicationProcessInfoList();
			}

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
	drawAdditionalFeatureSelect();
}

void MungPlex::Connection::drawAdditionalFeatureSelect()
{
	if (!_connected || _memoryViewers.size() >= 16) ImGui::BeginDisabled();
	{
		if (ImGui::Button("Open Memory Viewer"))
			_memoryViewers.emplace_back(++_memViewerCount);
	}

	if (!_connected || _memoryViewers.size() >= 16) ImGui::EndDisabled();

	for (int i = 0; i < _memoryViewers.size(); ++i)
	{
		if (!_memoryViewers[i].IsOpen())
			_memoryViewers.erase(_memoryViewers.begin() + i);
	}

	for (int i = 0; i < _memoryViewers.size(); ++i)
	{
		_memoryViewers[i].DrawWindow();
	}
}

bool MungPlex::Connection::IsConnected()
{
	return GetInstance()._connected;
}