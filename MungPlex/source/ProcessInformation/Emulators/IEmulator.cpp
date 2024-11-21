#include "IEmulator.hpp"

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

			int lj = base < 0x100000000 ? 8 : 16;
			label.append(": ");
			label.append(ToHexString(base, lj, false));
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

	return true;
}