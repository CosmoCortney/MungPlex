#include "DeviceControl.hpp"
#include <algorithm>
#include "../../Helperfunctions.hpp"
#include "../../ProcessInformation.hpp"
#include "../../Settings.hpp"

void MungPlex::DeviceControl::DrawWindow()
{
	static bool stateSet = false;

	if (ImGui::Begin("Device Control"))
	{
	//	if (!Connection::IsConnected())
	//		ImGui::BeginDisabled();
	//	else
		{
			if (!stateSet && Settings::GetGeneralSettings().EnableRichPresence && ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows))
			{
				Connection::GetDiscordRichPresence().SetRichPresenceState("Device Control");
				stateSet = true;
			}
		}

		GetInstance().drawList();

	//	if (!Connection::IsConnected())
	//		ImGui::EndDisabled();
	}
	else
		stateSet = false;

	if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows))
		stateSet = false;

	ImGui::End();
}

void MungPlex::DeviceControl::drawList()
{
	ImGui::BeginChild("Device List", ImGui::GetContentRegionAvail());
	{
		static int typeSelect = 0;

		SetUpCombo("New Device Type:", IDevice::s_DeviceTypes, typeSelect, 0.3f);

		ImGui::SameLine();

		if (ImGui::Button("Add Device"))
		{
			_deviceIds.push_back(_devices.size());

			for (int i = 0; i < _deviceIds.size() - 1; ++i)
			{
				if (_deviceIds.back() == _deviceIds[i])
				{
					++_deviceIds.back();
					i = 0;
				}
			}

			switch (IDevice::s_DeviceTypes[typeSelect].second)
			{
			case IDevice::LOVENSE:
				_devices.emplace_back(IDevice::LOVENSE, DeviceLovense());
				break;
			//default: //XInput
				//_devices.emplace_back(XINPUT, DeviceXInput(_deviceIds.back()));
			}
		}

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
	return false;
}

void MungPlex::DeviceControl::DeleteItem(const int id)
{
	/*auto& views = GetInstance()._views;

	for (int i = 0; i < views.size(); ++i)
	{
		switch (views[i].first)
		{
		case IDevice::LOVENSE::
			if (std::get<FloatView>(views[i].second).GetID() == id)
			{
				views.erase(views.begin() + i);
				return;
			}
			break;
		default: //XInput
			if (std::get<IntegralView>(views[i].second).GetID() == id)
			{
				views.erase(views.begin() + i);
				return;
			}
		}
	}*/
}