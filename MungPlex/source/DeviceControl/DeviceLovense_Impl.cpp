#include "DeviceControl.hpp"
#include "DeviceLovense_Impl.hpp"
#include "Settings.hpp"
#include "StringHelpers.hpp"

boost::atomic<bool> MungPlex::DeviceLovense::_toyControlThreadFlag = false;

MungPlex::DeviceLovense::DeviceLovense(const int id)
{
	_deviceTypeID = IDevice::LOVENSE;
	_deviceId = id;
	_idText = std::to_string(id);
	_tokenInput.SetText(Settings::GetDeviceControlSettings().LovenseToken.StdStrNoLeadinZeros());
	setHelpTexts();
}

MungPlex::DeviceLovense::DeviceLovense(const int id, const nlohmann::json& json)
{
	_tokenInput.SetText(Settings::GetDeviceControlSettings().LovenseToken.StdStrNoLeadinZeros());
	_deviceTypeID = IDevice::LOVENSE;
	_valueTypeSelectCombo.SetSelectedById(json["valueType"]);
	_nameInput.SetText(json["name"]);
	setHelpTexts();

	switch (_valueTypeSelectCombo.GetSelectedId())
	{
	case FLOAT:
		_maxFoatInput.SetValue(json["maxF"]);
		break;
	case DOUBLE:
		_maxDoubleInput.SetValue(json["maxD"]);
		break;
	default:
		_maxIntInput.SetValue(json["maxI"]);
	}

	_useModulePath = json["useModulePath"];
	_moduleInput.SetText(json["module"]);
	_pointerPathInput.SetText(json["pointerPathText"]);
	_rangeMin = json["rangeMin"];
	_rangeMax = json["rangeMax"];
}

MungPlex::DeviceLovense::~DeviceLovense()
{
	_abortPlot = true;

	if (_lovenseToy.IsInitialized())
	{
		_lovenseToy.~LovenseToy();
	}
}

MungPlex::DeviceLovense::DeviceLovense(const DeviceLovense& other)
{
	assign(other);
}

MungPlex::DeviceLovense& MungPlex::DeviceLovense::operator=(const DeviceLovense& other)
{
	return *this;
}

MungPlex::DeviceLovense::DeviceLovense(DeviceLovense&& other) noexcept
{
	assign(other);
}

MungPlex::DeviceLovense& MungPlex::DeviceLovense::operator=(DeviceLovense&& other) noexcept
{
	return *this;
}

void MungPlex::DeviceLovense::assign(const DeviceLovense& other)
{
	_deviceTypeID = other._deviceTypeID;
	_nameInput = other._nameInput;
	_lovenseToy = other._lovenseToy;
	_tokenInput = other._tokenInput;
	_toyError = other._toyError;
	_deviceId = other._deviceId;
	_idText = other._idText;
	_delete = other._delete;
	_active = other._active;
	_maxFoatInput = other._maxFoatInput;
	_maxDoubleInput = other._maxDoubleInput;
	_maxIntInput = other._maxIntInput;
	_useModulePath = other._useModulePath;
	_moduleAddress = other._moduleAddress;
	_moduleInput = other._moduleInput;
	_moduleW = other._moduleW;
	_pointerPathInput = other._pointerPathInput;
	_rangeMin = other._rangeMin;
	_rangeMax = other._rangeMax;
	_vibrationValue = other._vibrationValue;
	_previousVibrationValue = other._previousVibrationValue;
	_valueTypeSelectCombo = other._valueTypeSelectCombo;
	_plotVals = other._plotVals;
	_abortPlot = other._abortPlot;
	ParsePointerPath(_pointerPath, _pointerPathInput.GetStdStringNoZeros());
}

void MungPlex::DeviceLovense::setHelpTexts()
{
	static std::string helpText = "The maximum read value to be considered.";
	_maxFoatInput.SetHelpText(helpText, true);
	_maxDoubleInput.SetHelpText(helpText, true);
	_maxIntInput.SetHelpText(helpText, true);
	_tokenInput.SetHelpText("You need a token from the Lovense dev portal in order to use this feature. Go to Help/Get Lovense Token.", true);
}

