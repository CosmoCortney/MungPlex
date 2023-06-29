#include "ProcessInformation.h"

void MungPlex::ProcessInformation::DrawWindow()
{
	ImGui::Begin("Process Information");
	
	GetInstance().DrawMiscInformation();
	ImGui::Separator();
	GetInstance().DrawGameInformation();
	ImGui::Separator();
	GetInstance().DrawModuleList();
	ImGui::Separator();
	GetInstance().DrawRegionList();

	ImGui::End();
}

void MungPlex::ProcessInformation::RefreshData(const int pid)
{
	GetInstance()._pid = pid;
	GetInstance()._handle = Xertz::SystemInfo::GetProcessInfo(pid).GetHandle();
	GetInstance()._modules = Xertz::SystemInfo::GetProcessInfo(pid).GetModuleList();
	GetInstance()._regions = Xertz::SystemInfo::GetProcessInfo(pid).GetRegionList();
}

void MungPlex::ProcessInformation::DrawModuleList()
{
	if (!ImGui::CollapsingHeader("Modules"))
		return;

	enum ContentsType { CT_Text, CT_FillButton };
	static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY;
	static bool display_headers = true;
	static int contents_type = CT_Text;
	std::wstring currentModule;

	if (!ImGui::BeginTable("Modules", 2, flags))
		return;
	
	ImGui::TableSetupColumn("Module");
	ImGui::TableSetupColumn("Base Address");
	ImGui::TableHeadersRow();

	currentModule.reserve(256);

	for (int row = 0; row < GetInstance()._modules.size(); ++row)
	{
		ImGui::TableNextRow();
		for (int column = 0; column < 2; ++column)
		{
			currentModule = GetInstance()._modules[row].first;
			ImGui::TableSetColumnIndex(column);
			char buf[256];

			if (column == 0)
			{
				sprintf(buf, std::string(currentModule.begin(), currentModule.end()).c_str());
			}
			else
			{
				sprintf(buf, "%llX", GetInstance()._modules[row].second);
			}

			if (contents_type == CT_Text)
				ImGui::TextUnformatted(buf);
			else if (contents_type == CT_FillButton)
				ImGui::Button(buf, ImVec2(-FLT_MIN, 0.0f));
		}
	}

	ImGui::EndTable();
}


void MungPlex::ProcessInformation::DrawRegionList()
{
	if (!ImGui::CollapsingHeader("Regions"))
		return;

	enum ContentsType { CT_Text, CT_FillButton };
	static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY;
	static bool display_headers = true;
	static int contents_type = CT_Text;
	std::wstring currentModule;
	
	if (!ImGui::BeginTable("Regions", 8, flags))
		return;

	ImGui::TableSetupColumn("Base Address (hex)");
	ImGui::TableSetupColumn("Allocation Address (hex)");
	ImGui::TableSetupColumn("Allocation Protect (hex)");
	ImGui::TableSetupColumn("Partition ID");
	ImGui::TableSetupColumn("Protection");
	ImGui::TableSetupColumn("Size");
	ImGui::TableSetupColumn("State");
	ImGui::TableSetupColumn("Type");
	ImGui::TableHeadersRow();

	currentModule.reserve(256);

	for (int row = 0; row < GetInstance()._regions.size(); ++row)
	{
		ImGui::TableNextRow();
		for (int column = 0; column < 8; ++column)
		{
			std::stringstream stream;
			ImGui::TableSetColumnIndex(column);
			char buf[256];

			switch(column)
			{
			case 0:
				sprintf(buf, "%llX", GetInstance()._regions[row].GetBaseAddress<uint64_t>());
				break;
			case 1:
				sprintf(buf, "%llX", GetInstance()._regions[row].GetAllocationBase<uint64_t>());
				break;
			case 2:
				sprintf(buf, "%u", GetInstance()._regions[row].GetAllocationProtect());
				break;
			case 3:
				sprintf(buf, "%u", GetInstance()._regions[row].GetPartitionId());
				break;
			case 4:
				sprintf(buf, "%llX", GetInstance()._regions[row].GetProtect());
				break;
			case 5:
				sprintf(buf, "%llX", GetInstance()._regions[row].GetRegionSize());
				break;
			case 6:
				sprintf(buf, "%llX", GetInstance()._regions[row].GetState());
				break;
			case 7:
				sprintf(buf, "%llX", GetInstance()._regions[row].GetType());
				break;
			}

			if (contents_type == CT_Text)
				ImGui::TextUnformatted(buf);
			else if (contents_type == CT_FillButton)
				ImGui::Button(buf, ImVec2(-FLT_MIN, 0.0f));
		}
	}

	ImGui::EndTable();
}

