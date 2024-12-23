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
#include "search.hpp"
#include "Settings.hpp"
#include <thread>

void MungPlex::Connection::DrawWindow()
{
	if (ImGui::Begin("Connection"))
	{
		GetInstance().drawConnectionSelect();
		GetInstance().drawProcessControl();
	}
	ImGui::End();
}

void MungPlex::Connection::drawConnectionSelect()
{
	static ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;

	if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
	{
		drawEmulatorsTabItem();
		drawAppTabItem();
		drawConsoleTabItem();
		ImGui::EndTabBar();
	}

	static bool hex = false;

	ImGui::Checkbox("bleh", &hex);

	static InputInt<int8_t> test = InputInt<int8_t>("int8:", -1, true, 0, 0);
	test.Draw(1.0f, 0.4f, hex);
	static InputInt<int16_t> test2 = InputInt<int16_t>("int16:", -1, true, 0, 0);
	test2.Draw(1.0f, 0.4f, hex);
	static InputInt<int32_t> test3 = InputInt<int32_t>("int32:", -1, true, 0, 0);
	test3.Draw(1.0f, 0.4f, hex);
	static InputInt<int64_t> test4 = InputInt<int64_t>("int64:", -1, true, 0, 0);
	test4.Draw(1.0f, 0.4f, hex);

	static InputInt<uint8_t> test5 = InputInt<uint8_t>("uint8:", -1, true, 0, 0);
	test5.Draw(1.0f, 0.4f, hex);
	static InputInt<uint16_t> test6 = InputInt<uint16_t>("uint16:", -1, true, 0, 0);
	test6.Draw(1.0f, 0.4f, hex);
	static InputInt<uint32_t> test7 = InputInt<uint32_t>("uint32:", -1, true, 0, 0);
	test7.Draw(1.0f, 0.4f, hex);
	static InputInt<uint64_t> test8 = InputInt<uint64_t>("uint64:", -1, true, 0, 0);
	test8.Draw(1.0f, 0.4f, hex);

	if (Settings::GetGeneralSettings().EnableRichPresence)
		_discord.CheckGameState(_connected);

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

void MungPlex::Connection::SetConnectedStatus(const bool isConnected)
{
	GetInstance()._connected = isConnected;
}

MungPlex::DiscordRPC MungPlex::Connection::GetDiscordRichPresence()
{
	return GetInstance()._discord;
}

void MungPlex::Connection::startConnectionCheck()
{
	_checkConnectionThreadFlag = false;

	if (_checkConnectionThread.joinable())
		_checkConnectionThread.join();

	_checkConnectionThreadFlag = true;
	_checkConnectionThread = boost::thread(&MungPlex::Connection::checkConnection, this);
}

void MungPlex::Connection::checkConnection()
{
	static const auto millisecondsToWait = std::chrono::milliseconds(1000);

	while (_checkConnectionThreadFlag)
	{
		std::this_thread::sleep_for(millisecondsToWait);
		_connected = ProcessInformation::IsConnectionValid();
	}

	_connected = false;
	ProcessInformation::ResetWindowTitle();
}

void MungPlex::Connection::drawEmulatorsTabItem()
{
	static std::string emuSelect;
	static StringIdPairs emulators = ProcessInformation::GetEmulatorList();

	if (ImGui::BeginTabItem("Emulator"))
	{
		ImGui::Dummy(ImVec2(0.0f, 5.0f));

		SetUpPairCombo(emulators, &_selectedEmulatorIndex, 1.0f, 0.5f, true, "Emulators are always in development and therefore crucial things may change that will prevent MungPlex from finding the needed memory regions and game ID. If this is the case report it at the MungPlex discord server so it can be fixed :)");
		
		ImGui::Dummy(ImVec2(0.0f, 5.0f));

		bool disable = _checkConnectionThreadFlag;
		if (disable) ImGui::BeginDisabled();

		if (ImGui::Button("Connect"))
		{
			_connected = ProcessInformation::ConnectToEmulator(_selectedEmulatorIndex);

			if (_connected)
			{
				_connectionMessage = "Connected to emulator: " + ProcessInformation::GetEmulatorList().GetStdString(_selectedEmulatorIndex);
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
		} if (disable) ImGui::EndDisabled();

		ImGui::SameLine();

		drawDisconnectButton();

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
}

void MungPlex::Connection::drawAppTabItem()
{
	if (ImGui::BeginTabItem("Native App"))
	{
		ImGui::Dummy(ImVec2(0.0f, 5.0f));
		static bool app = true;
		static ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
		static ProcessInforPairs processInfoList("Process:");
		static ProcessApplicationInforPairs applicationProcessInfoList("Application:");

		if (ImGui::BeginTabBar("ProcessesTab", tab_bar_flags))
		{
			if (processInfoList.GetCount() == 0)
				processInfoList.RefreshProcessInfo();

			if (applicationProcessInfoList.GetCount() == 0)
				applicationProcessInfoList.RefreshApplicationProcessInfo();

			if (ImGui::BeginTabItem("Applications"))
			{
				ImGui::Dummy(ImVec2(0.0f, 5.0f));
				app = true;
				SetUpPairCombo(applicationProcessInfoList, &_selectedApplicationProcessIndex, 1.0f, 0.5f);
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Processes"))
			{
				ImGui::Dummy(ImVec2(0.0f, 5.0f));
				app = false;
				SetUpPairCombo(processInfoList, &_selectedProcessIndex, 1.0f, 0.5f);
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}

		bool disable = _checkConnectionThreadFlag;
		if (disable) ImGui::BeginDisabled();

		ImGui::Dummy(ImVec2(0.0f, 5.0f));

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
		} if (disable) ImGui::EndDisabled();
		
		ImGui::SameLine();
		
		drawDisconnectButton();

		ImGui::SameLine();

		if (ImGui::Button("Refresh List"))
		{
			Xertz::SystemInfo::RefreshProcessInfoList();
			Xertz::SystemInfo::RefreshApplicationProcessInfoList();
		}

		ImGui::EndTabItem();
	}
}

void MungPlex::Connection::drawConsoleTabItem()
{
	if (ImGui::BeginTabItem("Real Console"))
	{
		static int sel = 0;
		static StringIdPairs connectionTypes = ProcessInformation::GetConsoleConnectionTypeList();
		ImGui::Dummy(ImVec2(0.0f, 5.0f));
		SetUpPairCombo(connectionTypes, &sel, 1.0f, 0.5f);

		bool disable = _checkConnectionThreadFlag;
		if (disable) ImGui::BeginDisabled();

		if (ImGui::Button("Connect"))
		{
			_connected = ProcessInformation::ConnectToRealConsole(connectionTypes.GetId(sel));
		
			if (_connected)
			{
				_connectionMessage = "Connected to console: " + ProcessInformation::GetPlatform() + " via USB Gecko.";
				std::string details = std::string("Messing with "
					+ ProcessInformation::GetTitle()
					+ " (" + ProcessInformation::GetRpcGameID() + ", "
					+ ProcessInformation::GetRegion() + ") "
					+ " on " + ProcessInformation::GetPlatform() + " via USB Gecko");
				_discord.SetRichPresenceDetails(details);

				if (Settings::GetGeneralSettings().EnableRichPresence)
					_discord.InitRichPresence();

				startConnectionCheck();
			}
		} if (disable) ImGui::EndDisabled();

		ImGui::SameLine();
		drawDisconnectButton();
		ImGui::EndTabItem();
	}
}

void MungPlex::Connection::drawDisconnectButton()
{
	static bool disable = true;
	disable = !_checkConnectionThreadFlag;

	if (disable) ImGui::BeginDisabled();

	if (ImGui::Button("Disconnect"))
	{
		_connected = false;
		_checkConnectionThreadFlag = false;

		if (_checkConnectionThread.joinable())
			_checkConnectionThread.detach();
	}
	if (disable) ImGui::EndDisabled();
}

void MungPlex::Connection::drawProcessControl()
{
	if (ProcessInformation::GetProcessType() != ProcessInformation::CONSOLE)
		return;

	if (ProcessInformation::GetConsoleConnectionType() != ProcessInformation::CON_USBGecko)
		return;

	ImGui::Dummy(ImVec2(0.0f, 5.0f));

	static bool busySearching = false;
	busySearching = Search::IsBusySearching();

	if (!Connection::IsConnected() || busySearching)
		ImGui::BeginDisabled();

	static USBGecko* gecko = nullptr;
	gecko = ProcessInformation::GetUsbGecko();

	if (ImGui::Button("Pause"))
	{
		gecko->SendCommand(USBGecko::cmd_pause);
	}

	ImGui::SameLine();

	if (ImGui::Button("Resume"))
	{
		gecko->SendCommand(USBGecko::cmd_unfreeze);
	}

	if (!Connection::IsConnected() || busySearching)
		ImGui::EndDisabled();
}