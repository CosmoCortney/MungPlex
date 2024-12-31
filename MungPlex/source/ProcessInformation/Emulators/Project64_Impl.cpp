#include "Project64_Impl.hpp"
#include "BigNHelpers.hpp"

bool MungPlex::Project64::Init(const Xertz::ProcessInfo& process, std::vector<GameEntity>& entities, RegionCombo::Type& regions)
{
	ProcessInformation::SetMiscProcessInfo("Project64", true, true, 4, 4);
	bool found = false;
	static const uint64_t romFlag = 0x0000000F80371240;

	for (const auto& region : ProcessInformation::GetRegionList())
	{
		const uint64_t rSize = region.GetRegionSize();

		if (!(rSize == 0x400000 || rSize == 0x800000))
			continue;

		uint32_t temp;
		char* mainMemoryRegion = region.GetBaseAddress<char*>();
		process.ReadMemoryFast(&temp, mainMemoryRegion + 8, 4);

		if (temp != 0x03400008)
			continue;

		loadSystemInformationJSON("N64", regions);
		regions[0].BaseLocationProcess = reinterpret_cast<void*>(mainMemoryRegion);
		found = true;
		break;
	}

	if (!found)
		return false;

	for (const auto& region : ProcessInformation::GetRegionList())
	{
		const uint64_t rSize = region.GetRegionSize();

		if (rSize < 0x400000)
			continue;

		char* romRegion = region.GetBaseAddress<char*>();
		uint64_t temp;
		process.ReadMemoryFast(&temp, romRegion, 8);

		if (temp != romFlag)
			continue;

		_connectionCheckValue = romFlag;
		_connectionCheckPtr = reinterpret_cast<void*>(romRegion);
		region.SetProtect(ProcessInformation::GetHandle(), PAGE_EXECUTE_READWRITE);
		regions[1].BaseLocationProcess = region.GetBaseAddress<void*>();
		regions[1].Size = rSize;
		found = true;
		char tempID[5] = "";
		ReadFromReorderedRangeEx(process, reinterpret_cast<uint32_t*>(tempID), romRegion + 0x3B);
		_rpcGameID = _gameID = std::string(tempID);
		_gameName.resize(20);
		_gameRegion = GetRegionFromBigNRegionCode(_gameID[3]);

		for (int i = 0; i <= 20; i += 4)
			ReadFromReorderedRangeEx(process, reinterpret_cast<uint32_t*>(_gameName.data() + i), romRegion + 0x20 + i);

		_gameName = RemoveSpacePadding(_gameName);
		_platformID = ProcessInformation::N64;
		break;
	}

	return found && obtainGameEntities("N64", entities);
}