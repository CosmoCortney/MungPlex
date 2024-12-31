#include "BigNHelpers.hpp"
#include "IEmulator.hpp"
#include "USBGeckoConnectionWrapper_Impl.hpp"

bool MungPlex::USBGeckoConnectionWrapper::Init(USBGecko* usbGecko, std::vector<GameEntity>& gameEntities, RegionCombo::Type& systemRegions)
{
	if (usbGecko->Connect() != FT_OK)
		return false;

	
	uint32_t temp = 0;
	uint32_t flagDol = 0;
	uint32_t flagRvl = 0;
	std::string gameID(6, '\0');
	char discNo;
	char discVer;
	bool entitiesFound = false;



	bool memoryFound = false;
	char* mainMemRegion = nullptr;

	const int entityBufSize = 0x3200;
	std::vector<char> entityBuf(entityBufSize);
	int status = USBGecko::Unknown;

	for (int i = 0; i < 20; ++i)
	{
		status = usbGecko->GetCurrentStatus();

		if (status == USBGecko::Running)
			break;

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));

#ifndef NDEBUG
		std::cout << "Game not running yet. Retrying...\n";
#endif
	}

	if (status != USBGecko::Running)
	{
#ifndef NDEBUG
		std::cout << "Unable to connect to Gecko\n";
#endif
		return false;
	}

	if (usbGecko->Read(entityBuf.data(), 0x80000000, entityBufSize) != FT_OK)
		return false;

	flagDol = std::byteswap(*reinterpret_cast<uint32_t*>(entityBuf.data() + 0x18));
	flagRvl = std::byteswap(*reinterpret_cast<uint32_t*>(entityBuf.data() + 0x1C));
	std::memcpy(gameID.data(), entityBuf.data(), 6);
	discNo = *(entityBuf.data() + 6);
	discVer = *(entityBuf.data() + 7);
	_gameID = _rpcGameID = gameID;
	_gameID.append("-").append(std::to_string(discNo));
	_gameID.append("-").append(std::to_string(discVer));
	_gameRegion = GetRegionFromBigNRegionCode(_gameID[3]);
	entitiesFound = obtainGameEntities("GameCubeTriforceWii", gameEntities, entityBuf.data(), true, 0x80000000);

	if (flagDol == 0xC2339F3D || (flagRvl != 0 && flagDol == 0))
	{
		//Add checks for Triforce games
		ProcessInformation::SetMiscProcessInfo("GCN Game", true, false, 4, 4);
		_platformID = ProcessInformation::GAMECUBE;
		IEmulator::loadSystemInformationJSON("GameCube", systemRegions);
		systemRegions[0].BaseLocationProcess = nullptr;
		return entitiesFound;
	}

	IEmulator::loadSystemInformationJSON("Wii", systemRegions);
	systemRegions[0].BaseLocationProcess = nullptr;
	systemRegions[1].BaseLocationProcess = nullptr;
	_platformID = ProcessInformation::WII;
	ProcessInformation::SetMiscProcessInfo("RVL Game", true, false, 4, 4);

	//check if RVLware or disc
	int id0 = *reinterpret_cast<int*>(entityBuf.data());
	int idCopy = *reinterpret_cast<int*>(entityBuf.data() + 0x3180);

	if (id0 == 0 && idCopy != 0)
	{
		std::string tempId(4, '\0');
		memcpy(tempId.data(), &idCopy, 4);
		_rpcGameID = _gameID = tempId;
		_gameRegion = _gameID[3];
		return entitiesFound;
	}

	if (id0 == idCopy)
		return entitiesFound;
		
	return false;
}
