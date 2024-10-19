#include "IEmulator.hpp"

std::string MungPlex::IEmulator::getRegionFromBigNRegionCode(const char code) const
{
	switch (code)
	{
	case 'A':
		return "Any";
	case 'C':
		return "China";
	case 'D':
		return "Germany";
	case 'E':
		return "USA";
	case 'F':
		return "France";
	case 'H':
		return "Netherlands";
	case 'I':
		return "Italy";
	case 'J':
		return "Japan";
	case 'K':
		return "Korea";
	case 'L':
		return "JPN-PAL";
	case 'M':
		return "USA-PAL";
	case 'N':
		return "JPN-USA";
	case 'P': case 'X': case 'Y': case 'Z':
		return "Europe";
	case 'Q':
		return "JPN-KOR";
	case 'R':
		return "Russia";
	case 'S':
		return "Spain";
	case 'T':
		return "USA-KOR";
	case 'U':
		return "Australia";
	case 'V':
		return "Scandinavia";
	case 'W':
		return "Taiwan/Hong Kong/Macau";
	default://
		return "Unknown";
	}
}

bool MungPlex::IEmulator::loadSystemInformationJSON(const std::string& system, std::vector<SystemRegion>& systemRegions)
{
	systemRegions.clear();
	std::string buffer;
	std::string jsonstr;

	{
		std::ifstream inFile;
		inFile.open(GetResourcesFilePath("systeminformation.json"));

		if (inFile)
			while (std::getline(inFile, buffer))
				jsonstr.append(buffer).append("\n");
		else
			return false;
	}

	try
	{
		auto doc = nlohmann::json::parse(jsonstr);
		auto& regionsDoc = doc["Systems"][system]["Regions"];

		for (auto& region : regionsDoc)
		{
			auto label = region["Label"].get<std::string>();
			uint64_t base = std::stoll(region["Base"].get<std::string>(), 0, 0);
			uint64_t size = std::stoll(region["Size"].get<std::string>(), 0, 0);
			systemRegions.emplace_back(SystemRegion(label, base, size));
		}

		if (regionsDoc.empty())
			return false;
	}
	catch (const nlohmann::json::parse_error& exception)
	{
		std::cerr << "Parsing failed: " << exception.what() << std::endl;
		return false;
	}

	return true;
}

bool MungPlex::IEmulator::obtainGameEntities(const std::string& systemName, std::vector<GameEntity>& gameEntities)
{
	gameEntities.clear();
	std::string buffer;
	std::string jsonstr;
	std::ifstream inFile;
	inFile.open(GetResourcesFilePath("EntityInformation.json"));

	if (inFile)
		while (std::getline(inFile, buffer))
			jsonstr.append(buffer).append("\n");
	else
		return false;

	try
	{
		auto doc = nlohmann::json::parse(jsonstr);
		auto& entities = doc["Entities"][systemName];

		for (auto& gameEntity : entities)
		{
			std::string entity = gameEntity["Entity"].get<std::string>();
			uint64_t location = std::stoll(gameEntity["Location"].get<std::string>(), nullptr, 16);
			std::string datatype = gameEntity["Datatype"].get<std::string>();
			int size = std::stoi(gameEntity["Size"].get<std::string>(), nullptr, 10);
			bool hex = gameEntity["Hex"].get<bool>();
			gameEntities.emplace_back(GameEntity(entity, location, datatype, size, hex));
		}

		if (entities.empty())
			return false;
	}
	catch (const nlohmann::json::parse_error& exception)
	{
		std::cerr << "Parsing failed: " << exception.what() << std::endl;
		return false;
	}

	for (auto& [Entity, Location, Datatype, Size, Hex, Value] : gameEntities)
	{
		static char buffer[2048];
		std::stringstream stream;
		const int size = Size;
		const bool hex = Hex;
		std::string entityValue;

		if (Datatype.compare("INT") == 0)
		{
			uint64_t tempVal = ProcessInformation::ReadValue<uint64_t>(Location);
			tempVal >>= 64 - (8 * size);

			if (hex)
				stream << "0x" << std::hex << tempVal;
			else
				stream << tempVal;

			entityValue = stream.str();
		}
		else if (Datatype.compare("TEXT") == 0)
		{
			entityValue.resize(size);

			for (int x = 0; x < size; ++x)
				entityValue[x] = ProcessInformation::ReadValue<char>(Location + x);
		}
		else if (Datatype.compare("BIN") == 0)
		{
			uint8_t temp = 0;

			for (int x = 0; x < size; ++x)
			{
				temp = ProcessInformation::ReadValue<uint8_t>(Location + x);
				entityValue.append(ToHexString(temp, 2));
			}
		}

		Value = entityValue;
	}
}