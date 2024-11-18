#include "BigNHelpers.hpp"
#include "Fusion_Impl.hpp"
#include "Cemu_Impl.hpp"
#include "Cheats.hpp"
#include "DeviceControl.hpp"
#include "Dolphin_Impl.hpp"
#include "Lime3DS_Impl.hpp"
#include "../../Log.hpp"
#include <boost/iostreams/device/mapped_file.hpp>
#include "MelonDS_Impl.hpp"
#include "Mesen_Impl.hpp"
#include "mGBA_Impl.hpp"
#include "No$PSX_Impl.hpp"
#include "PCSX2_Impl.hpp"
#include "RPCS3_Impl.hpp"
#include "PPSSPP_Impl.hpp"
#include "Yuzu_Impl.hpp"
#include "PointerSearch.hpp"
#include "ProcessInformation.hpp"
#include "Project64_Impl.hpp"
#include "Search.hpp"
#include "USBGeckoConnectionWrapper_Impl.hpp"
#include "WatchControl.hpp"
#include <Windows.h>

inline const MungPlex::StringIdPairs MungPlex::ProcessInformation::_emulators =
{
	{ "Mesen", "Project64", "Dolphin", "Cemu", "Yuzu", "mGBA", "melonDS", "Lime3DS", "No$psx", "pcsx2", "Rpcs3", "PPSSPP", "Fusion" }, 
	{ MESEN,   PROJECT64,   DOLPHIN,   CEMU,   YUZU,   mGBA,   MELONDS,   LIME3DS,   NO$PSX,   PCSX2,   RPCS3,   PPSSPP,   FUSION }, 
	"Emulator:"
};

inline const MungPlex::StringIdPairs MungPlex::ProcessInformation::_systems =
{
	{ "NES",  "SNES",  "N64",  "GCN",  "Triforce",  "RVL",  "Cafe",  "Switch",  "GB",  "GBC",  "GBA",  "NDS",  "CTR",  
	  "PS1",  "PS2",  "PS3",  "PS4",  "PS5",  "PSP",  "PSV",  
	  "SMS",  "Genesis",  "32X",  "Mega CD",  "Saturn",  "DC",  "GG",  
	  "XBOX",  "360", "XBONE", "Series", "PC (x86)", "PC (x64)"},
	{ NES, SNES, N64, GAMECUBE, TRIFORCE, WII, WIIU, SWITCH, GB, GBC, GBA, NDS, N3DS, 
	  PS1, PS2, PS3, PS4, PS5, PSP, PSV, 
	  SMS, GENESIS, S32X, SMCD, SATURN, DREAMCAST, GG, 
	  XBOX, XBOX360, XBOXONE, XBOXSERIES, X86, X64 },
	  "System:"
};

inline const MungPlex::StringIdPairs MungPlex::ProcessInformation::_consoleConnectionTypes =
{ 
	{"USB Gecko"}, {CON_USBGecko}, "Connection Type:"
};

inline MungPlex::RegionPairs MungPlex::ProcessInformation::__systemRegions = { "Regions:" };

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

