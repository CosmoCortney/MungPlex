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

void MungPlex::ProcessInformation::DrawModuleList() const
{
	if (!ImGui::CollapsingHeader("Modules"))
		return;

	enum ContentsType { CT_Text, CT_FillButton };
	static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY;
	static int contents_type = CT_Text;

	if (!ImGui::BeginTable("Modules", 2, flags))
		return;
	
	ImGui::TableSetupColumn("Module");
	ImGui::TableSetupColumn("Base Address");
	ImGui::TableHeadersRow();

	for (const auto& [currentModule, moduleAddress] : GetInstance()._modules)
	{
		ImGui::TableNextRow();
		for (int column = 0; column < 2; ++column)
		{
			ImGui::TableSetColumnIndex(column);
			// TODO No C buffers usage, replace it with a std::string/std::stringstream usage
			char buf[256];

			if (column == 0)
			{
				sprintf(buf, std::string(currentModule.begin(), currentModule.end()).c_str());
			}
			else
			{
				sprintf(buf, "%llX", moduleAddress);
			}

			if (contents_type == CT_Text)
				ImGui::TextUnformatted(buf);
			else if (contents_type == CT_FillButton)
				ImGui::Button(buf, ImVec2(-FLT_MIN, 0.0f));
		}
	}

	ImGui::EndTable();
}


