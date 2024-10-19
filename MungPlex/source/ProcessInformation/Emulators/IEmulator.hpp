#pragma once
#include "ProcessInformation.hpp"

namespace MungPlex
{
	class IEmulator
	{
	public:
		virtual bool Init(const Xertz::ProcessInfo& process, std::vector<GameEntity>& gameEntities, std::vector<SystemRegion>& systemRegions) = 0;
		std::string GetGameName() { return _gameName; }
		std::string GetGameID() { return _gameID; };
		std::string GetRrpGameID() { return _rpcGameID; };
		std::string GetGameRegion() { return _gameRegion; };
		int GetPlatformID() { return _platformID; }

	protected:
		std::string _gameName;
		std::string _gameID;
		std::string _rpcGameID;
		std::string _gameRegion;
		int _connectionCheckValue = 0;
		void* _connectionCheckPtr = nullptr;
		int _platformID = ProcessInformation::UNK;

		std::string getRegionFromBigNRegionCode(const char code) const;
		bool loadSystemInformationJSON(const std::string& system, std::vector<SystemRegion>& systemRegions);
		bool obtainGameEntities(const std::string& systemName, std::vector<GameEntity>& gameEntities);

	private:
	};
}