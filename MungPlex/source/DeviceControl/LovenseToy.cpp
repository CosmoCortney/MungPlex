#include "LovenseToy.hpp"

void MungPlex::LovenseToy::CEventCallBack::LovenseDidSearchStart()
{
	Log::LogInformation("Lovense: Toy search started");
	std::cout << "Lovense: Toy search started\n";
}

void MungPlex::LovenseToy::CEventCallBack::LovenseSearchingToys(lovense_toy_info_t* info)
{
	SetToyInfo(info);
	Log::LogInformation("Lovense: Searching for toy(s): ");
	Log::LogInformation(info->toy_id);
	Log::LogInformation(info->toy_name);
	Log::LogInformation(std::to_string(info->toy_battery));
	Log::LogInformation(std::to_string(info->toy_type));
	Log::LogInformation(std::to_string(info->toy_connected));
	std::cout << "Lovense: Searching for toy(s)\n";
}

void MungPlex::LovenseToy::CEventCallBack::LovenseErrorOutPut(int errorCode, const char* errorMsg)
{
	Log::LogInformation("Lovense error ");
	Log::LogInformation(std::to_string(errorCode), true);
	Log::LogInformation(": ", true);
	Log::LogInformation(errorMsg, true);
	std::cout << "Lovense error " << errorCode << ": " << errorMsg << "\n";
}

void MungPlex::LovenseToy::CEventCallBack::LovenseDidSearchEnd()
{
	Log::LogInformation("Lovense: Searching for toy(s) ended");
	std::cout << "Lovense: Searching for toy(s) ended\n";
}

void MungPlex::LovenseToy::CEventCallBack::LovenseDidSendCmdStart()
{
	//Log::LogInformation("Lovense: Start sending command");
	//std::cout << "Lovense: Start sending command\n";
}

void MungPlex::LovenseToy::CEventCallBack::LovenseSendCmdResult(const char* szToyID, CLovenseToy::CmdType cmd, const char* result, CLovenseToy::Error errorCode)
{
	/*Log::LogInformation("Lovense: Command results: Toy ID: ");
	Log::LogInformation(szToyID, true);
	Log::LogInformation(", Command Type:", true);
	Log::LogInformation(std::to_string(cmd), true);
	Log::LogInformation(", Result: ", true);
	Log::LogInformation(result, true);
	std::cout << "Lovense: Command results: Toy ID: " << szToyID << ", Command Type: " << cmd << ", Error Code: " << errorCode << ", Result: " << result << "\n";
*/
}

void MungPlex::LovenseToy::CEventCallBack::LovenseDidSendCmdEnd()
{
	//Log::LogInformation("Lovense: Finished sending command");
	//std::cout << "Lovense: Finished sending command\n";
}

void MungPlex::LovenseToy::CEventCallBack::LovenseToyConnectedStatus(const char* szToyID, bool isConnected)
{
	Log::LogInformation("Lovense: Toy (");
	Log::LogInformation(szToyID, true);
	Log::LogInformation(") ", true);
	std::cout << "Lovense: Toy (" << szToyID << ") ";

	if (isConnected)
	{
		Log::LogInformation("connected", true);
		std::cout << "connected\n";
	}
	else
	{
		Log::LogInformation("disconnected", true);
		std::cout << "disconnected\n";
	}
}

MungPlex::LovenseToy::LovenseToy()
{
	//_token = Settings::GetDeviceControlSettings().LovenseToken.StdStrNoLeadinZeros();
}

MungPlex::LovenseToy::~LovenseToy()
{
	ReleaseLovenseToyManager();
	_manager.~shared_ptr();
}

void MungPlex::LovenseToy::InitManager(const char* token)
{
	if (_manager)
		return;

	_token = token;
	_manager = std::shared_ptr<CLovenseToyManager>(GetLovenseToyManager());
	bool tokenSet = _manager->SetDeveloperToken(_token.c_str());
	_manager->RegisterEventCallBack(_callbacks.get());
}

int MungPlex::LovenseToy::SendCommand(const CLovenseToy::CmdType cmd, const int param)
{
	return _manager->SendCommand(_toyInfo.toy_id, cmd, param);
}

int MungPlex::LovenseToy::SearchToy()
{
	return _manager->StartSearchToy();
}

