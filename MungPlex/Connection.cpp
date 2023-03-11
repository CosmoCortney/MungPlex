#include "Connection.h"

void MungPlex::Connection::DrawWindow()
{
	ImGui::Begin("Connection");
	GetInstance().DrawConnectionSelect();
	GetInstance().DrawGameInformation();
	ImGui::End();
}

void MungPlex::Connection::DrawConnectionSelect()
{
	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
	{
		std::string emuSelect;
		if (ImGui::BeginTabItem("Emulator"))
		{
			MungPlex::SetUpCombo("Select Emulator", _emulators, _currentEmulatorNumber);

			if (ImGui::Button("Connect", ImVec2(200, 50)))
			{
				std::wstring emuSelect = _emulators[_currentEmulatorNumber].first;
				LoadSystemInformationJSON(emuSelect);
			}
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Native Application"))
		{
			ImGui::Button("Connect", ImVec2(200, 50));
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
}

void MungPlex::Connection::DrawGameInformation()
{
	ImGui::SeparatorText("GameInformation");

	enum ContentsType { CT_Text, CT_FillButton };
	static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
	static bool display_headers = false;
	static int contents_type = CT_Text;

	if (ImGui::BeginTable("Game Info", 2, flags))
	{
		for (int row = 0; row < _gameEntities.size(); row++)
		{
			ImGui::TableNextRow();
			for (int column = 0; column < 2; column++)
			{
				ImGui::TableSetColumnIndex(column);
				char buf[256];
				if (column == 0)
				{
					sprintf(buf, _gameEntities[row].Entity.c_str());
				}
				else
				{
					sprintf(buf, _gameEntities[row].Value.c_str());
				}

				if (contents_type == CT_Text)
					ImGui::TextUnformatted(buf);
				else if (contents_type == CT_FillButton)
					ImGui::Button(buf, ImVec2(-FLT_MIN, 0.0f));
			}
		}
		ImGui::EndTable();
	}
}

void MungPlex::Connection::LoadSystemInformationJSON(std::wstring& emuName)
{
	_gameEntities.clear();
	_systemRegions.clear();
	std::ifstream inFile;
	std::string buffer;
	std::string jsonstr;
	inFile.open(SystemInfoJSON);

	if (inFile)
	{
		while (std::getline(inFile, buffer))
		{
			jsonstr.append(buffer).append("\n");
		}
	}
	inFile.close();

	auto doc = json::parse5(jsonstr);

	if (!doc) {
		std::cerr << "Parsing failed" << std::endl;
		return;
	}
	
	std::string emuNameBasic(emuName.begin(), emuName.end());

	auto& docv = doc.value();
	auto& regions = docv["Emulators"][emuNameBasic]["Regions"].as_array();
	auto& entities = docv["Emulators"][emuNameBasic]["Entities"].as_array();

	for (int i = 0; i < regions.size(); ++i)
	{
		std::string label = regions[i]["Label"].as_string();
		uint64_t base = regions[i]["Base"].as_long_long();
		uint64_t size = regions[i]["Size"].as_long_long();
		_systemRegions.push_back(SystemRegion(label, base, size));
	}

	for (int i = 0; i < entities.size(); ++i)
	{
		std::string entity = entities[i]["Entity"].as_string();
		int location = entities[i]["Location"].as_integer();
		std::string datatype = entities[i]["Datatype"].as_string();;
		int size = entities[i]["Size"].as_integer();
		bool hex = entities[i]["Hex"].as_boolean();
		_gameEntities.push_back(GameEntity(entity, location, datatype, size, hex));
	}

	std::cout << _currentEmulatorNumber;
	InitProcess(emuName, 0, std::pair<std::wstring, int>(emuName, _currentEmulatorNumber));
}

void MungPlex::Connection::InitProcess(std::wstring& processName, int connectionType, std::pair<std::wstring, int> emulator)
{
	_currentPID = Xertz::SystemInfo::GetProcessInfo(processName, Xertz::StringDefs::IS_SUBSTRING, Xertz::StringDefs::CASE_INSENSITIVE).GetPID();

	if (_currentPID == -1)
		return;

	_regions = Xertz::SystemInfo::GetProcessInfo(_currentPID).GetRegionList();
	_handle = Xertz::SystemInfo::GetProcessInfo(_currentPID).GetHandle();

	switch (connectionType)
	{
		case SELECT_EMULATOR:
		{
			switch (emulator.second)
			{
			case DOLPHIN:
				InitDolphin();
				break;
			case PROJECT64:
				InitProject64();
				break;
			}
		}

		ObtainGameEntities(_systemRegions[0].BaseLocationProcess);

		case SELECT_NATIVE_APP:
		{

			break;
		}
		case SELECT_REMOTE_SYSTEM:
		{

			break;
		}
	}

	MungPlex::ProcessInformation::RefreshData(_currentPID);
}

void MungPlex::Connection::InitProject64()
{
	BE = false;

	for (uint64_t i = 0; i < _regions.size(); ++i)
	{
		uint64_t rSize = _regions[i].GetRegionSize();

		if ((rSize == 0x400000) || (rSize == 0x800000))
		{
			uint32_t temp;

			Xertz::SystemInfo::GetProcessInfo(_currentPID).ReadExRAM(&temp, _regions[i].GetBaseAddress<char*>() + 8, 4);

			if (temp == 0x03400008)
			{
				_systemRegions[0].BaseLocationProcess = _regions[i].GetBaseAddress<void*>();
				ConnectionStatus = CONNECTED_PROJECT64;
				return;
			}
		}
	}
}

void MungPlex::Connection::InitDolphin()
{
	BE = true;
	_systemRegions.erase(_systemRegions.begin() + 2); //--
	_systemRegions.erase(_systemRegions.begin() + 2); // |- remove these lines once caches and sram are figured out
	_systemRegions.erase(_systemRegions.begin() + 2); //--

	unsigned int temp, flagGCN, flagWii;
	std::cout << _regions.size() << " fhgdju" << _currentPID << std::endl;

	for (uint64_t i = 0; i < _regions.size(); ++i)
	{
		if (_regions[i].GetRegionSize() == 0x2000000)
		{
			Xertz::SystemInfo::GetProcessInfo(_currentPID).ReadExRAM(&temp, reinterpret_cast<void*>(_regions[i].GetBaseAddress<uint64_t>() + 0x28), 4);
			Xertz::SystemInfo::GetProcessInfo(_currentPID).ReadExRAM(&flagGCN, reinterpret_cast<void*>(_regions[i].GetBaseAddress<uint64_t>() + 0x18), 4);
			Xertz::SystemInfo::GetProcessInfo(_currentPID).ReadExRAM(&flagWii, reinterpret_cast<void*>(_regions[i].GetBaseAddress<uint64_t>() + 0x1C), 4);

			if (temp == 0x8001)
			{
				_systemRegions[0].BaseLocationProcess = _regions[i].GetBaseAddress<void*>();
				break;
			}
		}
	}

	if (flagGCN == 0xC2339F3D || (flagWii != 0 && flagGCN == 0))
	{
		ConnectionStatus = CONNECTED_DOLPHIN_GAMECUBE;
		_systemRegions.erase(_systemRegions.begin() + 1);
		return;
	}

	for (uint64_t i = 0; i < _regions.size(); ++i)
	{
		if (_regions[i].GetRegionSize() == 0x4000000)
		{
			unsigned char temp;
			Xertz::SystemInfo::GetProcessInfo(_currentPID).ReadExRAM(&temp, reinterpret_cast<void*>(_regions[i].GetBaseAddress<uint64_t>() + 1), 1);

			if (temp == 0x9f)
			{
				_systemRegions[1].BaseLocationProcess = _regions[i].GetBaseAddress<void*>();
				break;
			}
		}
	}
	
	int IDcopy;
	Xertz::SystemInfo::GetProcessInfo(_currentPID).ReadExRAM(&temp, _systemRegions[0].BaseLocationProcess, 4);
	Xertz::SystemInfo::GetProcessInfo(_currentPID).ReadExRAM(&IDcopy, (void*)  ((char*)_systemRegions[0].BaseLocationProcess + 0x3180), 4);

 	if (temp == 0 && IDcopy != 0)
		ConnectionStatus = CONNECTED_DOLPHIN_WIIWARE;
	else if (IDcopy == temp)
		ConnectionStatus = CONNECTED_DOLPHIN_WII;
	else
		ConnectionStatus = "Connected with Dolphin";
}

void MungPlex::Connection::ObtainGameEntities(void* baseLocation)
{
	std::string entityValue;
	entityValue.reserve(2048);
	void* buffer = malloc(2048);

	for (int i = 0; i < _gameEntities.size(); ++i)
	{
		std::stringstream stream;
		uint64_t tempVal;
		int size = _gameEntities[i].Size;
		std::string dataType = _gameEntities[i].Datatype;
		bool hex = _gameEntities[i].Hex;

		void* readLocation = (void*)((char*)baseLocation + _gameEntities[i].Location);
		Xertz::SystemInfo::GetProcessInfo(_currentPID).ReadExRAM(buffer, readLocation, size);

		if (dataType.compare("INT") == 0)
		{
			tempVal = *(uint64_t*)buffer;
			tempVal &= ~(0xFFFFFFFFFFFFFFFF << (8 * size));

			if (hex)
				stream << "0x" << std::hex << tempVal;
			else
				stream << tempVal;

			entityValue = stream.str();
		}
		else if (dataType.compare("TEXT") == 0)
		{
			entityValue = std::string((char*)buffer, size);
		}
		else if (dataType.compare("BIN") == 0)
		{
			for (int x = 0; x < size; ++x)
			{
				stream << std::hex << (((int)*((char*)buffer + x)) & 0xff);
			}
			entityValue.append(stream.str());
		}

		_gameEntities[i].Value = entityValue;
	}

	free(buffer);
}

int MungPlex::Connection::GetCurrentPID()
{
	return GetInstance()._currentPID;
}

std::vector<MungPlex::SystemRegion>& MungPlex::Connection::GetRegions()
{
	return GetInstance()._systemRegions;
}

HANDLE MungPlex::Connection::GetCurrentHandle()
{
	return GetInstance()._handle;
}