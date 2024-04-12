#include "Connection.h"
#include"Settings.h"
#include"Log.h"
#include <chrono>
#include <thread>
#include <future>

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
					_connectionMessage = "Connected to emulator: " + MorphText::Utf16LE_To_Utf8(ProcessInformation::GetEmulatorList()[_selectedEmulatorIndex].first);
					_richPresenceDetails = std::string("Messing with "
						+ ProcessInformation::GetTitle()
						+ " (" + ProcessInformation::GetPlatform() + ", "
						+ ProcessInformation::GetRpcGameID() + ", "
						+ ProcessInformation::GetRegion() + ") "
						+ "running on " + ProcessInformation::GetProcessName());

					if (_core != nullptr)
						_core->~Core();

					if (Settings::GetGeneralSettings().EnableRichPresence)
						InitRichPresence();

					startConnectionCheck();
				}
			}

			if (_selectedEmulatorIndex == ProcessInformation::MESEN || _selectedEmulatorIndex == ProcessInformation::RPCS3 || _selectedEmulatorIndex == ProcessInformation::YUZU)
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
					_richPresenceDetails = "Messing with " + ProcessInformation::GetProcessName();

					if (_core != nullptr)
						_core->~Core();

					if (Settings::GetGeneralSettings().EnableRichPresence)
						InitRichPresence();
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

		/*if (ImGui::BeginTabItem("Remote Device"))
		{
			ImGui::Text("Select Console.");
			ImGui::Button("Connect", ImVec2(200, 50));
			ImGui::EndTabItem();
		}*/
		ImGui::EndTabBar();

		if (Settings::GetGeneralSettings().EnableRichPresence)
		{
			if (_connected && _core != nullptr)
			{
				if (_core->RunCallbacks() != discord::Result::Ok)
					_core->~Core();
			}
			else if (_core != nullptr)
			{
				_core->~Core();
			}
		}
	}

	ImGui::Dummy(ImVec2(0.0f, 5.0f));
	memoryViewerButton();
	ImGui::Dummy(ImVec2(0.0f, 5.0f));
	ImGui::Text(_connectionMessage.c_str());

	if (ProcessInformation::GetProcessType() == ProcessInformation::EMULATOR)
	{
		SetUpLableText("Platform:", ProcessInformation::GetPlatform().c_str(), ProcessInformation::GetPlatform().size(), 1.0f, 0.333f);
		SetUpLableText("Game Title:", ProcessInformation::GetTitle().c_str(), ProcessInformation::GetTitle().size(), 1.0f, 0.333f);
		SetUpLableText("Game ID:", ProcessInformation::GetRpcGameID().c_str(), ProcessInformation::GetRpcGameID().size(), 1.0f, 0.333f);
		SetUpLableText("Region:", ProcessInformation::GetRegion().c_str(), ProcessInformation::GetRegion().size(), 1.0f, 0.333f);
	}
}

void MungPlex::Connection::memoryViewerButton()
{
	if (!_connected || _memoryViewers.size() >= 16) ImGui::BeginDisabled();
	{
		ImGui::Dummy(ImVec2(0.0f, 5.0f));
		if (ImGui::Button("Open Memory Viewer"))
			_memoryViewers.emplace_back(++_memViewerCount);
	}

	if (!_connected || _memoryViewers.size() >= 16) ImGui::EndDisabled();

	for (int i = 0; i < _memoryViewers.size(); ++i)
	{
		if (!_memoryViewers[i].IsOpen())
			_memoryViewers.erase(_memoryViewers.begin() + i);
	}
}

bool MungPlex::Connection::IsConnected()
{
	return GetInstance()._connected;
}

void MungPlex::Connection::SetRichPresenceState(const std::string& action)
{
	if(GetInstance()._core == nullptr)
	    return;

	GetInstance()._activity.SetState(action.c_str());
	GetInstance()._core->ActivityManager().UpdateActivity(GetInstance()._activity, getDiscordActivityResult);
}

void MungPlex::Connection::InitRichPresence()
{
	discord::Core::Create(1175421760892567552, DiscordCreateFlags_Default, &GetInstance()._core);
	GetInstance()._core->SetLogHook(discord::LogLevel::Debug, logDiscordProblem);
	GetInstance()._activity.SetApplicationId(1175421760892567552);
	//_activity.SetName("Test Name");
	GetInstance()._activity.SetDetails(GetInstance()._richPresenceDetails.c_str());
	GetInstance()._activity.SetType(discord::ActivityType::Playing);
	GetInstance()._activity.SetInstance(true);
	GetInstance()._activity.GetTimestamps().SetStart(time(NULL));
	GetInstance()._activity.GetAssets().SetLargeImage("icon1024");
	GetInstance()._core->ActivityManager().UpdateActivity(GetInstance()._activity, getDiscordActivityResult);
}

void MungPlex::Connection::StopRichPresence()
{
	if(GetInstance()._core != nullptr)
		GetInstance()._core->~Core();
}

std::vector<MungPlex::MemoryViewer>& MungPlex::Connection::GetMemoryViews()
{
	return GetInstance()._memoryViewers;
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
}

void MungPlex::Connection::getDiscordActivityResult(discord::Result result)
{
	if (result != discord::Result::Ok)
	{
		std::string err = "Error handling Discord Rich PResence (err-code: " + std::to_string((int)result) + ")";
		std::cout << err << std::endl;
		Log::LogInformation(err);
	}
}

void MungPlex::Connection::logDiscordProblem(const discord::LogLevel level, const std::string message)
{
	std::cout << "discord error level: " << (int)level << " - " << message << std::endl;
}