int MungPlex::LovenseToy::StopSearchingToy()
{
	return _manager->StopSearchToy();
}

int MungPlex::LovenseToy::ConnectToToy()
{
	int result = _manager->ConnectToToy(_toyInfo.toy_id);

	if (result == CLovenseToy::TOYERR_SUCCESS)
	{
		_toyInfo.toy_connected = true;
		result = _manager->GetToyBattery(_toyInfo.toy_id, &_toyInfo.toy_battery);
	}
	else
	{
		_toyInfo.toy_connected = false;
	}

	return result;
}

int MungPlex::LovenseToy::DisconnectToy()
{
	SendCommand(CLovenseToy::COMMAND_VIBRATE, 0);
	int result = _manager->DisConnectedToy(_toyInfo.toy_id);

	if (result == CLovenseToy::TOYERR_SUCCESS)
	{
		_toyInfo.toy_connected = false;
		_toyInfo.toy_battery = 0;
		delete[] _toyInfo.toy_id;
		delete[] _toyInfo.toy_name;
		_toyInfo.toy_id = _toyInfo.toy_name = nullptr;
		_toyInfo.toy_type = 0;
	}

	return result;
}

bool MungPlex::LovenseToy::IsInitialized()
{
	return _manager != nullptr;
}

void MungPlex::LovenseToy::SetToyInfo(const lovense_toy_info_t* toyInfo)
{
	_toyInfo = *toyInfo;
}

lovense_toy_info_t* MungPlex::LovenseToy::GetToyInfo()
{
	return &_toyInfo;
}

const char* MungPlex::LovenseToy::GetErrorMessageByCode(const int err)
{
	switch (err)
	{
	case -1:
		return "Error -1: Unknown Error";
	case 0:
		return "No Errors :)";
	case 1001:
		return "Error 1001: Aborted";
	case 1002:
		return "Error 1002: HID Init Failed";
	case 1003:
		return "Error 1003: No HID USB Dongle found";
	case 1004:
		return "Error 1004: Unable to get HID USB Dongle Information";
	case 1005:
		return "Error 1005: No Toy connected";
	case 1006:
		return "Error 1006: Invalid Argument";
	case 1007:
		return "Error 1007: Invalid Command";
	case 1008:
		return "Error 1008: Invalid Token";
	case 1009:
		return "Error 1009: COM Read Error";
	case 1010:
		return "Error 1010: COM Write Error";
	case 1011:
		return "Error 1011: COM not found";
	case 1012:
		return "Error 1012: HID Read Error";
	case 1013:
		return "Error 1013: HID Write Error";
	case 1014:
		return "Error 1014: Device not found";
	case 1015:
		return "Error 1015: HID failed to open device";
	case 1016:
		return "Error 1016: COM failed to open device";
	case 1017:
		return "Error 1017: Failed writing to device";
	case 1018:
		return "Error 1018: Failed reading from device";
	case 1019:
		return "Error 1019: Search in progress. Can't send commands!";
	case 1020:
		return "Error 1020: Command(s) not matching";
	case 1100:
		return "Error 1100: Failed sending bootloader";
	case 1101:
		return "Error 1101: Failed receiving bootloader";
	case 1102:
		return "Error 1102: Break Mode failed";
	case 1103:
		return "Error 1103: Failed to restart bootloader";
	case 1112:
		return "Error 1112: DFU sending command failed";
	case 1113:
		return "Error 1113: DFU receiving command failed";
	case 1114:
		return "Error 1114: DFU respondce command failed";
	case 1120:
		return "Error 1120: Failed to open update file";
	case 1121:
		return "Error 1121: Failed to read update file";
	case 1122:
		return "Error 1122: Failed to copy update file";
	case 1130:
		return "Error 1130: Update notification failed";
	case 1131:
		return "Error 1131: Restarting notification failed";
	case 2000:
		return "Error 2000: Command Arg Size Error";
	case 2001:
		return "Error 2001: Command Arg Value Error";
	case 2002:
		return "Error 2002: Command Config doesn't exist";
	case 2003:
		return "Error 2003: Command Arg Type Error";
	default:
		return "Even the API doesn't know what the fuck happened lol";
	}
}
