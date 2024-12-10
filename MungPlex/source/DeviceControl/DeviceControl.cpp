#include "DeviceControl.hpp"
#include <algorithm>
#include "Helperfunctions.hpp"
#include "ProcessInformation.hpp"
#include "Settings.hpp"

//std::vector<std::pair<int, std::variant</*DeviceXInput, */MungPlex::DeviceLovense>>> MungPlex::DeviceControl::_devices;

MungPlex::DeviceControl::DeviceControl()
{
	_devices.reserve(50);
}

void MungPlex::DeviceControl::DrawWindow()
{
	static bool stateSet = false;

	if (ImGui::Begin("Device Control"))
	{
		if (!Connection::IsConnected())
			ImGui::BeginDisabled();
		else
		{
			if (!stateSet && Settings::GetGeneralSettings().EnableRichPresence && ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows))
			{
				Connection::GetDiscordRichPresence().SetRichPresenceState("Device Control");
				stateSet = true;
			}
		}

		GetInstance().drawList();

		if (!Connection::IsConnected())
			ImGui::EndDisabled();
	}
	else
		stateSet = false;

	if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows))
		stateSet = false;

	ImGui::End();
}

void MungPlex::DeviceControl::InitDevicesFile()
{
	GetInstance()._currentFile = MT::Convert<char*, std::wstring>(Settings::GetGeneralSettings().DocumentsPath, MT::UTF8, MT::UTF16LE) + L"\\MungPlex\\DeviceControl\\"
		+ MT::Convert<std::string, std::wstring>(ProcessInformation::GetPlatform(), MT::UTF8, MT::UTF16LE) + L"\\"
		+ MT::Convert<std::string, std::wstring>(ProcessInformation::GetGameID(), MT::UTF8, MT::UTF16LE) + L".json";

	if (!std::filesystem::exists(GetInstance()._currentFile))
	{
		std::ofstream file(GetInstance()._currentFile, std::ios::binary);

		if (file.is_open())
		{
			file << "\xEF\xBB\xBF"; //write BOM
			file << GetInstance()._placeholderFile;
		}
	}

	GetInstance()._devices.clear();
	GetInstance()._deviceIds.clear();

	std::string jsonstr;
	{
		std::ifstream inFile;
		inFile.open(GetInstance()._currentFile);

		if (inFile)
		{
			std::string buffer;
			while (std::getline(inFile, buffer))
			{
				jsonstr.append(buffer).append("\n");
			}
		}
	}

	try
	{
		auto doc = nlohmann::json::parse(jsonstr);
		auto& deviceList = doc["DeviceList"];

		for (int i = 0; i < deviceList.size(); ++i)
		{
			int type = deviceList[i]["deviceType"];
			GetInstance()._deviceIds.push_back(i);

			switch (type)
			{
			case IDevice::LOVENSE:
				GetInstance()._devices.emplace_back(IDevice::LOVENSE, DeviceLovense(i, deviceList[i]));
				break;
			//default: //XInput
				
			}
		}
	}
	catch (const nlohmann::json::parse_error& exception)
	{
		std::cerr << "Failed parsing Device Element: " << exception.what() << std::endl;
	}
}

void MungPlex::DeviceControl::drawList()
{
	ImGui::BeginChild("Device List", ImGui::GetContentRegionAvail(), true);
	{
		static int typeSelect = 0;
		static uint64_t devicesCount = 0;
		devicesCount = _devices.size();

		SetUpPairCombo(IDevice::s_DeviceTypes, &typeSelect, 0.3f);

		ImGui::SameLine();

		if (devicesCount >= 50) ImGui::BeginDisabled();

		if (ImGui::Button("Add Device"))
		{
			_deviceIds.push_back(devicesCount);

			for (int i = 0; i < _deviceIds.size() - 1; ++i)
			{
				if (_deviceIds.back() == _deviceIds[i])
				{
					++_deviceIds.back();
					i = 0;
				}
			}

			switch (IDevice::s_DeviceTypes.GetId(typeSelect))
			{
			case IDevice::LOVENSE:
				_devices.emplace_back(IDevice::LOVENSE, DeviceLovense(devicesCount));
				break;
			//default: //XInput
				//_devices.emplace_back(XINPUT, DeviceXInput(_deviceIds.back()));
			}
		} if (devicesCount >= 50) ImGui::EndDisabled();

		ImGui::SameLine();

		if (ImGui::Button("Save List"))
			GetInstance().saveList();

		for (int i = 0; i < _devices.size(); ++i)
		{
			switch (_devices[i].first)
			{
			case IDevice::LOVENSE:
				std::get<DeviceLovense>(_devices[i].second).Draw();
				break;
			//default: //XInput
				//std::get<DeviceXInput>(_devices[i].second).Draw();
			}
		}
	}
	ImGui::EndChild();
}

bool MungPlex::DeviceControl::saveList()
{
	std::ofstream file(_currentFile, std::ios::binary);
	const bool isOpen = file.is_open();

	if (isOpen)
	{
		nlohmann::json jsonData;

		if (!_devices.empty())
		{
			for (int i = 0; i < _devices.size(); ++i)
			{
				nlohmann::json watchJson;

				switch (_devices[i].first)
				{
				case IDevice::LOVENSE:
					jsonData["DeviceList"].emplace_back(std::get<DeviceLovense>(_devices[i].second).GetJSON());
					break;
				//default:
				}
			}
		}
		else
		{
			jsonData["DeviceList"].emplace_back("");
		}

		file << "\xEF\xBB\xBF"; //write BOM
		file << jsonData.dump(2);
	}

	return isOpen;
}

void MungPlex::DeviceControl::DeleteItem(const int id)
{
	auto& devices = GetInstance()._devices;

	for (int i = 0; i < devices.size(); ++i)
	{
		switch (devices[i].first)
		{
		case IDevice::LOVENSE:
			if (std::get<DeviceLovense>(devices[i].second).GetID() == id)
			{
				devices.erase(devices.begin() + i);
				return;
			}
			break;
		/*default: //XInput
			if (std::get<IntegralView>(views[i].second).GetID() == id)
			{
				views.erase(views.begin() + i);
				return;
			}*/
		}
	}
}