void MungPlex::ProcessInformation::DrawMiscInformation()
{
	char buf[2048];
	ImGui::Text("Misc. Information");

	ImGui::BeginGroup();

	strcpy_s(buf, std::string(_exePath.begin(), _exePath.end()).c_str());
	ImGui::InputText("Path", buf, IM_ARRAYSIZE(buf));

	strcpy_s(buf, std::to_string(GetInstance()._pid).c_str());
	ImGui::InputText("Process ID (dec)", buf, IM_ARRAYSIZE(buf));

	std::string strTemp = _isX64 ? "Yes" : "No";
	strcpy_s(buf, strTemp.c_str());
	ImGui::InputText("Is x64", buf, IM_ARRAYSIZE(buf));

	ImGui::EndGroup();
}


void MungPlex::ProcessInformation::DrawGameInformation()
{
	if (!ImGui::CollapsingHeader("Game Information"))
		return;

	//ImGui::SeparatorText("Game Information");

	static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
	static bool display_headers = false;

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

				ImGui::TextUnformatted(buf);
			}
		}
		ImGui::EndTable();
	}
}

bool MungPlex::ProcessInformation::LoadSystemInformationJSON(const int emulatorIndex)
{
	std::wstring emulator = GetInstance()._emulators[emulatorIndex].first;
	GetInstance()._gameEntities.clear();
	GetInstance()._systemRegions.clear();
	std::ifstream inFile;
	std::string buffer;
	std::string jsonstr;
	inFile.open(SystemInfoJSON);

	if (inFile)
		while (std::getline(inFile, buffer))
			jsonstr.append(buffer).append("\n");
	else
		return false;

	inFile.close();

	try
	{
		auto doc = nlohmann::json::parse(jsonstr);
		std::string emuNameBasic(emulator.begin(), emulator.end());
		auto& regions = doc["Emulators"][emuNameBasic]["Regions"];
		auto& entities = doc["Emulators"][emuNameBasic]["Entities"];

		for (int i = 0; i < regions.size(); ++i)
		{
			std::string label = regions[i]["Label"].get<std::string>();
			uint64_t base = std::stoll(regions[i]["Base"].get<std::string>(), 0, 0);
			uint64_t size = std::stoll(regions[i]["Size"].get<std::string>(), 0, 0);
			GetInstance()._systemRegions.push_back(SystemRegion(label, base, size));
		}

		for (int i = 0; i < entities.size(); ++i)
		{
			std::string entity = entities[i]["Entity"].get<std::string>();
			int location = std::stoi(entities[i]["Location"].get<std::string>(), 0, 0);
			std::string datatype = entities[i]["Datatype"].get<std::string>();
			int size = std::stoi(entities[i]["Size"].get<std::string>(), 0, 0);
			bool hex = entities[i]["Hex"].get<bool>();
			GetInstance()._gameEntities.push_back(GameEntity(entity, location, datatype, size, hex));
		}

		if (!regions.size() || !entities.size())
			return false;
	}
	catch (const nlohmann::json::parse_error& exception)
	{
		std::cerr << "Parsing failed: " << exception.what() << std::endl;
		return false;
	}

	return true;
}

bool MungPlex::ProcessInformation::InitEmulator(const int emulatorIndex)
{
	EMUPAIR emulator = GetInstance()._emulators[emulatorIndex];
	_pid = Xertz::SystemInfo::GetProcessInfo(emulator.first, true, false).GetPID();
	bool connected = false;

	if (_pid == -1)
		return false;

	RefreshData(_pid);
	
	switch (emulator.second)
	{
	case DOLPHIN:
		connected = InitDolphin();
		break;
	case PROJECT64:
		connected = InitProject64();
		break;
	}
	
	ObtainGameEntities(_systemRegions[0].BaseLocationProcess);
	return connected;
}

bool MungPlex::ProcessInformation::InitProject64()
{
	_underlyingIsBigEndian = false;
	_addressWidth = 4;

	for (uint64_t i = 0; i < _regions.size(); ++i)
	{
		uint64_t rSize = _regions[i].GetRegionSize();

		if ((rSize == 0x400000) || (rSize == 0x800000))
		{
			uint32_t temp;

			Xertz::SystemInfo::GetProcessInfo(_pid).ReadExRAM(&temp, _regions[i].GetBaseAddress<char*>() + 8, 4);

			if (temp == 0x03400008)
			{
				_systemRegions[0].BaseLocationProcess = _regions[i].GetBaseAddress<void*>();
				return true;
			}
		}
	}
	return false;
}

bool MungPlex::ProcessInformation::InitProcess(const std::wstring& processName) //placeholder
{
	return false;
}