void MungPlex::DeviceLovense::Draw()
{
	float itemWidth = ImGui::GetContentRegionAvail().x;
	float itemHeight = 192.0f * Settings::GetGeneralSettings().Scale;

#ifndef NDEBUG
	ImGui::SeparatorText("Lovense Toy (Token won't be recognized in Debug Mode!)");
#else
	ImGui::SeparatorText("Lovense Toy");
#endif

	ImGui::BeginChild(std::string("child_lovenseDevice" + _idText).c_str(), ImVec2(itemWidth, itemHeight));
	{
		ImGui::BeginChild("child_toyInfo", ImVec2(itemWidth * 0.3f, itemHeight), true);
		{
			drawToyConnectionOptions();
			drawToyInfo();
		} 
		ImGui::EndChild();

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

nlohmann::json MungPlex::DeviceLovense::GetJSON()
{
	nlohmann::json elemJson;
	elemJson["deviceType"] = LOVENSE;
	elemJson["valueType"] = _valueTypeSelectCombo.GetSelectedId();
	elemJson["name"] = _nameInput.GetStdStringNoZeros();

	switch (_valueTypeSelectCombo.GetSelectedId())
	{
	case FLOAT:
		elemJson["maxF"] = _maxFoatInput.GetValue();
		break;
	case DOUBLE:
		elemJson["maxD"] = _maxDoubleInput.GetValue();
		break;
	default:
		elemJson["maxI"] = _maxIntInput.GetValue();
		break;
	}

	elemJson["useModulePath"] = _useModulePath;
	elemJson["module"] = _moduleInput.GetStdStringNoZeros();
	elemJson["pointerPathText"] = _pointerPathInput.GetStdStringNoZeros();
	elemJson["rangeMin"] = _rangeMin;
	elemJson["rangeMax"] = _rangeMax;
	return elemJson;
}

void MungPlex::DeviceLovense::test()
{	
	for (int i = 0; i < 3; ++i)
	{
		_lovenseToy.SendCommand(CLovenseToy::COMMAND_VIBRATE, 2);
		boost::this_thread::sleep_for(boost::chrono::milliseconds(20));
		_lovenseToy.SendCommand(CLovenseToy::COMMAND_VIBRATE, 0);
		boost::this_thread::sleep_for(boost::chrono::milliseconds(20));
	}
}

void MungPlex::DeviceLovense::drawToyConnectionOptions()
{
	_nameInput.Draw(1.0f, 0.3f);
	if (_lovenseToy.GetToyInfo()->toy_connected)
	{
		if (ImGui::Button("Disconnect"))
		{
			_lovenseToy.SendCommand(CLovenseToy::COMMAND_VIBRATE, 0);
			_toyError = _lovenseToy.DisconnectToy();
		}
	}
	else
	{
		if (ImGui::Button("Search Toy"))
		{
			_lovenseToy.InitManager(_tokenInput.GetCString());
			_toyError = _lovenseToy.SearchToy();
		}
	}

	ImGui::SameLine();
	if (!_lovenseToy.GetToyInfo()->toy_connected) ImGui::BeginDisabled();
	{
		if (ImGui::Button("Test"))
		{
			_toyControlThread = boost::thread(&MungPlex::DeviceLovense::test, this);
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

	ImGui::SameLine();

	if (ImGui::Button("Delete"))
		DeviceControl::DeleteItem(_deviceId);

	_tokenInput.Draw(1.0f, 0.3f);
}

void MungPlex::DeviceLovense::drawToyInfo()
{
	static bool toyInfoValid = false;
	toyInfoValid = _lovenseToy.GetToyInfo()->toy_name != nullptr;

	if (toyInfoValid && !_lovenseToy.GetToyInfo()->toy_connected)
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
	_valueTypeSelectCombo.Draw(0.3f);

	if(_valueTypeSelectCombo.GetSelectedId() != BOOL)
		ImGui::SameLine();

	const std::string info = "The maximum read value to be considered.";

	switch (_valueTypeSelectCombo.GetSelectedId())
	{
	case FLOAT:
		_maxFoatInput.Draw(1.0f, 5.0f);
		break;
	case DOUBLE:
		_maxDoubleInput.Draw(1.0f, 5.0f);
		break;
	default:
		_maxIntInput.Draw(1.0f, 5.0f);
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
		if (_moduleInput.Draw(1.0f, 0.0f))
			_moduleW = MT::Convert<const char*, std::wstring>(_moduleInput.GetCString(), MT::UTF8, MT::UTF16LE);
	}
	if (!_useModulePath) ImGui::EndDisabled();

	if (_pointerPathInput.Draw(1.0f, 0.2f))
		ParsePointerPath(_pointerPath, _pointerPathInput.GetStdStringNoZeros());

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
				switch (_valueTypeSelectCombo.GetSelectedId())
				{
					case BOOL:
					{
						static bool readVal = false;
						readVal = ProcessInformation::ReadValue<bool>(valptr);
						_vibrationValue = ScaleValue<bool>(readVal, 1);
					} break;
					case INT8:
					{
						static int8_t readVal = 0;
						readVal = ProcessInformation::ReadValue<int8_t>(valptr);
						_vibrationValue = ScaleValue<int8_t>(readVal, _maxIntInput.GetValue());
					} break;
					case INT16:
					{
						static int16_t readVal = 0;
						readVal = ProcessInformation::ReadValue<int16_t>(valptr);
						_vibrationValue = ScaleValue<int16_t>(readVal, _maxIntInput.GetValue());
					} break;
					case INT64:
					{
						static int64_t readVal = 0;
						readVal = ProcessInformation::ReadValue<int64_t>(valptr);
						_vibrationValue = ScaleValue<int64_t>(readVal, _maxIntInput.GetValue());
					} break;
					case FLOAT:
					{
						static float readVal = 0.0f;
						readVal = ProcessInformation::ReadValue<float>(valptr);
						_vibrationValue = ScaleValue<float>(readVal, _maxFoatInput.GetValue());
					} break;
					case DOUBLE:
					{
						static double readVal = 0.0;
						readVal = ProcessInformation::ReadValue<double>(valptr);
						_vibrationValue = ScaleValue<double>(readVal, _maxDoubleInput.GetValue());
					} break;
					default: //INT32
					{
						static int32_t readVal = 0;
						readVal = ProcessInformation::ReadValue<int32_t>(valptr);
						_vibrationValue = ScaleValue<int32_t>(readVal, _maxIntInput.GetValue());
					}
				}

				if (_previousVibrationValue == _vibrationValue)
					continue;

				_toyError = _lovenseToy.SendCommand(CLovenseToy::CmdType::COMMAND_VIBRATE, _vibrationValue);
				_previousVibrationValue = _vibrationValue;
			}
		}
	}
}

void MungPlex::DeviceLovense::plotValues()
{
	if (_abortPlot)
		return;

	std::rotate(_plotVals.begin(), _plotVals.begin() + 1, _plotVals.end());
	_plotVals.back() = static_cast<float>(_vibrationValue);

	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
	const float plotMin = 0.0f;
	const float plotMax = 20.0f;
	ImGui::PlotLines("##Lines", _plotVals.data(), _plotCount, 0, NULL, 0.0f, 20.0f, ImVec2(0.0f, 40.0f));
}