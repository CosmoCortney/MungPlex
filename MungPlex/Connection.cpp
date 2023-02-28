#include "Connection.h"

namespace MungPlex
{
	bool BE = false;
	std::string ConnectionStatus = NO_CONNECTION;
}

void MungPlex::Connection::DrawWindow()
{
	//ImGui::NewFrame();

	ImGui::Begin("Connection");


	
	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
	{
		std::string emuSelect;
		if (ImGui::BeginTabItem("Emulator"))
		{
			const char* items[] = { GetInstance()._emulators[0].first.c_str(), 
									GetInstance()._emulators[1].first.c_str() };
			static int item_current = 0;
			ImGui::Combo("Select Emulator", &item_current, items, IM_ARRAYSIZE(items));


			if (ImGui::Button("Connect", ImVec2(200, 50)))
			{
				std::string emuSelect = GetInstance()._emulators[item_current].first;
				GetInstance().LoadSystemInformationJSON(emuSelect);
				std::cout << "connecting to " << emuSelect << std::endl;
			}

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Native Application"))
		{
			ImGui::Button("Connect", ImVec2(200, 50));
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Remote Device"))
		{
			ImGui::Text("Select Console.");
			ImGui::Button("Connect", ImVec2(200, 50));
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	ImGui::Separator();
	

	ImGui::Text("Game Information");
	//ImGui::Table


	// Expose a few Borders related flags interactively
	enum ContentsType { CT_Text, CT_FillButton };
	static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
	static bool display_headers = false;
	static int contents_type = CT_Text;

	if (ImGui::BeginTable("Game Info", 2, flags))
	{
		for (int row = 0; row < GetInstance()._gameEntities.size(); row++)
		{
			ImGui::TableNextRow();
			for (int column = 0; column < 2; column++)
			{
				ImGui::TableSetColumnIndex(column);
				char buf[256];
				if (column == 0)
				{
					sprintf(buf, GetInstance()._gameEntities[row].Entity.c_str());
				}
				else
				{
					sprintf(buf, GetInstance()._gameEntities[row].Value.c_str());
				}

				if (contents_type == CT_Text)
					ImGui::TextUnformatted(buf);
				else if (contents_type == CT_FillButton)
					ImGui::Button(buf, ImVec2(-FLT_MIN, 0.0f));
			}
		}
		ImGui::EndTable();
	}












	
	ImGui::End();
}

void MungPlex::Connection::LoadSystemInformationJSON(std::string& emuName)
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

	//std::cout << jsonstr.length();


	auto doc = json::parse5(jsonstr);

	if (!doc) {
		std::cerr << "Parsing failed" << std::endl;
		return;
	}

	auto& docv = doc.value();
	auto& regions = docv["Emulators"][emuName]["Regions"].as_array();
	auto& entities = docv["Emulators"][emuName]["Entities"].as_array();

	for (int i = 0; i < regions.size(); ++i)
	{
		std::string label = regions[i]["Label"].as_string();
		unsigned long long base = regions[i]["Base"].as_long_long();
		unsigned long long size = regions[i]["Size"].as_long_long();
		GetInstance()._systemRegions.push_back(SystemRegion(label, base, size));

		//std::cout << GetInstance()._systemRegions[i].Label << " " << std::hex << GetInstance()._systemRegions[i].Base << " " << GetInstance()._systemRegions[i].Size << std::endl;
	}

	for (int i = 0; i < entities.size(); ++i)
	{
		std::string entity = entities[i]["Entity"].as_string();
		int location = entities[i]["Location"].as_integer();
		std::string datatype = entities[i]["Datatype"].as_string();;
		int size = entities[i]["Size"].as_integer();
		bool hex = entities[i]["Hex"].as_boolean();
		_gameEntities.push_back(GameEntity(entity, location, datatype, size, hex));

		//std::cout << GetInstance()._gameEntities[i].Entity << " " << std::hex << GetInstance()._gameEntities[i].Location << " " << GetInstance()._gameEntities[i].Size << " " << GetInstance()._gameEntities[i].Datatype << " " << GetInstance()._gameEntities[i].Hex << std::endl;
	}


	for (int i = 0; i < _gameEntities.size(); ++i)
	{

	}

	InitProcess(emuName, 0, std::pair<std::string, int>(emuName, 0));
}

void MungPlex::Connection::ParseJsonToEntities()
{
	//static std::vector<MungPlex::GameEntity> GameEntities;
	//json::value& docv;
	//GetInstance()._gameEntities.clear();

	//
}

void MungPlex::Connection::InitProcess(std::string& processName, int connectionType, std::pair<std::string, int> emulator)
{
	switch (connectionType)
	{
		case SELECT_EMULATOR:
		{
			switch (emulator.second)
			{
				case DOLPHIN:
					InitDolphin();
			}
		}
		case SELECT_NATIVE_APP:
		{

		}
		case SELECT_REMOTE_SYSTEM:
		{

		}
	}
}

void MungPlex::Connection::InitDolphin()
{
	_systemRegions.erase(_systemRegions.begin() + 2); //--
	_systemRegions.erase(_systemRegions.begin() + 2); // |- remove these lines once caches and sram are figured out
	_systemRegions.erase(_systemRegions.begin() + 2); //--

	unsigned int temp, flagGCN, flagWii;
	_currentPID = Xertz::SystemInfo::GetProcessInfo(L"Dolphin", Xertz::StringDefs::IS_SUBSTRING, Xertz::StringDefs::CASE_INSENSITIVE).GetPID();
	REGION_LIST regions = Xertz::SystemInfo::GetProcessInfo(_currentPID).GetRegionList();

	for (unsigned long long i = 0; i < regions.size(); ++i)
	{
		if (regions[i].GetRegionSize() == 0x2000000)
		{
			Xertz::SystemInfo::GetProcessInfo(_currentPID).ReadExRAM(&temp, reinterpret_cast<void*>(regions[i].GetBaseAddress<unsigned long long>() + 0x28), 4);
			Xertz::SystemInfo::GetProcessInfo(_currentPID).ReadExRAM(&flagGCN, reinterpret_cast<void*>(regions[i].GetBaseAddress<unsigned long long>() + 0x18), 4);
			Xertz::SystemInfo::GetProcessInfo(_currentPID).ReadExRAM(&flagWii, reinterpret_cast<void*>(regions[i].GetBaseAddress<unsigned long long>() + 0x1C), 4);

			if (temp == 0x8001)
			{
				_systemRegions[0].BaseLocationProcess = regions[i].GetBaseAddress<void*>();
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

	for (unsigned long long i = 0; i < regions.size(); ++i)
	{
		if (regions[i].GetRegionSize() == 0x4000000)
		{
			unsigned char temp;
			Xertz::SystemInfo::GetProcessInfo(GetInstance()._currentPID).ReadExRAM(&temp, reinterpret_cast<void*>(regions[i].GetBaseAddress<unsigned long long>() + 1), 1);

			if (temp == 0x9f)
			{
				_systemRegions[1].BaseLocationProcess = regions[i].GetBaseAddress<void*>();
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

	ObtainGameEntities(_systemRegions[0].BaseLocationProcess);
}

void MungPlex::Connection::ObtainGameEntities(void* baseLocation)
{
	std::string entityValue;
	entityValue.reserve(2048);
	void* buffer = malloc(2048);

	for (int i = 0; i < _gameEntities.size(); ++i)
	{
		std::stringstream stream;
		unsigned long long tempVal;
		int size = _gameEntities[i].Size;
		std::string dataType = _gameEntities[i].Datatype;
		bool hex = _gameEntities[i].Hex;

		void* readLocation = (void*)((char*)baseLocation + _gameEntities[i].Location);
		Xertz::SystemInfo::GetProcessInfo(_currentPID).ReadExRAM(buffer, readLocation, size);

		if (dataType.compare("INT") == 0)
		{
			tempVal = *(unsigned long long*)buffer;
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