bool MungPlex::ProcessInformation::InitDolphin()
{
	_underlyingIsBigEndian = true;
	_addressWidth = 4;
	_systemRegions.erase(_systemRegions.begin() + 2); //--
	_systemRegions.erase(_systemRegions.begin() + 2); // |- remove these lines once caches and sram are figured out
	_systemRegions.erase(_systemRegions.begin() + 2); //--

	uint32_t temp = 0;
	uint32_t flagGCN = 0;
	uint32_t flagWii = 0;

	for (uint64_t i = 0; i < _regions.size(); ++i)
	{
		if (_regions[i].GetRegionSize() == 0x2000000)
		{
			Xertz::SystemInfo::GetProcessInfo(_pid).ReadExRAM(&temp, reinterpret_cast<void*>(_regions[i].GetBaseAddress<uint64_t>() + 0x28), 4);
			Xertz::SystemInfo::GetProcessInfo(_pid).ReadExRAM(&flagGCN, reinterpret_cast<void*>(_regions[i].GetBaseAddress<uint64_t>() + 0x18), 4);
			Xertz::SystemInfo::GetProcessInfo(_pid).ReadExRAM(&flagWii, reinterpret_cast<void*>(_regions[i].GetBaseAddress<uint64_t>() + 0x1C), 4);

			if (temp == 0x8001)
			{
				_systemRegions[0].BaseLocationProcess = _regions[i].GetBaseAddress<void*>();
				break;
			}
		}
	}

	if (flagGCN == 0xC2339F3D || (flagWii != 0 && flagGCN == 0))
	{
		_systemRegions.erase(_systemRegions.begin() + 1);
		return true;
	}

	for (uint64_t i = 0; i < _regions.size(); ++i)
	{
		if (_regions[i].GetRegionSize() == 0x4000000)
		{
			unsigned char temp;
			Xertz::SystemInfo::GetProcessInfo(_pid).ReadExRAM(&temp, reinterpret_cast<void*>(_regions[i].GetBaseAddress<uint64_t>() + 1), 1);

			if (temp == 0x9f)
			{
				_systemRegions[1].BaseLocationProcess = _regions[i].GetBaseAddress<void*>();
				break;
			}
		}
	}

	int IDcopy;
	Xertz::SystemInfo::GetProcessInfo(_pid).ReadExRAM(&temp, _systemRegions[0].BaseLocationProcess, 4);
	Xertz::SystemInfo::GetProcessInfo(_pid).ReadExRAM(&IDcopy, ((char*)_systemRegions[0].BaseLocationProcess + 0x3180), 4);

	if (temp == 0 && IDcopy != 0)
		//ConnectionStatus = CONNECTED_DOLPHIN_WIIWARE;
		return true;
	else if (IDcopy == temp)
		//ConnectionStatus = CONNECTED_DOLPHIN_WII;
		return true;

	return false;
}


std::vector<MungPlex::SystemRegion>& MungPlex::ProcessInformation::GetRegions()
{
	return GetInstance()._systemRegions;
}

void MungPlex::ProcessInformation::ObtainGameEntities(const void* baseLocation)
{
	std::string entityValue;
	entityValue.reserve(2048);
	char buffer[2048];

	for (int i = 0; i < _gameEntities.size(); ++i)
	{
		std::stringstream stream;
		uint64_t tempVal;
		int size = _gameEntities[i].Size;
		std::string dataType = _gameEntities[i].Datatype;
		bool hex = _gameEntities[i].Hex;

		void* readLocation = ((char*)baseLocation + _gameEntities[i].Location);
		Xertz::SystemInfo::GetProcessInfo(_pid).ReadExRAM(buffer, readLocation, size);

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
}

bool MungPlex::ProcessInformation::ConnectToEmulator(const int emulatorIndex)
{
	if (!GetInstance().LoadSystemInformationJSON(emulatorIndex))
		return false;

	if (!GetInstance().InitEmulator(emulatorIndex))
		return false;
	
	GetInstance()._exePath = Xertz::SystemInfo::GetProcessInfo(GetInstance()._pid).GetFilePath().c_str();	// refactor these two lines when implementing PC game support
	GetInstance()._isX64 = Xertz::SystemInfo::GetProcessInfo(GetInstance()._pid).IsX64();					//-^

	return true; 
}

void MungPlex::ProcessInformation::SetProcessType(const int32_t processType)
{
	GetInstance()._processType = processType;
}

std::vector<EMUPAIR>& MungPlex::ProcessInformation::GetEmulatorList()
{
	return GetInstance()._emulators;
}

int32_t MungPlex::ProcessInformation::GetProcessType()
{
	return GetInstance()._processType;
}

int32_t MungPlex::ProcessInformation::GetPID()
{
	return GetInstance()._pid;
}

void MungPlex::ProcessInformation::SetX64Flag(const bool isX64)
{
	GetInstance()._isX64 = isX64;
}

bool MungPlex::ProcessInformation::IsX64()
{
	return GetInstance()._isX64;
}

void MungPlex::ProcessInformation::SetUnderlyingBigEndianFlag(const bool isBigEndian)
{
	GetInstance()._underlyingIsBigEndian = isBigEndian;
}

bool MungPlex::ProcessInformation::UnderlyingIsBigEndian()
{
	return GetInstance()._underlyingIsBigEndian;
}

HANDLE MungPlex::ProcessInformation::GetHandle()
{
	return GetInstance()._handle;
}

int32_t MungPlex::ProcessInformation::GetAddressWidth()
{
	return GetInstance()._addressWidth;
}