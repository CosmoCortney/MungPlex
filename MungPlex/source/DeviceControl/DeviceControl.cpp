#include "DeviceControl.hpp"
#include <algorithm>
#include "../../Helperfunctions.hpp"
#include "../../ProcessInformation.hpp"
#include "../../Settings.hpp"


const std::vector<std::pair<std::string, int>> MungPlex::DeviceControl::Device::s_DeviceTypes =
{
	//{ "XInput", XINPUT},
	{ "Lovense", LOVENSE}
};


const std::vector<std::pair<std::string, int>> MungPlex::DeviceControl::Device::s_ValueTypes =
{
	{ "Int 8 (1 Byte)", INT8 },
	{ "Int 16 (2 Bytes)", INT16 },
	{ "Int 32 (4 Bytes)", INT32 },
	{ "Int 64 (8 Bytes)", INT64 },
	{ "Float Single", FLOAT },
	{ "Float Double", DOUBLE },
	{ "Bool", BOOL }
};
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

		SetUpCombo("New Device Type:", Device::s_DeviceTypes, typeSelect);

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

			switch (Device::s_DeviceTypes[typeSelect].second)
			{
			case LOVENSE:
				_devices.emplace_back(LOVENSE, DeviceLovense());
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
			case LOVENSE:
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
		case FLOAT:
			if (std::get<FloatView>(views[i].second).GetID() == id)
			{
				views.erase(views.begin() + i);
				return;
			}
			break;
		case BOOL:
			if (std::get<BoolView>(views[i].second).GetID() == id)
			{
				views.erase(views.begin() + i);
				return;
			}
			break;
		case MOUSEPIANO:
			if (std::get<MousePiano>(views[i].second).GetID() == id)
			{
				views.erase(views.begin() + i);
				return;
			}
			break;
		default: //INTEGRAL
			if (std::get<IntegralView>(views[i].second).GetID() == id)
			{
				views.erase(views.begin() + i);
				return;
			}
		}
	}*/
}

MungPlex::DeviceControl::DeviceLovense::DeviceLovense()
{
	_token = Settings::GetDeviceControlSettings().LovenseToken;
}

