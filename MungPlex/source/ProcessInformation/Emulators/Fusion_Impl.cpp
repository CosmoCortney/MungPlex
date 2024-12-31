#include "Fusion_Impl.hpp"

bool MungPlex::Fusion::Init(const Xertz::ProcessInfo& process, std::vector<GameEntity>& gameEntities, RegionCombo::Type& systemRegions)
{
	ProcessInformation::SetMiscProcessInfo("Fusion", true, false, 4, 1);
	bool titleIDFound = false;
	bool romFound = false;
	uint64_t modAddr = process.GetModuleAddress(L"Fusion.exe");
	uint16_t headerSize = 0x200;
	uint64_t romPTR = 0;
	uint64_t ramPTR = 0;
	static const uint32_t AGES = 0x41474553;
	std::vector<uint32_t> romHeader(headerSize / sizeof(uint32_t));

	//look for SEGA CD BOOT ROM
	process.ReadMemoryFast(&romPTR, reinterpret_cast<void*>(modAddr + 0x001C33C4), 4);
	process.ReadMemoryFast(romHeader.data(), reinterpret_cast<void*>(romPTR), headerSize);

	if (romHeader[0x4A] == 0x544F4F42) //if sega cd found
	{
		_connectionCheckValue = 0x544F4F42;
		_connectionCheckPtr = reinterpret_cast<char*>(romPTR) + 0x4A * 4;
		_gameID = std::string(reinterpret_cast<char*>(&romHeader[0x60]));
		_gameID.resize(14);
		loadSystemInformationJSON("MegaCD", systemRegions);
		systemRegions[0].BaseLocationProcess = reinterpret_cast<void*>(romPTR);
		systemRegions[0].Size = Xertz::SwapBytes<uint32_t>(romHeader[0x69]) + 1;
		systemRegions[0].Base = 0x0;

		ramPTR = 0x006A52D4;
		_gameName.resize(48);
		process.ReadMemoryFast(_gameName.data(), reinterpret_cast<void*>(ramPTR + 0x144), 48);
		_gameName = RemoveSpacePadding(_gameName);
		process.ReadMemoryFast(&ramPTR, reinterpret_cast<void*>(ramPTR), 4);
		systemRegions[1].BaseLocationProcess = reinterpret_cast<void*>(ramPTR);
		systemRegions[1].Size = 0x10000;
		systemRegions[1].Base = 0xFF0000;
		_rpcGameID = "#";
		_gameID = _gameName + " - " + _gameID;
		_platformID = ProcessInformation::SMCD;
		return true;
	}

	//look for Mega Drive or 32x ROM
	romPTR = 0x007CFC00;
	uint32_t test = 0;
	process.ReadMemoryFast(&test, reinterpret_cast<void*>(romPTR + 0x100), 4);

	if (test != AGES)//if not equal, fallback ROM header fetch
	{
		process.ReadMemoryFast(&romPTR, reinterpret_cast<void*>(modAddr + 0x0005D8EC), 4);
		romPTR += 0x14;
	}

	process.ReadMemoryFast(romHeader.data(), reinterpret_cast<void*>(romPTR), headerSize);

	if (romHeader[0x40] == AGES) //Mega Drive or 32x ROM found
	{
		if (romHeader[0x41] == 0x58323320) //32X ROM found
		{
			loadSystemInformationJSON("32X", systemRegions);
			_platformID = ProcessInformation::S32X;
		}
		else if ((romHeader[0x41] == 0x47454D20 && romHeader[0x42] == 0x52442041) || (romHeader[0x41] == 0x4E454720)) //mega drive ROM found
		{
			loadSystemInformationJSON("MegaDrive", systemRegions);
			_platformID = ProcessInformation::GENESIS;
		}
		else //something went wrong
			return false;

		_connectionCheckValue = *reinterpret_cast<int*>(&romHeader[0x41]);
		_connectionCheckPtr = reinterpret_cast<char*>(romPTR) + 0x41 * 4;
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

		systemRegions[0].BaseLocationProcess = reinterpret_cast<void*>(romPTR);
		systemRegions[0].Size = Xertz::SwapBytes<uint32_t>(romHeader[0x69]) + 1;
		systemRegions[0].Base = 0x0;

		process.ReadMemoryFast(&ramPTR, reinterpret_cast<char*>(0x00759F14), 4);
		systemRegions[1].BaseLocationProcess = reinterpret_cast<void*>(ramPTR);
		systemRegions[1].Size = 0x10000;
		systemRegions[1].Base = 0xFF0000;
		return true;
	}

	return false;
}
