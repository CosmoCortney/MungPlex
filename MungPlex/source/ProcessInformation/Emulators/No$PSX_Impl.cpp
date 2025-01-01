#include "No$PSX_Impl.hpp"

bool MungPlex::No$PSX::Init(const Xertz::ProcessInfo& process, std::vector<GameEntity>& gameEntities, std::vector<SystemRegion>& systemRegions)
{
	ProcessInformation::SetMiscProcessInfo("No$psx", false, false, 4, 1);
	static const uint64_t ramFlag = 0x696C6F626D795300;

	for (const Xertz::MemoryRegion& region : ProcessInformation::GetRegionList())
	{
		if (region.GetRegionSize() != 0x459000)
			continue;

		char* mainMemoryRegion = region.GetBaseAddress<char*>();
		loadSystemInformationJSON("PS1", systemRegions);
		systemRegions[0].BaseLocationProcess = mainMemoryRegion + 0x30100;
		_gameID = std::string(12, 0);
		process.ReadMemoryFast(_gameID.data(), mainMemoryRegion + 0x30100 + 0x00003A49, 11);
		_rpcGameID = _gameID = _gameID.c_str();
		char tempRegion[32];
		process.ReadMemoryFast(tempRegion, mainMemoryRegion + 0x30100 + 0x00003BE5, 32);
		_gameRegion = tempRegion;
		_gameRegion = _gameRegion.substr(0, _gameRegion.find(" area"));
		void* exeAddr = reinterpret_cast<void*>(process.GetModuleAddress(L"NO$PSX.EXE"));
		std::vector<char> buf(0x100000);
		process.ReadMemoryFast(buf.data(), exeAddr, 0x100000);

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
		_platformID = ProcessInformation::PS1;
		return true;
	}

	return false;
}
