#include "DeviceLovense_Impl.hpp"
#include "../../Settings.hpp"

boost::atomic<bool> MungPlex::DeviceLovense::_toyControlThreadFlag = false;

MungPlex::DeviceLovense::DeviceLovense()
{
	_token = Settings::GetDeviceControlSettings().LovenseToken;
}

void MungPlex::DeviceLovense::Draw()
{
	float itemWidth = ImGui::GetContentRegionAvail().x;
	float itemHeight = 192.0f * Settings::GetGeneralSettings().Scale;

#ifndef NDEBUG
	ImGui::SeparatorText("Lovence Toy (Token won't be recognized in Debug Mode!)");
#else
	ImGui::SeparatorText("Lovence Toy");
#endif

	ImGui::BeginChild(std::string("child_lovenseDevice" + _idText).c_str(), ImVec2(itemWidth, itemHeight));
	{
		ImGui::BeginChild("child_toyInfo", ImVec2(itemWidth * 0.3f, itemHeight), true);
		{
			drawToyConnectionOptions();
			drawToyInfo();

		} ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginChild("child_value", ImGui::GetContentRegionAvail(), true);
		{
			drawValueTypeOptions();
			drawPointerSettings();
			plotValues();
		}
		ImGui::EndChild();
	}
	ImGui::EndChild();
}

void MungPlex::DeviceLovense::drawToyConnectionOptions()
{
	if (ImGui::Button("Search Toy"))
	{
		_lovenseToy.InitManager(_token.CStr());
		_toyError = _lovenseToy.SearchToy();
	}

	ImGui::SameLine();

	if (!_lovenseToy.GetToyInfo()->toy_connected) ImGui::BeginDisabled();
	{
		if (ImGui::Button("Disconnect"))
		{
			_lovenseToy.SendCommand(CLovenseToy::COMMAND_VIBRATE, 0);
			_toyError = _lovenseToy.DisconnectToy();
		}
	}
	if (!_lovenseToy.GetToyInfo()->toy_connected) ImGui::EndDisabled();

	ImGui::SameLine();

	if (!_lovenseToy.GetToyInfo()->toy_connected) ImGui::BeginDisabled();
	{
		if (ImGui::Checkbox("Active", &_active))
		{
			if (!_active)
			{
				_toyControlThreadFlag = false;

				if (_toyControlThread.joinable())
					_toyControlThread.join();

				_lovenseToy.SendCommand(CLovenseToy::COMMAND_VIBRATE, 0);
			}
			else
			{
				_toyControlThreadFlag = true;
				_toyControlThread = boost::thread(&MungPlex::DeviceLovense::controlToy, this);
			}
		}
	}
	if (!_lovenseToy.GetToyInfo()->toy_connected) ImGui::EndDisabled();

	SetUpInputText("Token:", _token.Data(), _token.Size(), 1.0f, 0.3f, true, "You need a token from the Lovense dev portal in order to use this feature. Go to Help/Get Lovense Token.", ImGuiInputTextFlags_Password);

}

