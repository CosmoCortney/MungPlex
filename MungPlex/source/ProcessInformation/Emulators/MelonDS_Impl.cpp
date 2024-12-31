#include "BigNHelpers.hpp"
#include "MelonDS_Impl.hpp"

bool MungPlex::MelonDS::Init(const Xertz::ProcessInfo& process, std::vector<GameEntity>& gameEntities, RegionCombo::Type& systemRegions)
{
	ProcessInformation::SetMiscProcessInfo("melonDS", false, false, 4, 4);
	bool romFound = false;
	static const uint64_t romFlag = 0x21A29A6951AEFF24;
	//find ROM
	for (const auto& region : ProcessInformation::GetRegionList())
	{
		if (region.GetRegionSize() < 0x800000 || (region.GetProtect() & PAGE_READWRITE) != PAGE_READWRITE)
			continue;

		char* mainMemoryRegion = region.GetBaseAddress<char*>();
		std::vector<uint8_t> buf(region.GetRegionSize());
		process.ReadMemoryFast(buf.data(), region.GetBaseAddress<void*>(), region.GetRegionSize());

		for (int i = 0; i < 0x200; i += 8)
		{
			if (*reinterpret_cast<uint64_t*>(&buf[i]) != romFlag)
				continue;

			_connectionCheckPtr = reinterpret_cast<void*>(mainMemoryRegion + i);
			_connectionCheckValue = romFlag;
			const uint32_t romBase = i - 0xC0;
			_rpcGameID = _gameID = reinterpret_cast<char*>(&buf[romBase + 0xC]);
			_gameRegion = GetRegionFromBigNRegionCode(_gameID[3]);
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
	for (const auto& region : ProcessInformation::GetRegionList())
	{
		if (region.GetRegionSize() != 0x10F0000)
			continue;

		loadSystemInformationJSON("NDS", systemRegions);
		systemRegions[0].BaseLocationProcess = region.GetBaseAddress<void*>();
		_platformID = ProcessInformation::NDS;
		return true;
	}

	return false;
}
