#include "ProcessInformation.h"
#include <Windows.h>
#include"Search.h"
#include"Cheats.h"
#include "Log.h"
#include"PointerSearch.h"
#include"WatchControl.h"

void MungPlex::ProcessInformation::DrawWindow()
{
	if (ImGui::Begin("Process Information"))
	{
		GetInstance().drawMiscInformation();
		ImGui::Separator();
		GetInstance().drawGameInformation();
		ImGui::Separator();
		GetInstance().drawModuleList();
		ImGui::Separator();
		GetInstance().drawRegionList();
	}
	ImGui::End();
}

/*void MungPlex::ProcessInformation::refreshData(const int pid)
{
	_pid = pid;
	_process = Xertz::SystemInfo::GetProcessInfo(pid);
	_handle = _process.GetHandle();
	Log::LogInformation("Refreshed Process Information");
}*/

void MungPlex::ProcessInformation::drawModuleList()
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

	for (const auto& [currentModule, moduleAddress] : GetModuleList())
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

void MungPlex::ProcessInformation::drawRegionList()
{
	if (!ImGui::CollapsingHeader("Regions"))
		return;

	int regionCount = GetRegionList().size();
	enum ContentsType { CT_Text, CT_FillButton };
	static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY;
	static int contents_type = CT_Text;
	std::wstring currentModule;
	static int maxRows = 100;
	int pages = regionCount / maxRows;
	static int page = 1;
	static int lastPageRows = 0;
	lastPageRows = regionCount % maxRows - 1;

	if (lastPageRows)
		++pages;

	if(SetUpInputInt("Page:", &page, 1, 1))
	{
		if (page < 1)
			page = pages;
		else if (page > pages)
			page = 1;
	}

	ImGui::SameLine();

	SetUpLableText("Of: ", std::to_string(pages).c_str(), 3);

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

	int rows = page == pages ? lastPageRows : maxRows;
	
	for (int i = 0; i < rows; ++i)
	{
		ImGui::TableNextRow();
		for (int column = 0; column < 8; ++column)
		{
			std::stringstream stream;
			ImGui::TableSetColumnIndex(column);
			static std::string buf(64, 0);
			Xertz::MemoryRegion& region = GetRegionList()[(page - 1) * maxRows + i];

			switch(column)
			{
			case 0:
				sprintf(buf.data(), "%llX", region.GetBaseAddress<uint64_t>());
				break;
			case 1:
				sprintf(buf.data(), "%llX", region.GetAllocationBase<uint64_t>());
				break;
			case 2:
				sprintf(buf.data(), "%u", region.GetAllocationProtect());
				break;
			case 3:
				sprintf(buf.data(), "%u", region.GetPartitionId());
				break;
			case 4:
				sprintf(buf.data(), "%llX", region.GetProtect());
				break;
			case 5:
				sprintf(buf.data(), "%llX", region.GetRegionSize());
				break;
			case 6:
				sprintf(buf.data(), "%llX", region.GetState());
				break;
			case 7:
				sprintf(buf.data(), "%llX", region.GetType());
				break;
			}

			if (contents_type == CT_Text)
				ImGui::TextUnformatted(buf.c_str());
			else if (contents_type == CT_FillButton)
				ImGui::Button(buf.c_str(), ImVec2(-FLT_MIN, 0.0f));
		}
	}
	
	ImGui::EndTable();
}

void MungPlex::ProcessInformation::drawMiscInformation()
{
	static std::string buf(256, 0);
	ImGui::SeparatorText("Misc. Information");

	ImGui::BeginGroup();

	strcpy(buf.data(), std::string(_exePath.begin(), _exePath.end()).c_str());
	SetUpLableText("Path:", buf.c_str(), buf.size(), 1.0f, 0.1f);

	strcpy(buf.data(), std::to_string(GetInstance().GetPID()).c_str());
	SetUpLableText("Process ID (dec):", buf.c_str(), buf.size(), 1.0f, 0.1f);

	const std::string strTemp = IsX64() ? "Yes" : "No";
	strcpy(buf.data(), strTemp.c_str());
	SetUpLableText("Is x64:", buf.c_str(), buf.size(), 1.0f, 0.1f);

	ImGui::EndGroup();
}

