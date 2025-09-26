#include "Mesen_Impl.hpp"
#include "StringHelpers.hpp"

bool MungPlex::Mesen::Init(const Xertz::ProcessInfo& process, std::vector<GameEntity>& gameEntities, std::vector<SystemRegion>& systemRegions)
{
	ProcessInformation::SetMiscProcessInfo("Mesen", false, false, 2, 1);
	bool ramFound = false;
	bool romFound = false;
	char* RAM;
	uint64_t ROMflag = 0;
	static const uint64_t ramFlag = 0x78656C50676E754D;

	for (Xertz::MemoryRegion& region : ProcessInformation::GetRegionList())
	{
		if (ramFound)
			break;

		if ((region.GetProtect() & PAGE_READWRITE) != PAGE_READWRITE)
			continue;

		int regionSize = region.GetRegionSize();

		if (regionSize < 0x10000 || regionSize > 0x1000000)
			continue;

		std::vector<uint64_t> buf(regionSize / 8);
		char* mainMemoryRegion = region.GetBaseAddress<char*>();
		process.ReadMemoryFast(buf.data(), mainMemoryRegion, regionSize);

		for (int i = 0; i < regionSize / 8; i += 2)
		{
			if (buf[i] != ramFlag)
				continue;

			//std::cout << "ram found!\n";
			RAM = mainMemoryRegion + i * 8 - 0xD0;
			ROMflag = buf[i + 1];
			ramFound = true;
			loadSystemInformationJSON("SNES", systemRegions);
			systemRegions[0].BaseLocationProcess = reinterpret_cast<void*>(RAM);
			_connectionCheckValue = ramFlag;
			_connectionCheckPtr = mainMemoryRegion + i * 8;
			break;
		}
	}

	if (!ramFound)
		return false;

	for (Xertz::MemoryRegion& region : ProcessInformation::GetRegionList())
	{
		if (romFound)
			break;

		if ((region.GetProtect() & PAGE_READWRITE) != PAGE_READWRITE)
			continue;

		int regionSize = region.GetRegionSize();

		if (regionSize < 0x100000 || regionSize > 0x1000000)
			continue;

		uint64_t* buf = new uint64_t[regionSize / 8];
		process.ReadMemoryFast(buf, region.GetBaseAddress<void*>(), regionSize);

		for (int i = 0; i < regionSize / 8; i += 2)
		{
			if (buf[i] != ROMflag)
				continue;

			//std::cout << "rom found!\n";
			RAM = region.GetBaseAddress<char*>() + i * 8;
			ROMflag = buf[i + 1];
			romFound = true;
			systemRegions[1].BaseLocationProcess = reinterpret_cast<void*>(RAM);
			break;
		}
	}

	if (!romFound)
		return false;

	//todo JIS X 0201 encoding once MorphText supports it
	_gameID = std::string(21, 0);
	process.ReadMemoryFast(_gameID.data(), RAM + 0x7FC0, 21);
	_gameID = MT::Convert<char*, std::string>(_gameID.data(), MT::JIS_X_0201_FULLWIDTH, MT::UTF8);
	_gameID = RemoveSpacePadding(_gameID, false);
	_gameName = _gameID;
	_gameID.append("-");
	char tempByte = 0;
	process.ReadMemoryFast(&tempByte, RAM + 0x7FD9, 1);
	_gameID.append(std::to_string(tempByte)).append("-");
	process.ReadMemoryFast(&tempByte, RAM + 0x7FDB, 1);
	_gameID.append(std::to_string(tempByte));
	_platformID = ProcessInformation::SNES;

	return true;
}