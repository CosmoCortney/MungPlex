#include <boost/filesystem.hpp>
#include "Lime3DS_Impl.hpp"
#include "../../Log.hpp"
#include <boost/iostreams/device/mapped_file.hpp>
#include "Settings.hpp"

bool MungPlex::Lime3DS::Init(const Xertz::ProcessInfo& process, std::vector<GameEntity>& gameEntities, std::vector<SystemRegion>& systemRegions)
{
	ProcessInformation::SetMiscProcessInfo("Lime3DS", false, false, 4, 4);
	boost::filesystem::path gameInfoPath(std::string(Settings::GetGeneralSettings().DocumentsPath) + "\\MungPlex\\CurrentGame.json");

	if (!process.IsOpen())
		return false;

	if (!boost::filesystem::exists(gameInfoPath))
	{
		std::cerr << "File does not exist: " << gameInfoPath << std::endl;
		return false;
	}

	boost::iostreams::mapped_file_source gameInfoFile;
	gameInfoFile.open(gameInfoPath);

	if (!gameInfoFile.is_open())
	{
		std::cerr << "Failed to open file: " << gameInfoPath << std::endl;
		return false;
	}

	std::string gameInfo(gameInfoFile.data(), gameInfoFile.size());
	gameInfoFile.close();

	try
	{
		nlohmann::json json = nlohmann::json::parse(gameInfo);
		_gameName = json["Title"];
		_gameID = json["TitleID"];
		_gameRegion = json["Region"];
		_rpcGameID = _gameID;
		_platformID = ProcessInformation::N3DS;

		std::vector<std::string> basePtrs = json["BasePointers"];
		std::vector<std::string> bases = json["Bases"];
		std::vector<std::string> sizes = json["Sizes"];

		for (int i = 0; i < basePtrs.size(); ++i)
		{
			SystemRegion region;
			region.Label = std::string("FCRAM ").append(std::to_string(i + 1));
			region.Base = std::strtoull(bases[i].c_str(), NULL, 16);
			region.BaseLocationProcess = reinterpret_cast<void*>(std::strtoull(basePtrs[i].c_str(), NULL, 16));
			region.Size = std::strtoull(sizes[i].c_str(), NULL, 16);
			systemRegions.push_back(region);
		}
	}
	catch (const nlohmann::json::parse_error& exception)
	{
		Log::LogInformation((std::string("Failed parsing settings file: ") + exception.what()).c_str());
		return false;
	}

	return true;
}