void MungPlex::ProcessInformation::drawGameInformation()
{
	if (!ImGui::CollapsingHeader("Game Information"))
		return;

	static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;

	if (ImGui::BeginTable("Game Info", 2, flags))
	{
		for (const auto& [Entity, Location, Datatype, Size, Hex, Value] : _gameEntities)
		{
			ImGui::TableNextRow();
			for (int column = 0; column < 2; column++)
			{
				ImGui::TableSetColumnIndex(column);
				static std::string buf(256, 0);

				if (column == 0)
				{
					sprintf(buf.data(), Entity.c_str());
				}
				else
				{
					sprintf(buf.data(), Value.c_str());
				}

				ImGui::TextUnformatted(buf.data());
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
		inFile.open(GetResourcesFilePath("systeminformation.json"));

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

		if (regions.empty())
			return false;
	}
	catch (const nlohmann::json::parse_error& exception)
	{
		std::cerr << "Parsing failed: " << exception.what() << std::endl;
		return false;
	}

	return true;
}

bool MungPlex::ProcessInformation::initEmulator(const int emulatorIndex)
{
	const EMUPAIR emulator = _emulators[emulatorIndex];
	_gameID.clear();
	_gameRegion.clear();
	_gameID.clear();
	_platform.clear();

	if (!initProcess(emulator.first))
		return false;

	bool connected = false;
	
	switch (emulator.second)
	{
	case DOLPHIN:
		connected = initDolphin();
		break;
	case PROJECT64:
		connected = initProject64();
		break;
	case CEMU:
		connected = initCemu();
		break;
	case YUZU:
		connected = initYuzu();
		break;
	case MELONDS:
		connected = initMelonDS();
		break;
	case MESEN:
		connected = initMesen();
		break;
	case PPSSPP:
		connected = initPPSSPP();
		break;
	case NO$PSX:
		connected = initNo$psx();
		break;
	case PCSX2:
		connected = initPcsx2();
		break;
	case RPCS3:
		connected = initRpcs3();
		break;
	case FUSION:
		connected = initFusion();
		break;
	}

	setupSearch();
	setupCheats();
	WatchControl::InitWatchFile();
	obtainGameEntities(_systemRegions[0].BaseLocationProcess);
	return connected;
}

void MungPlex::ProcessInformation::refreshModuleList()
{
	_process.RefreshModuleList();
}

bool MungPlex::ProcessInformation::ConnectToProcess(int processIndex)
{
	GetInstance()._process = Xertz::SystemInfo::GetProcessInfoList()[processIndex];
	return GetInstance().connectToProcessFR();
}

 bool MungPlex::ProcessInformation::ConnectToApplicationProcess(int applicationProcessIndex)
{
	 GetInstance()._process = Xertz::SystemInfo::GetApplicationProcessInfoList()[applicationProcessIndex];
	 return GetInstance().connectToProcessFR();
}

bool MungPlex::ProcessInformation::connectToProcessFR()
 {
	 bool connected = _process.GetPID() > 0 && _process.IsOpen();
	 if (!connected)
		 return false;

	 RefreshRegionlistPC();
	 refreshModuleList();
	 setMiscProcessInfo(_process.GetProcessName(), false, false, _process.IsX64() ? 8 : 4, 4);
	 GetInstance().setupSearch();
	 GetInstance()._processType = NATIVE;
	 Cheats::InitCheatFile();
	 WatchControl::InitWatchFile();
	 return connected;
 }

void MungPlex::ProcessInformation::setupSearch()
{
	Search::SetRereorderRegion(_rereorderRegion);
	Search::SetUnderlyingBigEndianFlag(_underlyingIsBigEndian);
	Search::SetAlignment(_alignment);
}

void MungPlex::ProcessInformation::setupCheats()
{
	Cheats::InitCheatFile();
}

bool MungPlex::ProcessInformation::initMelonDS()
{
	setMiscProcessInfo("melonDS", false, false, 4, 4);

	for (const auto& region : GetRegionList())
	{
		if (region.GetRegionSize() != 0x10F0000)
			continue;

		_systemRegions[0].BaseLocationProcess = region.GetBaseAddress<void*>();
		char tempId[7] = "";
		_process.ReadExRAM(tempId, region.GetBaseAddress<char*>() + 0x3FFA8C, 6);
		_gameID = tempId;
		_gameName.resize(12);
		_process.ReadExRAM(_gameName.data(), region.GetBaseAddress<char*>() + 0x3FFA80, 12);


		if (_gameID.size() != 6)
		{
			_process.ReadExRAM(tempId, region.GetBaseAddress<char*>() + 0x3FFE0C, 6);
			_gameID = tempId;
			_process.ReadExRAM(_gameName.data(), region.GetBaseAddress<char*>() + 0x3FFE00, 12);
		}

		if (_gameID.size() != 6)
			continue;

		_gameRegion = _gameID[3];
		_platform = "NDS";
		PointerSearch::SelectPreset(NDS);
		return true;
	}

	return false;
}

bool MungPlex::ProcessInformation::initYuzu()
{
	setMiscProcessInfo("Yuzu", false, false, 8, 4);

	for (const auto& region : GetRegionList())
	{
		const uint64_t rSize = region.GetRegionSize();

		if (rSize != 0x100000000)
			continue;
		
		_systemRegions[0].BaseLocationProcess = region.GetBaseAddress<void*>();
		std::vector<uint64_t> dump(0x1000000/8);
		_process.ReadExRAM(dump.data(), _systemRegions[0].BaseLocationProcess, 0x1000000);

		//get title id
		//todo: append patch number to id
		//when games are closed and others are started IDs of previous games will remain in memory and cause retreival of wrong IDs
		for (int i = 0; i < 0x1000000 / 8; ++i)
		{
			if (dump[i] != 0xFFFFFFFF0000000)
				continue;

			if (dump[i - 1] != 7)
				continue;
				
			_gameID = ToHexString(dump[i - 25], 16, false);
			_platform = "Switch";
			PointerSearch::SelectPreset(SWITCH);
			return true;
		}
	}

	return false;
}

bool MungPlex::ProcessInformation::initMesen()
{
	setMiscProcessInfo("Mesen", false, false, 2, 1);
	_platform = "SNES";
	bool ramFound = false;
	bool romFound = false;
	char* RAM;
	uint64_t ROMflag = 0;

	for (Xertz::MemoryRegion& region : GetRegionList())
	{
		if (ramFound)
			break;

		if ((region.GetProtect() & PAGE_READWRITE) != PAGE_READWRITE)
			continue;

		int regionSize = region.GetRegionSize();

		if (regionSize < 0x100000 || regionSize > 0x1000000)
			continue;

		std::vector<uint64_t> buf(regionSize/8);
		_process.ReadExRAM(buf.data(), region.GetBaseAddress<void*>(), regionSize);

		for (int i = 0; i < regionSize/8; i += 2)
		{
			if (buf[i] != 0x78656C50676E754D)
				continue;
			
			//std::cout << "ram found!\n";
			RAM = region.GetBaseAddress<char*>() + i * 8 - 0xD0;
			ROMflag = buf[i + 1];
			ramFound = true;
			_systemRegions[0].BaseLocationProcess = reinterpret_cast<void*>(RAM);
			break;
		}
	}

	if (!ramFound)
		return false;

	for (Xertz::MemoryRegion& region : GetRegionList())
	{
		if (romFound)
			break;

		if ((region.GetProtect() & PAGE_READWRITE) != PAGE_READWRITE)
			continue;

		int regionSize = region.GetRegionSize();

		if (regionSize < 0x100000 || regionSize > 0x1000000)
			continue;

		uint64_t* buf = new uint64_t[regionSize / 8];
		_process.ReadExRAM(buf, region.GetBaseAddress<void*>(), regionSize);

		for (int i = 0; i < regionSize / 8; i += 2)
		{
			if (buf[i] != ROMflag)
				continue;
			
			//std::cout << "rom found!\n";
			RAM = region.GetBaseAddress<char*>() + i * 8;
			ROMflag = buf[i + 1];
			romFound = true;
			_systemRegions[1].BaseLocationProcess = reinterpret_cast<void*>(RAM);
			break;
		}
	}

	if (!romFound)
		return false;

	//todo JIS X 0201 encoding once MorphText supports it
	_gameID = std::string(21, 0);
	_process.ReadExRAM(_gameID.data(), RAM + 0x7FC0, 21);
	_gameID = MorphText::JIS_X_0201_FullWidth_To_Utf8(_gameID.data());
	_gameID = RemoveSpacePadding(_gameID);
	_gameName = _gameID;
	_gameID.append("-");
	char tempByte = 0;
	_process.ReadExRAM(&tempByte, RAM + 0x7FD9, 1);
	_gameID.append(std::to_string(tempByte)).append("-");
	_process.ReadExRAM(&tempByte, RAM + 0x7FDB, 1);
	_gameID.append(std::to_string(tempByte));
	PointerSearch::SelectPreset(SNES);

	return true;
}

bool MungPlex::ProcessInformation::initProject64()
{
	setMiscProcessInfo("Project64", true, true, 4, 4);
	bool found = false;

	for (const auto& region : GetRegionList())
	{
		const uint64_t rSize = region.GetRegionSize();

		if (!(rSize == 0x400000 || rSize == 0x800000))
			continue;

		uint32_t temp;
		_process.ReadExRAM(&temp, region.GetBaseAddress<char*>() + 8, 4);

		if (temp != 0x03400008)
			continue;

		_systemRegions[0].BaseLocationProcess = region.GetBaseAddress<void*>();
		found = true;
		break;
	}

	if (!found)
		return false;

	for (const auto& region : GetRegionList())
	{
		const uint64_t rSize = region.GetRegionSize();

		if (rSize < 0x400000)
			continue;
		
		uint64_t temp;
		_process.ReadExRAM(&temp, region.GetBaseAddress<char*>(), 8);

		if (temp != 0x0000000F80371240)
			continue;
		
		region.SetProtect(GetHandle(), PAGE_EXECUTE_READWRITE);
		_systemRegions[1].BaseLocationProcess = region.GetBaseAddress<void*>();
		_systemRegions[1].Size = rSize;
		found = true;
		_platform = "Nintendo 64";
		char tempID[5] = "";
		ReadFromReorderedRangeEx(_process, reinterpret_cast<uint32_t*>(tempID), region.GetBaseAddress<char*>() + 0x3B);
		_gameID = std::string(tempID);
		_gameName.resize(20);
		_gameRegion = _gameID[3];

		for (int i = 0; i <= 20; i+=4)
			ReadFromReorderedRangeEx(_process, reinterpret_cast<uint32_t*>(_gameName.data() + i), region.GetBaseAddress<char*>() + 0x20 + i);

		_gameName = RemoveSpacePadding(_gameName);
		PointerSearch::SelectPreset(N64);
		break;
	}
	
	return found;
}

bool MungPlex::ProcessInformation::initProcess(const std::wstring& processName)
{
	_process = Xertz::SystemInfo::GetProcessInfo(processName, true, false);

	if (_process.GetPID() == -1)
		return false;

	return true;
}

void MungPlex::ProcessInformation::setMiscProcessInfo(const std::string processName, const bool bigEndian, const bool rereorder, const int addressWidth, const int alignment)
{
	_processName = processName;
	_underlyingIsBigEndian = bigEndian;
	_addressWidth = addressWidth;
	_rereorderRegion = rereorder;
	_alignment = alignment;

}

bool MungPlex::ProcessInformation::initNo$psx()
{
	setMiscProcessInfo("No$psx", false, false, 4, 1);
	_platform = "PS1";
	PointerSearch::SelectPreset(PS1);

	for (const Xertz::MemoryRegion& region : GetRegionList())
	{
		if (region.GetRegionSize() != 0x459000)
			continue;

		_systemRegions[0].BaseLocationProcess = region.GetBaseAddress<char*>() + 0x30100;
		_gameID = std::string(12, 0);
		_process.ReadExRAM(_gameID.data(), region.GetBaseAddress<char*>() + 0x30100 + 0x00003A49, 11);
		return true;
	}

	return false;
}

bool MungPlex::ProcessInformation::initRpcs3()
{
	setMiscProcessInfo("Rpcs3", true, false, 4, 4);
	_platform = "PS3";
	PointerSearch::SelectPreset(PS3);

	_systemRegions[0].BaseLocationProcess = reinterpret_cast<void*>(0x300010000);
	_systemRegions[1].BaseLocationProcess = reinterpret_cast<void*>(0x330000000);
	_systemRegions[2].BaseLocationProcess = reinterpret_cast<void*>(0x340000000);
	_systemRegions[3].BaseLocationProcess = reinterpret_cast<void*>(0x350000000);

	uint32_t bufSize = 0x2000000;
	char* exeAddr = reinterpret_cast<char*>(_process.GetModuleAddress(L"rpcs3.exe")+0x2000000);
	std::vector<char> buf(bufSize);
	_process.ReadExRAM(buf.data(), exeAddr, bufSize);

	for (int offset = 0; offset < bufSize; ++offset)
	{
		if (*reinterpret_cast<uint64_t*>(&buf[offset]) != 0x227B3A2279746976)
			continue;

		_gameID = reinterpret_cast<char*>(&buf[offset + 16]);
		_gameID.resize(9);
		return true;
	}

	return false;
}

bool MungPlex::ProcessInformation::initPcsx2()
{
	setMiscProcessInfo("Pcsx2", false, false, 4, 4);
	_platform = "PS2";
	PointerSearch::SelectPreset(PS2);
	REGION_LIST& regions = GetRegionList();

	for (int i = 0; i < regions.size(); ++i)
	{
		jmp:
		if (regions[i].GetRegionSize() != 0x80000)
			continue;

		++i;
		for (int j = 0; j < 20; ++j)
			if (regions[i + j].GetRegionSize() != 0x1000)
			{
				++i;
				goto jmp;
			}

		for(int k = i; k < 0x1F80; ++k)
			if(regions[k].GetAllocationProtect() != PAGE_READWRITE)
				regions[k].SetProtect(GetHandle(), PAGE_READWRITE);

		_systemRegions[0].BaseLocationProcess = regions[i].GetBaseAddress<char*>();

		uint32_t bufSize = 0x1000000;
		uint64_t* exeAddr = reinterpret_cast<uint64_t*>(_process.GetModuleAddress(L"pcsx2-qt.exe"));
		std::vector<uint64_t> buf(bufSize / sizeof(uint64_t));
		_process.ReadExRAM(buf.data(), exeAddr, bufSize);

		for (int offset = 0; offset < bufSize / sizeof(uint64_t); ++offset)
		{
			if (buf[offset] != 0x6461655256413F2E)
				continue;

			_gameID = reinterpret_cast<char*>(&buf[offset + 7]);
			return true;
		}
	}

	return false;
}

bool MungPlex::ProcessInformation::initDolphin()
{
	setMiscProcessInfo("Dolphin", true, false, 4, 4);
	_systemRegions.erase(_systemRegions.begin() + 2); //--
	_systemRegions.erase(_systemRegions.begin() + 2); // |- remove these lines once caches and sram are figured out
	_systemRegions.erase(_systemRegions.begin() + 2); //--

	uint32_t temp = 0;
	uint32_t flagGCN = 0;
	uint32_t flagWii = 0;
	char tempID[7] = "";
	char discNo;
	char discVer;


	
	for (HWND wHandle : Xertz::SystemInfo::GetWindowHandleList())
	{
		LPSTR str = new CHAR[256];
		GetWindowTextA(wHandle, str, 256);
		std::string wTitle = str;
		delete[] str;

		if (wTitle.find("Dolphin") != 0)
			continue;

		int posEnd = wTitle.find("(");
		int posBeg = 0;

		if (posEnd < 0)
			continue;

		for (int i = posEnd; wTitle[i] != '|'; --i)
			posBeg = i;

		_gameName = wTitle.substr(posBeg + 1, posEnd - posBeg - 2);

		std::cout << _gameName << std::endl;
		break;
	}

	for (const auto& _region : GetRegionList())
	{
		if (_region.GetRegionSize() != 0x2000000)
			continue;

		_process.ReadExRAM(&temp, reinterpret_cast<void*>(_region.GetBaseAddress<uint64_t>() + 0x28), 4);
		_process.ReadExRAM(&flagGCN, reinterpret_cast<void*>(_region.GetBaseAddress<uint64_t>() + 0x18), 4);
		_process.ReadExRAM(&flagWii, reinterpret_cast<void*>(_region.GetBaseAddress<uint64_t>() + 0x1C), 4);

		if (temp != 0x8001)
			continue;

		_systemRegions[0].BaseLocationProcess = _region.GetBaseAddress<void*>();
		break;
	}

	_process.ReadExRAM(tempID, _systemRegions[0].BaseLocationProcess, 6);
	_process.ReadExRAM(&discNo, reinterpret_cast<char*>(_systemRegions[0].BaseLocationProcess) + 6, 1);
	_process.ReadExRAM(&discVer, reinterpret_cast<char*>(_systemRegions[0].BaseLocationProcess) + 7, 1);
	_gameID = tempID;
	_gameID.append("-").append(std::to_string(discNo));
	_gameID.append("-").append(std::to_string(discVer));
	_gameRegion = _gameID[3];

	if (flagGCN == 0xC2339F3D || (flagWii != 0 && flagGCN == 0))
	{
		_platform = "GameCube";
		PointerSearch::SelectPreset(GAMECUBE);
		_systemRegions.erase(_systemRegions.begin() + 1);
		return true;
	}

	for (const auto& _region : GetRegionList())
	{
		if (_region.GetRegionSize() != 0x4000000)
			continue;

		unsigned char temp;
		_process.ReadExRAM(&temp, reinterpret_cast<void*>(_region.GetBaseAddress<uint64_t>() + 1), 1);

		if (temp == 0x9f)
		{
			_systemRegions[1].BaseLocationProcess = _region.GetBaseAddress<void*>();
			break;
		}
	}

	int IDcopy;
	_process.ReadExRAM(&temp, _systemRegions[0].BaseLocationProcess, 4);
	_process.ReadExRAM(&IDcopy, static_cast<char*>(_systemRegions[0].BaseLocationProcess) + 0x3180, 4);
	_platform = "Wii";
	PointerSearch::SelectPreset(WII);

	if (temp == 0 && IDcopy != 0)
	{
		memcpy_s(tempID, 7, &IDcopy, 4);
		_gameID = tempID;
		_gameRegion = _gameID[3];
		return true;
	}

	if (IDcopy == temp)
		return true;

	return false;
}

bool MungPlex::ProcessInformation::initFusion()
{
	setMiscProcessInfo("Fusion", true, false, 4, 1);
	bool titleIDFound = false;
	bool romFound = false;
	uint64_t modAddr = _process.GetModuleAddress(L"Fusion.exe");
	uint16_t headerSize = 0x200;
	uint32_t romPTR = 0;
	uint32_t ramPTR = 0;
	const uint32_t AGES = 0x41474553;
	std::vector<uint32_t> romHeader(headerSize / sizeof(uint32_t));

	//look for SEGA CD BOOT ROM
	_process.ReadExRAM(&romPTR, reinterpret_cast<void*>(modAddr + 0x001C33C4), 4);
	_process.ReadExRAM(romHeader.data(), reinterpret_cast<void*>(romPTR), headerSize);

	if (romHeader[0x4A] == 0x544F4F42) //if sega cd found
	{
		_platform = "Mega-CD";
		_gameID = std::string(reinterpret_cast<char*>(&romHeader[0x60]));
		_gameID.resize(14);
		_systemRegions[0].BaseLocationProcess = reinterpret_cast<void*>(romPTR);
		_systemRegions[0].Size = Xertz::SwapBytes<uint32_t>(romHeader[0x69]) + 1;
		_systemRegions[0].Base = 0x0;
		_systemRegions[0].Label = "BOOT ROM";

		ramPTR = 0x006A52D4;
		_gameName.resize(48);
		_process.ReadExRAM(_gameName.data(), reinterpret_cast<void*>(ramPTR + 0x144), 48);
		_gameName = RemoveSpacePadding(_gameName);
		_process.ReadExRAM(&ramPTR, reinterpret_cast<void*>(ramPTR), 48);
		_systemRegions[1].BaseLocationProcess = reinterpret_cast<void*>(ramPTR);
		_systemRegions[1].Size = 0x10000;
		_systemRegions[1].Base = 0xFF0000;
		_gameID = _gameName + " - " + _gameID;

		return true;
	}

	//look for Mega Drive or 32x ROM
	romPTR = 0x007CFC00;
	uint32_t test = 0;
	_process.ReadExRAM(&test, reinterpret_cast<void*>(romPTR + 0x100), 4);
	
	if (test != AGES)//if not equal, fallback ROM header fetch
	{
		_process.ReadExRAM(&romPTR, reinterpret_cast<void*>(modAddr + 0x0005D8EC), 4);
		romPTR += 0x14;
	}

	_process.ReadExRAM(romHeader.data(), reinterpret_cast<void*>(romPTR), headerSize);

	if (romHeader[0x40] == AGES) //Mega Drive or 32x ROM found
	{
		if (romHeader[0x41] == 0x58323320) //32X ROM found
			_platform = "32X";
		else if ((romHeader[0x41] == 0x47454D20 && romHeader[0x42] == 0x52442041) || (romHeader[0x41] == 0x4E454720)) //mega drive ROM found
			_platform = "Mega Drive";
		else //something went wrong
			return false; 

		_gameID = std::string(reinterpret_cast<char*>(&romHeader[0x60]));
		_gameID.resize(14);
		_gameName = std::string(reinterpret_cast<char*>(&romHeader[0x54]));
		_gameName.resize(48);
		_gameName = RemoveSpacePadding(_gameName);
		std::string gameNameAlt = std::string(reinterpret_cast<char*>(&romHeader[0x48]));
		gameNameAlt.resize(48);
		gameNameAlt = RemoveSpacePadding(gameNameAlt);

		if (_gameName.compare(gameNameAlt) != 0)
			_gameName.append(" / " + gameNameAlt);

		_systemRegions[0].BaseLocationProcess = reinterpret_cast<void*>(romPTR);
		_systemRegions[0].Size = Xertz::SwapBytes<uint32_t>(romHeader[0x69]) + 1;
		_systemRegions[0].Base = 0x0;

		_process.ReadExRAM(&ramPTR, reinterpret_cast<char*>(0x00759F14), 4);
		_systemRegions[1].BaseLocationProcess = reinterpret_cast<void*>(ramPTR);
		_systemRegions[1].Size = 0x10000;
		_systemRegions[1].Base = 0xFF0000;

		return true;
	}

	return false;
}

bool MungPlex::ProcessInformation::initCemu()
{
	setMiscProcessInfo("Cemu", true, false, 4, 4);
	bool titleIDFound = false;
	_platform = "Wii U";
	PointerSearch::SelectPreset(WIIU);

	for (const auto& region : GetRegionList())
	{
		if (region.GetRegionSize() != 0x4E000000)
			continue;

		_systemRegions[0].BaseLocationProcess = region.GetBaseAddress<void*>();
		_systemRegions[1].BaseLocationProcess = reinterpret_cast<void*>(region.GetBaseAddress<char*>() + 0x0E000000);


		//old title id fetch code, just leave it there in case it may be needed again
		/*if (!(region.GetRegionSize() == 0x1E000 && !titleIDFound))
			continue;

		const int bufSize = 0x6000;
		std::vector<char> buf(bufSize);
		_process.ReadExRAM(buf.data(), region.GetBaseAddress<void*>(), bufSize);

		for (int offset = 0; offset < bufSize; offset += 4)
		{
			if (*reinterpret_cast<int*>(buf.data() + offset) != 0x746F6F52)
				continue;
			
			for (int i = 0x9C; i <= 0xA4; i += 4)
			{
				uint32_t tempID = *reinterpret_cast<uint32_t*>(buf.data() + offset + i);
				tempID = Xertz::SwapBytes<uint32_t>(tempID);

				if (i == 0x9C && tempID != 0x00050000)
					break;

				_gameID.append(ToHexString(tempID, 8, false));

				if(i <= 0xA0)
					_gameID.append("-");

				titleIDFound = true;
			}
		}*/
	}

	//get title type id, title id and version
	LPSTR str = new CHAR[128];
	for (HWND wHandle : Xertz::SystemInfo::GetWindowHandleList())
	{
		GetWindowTextA(wHandle, str, 128);
		std::string wTitle = str;

		if (wTitle.find("Cemu") != 0)
			continue;

		int pos = wTitle.find("TitleId: ");

		if (pos < 0)
			continue;

		wTitle = wTitle.substr(pos + 9);
		_gameID = wTitle.substr(0, 17);
		wTitle = wTitle.substr(19);
		pos = wTitle.find("[");
		_gameRegion = wTitle.substr(pos+1, 2);
		_gameName = wTitle.substr(0, pos - 1);
		wTitle = wTitle.substr(wTitle.find("v"));
		_gameID.append("-" + wTitle.substr(0, wTitle.size() - 1));
		std::cout << _gameID << std::endl;
		std::cout << _gameName << std::endl;
		delete[] str;
		return true;
	}

	delete[] str;
	return false;
}

bool MungPlex::ProcessInformation::initPPSSPP()
{
	setMiscProcessInfo("PPSSPP", false, false, 4, 4);
	bool titleIDFound = false;
	_platform = "PSP";
	PointerSearch::SelectPreset(PSP);

	for (Xertz::MemoryRegion& region : GetRegionList())
	{
		if (region.GetRegionSize() == 0x1f00000)
		{
			_systemRegions[0].BaseLocationProcess = region.GetBaseAddress<char*>() + 0x1000000;
			break;
		}
	}

	LPSTR str = new CHAR[64];
	for (HWND wHandle: Xertz::SystemInfo::GetWindowHandleList())
	{
		GetWindowTextA(wHandle, str, 64);
		std::string wTitle = str;

		if (wTitle.find("PPSSPP") != 0)
			continue;

		int pos = wTitle.find("-");
		_gameID = wTitle.substr(pos+2, 9);
		_gameName = wTitle.substr(pos + 14);
		//std::cout << _gameID << std::endl;
		//std::cout << _gameName << std::endl;
		delete[] str;
		return true;
	}

	delete[] str;
	return false;
}

std::string& MungPlex::ProcessInformation::GetGameID()
{
	return GetInstance()._gameID;
}

std::string& MungPlex::ProcessInformation::GetRpcGameID()
{
	return GetInstance()._rpcGameID;
}

void MungPlex::ProcessInformation::obtainGameEntities(void* baseLocation)
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
		_process.ReadExRAM(buffer, readLocation, size);

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
	if (!LoadSystemInformationJSON(emulatorIndex))
		return false;

	if (!GetInstance().initEmulator(emulatorIndex))
		return false;
	
	GetInstance()._exePath = GetInstance()._process.GetFilePath(); // refactor these two lines when implementing PC game support
	GetInstance()._processType = EMULATOR;

	std::string msg("Connected to ");
	msg.append(GetInstance()._processName + " (");
	msg.append(GetInstance()._platform + ") - Game ID: ");
	msg.append(GetInstance()._gameID);
	Log::LogInformation(msg.c_str());

	return true; 
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
	return GetInstance()._process.GetPID();
}

bool MungPlex::ProcessInformation::IsX64()
{
	return GetInstance()._process.IsX64();
}

bool MungPlex::ProcessInformation::UnderlyingIsBigEndian()
{
	return GetInstance()._underlyingIsBigEndian;
}

HANDLE MungPlex::ProcessInformation::GetHandle()
{
	return GetInstance()._process.GetHandle();
}

int32_t MungPlex::ProcessInformation::GetAddressWidth()
{
	return GetInstance()._addressWidth;
}

bool MungPlex::ProcessInformation::GetRereorderFlag()
{
	return GetInstance()._rereorderRegion;
}

void MungPlex::ProcessInformation::RefreshRegionlistPC()
{
	GetInstance()._systemRegions.clear();
	int flags = 0;

	if (GetInstance()._write || GetInstance()._execute)
		GetInstance()._read = true;

	if (GetInstance()._read)
		flags |= PAGE_READONLY;
	if (GetInstance()._write)
		flags |= PAGE_READWRITE;
	if(GetInstance()._execute)
		flags |= PAGE_EXECUTE_READ;
	if(GetInstance()._write && GetInstance()._execute)
		flags |= PAGE_EXECUTE_READWRITE;

	for (Xertz::MemoryRegion& region : GetInstance()._process.GetRegionList())
	{
		std::string label;

		if (region.GetProtect() & (flags) && region.GetRegionSize() > 0)
		{
			//std::cout << std::hex << region.GetBaseAddress<uint64_t>() << " - " << region.GetRegionSize() << std::endl;
			label = "R";
			//total += region.GetRegionSize();

			if (region.GetProtect() & (PAGE_EXECUTE_READWRITE | PAGE_READWRITE) & flags)
				label.append("W");

			if (region.GetProtect() & (PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE) & flags)
				label.append("E");

			GetInstance()._systemRegions.emplace_back(label, region.GetBaseAddress<uint64_t>(), region.GetRegionSize(), region.GetBaseAddress<void*>());
		}
	}
}

std::string& MungPlex::ProcessInformation::GetProcessName()
{
	return GetInstance()._processName;
}

std::string& MungPlex::ProcessInformation::GetTitle()
{
	return GetInstance()._gameName;
}

std::string& MungPlex::ProcessInformation::GetRegion()
{
	return GetInstance()._gameRegion;
}

std::string& MungPlex::ProcessInformation::GetPlatform()
{
	return GetInstance()._platform;
}

bool* MungPlex::ProcessInformation::GetRangeFlagRead()
{
	return &GetInstance()._read;
}

bool* MungPlex::ProcessInformation::GetRangeFlagWrite()
{
	return &GetInstance()._write;
}

bool* MungPlex::ProcessInformation::GetRangeFlagExecute()
{
	return &GetInstance()._execute;
}

int MungPlex::ProcessInformation::GetRegionIndex(const uint64_t baseAddress)
{
	for (size_t i = 0; i < GetInstance()._systemRegions.size(); ++i)
	{
		if (baseAddress >= GetInstance()._systemRegions[i].Base && baseAddress < GetInstance()._systemRegions[i].Base + GetInstance()._systemRegions[i].Size)
			return i;
	}

	return -1;
}

PROCESS_INFO& MungPlex::ProcessInformation::GetProcess()
{
	return GetInstance()._process;
}

MODULE_LIST& MungPlex::ProcessInformation::GetModuleList()
{
	return GetInstance()._process.GetModuleList();
}

REGION_LIST& MungPlex::ProcessInformation::GetRegionList()
{
	return GetInstance()._process.GetRegionList();
}

std::vector<MungPlex::SystemRegion>& MungPlex::ProcessInformation::GetSystemRegionList()
{
	return GetInstance()._systemRegions;
}