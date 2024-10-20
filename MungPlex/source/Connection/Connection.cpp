#include <chrono>
#include "Connection.hpp"
#include "examples/libs/emscripten/emscripten_mainloop_stub.h"
#include <fstream>
#include <future>
#include "imgui.h"
#include "imgui_internal.h"
#include <iomanip>
#include "../../Log.hpp"
#include "MungPlexConfig.hpp"
#include "ProcessInformation.hpp"
#include "Settings.hpp"
#include <thread>

void MungPlex::Connection::DrawWindow()
{
	if (ImGui::Begin("Connection"))
	{
		GetInstance().drawConnectionSelect();
	}
	ImGui::End();
}

void MungPlex::Connection::drawConnectionSelect()
{
	static ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;

	if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
	{
		static std::string emuSelect;

		if (ImGui::BeginTabItem("Emulator"))
		{
			ImGui::Dummy(ImVec2(0.0f, 5.0f));
			SetUpCombo("Emulator:", ProcessInformation::GetEmulatorList(), _selectedEmulatorIndex, 1.0f, 0.35f, true, "Emulators are always in development and therefore crucial things may change that will prevent MungPlex from finding the needed memory regions and game ID. If this is the case report it at the MungPlex discord server so it can be fixed :)");

			ImGui::Dummy(ImVec2(0.0f, 5.0f));

			if (ImGui::Button("Connect"))
			{
				_connected = ProcessInformation::ConnectToEmulator(_selectedEmulatorIndex);

				if (_connected)
				{
					_connectionMessage = "Connected to emulator: " + MT::Convert<std::wstring, std::string>(ProcessInformation::GetEmulatorList()[_selectedEmulatorIndex].first, MT::UTF16LE, MT::UTF8);
					std::string details = std::string("Messing with "
						+ ProcessInformation::GetTitle()
						+ " (" + ProcessInformation::GetPlatform() + ", "
						+ ProcessInformation::GetRpcGameID() + ", "
						+ ProcessInformation::GetRegion() + ") "
						+ "running on " + ProcessInformation::GetProcessName());
					_discord.SetRichPresenceDetails(details);



					if (Settings::GetGeneralSettings().EnableRichPresence)
						_discord.InitRichPresence();

					startConnectionCheck();
				}
			}

			if (_selectedEmulatorIndex == ProcessInformation::MESEN || _selectedEmulatorIndex == ProcessInformation::LIME3DS || _selectedEmulatorIndex == ProcessInformation::RPCS3 || _selectedEmulatorIndex == ProcessInformation::YUZU)
			{
				ImGui::SameLine();
				ImGui::Text("Important:");
				ImGui::SameLine();

				switch (_selectedEmulatorIndex)
				{
				case ProcessInformation::MESEN:
					HelpMarker("SNES support only. In order to connect to Mesen disable rewind by going to \"Settings/Preferences/Advanced/\" and uncheck \"Allow rewind to use up to...\". Also apply the lua script \"MungPlex/resources/setMesenMungPlexFlag.lua\"");
					break;
				case ProcessInformation::RPCS3:
					HelpMarker("Rpcs3 has unique memory mapping for each game(?) so you may need to figure out how much of each range is mapped.");
					break;
				case ProcessInformation::YUZU:
					HelpMarker("Experimental, base adresses are not yet figured out.");
					break;
				case ProcessInformation::LIME3DS:
					HelpMarker("A special version of Lime3DS is required. You can download it from https://github.com/CosmoCortney/Lime3DS-for-MungPlex");
					break; 
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

				if (_connected)
				{
					_connectionMessage = "Connected to Process: " + ProcessInformation::GetProcessName();
					std::string details = "Messing with " + ProcessInformation::GetProcessName();
					_discord.SetRichPresenceDetails(details);

					if (Settings::GetGeneralSettings().EnableRichPresence)
						_discord.InitRichPresence();
				}

				startConnectionCheck();
			}

			ImGui::SameLine();

			if (ImGui::Button("Refresh List"))
			{
				Xertz::SystemInfo::RefreshProcessInfoList();
				Xertz::SystemInfo::RefreshApplicationProcessInfoList();
			}

			ImGui::EndTabItem();
		}
		/*
		if (ImGui::BeginTabItem("Remote Device"))
		{
			ImGui::Text("Select Console.");

			if (ImGui::Button("Connect", ImVec2(200, 50)))
			{
				_usbGecko.Init();
			}


			ImGui::EndTabItem();
		}*/
		ImGui::EndTabBar();

		if (Settings::GetGeneralSettings().EnableRichPresence)
			_discord.CheckGameState(_connected);
	}

	ImGui::Text(_connectionMessage.c_str());

	if (ProcessInformation::GetProcessType() == ProcessInformation::EMULATOR)
	{
		SetUpLableText("Platform:", ProcessInformation::GetPlatform().c_str(), ProcessInformation::GetPlatform().size(), 1.0f, 0.333f);
		SetUpLableText("Game Title:", ProcessInformation::GetTitle().c_str(), ProcessInformation::GetTitle().size(), 1.0f, 0.333f);
		SetUpLableText("Game ID:", ProcessInformation::GetRpcGameID().c_str(), ProcessInformation::GetRpcGameID().size(), 1.0f, 0.333f);
		SetUpLableText("Region:", ProcessInformation::GetRegion().c_str(), ProcessInformation::GetRegion().size(), 1.0f, 0.333f);
	}
}

bool MungPlex::Connection::IsConnected()
{
	return GetInstance()._connected;
}

MungPlex::DiscordRPC MungPlex::Connection::GetDiscordRichPresence()
{
	return GetInstance()._discord;
}

void MungPlex::Connection::startConnectionCheck()
{
	std::thread waitNcheck(&MungPlex::Connection::checkConnection, this);
	waitNcheck.detach();
}

void MungPlex::Connection::checkConnection()
{
	static const auto millisecondsToWait = std::chrono::seconds(1);

	while (_connected)
	{
		std::this_thread::sleep_for(millisecondsToWait);
		_connected = ProcessInformation::IsConnectionValid();
	}

	ProcessInformation::ResetWindowTitle();
}