bool MungPlex::ProcessInformation::initEmulator(const int emulatorIndex)
{
	std::shared_ptr<IEmulator> iemulator;
	const std::wstring emuName = MT::Convert<const char*, std::wstring>(_emulators.GetCString(emulatorIndex), MT::UTF8, MT::UTF16LE);
	_gameID.clear();
	_gameRegion.clear();
	_platform.clear();
	_rpcGameID.clear();
	_gameName.clear();

	if (!initProcess(emuName))
		return false;
	
	bool connected;

	switch (_emulators.GetId(emulatorIndex))
	{
		case DOLPHIN:
		{
			iemulator = std::make_shared<Dolphin>();
		}break;
		case PROJECT64:
			iemulator = std::make_shared<Project64>();
			break;
		case CEMU:
			iemulator = std::make_shared<Cemu>();
			break;
		case YUZU:
			iemulator = std::make_shared<Yuzu>();
			break;
		case mGBA:
			iemulator = std::make_shared<MGBA>();
			break;
		case MELONDS:
			iemulator = std::make_shared<MelonDS>();
			break;
		case LIME3DS:
			iemulator = std::make_shared<Lime3DS>();
			break;
		case MESEN:
			iemulator = std::make_shared<Mesen>();
			break;
		case PPSSPP:
			iemulator = std::make_shared<PpSsPp>();
			break;
		case NO$PSX:
			iemulator = std::make_shared<No$PSX>();
			break;
		case PCSX2:
			iemulator = std::make_shared<PCSx2>();
			break;
		case RPCS3:
			iemulator = std::make_shared<RPcS3>();
			break;
		case FUSION:
			iemulator = std::make_shared<Fusion>();
			break;
	}

	if (!iemulator->Init(_process, _gameEntities, _systemRegions))
		return false;

	_gameID = iemulator->GetGameID();
	_gameRegion = iemulator->GetGameRegion();
	_rpcGameID = iemulator->GetRrpGameID();
	_gameName = iemulator->GetGameName();
	_platformID = iemulator->GetPlatformID();
	_platform = _systems.GetStdStringById(_platformID);

	PointerSearch::SelectPreset(_platformID);
	setupSearch();
	Search::SetDefaultSearchSettings();
	setupCheats();
	WatchControl::InitWatchFile();
	DeviceControl::InitDevicesFile();
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

 bool MungPlex::ProcessInformation::ConnectToRealConsole(const int type)
 {
	 GetInstance()._processType = CONSOLE;

	 if (!GetInstance().initConsoleConnection(type))
		 return false;

	 __systemRegions.SetRegions(GetInstance()._systemRegions);
	 GetInstance()._exePath = GetInstance()._process.GetFilePath();
	 std::string msg("Connected to "
	 + GetInstance()._processName + " ("
     + GetInstance()._platform + ") - Game ID: "
	 + GetInstance()._gameID);
	 Log::LogInformation(msg.c_str());

	 std::string windowTitle(GetWindowTitleBase()
	 + " | " + GetInstance()._processName
	 + " | " + GetInstance()._gameName
	 + " | " + GetInstance()._gameID
	 + " (" + GetInstance()._platform + ")");
	 glfwSetWindowTitle(GetInstance()._window, windowTitle.c_str());

	 return true;
}

bool MungPlex::ProcessInformation::connectToProcessFR()
 {
	 bool connected = _process.GetPID() > 0 && _process.IsOpen();
	 if (!connected)
		 return false;

	 RefreshRegionlistPC();
	 refreshModuleList();
	 SetMiscProcessInfo(_process.GetProcessName(), false, false, _process.IsX64() ? 8 : 4, 4);
	 GetInstance().setupSearch();
	 GetInstance()._processType = NATIVE;
	 Cheats::InitCheatFile();
	 WatchControl::InitWatchFile();
	 DeviceControl::InitDevicesFile();
	 Search::SetNativeAppSearchSettings();
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

bool MungPlex::ProcessInformation::initProcess(const std::wstring& processName)
{
	_process = Xertz::SystemInfo::GetProcessInfo(processName, true, false);

	if (_process.GetPID() == -1)
		return false;

	return true;
}

bool MungPlex::ProcessInformation::initConsoleConnection(const int connectionType)
{
	_currentConsoleConnectionType = connectionType;
	_gameID.clear();
	_gameRegion.clear();
	_platform.clear();
	_rpcGameID.clear();
	_gameName.clear();
	std::shared_ptr<IConsoleConnectionWrapper> iConsoleConnectionWrapper;

	switch (connectionType)
	{
	case CON_USBGecko:
	{
		_usbGecko = std::make_shared<USBGecko>();
		iConsoleConnectionWrapper = std::make_shared<USBGeckoConnectionWrapper>();

		if (!iConsoleConnectionWrapper->Init(_usbGecko.get(), _gameEntities, _systemRegions)) //also establishes connection
			return false;
	}break;
	default:
		return false;
	}

	_gameID = iConsoleConnectionWrapper->GetGameID();
	_gameRegion = iConsoleConnectionWrapper->GetGameRegion();
	_rpcGameID = iConsoleConnectionWrapper->GetRrpGameID();
	_gameName = iConsoleConnectionWrapper->GetGameName();
	_platformID = iConsoleConnectionWrapper->GetPlatformID();
	_platform = _systems.GetStdStringById(_platformID);
	PointerSearch::SelectPreset(_platformID);
	setupSearch();
	Search::SetDefaultSearchSettings();
	setupCheats();
	WatchControl::InitWatchFile();
	DeviceControl::InitDevicesFile();
	return true;
}

void MungPlex::ProcessInformation::SetMiscProcessInfo(const std::string processName, const bool bigEndian, const bool rereorder, const int addressWidth, const int alignment)
{
	GetInstance()._processName = processName;
	std::string windowTitle = GetWindowTitleBase();
	windowTitle.append(" | " + GetInstance()._processName);
	glfwSetWindowTitle(GetInstance()._window, windowTitle.c_str());
	GetInstance()._underlyingIsBigEndian = bigEndian;
	GetInstance()._addressWidth = addressWidth;
	GetInstance()._rereorderRegion = rereorder;
	GetInstance()._alignment = alignment;
}

MungPlex::USBGecko* MungPlex::ProcessInformation::GetUsbGecko()
{
	return GetInstance()._usbGecko.get();
}

std::string& MungPlex::ProcessInformation::GetGameID()
{
	return GetInstance()._gameID;
}

std::string& MungPlex::ProcessInformation::GetRpcGameID()
{
	return GetInstance()._rpcGameID;
}

bool MungPlex::ProcessInformation::ConnectToEmulator(const int emulatorIndex)
{
	GetInstance()._processType = EMULATOR;

	if (!GetInstance().initEmulator(emulatorIndex))
		return false;
	
	__systemRegions.SetRegions(GetInstance()._systemRegions);
	GetInstance()._exePath = GetInstance()._process.GetFilePath();
	std::string msg("Connected to ");
	msg.append(GetInstance()._processName + " (");
	msg.append(GetInstance()._platform + ") - Game ID: ");
	msg.append(GetInstance()._gameID);
	Log::LogInformation(msg.c_str());

	std::string windowTitle = GetWindowTitleBase();
	windowTitle.append(" | " + GetInstance()._processName);
	windowTitle.append(" | " + GetInstance()._gameName);
	windowTitle.append(" | " + GetInstance()._gameID);
	windowTitle.append(" (" + GetInstance()._platform + ")");
	glfwSetWindowTitle(GetInstance()._window, windowTitle.c_str());

	return true; 
}

const MungPlex::StringIdPairs& MungPlex::ProcessInformation::GetEmulatorList()
{
	return _emulators;
}

const MungPlex::StringIdPairs& MungPlex::ProcessInformation::GetConsoleConnectionTypeList()
{
	return _consoleConnectionTypes;
}

int32_t MungPlex::ProcessInformation::GetProcessType()
{
	return GetInstance()._processType;
}

int32_t MungPlex::ProcessInformation::GetConsoleConnectionType()
{
	return GetInstance()._currentConsoleConnectionType;
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
	__systemRegions.Clear();

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

			label.append(": ");
			int lj = region.GetBaseAddress<uint64_t>() < 0x100000000 ? 8 : 16;
			label.append(ToHexString(region.GetBaseAddress<uint64_t>(), lj, false));
			GetInstance()._systemRegions.emplace_back(label, region.GetBaseAddress<uint64_t>(), region.GetRegionSize(), region.GetBaseAddress<void*>());
		}
	}

	__systemRegions.SetRegions(GetInstance()._systemRegions);
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

const MungPlex::RegionPairs& MungPlex::ProcessInformation::GetSystemRegionList_()
{
	return __systemRegions;
}

std::vector<MungPlex::SystemRegion>& MungPlex::ProcessInformation::GetSystemRegionList()
{
	return GetInstance()._systemRegions;
}

bool MungPlex::ProcessInformation::IsConnectionValid()
{
	if (GetInstance()._processType != CONSOLE)
	{
		const bool processOK = GetInstance()._process.GetPID() > 0 && GetInstance()._process.IsRunning();

		if (!processOK)
			return false;
	}

	switch (GetInstance()._processType)
	{
	case CONSOLE:
	{
		switch (GetInstance()._currentConsoleConnectionType)
		{
		case CON_USBGecko:
			return GetInstance()._usbGecko->IsConnectedAndReady();
			break;
		default:
			return false;
		}
	}break;
	case EMULATOR: 
	{
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

const MungPlex::StringIdPairs& MungPlex::ProcessInformation::GetSystemList()
{
	return _systems;
}

void MungPlex::ProcessInformation::SetWindowRef(GLFWwindow* window)
{
	GetInstance()._window = window;
}

void MungPlex::ProcessInformation::ResetWindowTitle()
{
	glfwSetWindowTitle(GetInstance()._window, GetWindowTitleBase().c_str());
}

void* MungPlex::ProcessInformation::GetPointerFromPointerPathExpression(const std::vector<int64_t>& pointerPath, const bool useModule, const int64_t moduleAddress)
{
	if (pointerPath.empty())
		return nullptr;

	int64_t ptr = 0;

	if (useModule)
		ptr = moduleAddress;

	for (int i = 0; i < pointerPath.size() - 1; ++i)
	{
		ptr += pointerPath[i];

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

	return reinterpret_cast<void*>(ptr + pointerPath.back());
}