void MungPlex::DeviceControl::DeviceLovense::Draw()
{
	float itemWidth = ImGui::GetContentRegionAvail().x;
	float itemHeight = 256.0f * Settings::GetGeneralSettings().Scale;

	ImGui::SeparatorText("Lovence Toy");

	ImGui::BeginChild(std::string("child_lovenseDevice" + _idText).c_str(), ImVec2(itemWidth, itemHeight));
	{
		static int error = 0;

		ImGui::BeginChild("child_toyInfo", ImVec2(itemWidth * 0.3f, itemHeight * 1.5f), true);
		{
			if (ImGui::Button("Search Toy"))
			{
				_lovenseToy.InitManager(_token.CStr());
				error = _lovenseToy.SearchToy();
			}

			ImGui::SameLine();

			if (!_lovenseToy.GetToyInfo()->toy_connected) ImGui::BeginDisabled();
			{
				if (ImGui::Button("Disconnect"))
				{
					_lovenseToy.SendCommand(CLovenseToy::COMMAND_VIBRATE, 0);
					error = _lovenseToy.DisconnectToy();
				}
			}
			if (!_lovenseToy.GetToyInfo()->toy_connected) ImGui::EndDisabled();

			ImGui::SameLine();

			if (!_lovenseToy.GetToyInfo()->toy_connected) ImGui::BeginDisabled();
			{
				if (ImGui::Checkbox("Active", &_active))
				{
					if(!_active)
						_lovenseToy.SendCommand(CLovenseToy::COMMAND_VIBRATE, 0);
				}
			}
			if (!_lovenseToy.GetToyInfo()->toy_connected) ImGui::EndDisabled();

			SetUpInputText("Token:", _token.Data(), _token.Size(), 1.0f, 0.3f, true, "You need a token from the Lovense dev portal in order to use this feature. Go to Help/Get Lovense Token.", ImGuiInputTextFlags_Password);

			static bool toyInfoValid = false;
			toyInfoValid = _lovenseToy.GetToyInfo()->toy_name != nullptr;

			if (toyInfoValid && !_lovenseToy.GetToyInfo()->toy_connected && _lovenseToy.IsInitialized())
			{
				error = _lovenseToy.ConnectToToy();
				_lovenseToy.StopSearchingToy();
			}

			ImGui::Text("Toy Name: ");
			if (toyInfoValid)
			{
				ImGui::SameLine();
				ImGui::Text(_lovenseToy.GetToyInfo()->toy_name);
			}

			ImGui::Text("Toy ID: ");
			if (toyInfoValid)
			{
				ImGui::SameLine();
				ImGui::Text(_lovenseToy.GetToyInfo()->toy_id);
			}

			ImGui::Text("Battery: ");
			if (toyInfoValid)
			{
				ImGui::SameLine();
				ImGui::InputInt("##toybattery", &_lovenseToy.GetToyInfo()->toy_battery, 0, 0, ImGuiInputTextFlags_ReadOnly);
			}

			ImGui::Text("Toy Connected: ");
			if (toyInfoValid)
			{
				ImGui::SameLine();
				ImGui::Text(_lovenseToy.GetToyInfo()->toy_connected ? "Yes" : "No");
			}
			
			ImGui::Text(_lovenseToy.GetErrorMessageByCode(error));
			


		} ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginChild("child_value", ImGui::GetContentRegionAvail(), true);
		{
			static int type = s_ValueTypes[_valueTypeIndex].second;

			if(SetUpCombo("Value Type:", s_ValueTypes, _valueTypeIndex, 0.5f))
				type = s_ValueTypes[_valueTypeIndex].second;

			ImGui::SameLine();

			switch (type)
			{
			case FLOAT:
				SetUpInputFloat("Max. Value:", &_maxF, 1.0f, 5.0f, "%3f", 1.0f);
				break;
			case DOUBLE:
				SetUpInputDouble("Max. Value:", &_maxD, 1.0, 5.0, "%3f", 1.0f);
				break;
			case INT8: case INT16: case INT32:
				SetUpInputInt("Max. Value:", &_maxI, 1, 5, 1.0f);
				break;
			case INT64:
				SetUpInputInt64("Max. Value:", &_maxL, 1, 5, 1.0f);
				break;
			}

			if (ImGui::Checkbox("Use Module", &_useModulePath))
				if (_useModulePath && _moduleAddress != 0)
					_moduleAddress = ProcessInformation::GetModuleAddress<uint64_t>(_moduleW);

			ImGui::SameLine();

			if (!_useModulePath) ImGui::BeginDisabled();
			{
				if (SetUpInputText("Module", _module.Data(), _module.Size(), 1.0f, 0.0f, false))
					_moduleW = MT::Convert<const char*, std::wstring>(_module.CStr(), MT::UTF8, MT::UTF16LE);
			}
			if (!_useModulePath) ImGui::EndDisabled();

			if (SetUpInputText("Pointer Path:", _pointerPathText.data(), _pointerPathText.size(), 1.0f, 0.3f))
			{
				_pointerPath.clear();
				std::string line;

				if (_pointerPathText.find(',') == std::string::npos)
				{
					line = RemoveSpacePadding(_pointerPathText, true);

					if (!line.empty())
						if (line.front() != '\0')
							if (IsValidHexString(line))
								_pointerPath.push_back(stoll(line, 0, 16));
				}
				else
				{
					std::stringstream stream;
					stream << _pointerPathText;

					while (std::getline(stream, line, ','))
					{
						line = RemoveSpacePadding(line, true);

						if (line.empty())
							break;

						if (line.front() == '\0')
							break;

						if (!IsValidHexString(line))
							break;

						_pointerPath.push_back(stoll(line, 0, 16));
					}
				}
			}

			int addrType = ProcessInformation::GetAddressWidth() == 8 ? ImGuiDataType_U64 : ImGuiDataType_U32;
			std::string format = ProcessInformation::GetAddressWidth() == 8 ? "%016X" : "%08X";
			ImGui::Text("Target Addr Range:");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(itemWidth * 0.1f);
			ImGui::InputScalar("##Target addr range:", addrType, &_rangeMin, NULL, NULL, format.c_str());
			ImGui::SameLine();
			ImGui::Text(" - ");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 2.0f);
			ImGui::InputScalar("## - ", addrType, &_rangeMax, NULL, NULL, format.c_str());

			//static std::string format(8, '\0');
			static int vibrationVal = 0;
			if (_active)
			{
				static uint64_t valptr = 0;
				valptr = reinterpret_cast<uint64_t>(getCurrentPointer());
				

				if ((valptr >= _rangeMin && valptr < _rangeMax) || _pointerPath.size() == 1)
				{
					switch (type)
					{
					case BOOL:
					{
						static bool readVal = false;
						readVal = ProcessInformation::ReadValue<bool>(valptr);
						vibrationVal = ScaleValue<bool>(readVal, 1);
						error = _lovenseToy.SendCommand(CLovenseToy::CmdType::COMMAND_VIBRATE, vibrationVal);
					} break;
					case INT8:
					{
						static int8_t readVal = 0;
						readVal = ProcessInformation::ReadValue<int8_t>(valptr);
						vibrationVal = ScaleValue<int8_t>(readVal, _maxI);
						error = _lovenseToy.SendCommand(CLovenseToy::CmdType::COMMAND_VIBRATE, vibrationVal);
					} break;
					case INT16:
					{
						static int16_t readVal = 0;
						readVal = ProcessInformation::ReadValue<int16_t>(valptr);
						vibrationVal = ScaleValue<int16_t>(readVal, _maxI);
						error = _lovenseToy.SendCommand(CLovenseToy::CmdType::COMMAND_VIBRATE, vibrationVal);
					} break;
					case INT64:
					{
						static int64_t readVal = 0;
						readVal = ProcessInformation::ReadValue<int64_t>(valptr);
						vibrationVal = ScaleValue<int64_t>(readVal, _maxL);
						error = _lovenseToy.SendCommand(CLovenseToy::CmdType::COMMAND_VIBRATE, vibrationVal);
					} break;
					case FLOAT:
					{
						static float readVal = 0.0f;
						readVal = ProcessInformation::ReadValue<float>(valptr);
						vibrationVal = ScaleValue<float>(readVal, _maxF);
						error = _lovenseToy.SendCommand(CLovenseToy::CmdType::COMMAND_VIBRATE, vibrationVal);
					} break;
					case DOUBLE:
					{
						static double readVal = 0.0;
						readVal = ProcessInformation::ReadValue<double>(valptr);
						vibrationVal = ScaleValue<double>(readVal, _maxD);
						error = _lovenseToy.SendCommand(CLovenseToy::CmdType::COMMAND_VIBRATE, vibrationVal);
					} break;
					default: //INT32
					{
						static int32_t readVal = 0;
						readVal = ProcessInformation::ReadValue<int32_t>(valptr);
						vibrationVal = ScaleValue<int32_t>(readVal, _maxI);
						error = _lovenseToy.SendCommand(CLovenseToy::CmdType::COMMAND_VIBRATE, vibrationVal);
					} break;
					}
				}

			}


			std::rotate(_plotVals.begin(), _plotVals.begin() + 1, _plotVals.end());
			_plotVals.back() = static_cast<float>(vibrationVal);

			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			float plotMin = 0.0f;
			float plotMax = 20.0f;
			ImGui::PlotLines("##Lines", _plotVals.data(), _plotCount, 0, NULL, 0.0f, 20.0f, ImVec2(0.0f, 40.0f));




		//	static int val = 0;
		//	if (SetUpSliderInt("Vibrate", &val, 0, 20, "%d", 1.0f))
		//	{
		//		error = _lovenseToy.SendCommand(CLovenseToy::CmdType::COMMAND_VIBRATE, val);
		//	}

			

		//	ImGui::SameLine();

		//	ImGui::SetNextItemWidth(itemWidth * 0.15f);
			//ImGui::InputDouble("##value", &_val, NULL, NULL, "%.6f");

			//ImGui::SameLine();

			//ImGui::Text("Plot Range:");
			//ImGui::SameLine();
			//ImGui::SetNextItemWidth(itemWidth * 0.1f);

			//if (ImGui::InputFloat("##plot range start", &_plotMin, NULL, NULL, "%.6f"))
				//_plotMin = _plotMin;

			//ImGui::SameLine();
			//ImGui::Text(" - ");
			//ImGui::SameLine();

			//if (ImGui::InputFloat("##plot range end", &_plotMax, NULL, NULL, "%.6f"))
				//_plotMax = _plotMax;

			//ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			//ImGui::PlotLines("##Lines", _plotVals.data(), _plotCount, 0, NULL, _plotMin, _plotMax, ImVec2(0.0f, 40.0f));

			//float progVal = 0;

			if (_active)
			{
				//progVal = _val / _plotMax;
				//sprintf(_plotBuf.data(), "%.6f/%.6f", _val, _plotMax);
			}

			//ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

			//if (_freeze)
			//{
				//float val = static_cast<float>(_val);
				//SetUpSliderFloat("writeval", &val, _plotMin, _plotMax, "%.6f", 1.0f, 0.0f, false);
				//_val = static_cast<double>(val);
			//}
			//else
				//ImGui::ProgressBar(progVal, ImVec2(0.0f, 0.0f), _plotBuf.data());
		}
		ImGui::EndChild();

	}
	ImGui::EndChild();

	if (_delete)
		DeviceControl::DeleteItem(_deviceId);
}

