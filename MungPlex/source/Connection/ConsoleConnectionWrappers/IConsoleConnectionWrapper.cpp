#include "IConsoleConnectionWrapper.hpp"
#include <StringHelpers.hpp>

bool MungPlex::IConsoleConnectionWrapper::obtainGameEntities(const std::string& systemName, std::vector<GameEntity>& gameEntities, char* memDump, const bool bigEndian, const uint64_t baseAddress)
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
			uint64_t tempVal = std::byteswap<uint64_t>(*reinterpret_cast<uint64_t*>(memDump + Location - baseAddress));
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
			std::memcpy(entityValue.data(), memDump + Location - baseAddress, size);
		}
		else if (Datatype.compare("BIN") == 0)
		{
			uint8_t temp = 0;

			for (int x = 0; x < size; ++x)
			{
				temp = *(memDump + Location - baseAddress + x);
				entityValue.append(ToHexString(temp, 2));
			}
		}

		Value = entityValue;
	}

	return true;
}