void MungPlex::DeviceLovense::drawToyInfo()
{
	static bool toyInfoValid = false;
	toyInfoValid = _lovenseToy.GetToyInfo()->toy_name != nullptr;

	if (toyInfoValid && !_lovenseToy.GetToyInfo()->toy_connected && _lovenseToy.IsInitialized())
	{
		_toyError = _lovenseToy.ConnectToToy();
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

	ImGui::Text(_lovenseToy.GetErrorMessageByCode(_toyError));
}

void MungPlex::DeviceLovense::drawValueTypeOptions()
{
	if (SetUpCombo("Value Type:", s_ValueTypes, _valueTypeIndex, 0.5f))
		_valueType = s_ValueTypes[_valueTypeIndex].second;

	if(_valueType != BOOL)
		ImGui::SameLine();

	const std::string info = "The maximum read value to be considered.";

	switch (_valueType)
	{
	case FLOAT:
		SetUpInputFloat("Max. Value:", &_maxF, 1.0f, 5.0f, "%3f", 1.0f, 0.4f, 0, true, info.c_str());
		break;
	case DOUBLE:
		SetUpInputDouble("Max. Value:", &_maxD, 1.0, 5.0, "%3f", 1.0f, 0.4f, 0, true, info.c_str());
		break;
	case INT8: case INT16: case INT32:
		SetUpInputInt("Max. Value:", &_maxI, 1, 5, 1.0f, 0.4f, 0, true, info.c_str());
		break;
	case INT64:
		SetUpInputInt64("Max. Value:", &_maxL, 1, 5, 1.0f, 0.4f, 0, true, info.c_str());
		break;
	}
}

void MungPlex::DeviceLovense::drawPointerSettings()
{
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

	if (SetUpInputText("Pointer Path:", _pointerPathText.data(), _pointerPathText.size(), 1.0f, 0.2f))
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
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
	ImGui::InputScalar("##Target addr range:", addrType, &_rangeMin, NULL, NULL, format.c_str());
	ImGui::SameLine();
	ImGui::Text(" - ");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
	ImGui::InputScalar("## - ", addrType, &_rangeMax, NULL, NULL, format.c_str());
}

void MungPlex::DeviceLovense::controlToy()
{
	while (_toyControlThreadFlag)
	{
		if (_active && Connection::IsConnected())
		{
			boost::this_thread::sleep_for(boost::chrono::milliseconds(_toyControlIntervalMilliseconds));

			static uint64_t valptr = 0;
			valptr = reinterpret_cast<uint64_t>(ProcessInformation::GetPointerFromPointerPathExpression(_pointerPath, _useModulePath, _moduleAddress));

			if ((valptr >= _rangeMin && valptr < _rangeMax) || _pointerPath.size() == 1)
			{
				switch (_valueType)
				{
				case BOOL:
				{
					static bool readVal = false;
					readVal = ProcessInformation::ReadValue<bool>(valptr);
					_vibrationValue = ScaleValue<bool>(readVal, 1);
					_toyError = _lovenseToy.SendCommand(CLovenseToy::CmdType::COMMAND_VIBRATE, _vibrationValue);
				} break;
				case INT8:
				{
					static int8_t readVal = 0;
					readVal = ProcessInformation::ReadValue<int8_t>(valptr);
					_vibrationValue = ScaleValue<int8_t>(readVal, _maxI);
					_toyError = _lovenseToy.SendCommand(CLovenseToy::CmdType::COMMAND_VIBRATE, _vibrationValue);
				} break;
				case INT16:
				{
					static int16_t readVal = 0;
					readVal = ProcessInformation::ReadValue<int16_t>(valptr);
					_vibrationValue = ScaleValue<int16_t>(readVal, _maxI);
					_toyError = _lovenseToy.SendCommand(CLovenseToy::CmdType::COMMAND_VIBRATE, _vibrationValue);
				} break;
				case INT64:
				{
					static int64_t readVal = 0;
					readVal = ProcessInformation::ReadValue<int64_t>(valptr);
					_vibrationValue = ScaleValue<int64_t>(readVal, _maxL);
					_toyError = _lovenseToy.SendCommand(CLovenseToy::CmdType::COMMAND_VIBRATE, _vibrationValue);
				} break;
				case FLOAT:
				{
					static float readVal = 0.0f;
					readVal = ProcessInformation::ReadValue<float>(valptr);
					_vibrationValue = ScaleValue<float>(readVal, _maxF);
					_toyError = _lovenseToy.SendCommand(CLovenseToy::CmdType::COMMAND_VIBRATE, _vibrationValue);
				} break;
				case DOUBLE:
				{
					static double readVal = 0.0;
					readVal = ProcessInformation::ReadValue<double>(valptr);
					_vibrationValue = ScaleValue<double>(readVal, _maxD);
					_toyError = _lovenseToy.SendCommand(CLovenseToy::CmdType::COMMAND_VIBRATE, _vibrationValue);
				} break;
				default: //INT32
				{
					static int32_t readVal = 0;
					readVal = ProcessInformation::ReadValue<int32_t>(valptr);
					_vibrationValue = ScaleValue<int32_t>(readVal, _maxI);
					_toyError = _lovenseToy.SendCommand(CLovenseToy::CmdType::COMMAND_VIBRATE, _vibrationValue);
				} break;
				}
			}
		}
	}
}

void MungPlex::DeviceLovense::plotValues()
{
	std::rotate(_plotVals.begin(), _plotVals.begin() + 1, _plotVals.end());
	_plotVals.back() = static_cast<float>(_vibrationValue);

	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
	const float plotMin = 0.0f;
	const float plotMax = 20.0f;
	ImGui::PlotLines("##Lines", _plotVals.data(), _plotCount, 0, NULL, 0.0f, 20.0f, ImVec2(0.0f, 40.0f));
}