void MungPlex::DeviceControl::Device::DrawSetup(const float itemWidth, const float itemHeight, const int type)
{
	SetUpInputText("Title:", _label.data(), _label.size(), 0.25f, 0.2f);

	ImGui::BeginChild("child_Setup", ImVec2(itemWidth * 0.5f, itemHeight * 1.5f));
	{
		ImGui::BeginChild("child_setup", ImVec2(itemWidth * 0.15f, itemHeight * 1.5f), true);
		{
			if (ImGui::Checkbox("Active", &_active))
				;//	if (_active && _useModulePath)
				//	_moduleAddress = ProcessInformation::GetModuleAddress<uint64_t>(_moduleW);



			if (ImGui::Button("Delete"))
			{
				_delete = true;
			}

		}
		ImGui::EndChild();
		/*
		ImGui::SameLine();

		ImGui::BeginChild("child_pointer", ImVec2(itemWidth * 0.345f, itemHeight * 1.5f), true);
		{
			if (ImGui::Checkbox("Use Module", &_useModulePath))
				if (_useModulePath)
					_moduleAddress = ProcessInformation::GetModuleAddress<uint64_t>(_moduleW);

			ImGui::SameLine();
			if (!_useModulePath) ImGui::BeginDisabled();
			SetUpInputText("Module", _module.data(), _module.size(), 1.0f, 0.0f, false);
			if (!_useModulePath) ImGui::EndDisabled();

			if (SetUpInputText("Pointer Path:", _pointerPathText.data(), _pointerPathText.size(), 1.0f, 0.3f))
			{
				_pointerPath.clear();
				std::string line;

				if (_pointerPathText.find(',') == std::string::npos)
				{
					line = RemoveSpacePadding(_pointerPathText, true);

					if (!line.empty())
						if (line.front() != '\0')
							if (IsValidHexString(line))
								_pointerPath.push_back(stoll(line, 0, 16));
				}
				else
				{
					std::stringstream stream;
					stream << _pointerPathText;

					while (std::getline(stream, line, ','))
					{
						line = RemoveSpacePadding(line, true);

						if (line.empty())
							break;

						if (line.front() == '\0')
							break;

						if (!IsValidHexString(line))
							break;

						_pointerPath.push_back(stoll(line, 0, 16));
					}
				}
			}

			int addrType = ProcessInformation::GetAddressWidth() == 8 ? ImGuiDataType_U64 : ImGuiDataType_U32;
			std::string format = ProcessInformation::GetAddressWidth() == 8 ? "%016X" : "%08X";
			ImGui::Text("Target Addr Range:");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(itemWidth * 0.1f);
			ImGui::InputScalar("##Target addr range:", addrType, &_rangeMin, NULL, NULL, format.c_str());
			ImGui::SameLine();
			ImGui::Text(" - ");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 2.0f);
			ImGui::InputScalar("## - ", addrType, &_rangeMax, NULL, NULL, format.c_str());
		}
		ImGui::EndChild();*/
	}
	ImGui::EndChild();
}

void* MungPlex::DeviceControl::DeviceLovense::getCurrentPointer()
{
	if (_pointerPath.empty())
		return 0;

	int64_t ptr = 0;

	if (_useModulePath)
		ptr = _moduleAddress;

	for (int i = 0; i < _pointerPath.size() - 1; ++i)
	{
		ptr += _pointerPath[i];

		if (ProcessInformation::GetProcessType() == ProcessInformation::EMULATOR)
		{
			int regionIndex = ProcessInformation::GetRegionIndex(ptr);

			if (regionIndex >= 0)
			{
				switch (ProcessInformation::GetAddressWidth())
				{
				case 2:
					ptr = ProcessInformation::ReadValue<uint8_t>(ptr);
					break;
				case 4:
					ptr = ProcessInformation::ReadValue<uint32_t>(ptr);
					break;
				default://8
					ptr = ProcessInformation::ReadValue<uint64_t>(ptr);
				}
			}
			else
			{
				ptr = 0;
				break;
			}
		}
		else
		{
			ptr = ProcessInformation::ReadValue<uint64_t>(ptr);
		}
	}

	return reinterpret_cast<void*>(ptr + _pointerPath.back());
}