#include "mGBA_Impl.hpp"
#include "BigNHelpers.hpp"

bool MungPlex::MGBA::Init(const Xertz::ProcessInfo& process, std::vector<GameEntity>& gameEntities, RegionCombo::Type& systemRegions)
{
	bool romFound = false;
	uint64_t bufLogo = 0;

	for (const auto& region : ProcessInformation::GetRegionList())
	{
		if (!(region.GetProtect() & PAGE_READWRITE))
			continue;

		uint8_t* romBasePtr = region.GetBaseAddress<uint8_t*>();
		process.ReadMemorySafe(&bufLogo, romBasePtr + 0x104, 8);

		if (bufLogo == 0x0B000DCC6666EDCE)
		{
			uint8_t typeFlag = 0;
			process.ReadMemorySafe(&typeFlag, romBasePtr + 0x143, 1);

			if (typeFlag == 0x80 || typeFlag == 0xC0)
			{
				loadSystemInformationJSON("GBC", systemRegions);
				ProcessInformation::SetMiscProcessInfo("GBC", false, false, 2, 1);
				_platformID = ProcessInformation::GBC;
			}
			else
			{
				loadSystemInformationJSON("GB", systemRegions);
				ProcessInformation::SetMiscProcessInfo("GB", false, false, 2, 1);
				_platformID = ProcessInformation::GB;
			}

			std::string title(16, '\0');
			process.ReadMemorySafe(title.data(), romBasePtr + 0x134, 16);
			_gameName = title;

			uint16_t checksum = 0;
			process.ReadMemorySafe(&checksum, romBasePtr + 0x14E, 2);
			_rpcGameID = _gameID = ToHexString(checksum, 4);

			uint32_t romSize = 0;
			process.ReadMemorySafe(&romSize, romBasePtr + 0x148, 1);
			systemRegions[0].BaseLocationProcess = romBasePtr;
			systemRegions[0].Size = 0x8000 * (1 << romSize);
			romFound = true;
			break;
		}

		process.ReadMemorySafe(&bufLogo, romBasePtr + 4, 8);

		if (bufLogo == 0x21A29A6951AEFF24)
		{
			ProcessInformation::SetMiscProcessInfo("GameBoy Advance", false, false, 4, 4);
			loadSystemInformationJSON("GBA", systemRegions);
			_platformID = ProcessInformation::GBA;
			systemRegions[1].BaseLocationProcess = romBasePtr;

			std::string title(12, '\0');
			process.ReadMemorySafe(title.data(), romBasePtr + 0xA0, 12);
			_gameName = title;

			std::string id(6, '\0');
			process.ReadMemorySafe(id.data(), romBasePtr + 0xAC, 6);
			_rpcGameID = _gameID = id;

			_gameRegion = GetRegionFromBigNRegionCode(id[3]);
			systemRegions[0].BaseLocationProcess = romBasePtr;
			romFound = true;
			obtainGameEntities("GBA", gameEntities);
			break;
		}
	}

	if (!romFound)
		return false;

	if (_platformID == ProcessInformation::GB || _platformID == ProcessInformation::GBC)
	{
		for (const auto& region : ProcessInformation::GetRegionList())
		{
			uint64_t tempBaseAddr = region.GetBaseAddress<uint64_t>();

			if (tempBaseAddr < 0x10000000000)
				continue;

			if (tempBaseAddr > 0x100000000000)
				return false;

			if (!(region.GetProtect() & PAGE_READWRITE))
				continue;

			if (!(region.GetType() & MEM_PRIVATE))
				continue;

			if (region.GetRegionSize() != 0x8000)
				continue;

			//the second or region of size 0x8000 is the RAM
			systemRegions[1].BaseLocationProcess = region.GetBaseAddress<void*>();
			obtainGameEntities("GB", gameEntities);
			return true;
		}
	}

	for (const auto& region : ProcessInformation::GetRegionList())
	{
		uint64_t tempBaseAddr = region.GetBaseAddress<uint64_t>();

		if (tempBaseAddr < 0x10000000000)
			continue;

		if (tempBaseAddr > 0x100000000000)
			return false;

		if (!(region.GetProtect() & PAGE_READWRITE))
			continue;

		if (!(region.GetType() & MEM_PRIVATE))
			continue;

		if (region.GetRegionSize() != 0x48000)
			continue;

		//the second or region of size 0x48000 is the RAM
		systemRegions[0].BaseLocationProcess = region.GetBaseAddress<void*>();
		return true;
	}

	return false;
}
