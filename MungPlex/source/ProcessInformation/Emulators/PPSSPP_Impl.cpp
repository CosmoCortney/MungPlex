#include "BigSHelpers.hpp"
#include "PPSSPP_Impl.hpp"

bool MungPlex::PpSsPp::Init(const Xertz::ProcessInfo& process, std::vector<GameEntity>& gameEntities, std::vector<SystemRegion>& systemRegions)
{
	ProcessInformation::SetMiscProcessInfo("PPSSPP", false, false, 4, 4);
	bool titleIDFound = false;

	for (Xertz::MemoryRegion& region : ProcessInformation::GetRegionList())
	{
		if (region.GetRegionSize() == 0x1f00000)
		{
			loadSystemInformationJSON("PSP", systemRegions);
			char* mainMemoryRegion = region.GetBaseAddress<char*>();
			systemRegions[0].BaseLocationProcess = reinterpret_cast<void*>(mainMemoryRegion + 0x1000000);
			break;
		}
	}

	std::wstring wTitleBuf(512, L'\0');
	for (HWND wHandle : Xertz::SystemInfo::GetWindowHandleList())
	{
		GetWindowTextW(wHandle, wTitleBuf.data(), 512);

		if (wTitleBuf.find(L"PPSSPP v") == std::wstring::npos)
			continue;

		int pos = wTitleBuf.find(L"-");
		_rpcGameID = _gameID = MT::Convert<std::wstring, std::string>(wTitleBuf.substr(pos + 2, 9), MT::UTF16LE, MT::UTF8);
		_gameName = MT::Convert<std::wstring, std::string>(wTitleBuf.substr(pos + 14), MT::UTF16LE, MT::UTF8);
#ifndef NDEBUG
		std::cout << _gameID << std::endl;
		std::cout << _gameName << std::endl;
#endif
		_platformID = ProcessInformation::PSP;
		_gameRegion = GetRegionFromBigSRegionCode(_gameID[2]);
		return true;
	}

	return false;
}
