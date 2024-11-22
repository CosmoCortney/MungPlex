#include "BigNHelpers.hpp"
#include "Dolphin_Impl.hpp"

bool MungPlex::Dolphin::Init(const Xertz::ProcessInfo& process, std::vector<GameEntity>& gameEntities, std::vector<SystemRegion>& systemRegions)
{
	ProcessInformation::SetMiscProcessInfo("Dolphin", true, false, 4, 4);
	uint32_t temp = 0;
	uint32_t flagGCN = 0;
	uint32_t flagWii = 0;
	char tempID[7] = "";
	char discNo;
	char discVer;
	std::wstring wTitleBuf(512, L'\0');
	bool entitiesFound = false;

	for (HWND wHandle : Xertz::SystemInfo::GetWindowHandleList())
	{
		GetWindowTextW(wHandle, wTitleBuf.data(), 512);

		if (wTitleBuf.find(L"Dolphin") != 0)
			continue;

		int posEnd = wTitleBuf.find(L"(");

		if (posEnd < 0)
			continue;

		int posBeg = 0;

		if (posEnd < 0)
			continue;

		for (int i = posEnd; wTitleBuf[i] != '|'; --i)
			posBeg = i;

		if (posBeg < 0)
			break;

		const std::wstring wTitle = wTitleBuf.substr(posBeg + 1, posEnd - posBeg - 2);
		_gameName = MT::Convert<std::wstring, std::string>(wTitle, MT::UTF16LE, MT::UTF8);
		//std::cout << _gameName << std::endl;
		break;
	}

	bool memoryFound = false;
	char* mainMemRegion = nullptr;

	for (const auto& _region : ProcessInformation::GetRegionList())
	{
		if (_region.GetRegionSize() != 0x2000000)
			continue;

		memoryFound = true;
		mainMemRegion = _region.GetBaseAddress<char*>();;
		process.ReadMemoryFast(&temp, mainMemRegion + 0x28, 4);
		process.ReadMemoryFast(&flagGCN, mainMemRegion + 0x18, 4);
		process.ReadMemoryFast(&flagWii, mainMemRegion + 0x1C, 4);

		if (temp != 0x8001)
			continue;

		_connectionCheckPtr = _region.GetBaseAddress<void*>();
		process.ReadMemoryFast(&_connectionCheckValue, _connectionCheckPtr, 4);
		break;
	}

	if (!memoryFound)
		return false;

	process.ReadMemoryFast(tempID, mainMemRegion, 6);
	process.ReadMemoryFast(&discNo, mainMemRegion + 6, 1);
	process.ReadMemoryFast(&discVer, mainMemRegion + 7, 1);
	_gameID = _rpcGameID = tempID;
	_gameID.append("-").append(std::to_string(discNo));
	_gameID.append("-").append(std::to_string(discVer));
	_gameRegion = GetRegionFromBigNRegionCode(_gameID[3]);

	if (flagGCN == 0xC2339F3D || (flagWii != 0 && flagGCN == 0))
	{
		//Add checks for Triforce games
		_platformID = ProcessInformation::GAMECUBE;
		loadSystemInformationJSON("GameCube", systemRegions);
		systemRegions[0].BaseLocationProcess = reinterpret_cast<void*>(mainMemRegion);
		entitiesFound = obtainGameEntities("GameCubeTriforceWii", gameEntities);
		return entitiesFound;
	}

	for (const auto& region : ProcessInformation::GetRegionList())
	{
		if (region.GetRegionSize() != 0x4000000)
			continue;

		unsigned char temp;
		process.ReadMemoryFast(&temp, reinterpret_cast<void*>(region.GetBaseAddress<uint64_t>() + 1), 1);

		if (temp == 0x9f)
		{
			loadSystemInformationJSON("Wii", systemRegions);
			systemRegions[0].BaseLocationProcess = reinterpret_cast<void*>(mainMemRegion);
			systemRegions[1].BaseLocationProcess = region.GetBaseAddress<void*>();
			entitiesFound = obtainGameEntities("GameCubeTriforceWii", gameEntities);
			break;
		}
	}

	int IDcopy;
	process.ReadMemoryFast(&temp, mainMemRegion, 4);
	process.ReadMemoryFast(&IDcopy, mainMemRegion + 0x3180, 4);
	_platformID = ProcessInformation::WII;
	
	if (temp == 0 && IDcopy != 0)
	{
		memcpy_s(tempID, 7, &IDcopy, 4);
		_rpcGameID = _gameID = tempID;
		_gameRegion = _gameID[3];
		return entitiesFound;
	}

	if (IDcopy == temp)
		return entitiesFound;

	return false;
}
