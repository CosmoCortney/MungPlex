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
			static std::string buf(256, '\0');

			if (column == 0)
			{
				sprintf(buf.data(), std::string(currentModule.begin(), currentModule.end()).c_str());
			}
			else
			{
				sprintf(buf.data(), "%llX", moduleAddress);
			}

			if (contents_type == CT_Text)
				ImGui::TextUnformatted(buf.data());
			else if (contents_type == CT_FillButton)
				ImGui::Button(buf.data(), ImVec2(-FLT_MIN, 0.0f));
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

bool MungPlex::ProcessInformation::LoadSystemInformationJSON(const std::string& system)
{
	//std::wstring emulator = GetInstance()._emulators[emulatorIndex].first;
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
		//std::string emuNameBasic(emulator.begin(), emulator.end());
		auto& regions = doc["Systems"][system]["Regions"];
		//auto& entities = doc["Emulators"][system]["Entities"];

		for (auto& region : regions)
		{
			auto label = region["Label"].get<std::string>();
			uint64_t base = std::stoll(region["Base"].get<std::string>(), 0, 0);
			uint64_t size = std::stoll(region["Size"].get<std::string>(), 0, 0);
			GetInstance()._systemRegions.emplace_back(SystemRegion(label, base, size));
		}

		/*for (auto& gameEntity : entities)
		{
			auto entity = gameEntity["Entity"].get<std::string>();
			int location = std::stoi(gameEntity["Location"].get<std::string>(), 0, 0);
			auto datatype = gameEntity["Datatype"].get<std::string>();
			int size = std::stoi(gameEntity["Size"].get<std::string>(), 0, 0);
			bool hex = gameEntity["Hex"].get<bool>();
			GetInstance()._gameEntities.emplace_back(GameEntity(entity, location, datatype, size, hex));
		}*/

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
	_rpcGameID.clear();

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
		case VBA:
			connected = initVBA();
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

	if (!connected)
		return false;

	_platform = GetSystemNameByID(_platformID);
	PointerSearch::SelectPreset(_platformID);
	setupSearch();
	setupCheats();
	WatchControl::InitWatchFile();
	obtainGameEntities(_systemRegions[0].BaseLocationProcess);
	return true;
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

bool MungPlex::ProcessInformation::initVBA()
{
	uint64_t regionSize;
	bool romFound = false;
	//enum type { GB = 1, GBC, GBA };
	uint64_t bufLogo = 0;
	std::vector<uint8_t> bufRegion(0x8000000);
	uint8_t* romBasePtr = nullptr;
	char* mainMemoryRegion = nullptr;
	bool makeThisTrueLater = false;
	char* moduleAddr = reinterpret_cast<char*>(_process.GetModuleAddress(L"visualboyadvance-m.exe") + 0x3000000);
	char* identifierAddr = nullptr;
	std::vector<uint64_t> moduleBuf(0x2000000 / 8);
	_process.ReadMemoryFast(moduleBuf.data(), moduleAddr, 0x2000000);

	for (const auto& region : GetRegionList())
	{
		regionSize = region.GetRegionSize();
		romBasePtr = region.GetBaseAddress<uint8_t*>();

		if (regionSize == 0x2001000 && (region.GetProtect() & PAGE_READWRITE) == PAGE_READWRITE)
		{
			_process.ReadMemoryFast(&bufLogo, romBasePtr +0x44, 8);

			if (bufLogo != 0x21A29A6951AEFF24)
				continue;

			setMiscProcessInfo("VisualBoyAdvace", false, false, 4, 1);
			LoadSystemInformationJSON("GBA");
			_systemRegions[7].BaseLocationProcess = romBasePtr + 0x40;
			_platformID = GBA;

			//ToDo: Find WRAM pointer by comparing the region size it is stored in. Check if 0x323CAB23 is a valid marker

			for (int j = 0; j < moduleBuf.size(); ++j)
			{
				if (moduleBuf[j] != 0x0C6E0C6D0C6D0C6C)
					continue;

				identifierAddr = j * 8 + moduleAddr;
				uint64_t tempPtr = 0;
				_process.ReadMemoryFast(&tempPtr, identifierAddr - 0x78, 8);
				_systemRegions[0].BaseLocationProcess = reinterpret_cast<void*>(tempPtr);
				break;
			}




			std::cout << "type: " << _platformID << " " << _systemRegions[0].BaseLocationProcess << std::endl;
			return true;
		}
		else if ((regionSize >= 0x1000 && regionSize < 0x8000000) && (region.GetProtect() & PAGE_READWRITE) == PAGE_READWRITE) //ToDo: Ensure this is future-proof
		{
			_process.ReadMemoryFast(bufRegion.data(), romBasePtr, 0x8000000);
			uint8_t* bufPtr = bufRegion.data();

			for (int i = 4; i < regionSize; i += 0x10)
			{
				if (*reinterpret_cast<uint64_t*>(bufPtr + i) != 0x0B000DCC6666EDCE)
					continue;

				setMiscProcessInfo("VisualBoyAdvace", false, false, 2, 1);
				uint8_t* typePtr = bufPtr + i + 0x3F;

				if (*typePtr == 0x80 || *typePtr == 0xC0)
				{
					LoadSystemInformationJSON("GBC");
					_platformID = GBC;
				}
				else
				{
					LoadSystemInformationJSON("GB");
					_platformID = GB;
				}

				_systemRegions[0].BaseLocationProcess = romBasePtr + i - 0x104;

				for (int j = 0; j < moduleBuf.size(); ++j)
				{
					if (moduleBuf[j] != 0x0C6E0C6D0C6D0C6C)
						continue;

					identifierAddr = j * 8 + moduleAddr;
					uint64_t tempPtr = 0;
					_process.ReadMemoryFast(&tempPtr, identifierAddr - 0x98, 8);
					_systemRegions[1].BaseLocationProcess = reinterpret_cast<void*>(tempPtr);
					_process.ReadMemoryFast(&tempPtr, identifierAddr - 0x78, 8);
					_systemRegions[2].BaseLocationProcess = reinterpret_cast<void*>(tempPtr);
					_process.ReadMemoryFast(&tempPtr, identifierAddr - 0x68, 8);
					_systemRegions[3].BaseLocationProcess = reinterpret_cast<void*>(tempPtr);
					_process.ReadMemoryFast(&tempPtr, identifierAddr - 0x58, 8);
					_systemRegions[4].BaseLocationProcess = reinterpret_cast<void*>(tempPtr);
					_process.ReadMemoryFast(&tempPtr, identifierAddr - 0x50, 8);
					_systemRegions[5].BaseLocationProcess = reinterpret_cast<void*>(tempPtr);
					_systemRegions[6].BaseLocationProcess = reinterpret_cast<void*>(tempPtr + 0x2F00);
					break;
				}
				


				std::cout << "type: " << _platformID << " " << _systemRegions[0].BaseLocationProcess << std::endl;
				return true;
			}
		}
	}

	return false;
}

bool MungPlex::ProcessInformation::initMelonDS()
{
	setMiscProcessInfo("melonDS", false, false, 4, 4);
	bool romFound = false;
	static const uint64_t romFlag = 0x21A29A6951AEFF24;
	//find ROM
	for (const auto& region : GetRegionList())
	{
		if (region.GetRegionSize() < 0x800000 || (region.GetProtect() & PAGE_READWRITE) != PAGE_READWRITE)
			continue;

		char* mainMemoryRegion = region.GetBaseAddress<char*>();
		std::vector<uint8_t> buf(region.GetRegionSize());
		_process.ReadMemoryFast(buf.data(), region.GetBaseAddress<void*>(), region.GetRegionSize());

		for (int i = 0; i < 0x200; i+=8)
		{
			if (*reinterpret_cast<uint64_t*>(&buf[i]) != romFlag)
				continue;

			_connectionCheckPtr = reinterpret_cast<void*>(mainMemoryRegion + i);
			_connectionCheckValue = romFlag;
			const uint32_t romBase = i - 0xC0;
			_rpcGameID = _gameID = reinterpret_cast<char*>(&buf[romBase + 0xC]);
			_gameRegion = getRegionFromNintendoRegionCode(_gameID[3]);
			uint32_t titleOffset = *reinterpret_cast<uint32_t*>(&buf[romBase + 0x68]);

			if (_gameID[3] == 'J')
				titleOffset += 0x240;
			else
				titleOffset += 0x340;

			std::wstring tempTitle;

			for (int ch = 0; *reinterpret_cast<wchar_t*>(&buf[romBase + titleOffset + ch]) != 0x000A; ch += 2)
			{
				tempTitle += *reinterpret_cast<wchar_t*>(&buf[romBase + titleOffset + ch]);
			}

			_gameName = MT::Convert<std::wstring, std::string>(tempTitle, MT::UTF16LE, MT::UTF8);
			romFound = true;
			break;
		}

		if (romFound)
			break;
	}

	//find RAM
	for (const auto& region : GetRegionList())
	{
		if (region.GetRegionSize() != 0x10F0000)
			continue;

		LoadSystemInformationJSON("NDS");
		_systemRegions[0].BaseLocationProcess = region.GetBaseAddress<void*>();
		_platformID = NDS;
		return true;
	}

	return false;
}

bool MungPlex::ProcessInformation::initYuzu()
{
	setMiscProcessInfo("Yuzu", false, false, 8, 4);
	std::wstring wTitleBuf(512, L'\0');
	std::wstring gTitle;
	std::wstring version;

	//get title
	for (HWND wHandle : Xertz::SystemInfo::GetWindowHandleList())
	{
		GetWindowTextW(wHandle, wTitleBuf.data(), 512);

		if (wTitleBuf.find(L"yuzu") == std::wstring::npos)
			continue;

		if (std::ranges::count(wTitleBuf, '|') < 2)
			continue;

		int posStart = wTitleBuf.find(L"|");
		wTitleBuf = wTitleBuf.substr(posStart+1);
		int posEnd = wTitleBuf.find(L"(");
		gTitle = wTitleBuf.substr(0, posEnd);

		while (gTitle.front() == 0x0020)
			gTitle = gTitle.substr(1, gTitle.size());

		while (gTitle.back() == 0x0020)
			gTitle = gTitle.substr(0, gTitle.size()-1);

		_gameName = MT::Convert<std::wstring, std::string>(gTitle, MT::UTF16LE, MT::UTF8);
		//std::cout << _gameName << std::endl;

		posStart = wTitleBuf.find(L"|");
		version = wTitleBuf.substr(posStart + 1);
		posEnd = version.find(L"|");
		version = version.substr(0, posEnd);

		while (version.front() == 0x0020)
			version = version.substr(1, version.size());

		while (version.back() == 0x0020)
			version = version.substr(0, version.size() - 1);

		break;
	}

	//get title id
	bool idFound = false;
	for (const auto& region : GetRegionList())
	{
		uint64_t regionSize = region.GetRegionSize();
		if (regionSize < 0x20000 || regionSize >= 0x200000)
			continue;

		std::vector<uint8_t> buf(regionSize);
		_process.ReadMemoryFast(buf.data(), region.GetBaseAddress<void*>(), regionSize);

		for (int i = 0; i < regionSize; i += 4)
		{
			if (*reinterpret_cast<uint64_t*>(&buf[i]) != *reinterpret_cast<uint64_t*>(gTitle.data()))
				continue;

			for (int prefixOffset = 0; prefixOffset < 0x100; ++prefixOffset)
			{
				if (buf.size() <= i + prefixOffset + 4)
					break;

				static int prependSize = 0;

				if (*reinterpret_cast<uint32_t*>(&buf[i + prefixOffset]) == 0x00780030)
					prependSize = 4;
				else if(*reinterpret_cast<uint16_t*>(&buf[i + prefixOffset]) == 0x0028)
					prependSize = 2;
				else
					continue;

				std::wstring tempID = reinterpret_cast<wchar_t*>(&buf[i + prefixOffset + prependSize]);
				_connectionCheckValue = *reinterpret_cast<int*>(tempID.data());
				_connectionCheckPtr = region.GetBaseAddress<char*>() + i + prefixOffset + prependSize;

				if (prependSize == 2)
					tempID = tempID.substr(0, tempID.find(L')'));

				if (tempID.size() != 16)
					continue;

				idFound = true;
				_rpcGameID = _gameID = MT::Convert<std::wstring, std::string>(tempID + L"-" + version, MT::UTF16LE, MT::UTF8);
				break;
			}

			if (!idFound)
				continue;

			break;
		}

		if (idFound)
			break;
	}

	//get RAM
	for (const auto& region : GetRegionList())
	{
		const uint64_t rSize = region.GetRegionSize();

		if (rSize != 0x100000000)
			continue;
		
		_systemRegions[0].BaseLocationProcess = region.GetBaseAddress<void*>();
		_platformID = SWITCH;
		return true;
	}

	return false;
}

bool MungPlex::ProcessInformation::initMesen()
{
	setMiscProcessInfo("Mesen", false, false, 2, 1);
	bool ramFound = false;
	bool romFound = false;
	char* RAM;
	uint64_t ROMflag = 0;
	static const uint64_t ramFlag = 0x78656C50676E754D;

	for (Xertz::MemoryRegion& region : GetRegionList())
	{
		if (ramFound)
			break;

		if ((region.GetProtect() & PAGE_READWRITE) != PAGE_READWRITE)
			continue;

		int regionSize = region.GetRegionSize();

		if (regionSize < 0x10000 || regionSize > 0x1000000)
			continue;

		std::vector<uint64_t> buf(regionSize/8);
		char* mainMemoryRegion = region.GetBaseAddress<char*>();
		_process.ReadMemoryFast(buf.data(), mainMemoryRegion, regionSize);

		for (int i = 0; i < regionSize/8; i += 2)
		{
			if (buf[i] != ramFlag)
				continue;
			
			//std::cout << "ram found!\n";
			RAM = mainMemoryRegion + i * 8 - 0xD0;
			ROMflag = buf[i + 1];
			ramFound = true;
			LoadSystemInformationJSON("SNES");
			_systemRegions[0].BaseLocationProcess = reinterpret_cast<void*>(RAM);
			_connectionCheckValue = ramFlag;
			_connectionCheckPtr = mainMemoryRegion + i * 8;
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
		_process.ReadMemoryFast(buf, region.GetBaseAddress<void*>(), regionSize);

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
	_process.ReadMemoryFast(_gameID.data(), RAM + 0x7FC0, 21);
	_gameID = MT::Convert<char*, std::string>(_gameID.data(), MT::JIS_X_0201_FULLWIDTH, MT::UTF8);
	_gameID = RemoveSpacePadding(_gameID, false);
	_gameName = _gameID;
	_gameID.append("-");
	char tempByte = 0;
	_process.ReadMemoryFast(&tempByte, RAM + 0x7FD9, 1);
	_gameID.append(std::to_string(tempByte)).append("-");
	_process.ReadMemoryFast(&tempByte, RAM + 0x7FDB, 1);
	_gameID.append(std::to_string(tempByte));
	_platformID = SNES;

	return true;
}

bool MungPlex::ProcessInformation::initProject64()
{
	setMiscProcessInfo("Project64", true, true, 4, 4);
	bool found = false;
	static const uint64_t romFlag = 0x0000000F80371240;

	for (const auto& region : GetRegionList())
	{
		const uint64_t rSize = region.GetRegionSize();

		if (!(rSize == 0x400000 || rSize == 0x800000))
			continue;

		uint32_t temp;
		char* mainMemoryRegion = region.GetBaseAddress<char*>();
		_process.ReadMemoryFast(&temp, mainMemoryRegion + 8, 4);

		if (temp != 0x03400008)
			continue;

		LoadSystemInformationJSON("N64");
		_systemRegions[0].BaseLocationProcess = reinterpret_cast<void*>(mainMemoryRegion);
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
		
		char* romRegion = region.GetBaseAddress<char*>();
		uint64_t temp;
		_process.ReadMemoryFast(&temp, romRegion, 8);

		if (temp != romFlag)
			continue;
		
		_connectionCheckValue = romFlag;
		_connectionCheckPtr = reinterpret_cast<void*>(romRegion);
		region.SetProtect(GetHandle(), PAGE_EXECUTE_READWRITE);
		_systemRegions[1].BaseLocationProcess = region.GetBaseAddress<void*>();
		_systemRegions[1].Size = rSize;
		found = true;
		char tempID[5] = "";
		ReadFromReorderedRangeEx(_process, reinterpret_cast<uint32_t*>(tempID), romRegion + 0x3B);
		_rpcGameID = _gameID = std::string(tempID);
		_gameName.resize(20);
		_gameRegion = getRegionFromNintendoRegionCode(_gameID[3]);

		for (int i = 0; i <= 20; i+=4)
			ReadFromReorderedRangeEx(_process, reinterpret_cast<uint32_t*>(_gameName.data() + i), romRegion + 0x20 + i);

		_gameName = RemoveSpacePadding(_gameName);
		_platformID = N64;
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
	static const uint64_t ramFlag = 0x696C6F626D795300;

	for (const Xertz::MemoryRegion& region : GetRegionList())
	{
		if (region.GetRegionSize() != 0x459000)
			continue;

		char* mainMemoryRegion = region.GetBaseAddress<char*>();
		LoadSystemInformationJSON("PS1");
		_systemRegions[0].BaseLocationProcess = mainMemoryRegion + 0x30100;
		_gameID = std::string(12, 0);
		_process.ReadMemoryFast(_gameID.data(), mainMemoryRegion + 0x30100 + 0x00003A49, 11);
		_rpcGameID = _gameID = _gameID.c_str();
		char tempRegion[32];
		_process.ReadMemoryFast(tempRegion, mainMemoryRegion + 0x30100 + 0x00003BE5, 32);
		_gameRegion = tempRegion;
		_gameRegion = _gameRegion.substr(0, _gameRegion.find(" area"));
		void* exeAddr = reinterpret_cast<void*>(_process.GetModuleAddress(L"NO$PSX.EXE"));
		std::vector<char> buf(0x100000);
		_process.ReadMemoryFast(buf.data(), exeAddr, 0x100000);

		for (int i = 0; i < 0x100000; i += 4)
		{
			if (*reinterpret_cast<uint64_t*>(&buf[i]) != ramFlag)
				continue;

			_gameName = &buf[i - 0x10C];
			_connectionCheckValue = *reinterpret_cast<int*>(&buf[i - 0x10C]);
			_connectionCheckPtr = reinterpret_cast<char*>(exeAddr) + i - 0x10C;
			break;
		}

		_gameName = _gameName.substr(0, _gameName.find(".SYM"));
		_platformID = PS1;
		return true;
	}

	return false;
}

bool MungPlex::ProcessInformation::initRpcs3()
{
	setMiscProcessInfo("Rpcs3", true, false, 4, 4);
	LoadSystemInformationJSON("PS3");
	_systemRegions[0].BaseLocationProcess = reinterpret_cast<void*>(0x300010000);
	_systemRegions[1].BaseLocationProcess = reinterpret_cast<void*>(0x330000000);
	_systemRegions[2].BaseLocationProcess = reinterpret_cast<void*>(0x340000000);
	_systemRegions[3].BaseLocationProcess = reinterpret_cast<void*>(0x350000000);

	uint32_t bufSize = 0x1000000;
	char* exeAddr = reinterpret_cast<char*>(_process.GetModuleAddress(L"rpcs3.exe")+0x5000000);
	std::vector<char> buf(bufSize);
	_process.ReadMemoryFast(buf.data(), exeAddr, bufSize);
	bool gameIdFound = false;
	std::wstring gIdWindow;

	for (int offset = 0; offset < bufSize; offset += 4)
	{
		if (*reinterpret_cast<uint64_t*>(&buf[offset]) != 0x454D41475F335350)
			continue;

		_rpcGameID = _gameID = reinterpret_cast<char*>(&buf[offset - 0xE0]);
		_gameID.resize(9);
		gIdWindow = MT::Convert<std::string, std::wstring>(_gameID, MT::UTF8, MT::UTF16LE);
		_rpcGameID = _gameID.append("-[" + std::string(reinterpret_cast<char*>(&buf[offset - 0xA0])) + "]");
		gameIdFound = true;
		_connectionCheckValue = *reinterpret_cast<int*>(&buf[offset - 0xDC]);
		_connectionCheckPtr = exeAddr + offset - 0xDC;
	}

	if (!gameIdFound)
		return false;

	std::wstring gTitle;
	std::wstring wTitleBuf(512, L'\0');
	//get title
	for (HWND wHandle : Xertz::SystemInfo::GetWindowHandleList())
	{
		GetWindowTextW(wHandle, wTitleBuf.data(), 512);

		if (wTitleBuf.find(gIdWindow) == std::wstring::npos)
			continue;

		int posStart = wTitleBuf.find_last_of(L"|");
		wTitleBuf = wTitleBuf.substr(posStart + 1);
		int posEnd = wTitleBuf.find(L"[");
		gTitle = wTitleBuf.substr(0, posEnd);

		while (gTitle.front() == 0x0020)
			gTitle = gTitle.substr(1, gTitle.size());

		while (gTitle.back() == 0x0020)
			gTitle = gTitle.substr(0, gTitle.size() - 1);

		_gameName = MT::Convert<std::wstring, std::string>(gTitle, MT::UTF16LE, MT::UTF8);
		_platformID = PS3;
		return true;
	}

	return false;
}

bool MungPlex::ProcessInformation::initPcsx2()
{
	setMiscProcessInfo("Pcsx2", false, false, 4, 4);
	REGION_LIST& regions = GetRegionList();
	bool idFound = false;
	static const uint64_t ramFlag = 0x6461655256413F2E;

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

		LoadSystemInformationJSON("PS2");
		_systemRegions[0].BaseLocationProcess = regions[i].GetBaseAddress<char*>();

		uint32_t bufSize = 0x1000000;
		uint64_t* exeAddr = reinterpret_cast<uint64_t*>(_process.GetModuleAddress(L"pcsx2-qt.exe"));
		std::vector<uint64_t> buf(bufSize / sizeof(uint64_t));
		_process.ReadMemoryFast(buf.data(), exeAddr, bufSize);

		for (int offset = 0; offset < bufSize / sizeof(uint64_t); ++offset)
		{
			if (buf[offset] != ramFlag)
				continue;

			_rpcGameID = _gameID = reinterpret_cast<char*>(&buf[offset + 7]);
			idFound = true;
			_connectionCheckValue = *reinterpret_cast<int*>(&buf[offset + 7]);
			_connectionCheckPtr = exeAddr + offset + 7;
		}
	}

	if (!idFound)
		return false;

	//get title
	DWORD pid;
	std::wstring wTitleBuf(512, L'\0');

	for (HWND wHandle : Xertz::SystemInfo::GetWindowHandleList())
	{
		GetWindowThreadProcessId(wHandle, &pid);

		if (pid != _process.GetPID())
			continue;

		GetWindowTextW(wHandle, wTitleBuf.data(), 512);
		_gameName = MT::Convert<wchar_t*, std::string>(wTitleBuf.c_str(), MT::UTF16LE, MT::UTF8);
		_platformID = PS2;
		return true;
	}

	return false;
}

bool MungPlex::ProcessInformation::initDolphin()
{
	setMiscProcessInfo("Dolphin", true, false, 4, 4);
	uint32_t temp = 0;
	uint32_t flagGCN = 0;
	uint32_t flagWii = 0;
	char tempID[7] = "";
	char discNo;
	char discVer;
	std::wstring wTitleBuf(512, L'\0');

	for (HWND wHandle : Xertz::SystemInfo::GetWindowHandleList())
	{
		GetWindowTextW(wHandle, wTitleBuf.data(), 512);

		if (wTitleBuf.find(L"Dolphin") != 0)
			continue;

		int posEnd = wTitleBuf.find(L"(");
		int posBeg = 0;

		if (posEnd < 0)
			continue;

		for (int i = posEnd; wTitleBuf[i] != '|'; --i)
			posBeg = i;

		const std::wstring wTitle = wTitleBuf.substr(posBeg + 1, posEnd - posBeg - 2);
		_gameName = MT::Convert<std::wstring, std::string>(wTitle, MT::UTF16LE, MT::UTF8);
		//std::cout << _gameName << std::endl;
		break;
	}

	bool memoryFound = false;
	char* mainMemRegion = nullptr;

	for (const auto& _region : GetRegionList())
	{
		if (_region.GetRegionSize() != 0x2000000)
			continue;

		memoryFound = true;
		mainMemRegion = _region.GetBaseAddress<char*>();;
		_process.ReadMemoryFast(&temp, mainMemRegion + 0x28, 4);
		_process.ReadMemoryFast(&flagGCN, mainMemRegion + 0x18, 4);
		_process.ReadMemoryFast(&flagWii, mainMemRegion + 0x1C, 4);

		if (temp != 0x8001)
			continue;

		_connectionCheckPtr = _region.GetBaseAddress<void*>();
		_process.ReadMemoryFast(&_connectionCheckValue, _connectionCheckPtr, 4);
		break;
	}

	if (!memoryFound)
		return false;

	_process.ReadMemoryFast(tempID, mainMemRegion, 6);
	_process.ReadMemoryFast(&discNo, mainMemRegion + 6, 1);
	_process.ReadMemoryFast(&discVer, mainMemRegion + 7, 1);
	_gameID = _rpcGameID = tempID;
	_gameID.append("-").append(std::to_string(discNo));
	_gameID.append("-").append(std::to_string(discVer));
	_gameRegion = getRegionFromNintendoRegionCode(_gameID[3]);

	if (flagGCN == 0xC2339F3D || (flagWii != 0 && flagGCN == 0))
	{
		//Add checks for Triforce games
		_platformID = GAMECUBE;
		LoadSystemInformationJSON("GameCube");
		_systemRegions[0].BaseLocationProcess = reinterpret_cast<void*>(mainMemRegion);
		return true;
	}

	for (const auto& _region : GetRegionList())
	{
		if (_region.GetRegionSize() != 0x4000000)
			continue;

		unsigned char temp;
		_process.ReadMemoryFast(&temp, reinterpret_cast<void*>(_region.GetBaseAddress<uint64_t>() + 1), 1);

		if (temp == 0x9f)
		{
			_systemRegions[1].BaseLocationProcess = _region.GetBaseAddress<void*>();
			break;
		}
	}

	int IDcopy;
	_process.ReadMemoryFast(&temp, mainMemRegion, 4);
	_process.ReadMemoryFast(&IDcopy, mainMemRegion + 0x3180, 4);
	_platformID = WII;
	LoadSystemInformationJSON("Wii");

	if (temp == 0 && IDcopy != 0)
	{
		memcpy_s(tempID, 7, &IDcopy, 4);
		_rpcGameID = _gameID = tempID;
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
	static const uint32_t AGES = 0x41474553;
	std::vector<uint32_t> romHeader(headerSize / sizeof(uint32_t));

	//look for SEGA CD BOOT ROM
	_process.ReadMemoryFast(&romPTR, reinterpret_cast<void*>(modAddr + 0x001C33C4), 4);
	_process.ReadMemoryFast(romHeader.data(), reinterpret_cast<void*>(romPTR), headerSize);

	if (romHeader[0x4A] == 0x544F4F42) //if sega cd found
	{
		_connectionCheckValue = 0x544F4F42;
		_connectionCheckPtr = reinterpret_cast<char*>(romPTR) + 0x4A * 4;
		_gameID = std::string(reinterpret_cast<char*>(&romHeader[0x60]));
		_gameID.resize(14);
		LoadSystemInformationJSON("MegaCD");
		_systemRegions[0].BaseLocationProcess = reinterpret_cast<void*>(romPTR);
		_systemRegions[0].Size = Xertz::SwapBytes<uint32_t>(romHeader[0x69]) + 1;
		_systemRegions[0].Base = 0x0;

		ramPTR = 0x006A52D4;
		_gameName.resize(48);
		_process.ReadMemoryFast(_gameName.data(), reinterpret_cast<void*>(ramPTR + 0x144), 48);
		_gameName = RemoveSpacePadding(_gameName);
		_process.ReadMemoryFast(&ramPTR, reinterpret_cast<void*>(ramPTR), 4);
		_systemRegions[1].BaseLocationProcess = reinterpret_cast<void*>(ramPTR);
		_systemRegions[1].Size = 0x10000;
		_systemRegions[1].Base = 0xFF0000;
		_rpcGameID = "#";
		_gameID = _gameName + " - " + _gameID;
		_platformID = SMCD;
		return true;
	}

	//look for Mega Drive or 32x ROM
	romPTR = 0x007CFC00;
	uint32_t test = 0;
	_process.ReadMemoryFast(&test, reinterpret_cast<void*>(romPTR + 0x100), 4);
	
	if (test != AGES)//if not equal, fallback ROM header fetch
	{
		_process.ReadMemoryFast(&romPTR, reinterpret_cast<void*>(modAddr + 0x0005D8EC), 4);
		romPTR += 0x14;
	}

	_process.ReadMemoryFast(romHeader.data(), reinterpret_cast<void*>(romPTR), headerSize);

	if (romHeader[0x40] == AGES) //Mega Drive or 32x ROM found
	{
		if (romHeader[0x41] == 0x58323320) //32X ROM found
		{
			LoadSystemInformationJSON("32X");
			_platformID = S32X;
		}
		else if ((romHeader[0x41] == 0x47454D20 && romHeader[0x42] == 0x52442041) || (romHeader[0x41] == 0x4E454720)) //mega drive ROM found
		{
			LoadSystemInformationJSON("MegaDrive");
			_platformID = GENESIS;
		}
		else //something went wrong
			return false; 

		_connectionCheckValue = *reinterpret_cast<int*>(&romHeader[0x41]);
		_connectionCheckPtr = reinterpret_cast<char*>(romPTR) + 0x41*4;
		_gameID = std::string(reinterpret_cast<char*>(&romHeader[0x60]));
		_gameID.resize(14);
		_rpcGameID = _gameID;
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

		_process.ReadMemoryFast(&ramPTR, reinterpret_cast<char*>(0x00759F14), 4);
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

	for (const auto& region : GetRegionList())
	{
		if (region.GetRegionSize() != 0x4E000000)
			continue;

		LoadSystemInformationJSON("WiiU");
		char* mainBase = region.GetBaseAddress<char*>();
		_systemRegions[0].BaseLocationProcess = reinterpret_cast<void*>(mainBase);
		_systemRegions[1].BaseLocationProcess = reinterpret_cast<void*>(mainBase + 0x0E000000);


		//old title id fetch code, just leave it there in case it may be needed again
		/*if (!(region.GetRegionSize() == 0x1E000 && !titleIDFound))
			continue;

		const int bufSize = 0x6000;
		std::vector<char> buf(bufSize);
		_process.ReadMemoryFast(buf.data(), region.GetBaseAddress<void*>(), bufSize);

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
	std::wstring wTitleBuf(512, L'\0');

	for (HWND wHandle : Xertz::SystemInfo::GetWindowHandleList())
	{
		GetWindowTextW(wHandle, wTitleBuf.data(), 512);

		if (wTitleBuf.find(L"Cemu") == std::wstring::npos)
			continue;

		int pos = wTitleBuf.find(L"TitleId: ");

		if (pos == std::wstring::npos)
			continue;

		std::wstring wTitle = wTitleBuf.substr(pos + 9);
		_gameID = MT::Convert<std::wstring, std::string>(wTitle.substr(0, 17), MT::UTF16LE, MT::UTF8);
		wTitle = wTitle.substr(19);
		pos = wTitle.find(L"[");
		_gameRegion = MT::Convert<std::wstring, std::string>(wTitle.substr(pos+1, 2), MT::UTF16LE, MT::UTF8);
		_gameName = MT::Convert<std::wstring, std::string>(wTitle.substr(0, pos - 1), MT::UTF16LE, MT::UTF8);
		wTitle = wTitle.substr(wTitle.find(L"v"));
		_rpcGameID = _gameID.append("-" + MT::Convert<std::wstring, std::string>(wTitle.substr(0, wTitle.find(L"]"))), MT::UTF16LE, MT::UTF8);
		//std::cout << _gameID << std::endl;
		//std::cout << _gameName << std::endl;
		_platformID = WIIU;
		return true;
	}

	return false;
}

bool MungPlex::ProcessInformation::initPPSSPP()
{
	setMiscProcessInfo("PPSSPP", false, false, 4, 4);
	bool titleIDFound = false;

	for (Xertz::MemoryRegion& region : GetRegionList())
	{
		if (region.GetRegionSize() == 0x1f00000)
		{
			LoadSystemInformationJSON("PSP");
			char* mainMemoryRegion = region.GetBaseAddress<char*>();
			_systemRegions[0].BaseLocationProcess = reinterpret_cast<void*>(mainMemoryRegion + 0x1000000);
			break;
		}
	}

	std::wstring wTitleBuf(512, L'\0');
	for (HWND wHandle: Xertz::SystemInfo::GetWindowHandleList())
	{
		GetWindowTextW(wHandle, wTitleBuf.data(), 512);

		if (wTitleBuf.find(L"PPSSPP") == std::wstring::npos)
			continue;

		int pos = wTitleBuf.find(L"-");
		_rpcGameID = _gameID = MT::Convert<std::wstring, std::string>(wTitleBuf.substr(pos + 2, 9), MT::UTF16LE, MT::UTF8);
		_gameName = MT::Convert<std::wstring, std::string>(wTitleBuf.substr(pos + 14), MT::UTF16LE, MT::UTF8);
		//std::cout << _gameID << std::endl;
		//std::cout << _gameName << std::endl;
		_platformID = PSP;
		return true;
	}

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

		void* readLocation = static_cast<char*>(baseLocation) + Location;
		_process.ReadMemoryFast(buffer, readLocation, size);

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
	//if (!LoadSystemInformationJSON(emulatorIndex))
		//return false;

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

const std::vector<EMUPAIR>& MungPlex::ProcessInformation::GetEmulatorList()
{
	return _emulators;
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

std::string MungPlex::ProcessInformation::getRegionFromNintendoRegionCode(const char code) const
{
	switch (code)
	{
	case 'A':
		return "Any";
	case 'C':
		return "China";
	case 'D':
		return "Germany";
	case 'E':
		return "USA";
	case 'F':
		return "France";
	case 'H':
		return "Netherlands";
	case 'I':
		return "Italy";
	case 'J':
		return "Japan";
	case 'K':
		return "Korea";
	case 'L':
		return "JPN-PAL";
	case 'M':
		return "USA-PAL";
	case 'N':
		return "JPN-USA";
	case 'P': case 'X': case 'Y': case 'Z':
		return "Europe";
	case 'Q':
		return "JPN-KOR";
	case 'R':
		return "Russia";
	case 'S':
		return "Spain";
	case 'T':
		return "USA-KOR";
	case 'U':
		return "Australia";
	case 'V':
		return "Scandinavia";
	case 'W':
		return "Taiwan/Hong Kong/Macau";
	default://
		return "Unknown";
	}
}

bool MungPlex::ProcessInformation::IsConnectionValid()
{
	const bool processOK = GetInstance()._process.GetPID() > 0 && GetInstance()._process.IsRunning();

	if (!processOK)
		return false;

	switch (GetInstance()._processType)
	{
	case EMULATOR: {

		switch (GetInstance()._platformID)
		{
		case PSP: {
			std::wstring wTitleBuf(512, L'\0');

			for (HWND wHandle : Xertz::SystemInfo::GetWindowHandleList())
			{
				GetWindowTextW(wHandle, wTitleBuf.data(), 512);
				std::wstring idW = MT::Convert<std::string, std::wstring>(GetInstance()._gameID, MT::UTF8, MT::UTF16LE);
				
				if (wTitleBuf.find(idW) == std::wstring::npos)
					continue;

				int pos = wTitleBuf.find(L"-");

				if (pos == std::wstring::npos)
					continue;
				else
					return true;
			}
		}return false;
		case WIIU: {
			std::wstring wTitleBuf(512, L'\0');

			for (HWND wHandle : Xertz::SystemInfo::GetWindowHandleList())
			{
				GetWindowTextW(wHandle, wTitleBuf.data(), 512);

				if (wTitleBuf.find(L"Cemu") == std::wstring::npos)
					continue;

				int pos = wTitleBuf.find(L"TitleId");

				if (pos == std::wstring::npos)
					continue;
				else
					return true;
			}
		}return false;
		default: { //all other
			int temp = 0;
			GetInstance()._process.ReadMemoryFast(&temp, GetInstance()._connectionCheckPtr, 4);
			return temp == GetInstance()._connectionCheckValue;
		}
		}

	}break;
	default: //PC
		return true;
	}
}

const std::vector<std::pair<int, std::string>>& MungPlex::ProcessInformation::GetSystemPairs()
{
	return _systemPairs;
}


std::string MungPlex::ProcessInformation::GetSystemNameByID(const int id)
{
	auto name = std::find_if(_systemPairs.begin(), _systemPairs.end(), 
		[id](const std::pair<int, std::string>& pair) { return pair.first == id; 
	});

	if (name != _systemPairs.end())
	{
		return name->second;
	}

	return "Undefined";
}