void MungPlex::ProcessInformation::DrawRegionList() const
{
	if (!ImGui::CollapsingHeader("Regions"))
		return;

	enum ContentsType { CT_Text, CT_FillButton };
	static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY;
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

	for (auto& region : GetInstance()._regions)
	{
		ImGui::TableNextRow();
		for (int column = 0; column < 8; ++column)
		{
			std::stringstream stream;
			ImGui::TableSetColumnIndex(column);

			// TODO No C buffers in C++ code, use std::string and std::stringstream etc.
			char buf[256];

			switch(column)
			{
			case 0:
				sprintf_s(buf, "%llX", region.GetBaseAddress<uint64_t>());
				break;
			case 1:
				sprintf_s(buf, "%llX", region.GetAllocationBase<uint64_t>());
				break;
			case 2:
				sprintf_s(buf, "%u", region.GetAllocationProtect());
				break;
			case 3:
				sprintf_s(buf, "%u", region.GetPartitionId());
				break;
			case 4:
				sprintf_s(buf, "%llX", region.GetProtect());
				break;
			case 5:
				sprintf_s(buf, "%llX", region.GetRegionSize());
				break;
			case 6:
				sprintf_s(buf, "%llX", region.GetState());
				break;
			case 7:
				sprintf_s(buf, "%llX", region.GetType());
				break;
			default:
				sprintf_s(buf, sizeof(buf), "");
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
	// TODO No C buffers in C++ code, use std::string and std::stringstream etc.
	char buf[2048];
	ImGui::SeparatorText("Misc. Information");

	ImGui::BeginGroup();

	strcpy_s(buf, std::string(_exePath.begin(), _exePath.end()).c_str());
	SetUpLableText("Path:", buf, IM_ARRAYSIZE(buf), 1.0f, 0.1f);

	strcpy_s(buf, std::to_string(GetInstance()._pid).c_str());
	SetUpLableText("Process ID (dec):", buf, IM_ARRAYSIZE(buf), 1.0f, 0.1f);

	const std::string strTemp = _isX64 ? "Yes" : "No";
	strcpy_s(buf, strTemp.c_str());
	SetUpLableText("Is x64:", buf, IM_ARRAYSIZE(buf), 1.0f, 0.1f);

	ImGui::EndGroup();
}


void MungPlex::ProcessInformation::DrawGameInformation() const
{
	if (!ImGui::CollapsingHeader("Game Information"))
		return;

	//ImGui::SeparatorText("Game Information");

	static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;

	if (ImGui::BeginTable("Game Info", 2, flags))
	{
		for (const auto& [Entity, Location, Datatype,
			Size, Hex, Value] : _gameEntities)
		{
			ImGui::TableNextRow();
			for (int column = 0; column < 2; column++)
			{
				ImGui::TableSetColumnIndex(column);
				char buf[256];
				if (column == 0)
				{
					sprintf_s(buf, Entity.c_str());
				}
				else
				{
					sprintf_s(buf, Value.c_str());
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
	std::string buffer;
	std::string jsonstr;

	{
		std::ifstream inFile;
		inFile.open(SystemInfoJSON);

		if (inFile)
			while (std::getline(inFile, buffer))
				jsonstr.append(buffer).append("\n");
		else
			return false;
	}

	try
	{
		auto doc = nlohmann::json::parse(jsonstr);
		std::string emuNameBasic(emulator.begin(), emulator.end());
		auto& regions = doc["Emulators"][emuNameBasic]["Regions"];
		auto& entities = doc["Emulators"][emuNameBasic]["Entities"];

		for (auto& region : regions)
		{
			auto label = region["Label"].get<std::string>();
			uint64_t base = std::stoll(region["Base"].get<std::string>(), 0, 0);
			uint64_t size = std::stoll(region["Size"].get<std::string>(), 0, 0);
			GetInstance()._systemRegions.emplace_back(SystemRegion(label, base, size));
		}

		for (auto& gameEntity : entities)
		{
			auto entity = gameEntity["Entity"].get<std::string>();
			int location = std::stoi(gameEntity["Location"].get<std::string>(), 0, 0);
			auto datatype = gameEntity["Datatype"].get<std::string>();
			int size = std::stoi(gameEntity["Size"].get<std::string>(), 0, 0);
			bool hex = gameEntity["Hex"].get<bool>();
			GetInstance()._gameEntities.emplace_back(GameEntity(entity, location, datatype, size, hex));
		}

		if (regions.empty() || entities.empty())
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
	const EMUPAIR emulator = GetInstance()._emulators[emulatorIndex];
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

	for (const auto& region : _regions)
	{
		const uint64_t rSize = region.GetRegionSize();

		if (rSize == 0x400000 || rSize == 0x800000)
		{
			uint32_t temp;

			Xertz::SystemInfo::GetProcessInfo(_pid).ReadExRAM(&temp, region.GetBaseAddress<char*>() + 8, 4);

			if (temp == 0x03400008)
			{
				_systemRegions[0].BaseLocationProcess = region.GetBaseAddress<void*>();
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

	for (const auto& _region : _regions)
	{
		if (_region.GetRegionSize() == 0x2000000)
		{
			Xertz::SystemInfo::GetProcessInfo(_pid).ReadExRAM(&temp, reinterpret_cast<void*>(_region.GetBaseAddress<uint64_t>() + 0x28), 4);
			Xertz::SystemInfo::GetProcessInfo(_pid).ReadExRAM(&flagGCN, reinterpret_cast<void*>(_region.GetBaseAddress<uint64_t>() + 0x18), 4);
			Xertz::SystemInfo::GetProcessInfo(_pid).ReadExRAM(&flagWii, reinterpret_cast<void*>(_region.GetBaseAddress<uint64_t>() + 0x1C), 4);

			if (temp == 0x8001)
			{
				_systemRegions[0].BaseLocationProcess = _region.GetBaseAddress<void*>();
				break;
			}
		}
	}

	if (flagGCN == 0xC2339F3D || (flagWii != 0 && flagGCN == 0))
	{
		_systemRegions.erase(_systemRegions.begin() + 1);
		return true;
	}

	for (const auto& _region : _regions)
	{
		if (_region.GetRegionSize() == 0x4000000)
		{
			unsigned char temp;
			Xertz::SystemInfo::GetProcessInfo(_pid).ReadExRAM(&temp, reinterpret_cast<void*>(_region.GetBaseAddress<uint64_t>() + 1), 1);

			if (temp == 0x9f)
			{
				_systemRegions[1].BaseLocationProcess = _region.GetBaseAddress<void*>();
				break;
			}
		}
	}

	int IDcopy;
	Xertz::SystemInfo::GetProcessInfo(_pid).ReadExRAM(&temp, _systemRegions[0].BaseLocationProcess, 4);
	Xertz::SystemInfo::GetProcessInfo(_pid).ReadExRAM(&IDcopy, static_cast<char*>(_systemRegions[0].BaseLocationProcess) + 0x3180, 4);

	if (temp == 0 && IDcopy != 0)
		//ConnectionStatus = CONNECTED_DOLPHIN_WIIWARE;
		return true;
	if (IDcopy == temp)
		//ConnectionStatus = CONNECTED_DOLPHIN_WII;
		return true;

	return false;
}


std::vector<MungPlex::SystemRegion>& MungPlex::ProcessInformation::GetRegions()
{
	return GetInstance()._systemRegions;
}

void MungPlex::ProcessInformation::ObtainGameEntities(void* baseLocation)
{
	std::string entityValue;
	entityValue.reserve(2048);

	for (auto& [Entity, Location, Datatype,
		Size, Hex, Value] : _gameEntities)
	{
		static char buffer[2048];
		std::stringstream stream;
		const int size = Size;
		const bool hex = Hex;

		const void* readLocation = static_cast<char*>(baseLocation) + Location;
		Xertz::SystemInfo::GetProcessInfo(_pid).ReadExRAM(buffer, readLocation, size);

		if (Datatype.compare("INT") == 0)
		{
			uint64_t tempVal = *reinterpret_cast<uint64_t*>(buffer);
			tempVal &= ~(0xFFFFFFFFFFFFFFFF << (8 * size));

			if (hex)
				stream << "0x" << std::hex << tempVal;
			else
				stream << tempVal;

			entityValue = stream.str();
		}
		else if (Datatype.compare("TEXT") == 0)
		{
			entityValue = std::string(buffer, size);
		}
		else if (Datatype.compare("BIN") == 0)
		{
			for (int x = 0; x < size; ++x)
			{
				stream << std::hex << (static_cast<int>(  *(reinterpret_cast<char*>(buffer) + x)  ) & 0xff);
			}
			entityValue.append(stream.str());
		}

		Value = entityValue;
	}
}

bool MungPlex::ProcessInformation::ConnectToEmulator(const int emulatorIndex)
{
	if (!GetInstance().LoadSystemInformationJSON(emulatorIndex))
		return false;

	if (!GetInstance().InitEmulator(emulatorIndex))
		return false;
	
	GetInstance()._exePath = Xertz::SystemInfo::GetProcessInfo(GetInstance()._pid).GetFilePath(); // refactor these two lines when implementing PC game support
	GetInstance()._isX64 = Xertz::SystemInfo::GetProcessInfo(GetInstance()._pid).IsX64();		  //-^

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