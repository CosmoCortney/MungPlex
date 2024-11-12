#pragma once
#include "ProcessInformation.hpp"

namespace MungPlex
{
	class IConsoleConnectionWrapper
	{
	public:
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

		bool obtainGameEntities(const std::string& systemName, std::vector<GameEntity>& gameEntities, char* memDump, const bool bigEndian, const uint64_t baseAddress);

	private:
	};
}