#include"Settings.h"
#include "Log.h"
#include <shlobj_core.h>
#include"MungPlexConfig.h"

MungPlex::Settings::Settings()
{
	_styles.emplace_back("ImGui Default", ColorScheme());
	_styles.push_back(std::make_pair("Dark", []() -> ColorScheme
	{
		ColorScheme dark;
		constexpr ImVec4 topNormal = { 0.40f, 0.2f, 0.8f, 1.0f };
		constexpr ImVec4 topHovered = { 0.5f, 0.25f, 0.85f, 1.0f };
		constexpr ImVec4 topActive = { 0.6f, 0.3f, 0.9f, 1.0f };
		constexpr ImVec4 midNormal = { 0.2f, 0.17f, 0.35f, 1.0f };
		constexpr ImVec4 midHovered = { 0.25f, 0.19f, 0.4f, 1.0f };
		constexpr ImVec4 midActive = { 0.3f, 0.2f, 0.45f, 1.0f };

		dark.Background = { 0.1f, 0.1f, 0.1f, 1.0f };
		dark.Text = { 1.0f, 1.0f, 1.0f, 1.0f };
		dark.TextDisabled = { 0.5f, 0.5f, 0.5f, 1.0f };
		dark.ChildBG = { 0.12f, 0.12f, 0.12f, 1.0f };
		dark.PopUpBG = { 0.15f, 0.15f, 0.15f, 1.0f };
		dark.FrameBG = midNormal;
		dark.FrameBGHovered = midHovered;
		dark.FrameBGActive = midActive;
		dark.TitleBG = topNormal;
		dark.TitleBGActive = topActive;
		dark.TitleBGCollapsed = midNormal;
		dark.ScrollbarBg = midNormal;
		dark.ScrollbarGrab = topNormal;
		dark.ScrollbarGrabHovered = topHovered;
		dark.ScrollbarGrabActive = topActive;
		dark.CheckMark = { 0.8f, 0.8f, 0.8f, 1.0f };
		dark.SliderGrab = topNormal;
		dark.SliderGrabActive = topActive;
		dark.Button = dark.TitleBG;
		dark.ButtonHovered = topHovered;
		dark.ButtonActive = topActive;
		dark.Header = topNormal;
		dark.HeaderHovered = topHovered;
		dark.HeaderActive = topActive;
		dark.Separator = { 0.3f, 0.3f, 0.3f, 1.0f };
		dark.SeparatorHovered = { 0.4f, 0.4f, 0.4f, 1.0f };
		dark.SeparatorActive = { 0.5f, 0.5f, 0.5f, 1.0f };
		dark.ResizeGrip = topNormal;
		dark.ResizeGripHovered = topHovered;
		dark.ResizeGripActive = topActive;
		dark.Tab = { 0.15f, 0.15f, 0.3f, 1.0f };
		dark.TabHovered = { 0.25f, 0.25f, 0.4f, 1.0f };
		dark.TabActive = { 0.35f, 0.35f, 0.5f, 1.0f };
		dark.TabUnfocused = dark.Tab;
		dark.TabUnfocusedActive = dark.TabActive;
		dark.DockingPreview = topNormal;
		dark.DockingEmptyBG = midNormal;
		dark.WindowDim = { 0.10f, 0.08f, 0.2f, 0.5f };
		return dark;
	}()));
	/*_styles.push_back(std::make_pair("Candy", []() -> ColorScheme
	{
		ColorScheme candy;
		constexpr ImVec4 topNormal = { 0.93f, 0.63f, 0.69f, 1.0f }; 
		constexpr ImVec4 topHovered = { 0.97f, 0.67f, 0.75f, 1.0f };
		constexpr ImVec4 topActive = topHovered;

		candy.Background = { 0.35f, 0.8f, 0.98f, 1.0f };
		candy.Text = { 0.0f, 0.0f, 0.0f, 1.0f };
		candy.TextDisabled = { 0.5f, 0.5f, 0.5f, 1.0f };
		candy.ChildBG = { 0.27f, 0.72f, 0.93f, 1.0f };
		candy.PopUpBG = topNormal;
		candy.FrameBG = { 0.95f, 0.95f, 0.95f, 1.0f };
		candy.FrameBGHovered = { 1.0f, 1.0f, 1.0f, 1.0f };;
		candy.FrameBGActive = candy.FrameBGHovered;
		candy.TitleBG = topNormal;
		candy.TitleBGActive = topActive;
		candy.TitleBGCollapsed = { 0.5f, 0.5f, 0.5f, 1.0f };
		candy.ScrollbarBg = { 0.5f, 0.3f, 0.4f, 1.0f };
		candy.ScrollbarGrab = topNormal;
		candy.ScrollbarGrabHovered = topHovered;
		candy.ScrollbarGrabActive = topActive;
		candy.CheckMark = candy.Text;
		candy.SliderGrab = topNormal;
		candy.SliderGrabActive = topActive;
		candy.Button = topNormal;
		candy.ButtonHovered = topHovered;
		candy.ButtonActive = topActive;
		candy.Header = topNormal;
		candy.HeaderHovered = topHovered;
		candy.HeaderActive = topActive;
		candy.Separator = { 1.0f, 0.0f, 0.0f, 1.0f };
		candy.SeparatorHovered = { 0.0f, 1.0f, 0.0f, 1.0f };
		candy.SeparatorActive = { 0.0f, 0.0f, 1.0f, 1.0f };
		candy.ResizeGrip = topNormal;
		candy.ResizeGripHovered = topHovered;
		candy.ResizeGripActive = topActive;
		candy.Tab = topNormal;
		candy.TabHovered = candy.FrameBGHovered;
		candy.TabActive = candy.FrameBGHovered;
		candy.TabUnfocused = candy.Tab;
		candy.TabUnfocusedActive = candy.FrameBGHovered;
		candy.DockingPreview = topNormal;
		candy.DockingEmptyBG = candy.ScrollbarBg;
		candy.TableHeaderBg = topNormal;
		candy.WindowDim = { 0.0f, 0.0f, 0.8f, 0.5f };
		return candy;
	}()));*/

	//_styles.emplace_back("Bright", BRIGHT);
	//_styles.emplace_back("Pastell", IMGUIDEFAULT);


	bool save = false;
	_generalSettings.Windows.emplace_back("Search");
	_generalSettings.Windows.emplace_back("Cheats");
	_generalSettings.Windows.emplace_back("Pointer Search");
	_generalSettings.Windows.emplace_back("Process Information");
	_generalSettings.Windows.emplace_back("Settings");

	try
	{
		auto save = false;
		std::ifstream file_reader(GetResourcesFilePath("settings.json"));
		auto doc = nlohmann::json::parse(file_reader);
		auto& settings = doc["Settings"];

		//set general settings
		strcpy_s(_generalSettings.DocumentsPath, settings["General"]["DocumentsPath"].get<std::string>().c_str());
		_generalSettings.Scale = settings["General"]["Scale"].get<float>();
		_generalSettings.DefaultWindowSelect = settings["General"]["DefaultWindowSelect"].get<int>();
		_generalSettings.Style = settings["General"]["ColorScheme"].get<int>();

		if(std::filesystem::is_directory(_generalSettings.DocumentsPath))
			createDocFolders(); //ensure changes to docfolder also become updated
		else
		{
			auto tmp = new wchar_t[512];
			SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &tmp);
			strcpy_s(_generalSettings.DocumentsPath, MorphText::Utf16LE_To_Utf8(tmp).c_str());
			CoTaskMemFree(tmp);
			createDocFolders();
			save = true;
		}

		//set search defaults
		_searchSettings.DefaultCaseSensitive = settings["Search"]["DefaultCaseSensitive"].get<bool>();
		_searchSettings.DefaultColorWheel = settings["Search"]["DefaultColorWheel"].get<bool>();
		_searchSettings.DefaultValuesHex = settings["Search"]["DefaultValuesHex"].get<bool>();
		_searchSettings.DefaultCached = settings["Search"]["DefaultCached"].get<bool>();
		_searchSettings.DefaultAlignment = settings["Search"]["DefaultAlignment"].get<int>();

		if (_searchSettings.DefaultAlignment < 1)
			_searchSettings.DefaultAlignment = 1;
		
		//set cheats defaults
		_cheatsSettings.DefaultCheatList = settings["Cheats"]["DefaultCheatList"].get<bool>();
		_cheatsSettings.DefaultInterval = settings["Cheats"]["DefaultInterval"].get<int>();

		if (_cheatsSettings.DefaultInterval < 1)
			_cheatsSettings.DefaultInterval = 1;
		else if (_cheatsSettings.DefaultInterval > 240)
			_cheatsSettings.DefaultInterval = 240;

		if (save)
			saveSettings();
	}
	catch (const nlohmann::json::parse_error& exception)
	{
		Log::LogInformation((std::string("Failed parsing settings file: ") + exception.what()).c_str());
		return;
	}
}

MungPlex::ColorScheme& MungPlex::Settings::GetColorScheme(const int id)
{
	return GetInstance()._styles[id].second;
}

void MungPlex::Settings::DrawWindow()
{
	ImGui::Begin("Settings");
	
	GetInstance().drawGeneralSettings();
	GetInstance().drawSearchSettings();
	GetInstance().drawCheatSettings();
	ImGui::Separator();

	if (ImGui::Button("Save"))
	{
		GetInstance().saveSettings();
	}

	ImGui::SameLine();
	ImGui::Button("Reset");
	ImGui::SameLine();
	ImGui::Text("Changes take effect after restarting MungPlex.");
	ImGui::End();
}

void MungPlex::Settings::drawGeneralSettings()
{
	const ImVec2 childYX(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y * 0.5f);
	ImGui::BeginChild("child", childYX);
	{
		ImGui::SeparatorText("General Settings");
		
		SetUpInputText("Documents Path:", _generalSettings.DocumentsPath, 512, 1.0f, 0.2f, true, "Where you want MungPlex to save data. An SSD is recommended for fast memory dump streeaming. Changes take after after restarting.");
		SetUpSliderFloat("UI Scale:", &_generalSettings.Scale, 0.65f, 2.0f, "%3f", 1.0f, 0.2f, true, "If the UI looks off you can change the scale. Changes take after after restarting.");
		SetUpCombo("Default Active Window", _generalSettings.Windows, _generalSettings.DefaultWindowSelect, 1.0f, 0.2f, true, "Window to be active on startup (Search, Cheats, ...). Changes take after after restarting.");
		SetUpCombo("Color Theme:", _styles, _generalSettings.Style, 1.0f, 0.2f, true, "Select UI Color Theme. Changes take after after restarting.");
	}
	ImGui::EndChild();
}

void MungPlex::Settings::drawSearchSettings()
{
	const ImVec2 childYX(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y * 0.5f);
	ImGui::BeginChild("child_SearchSettings", childYX);
	{
		ImGui::SeparatorText("Search Settings");
		ImGui::Checkbox("Case Senstive by default", &_searchSettings.DefaultCaseSensitive);
		ImGui::Checkbox("Color Wheel by default", &_searchSettings.DefaultColorWheel);

		if (SetUpInputInt("Default Alignment:", &_searchSettings.DefaultAlignment, 1, 1, 1.0f, 0.2f))
		{
			if (_searchSettings.DefaultAlignment < 1)
				_searchSettings.DefaultAlignment = 1;
		}

		ImGui::Checkbox("Values are hex by default", &_searchSettings.DefaultValuesHex);
		ImGui::Checkbox("Cached Searches by default", &_searchSettings.DefaultCached);
		ImGui::SameLine();
		HelpMarker("Whether search results shall be kept in RAM (uses more RAM, but faster) or saved to local starage (uses less RAM but slower).");
	}
	ImGui::EndChild();
}

void MungPlex::Settings::drawCheatSettings()
{
	const ImVec2 childYX(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y * 0.6f);
	ImGui::BeginChild("child_CheatSettings", childYX);
	{
		ImGui::SeparatorText("Cheats Settings");

		ImGui::Checkbox("Cheat List by default", &_cheatsSettings.DefaultCheatList);

		if (SetUpInputInt("Default Cheat interval", &_cheatsSettings.DefaultInterval, 1, 10, 1.0f, 0.2f))
		{
			if (_cheatsSettings.DefaultInterval < 1)
				_cheatsSettings.DefaultInterval = 1;
			else if (_cheatsSettings.DefaultInterval > 240)
				_cheatsSettings.DefaultInterval = 240;
		}
	}
	ImGui::EndChild();
}

bool MungPlex::Settings::saveSettings()
{
	std::ofstream file(GetResourcesFilePath("settings.json"), std::ios::binary);
	const bool isOpen = file.is_open();

	if (isOpen)
	{
		nlohmann::json jsonData;
		nlohmann::json jsonChunk;

		jsonChunk["DocumentsPath"] = _generalSettings.DocumentsPath;
		if (!std::filesystem::is_directory(_generalSettings.DocumentsPath))
		{
			auto path = new wchar_t[256];
			wcscpy(path, MorphText::Utf8_To_Utf16LE(_generalSettings.DocumentsPath).c_str());

			if (!SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &path)))
			{
				strcpy_s(_generalSettings.DocumentsPath, MorphText::Utf16LE_To_Utf8(path).c_str());
			}

			CoTaskMemFree(path);
		}

		createDocFolders();

		jsonChunk["Scale"] = _generalSettings.Scale;
		jsonChunk["DefaultWindowSelect"] = _generalSettings.DefaultWindowSelect;
		jsonChunk["ColorScheme"] = _generalSettings.Style;
		jsonData["Settings"]["General"] = jsonChunk;
		jsonChunk.clear();

		jsonChunk["DefaultCaseSensitive"] = _searchSettings.DefaultCaseSensitive;
		jsonChunk["DefaultColorWheel"] = _searchSettings.DefaultColorWheel;
		jsonChunk["DefaultAlignment"] = _searchSettings.DefaultAlignment;
		jsonChunk["DefaultValuesHex"] = _searchSettings.DefaultValuesHex;
		jsonChunk["DefaultCached"] = _searchSettings.DefaultCached;
		jsonData["Settings"]["Search"] = jsonChunk;
		jsonChunk.clear();

		jsonChunk["DefaultCheatList"] = _cheatsSettings.DefaultCheatList;
		jsonChunk["DefaultInterval"] = _cheatsSettings.DefaultInterval;
		jsonData["Settings"]["Cheats"] = jsonChunk;

		file << "\xEF\xBB\xBF"; //write BOM
		file << jsonData.dump(2);
		Log::LogInformation("Settings saved");
		return true;
	}
	
	Log::LogInformation("Unable to save settings");
	return false;
}

void MungPlex::Settings::createDocFolders() 
{
	std::string mungPlexDocsPath = std::string(_generalSettings.DocumentsPath) + "\\MungPlex";
	validateDir(mungPlexDocsPath);
	validateDir(mungPlexDocsPath + "\\Search");
	validateDir(mungPlexDocsPath + "\\Dumps");
	validateDir(mungPlexDocsPath + "\\PointerSearch");
	validateDir(mungPlexDocsPath + "\\Cheats");
	validateDir(mungPlexDocsPath + "\\Cheats\\Nintendo 64");
	validateDir(mungPlexDocsPath + "\\Cheats\\SNES");
	validateDir(mungPlexDocsPath + "\\Cheats\\GameCube");
	validateDir(mungPlexDocsPath + "\\Cheats\\Wii");
	validateDir(mungPlexDocsPath + "\\Cheats\\Wii U");
	validateDir(mungPlexDocsPath + "\\Cheats\\PC");
	validateDir(mungPlexDocsPath + "\\Cheats\\NDS");
	validateDir(mungPlexDocsPath + "\\Cheats\\Switch");
	validateDir(mungPlexDocsPath + "\\Cheats\\PSP");
	validateDir(mungPlexDocsPath + "\\Cheats\\PS1");
	validateDir(mungPlexDocsPath + "\\Cheats\\PS2");
	validateDir(mungPlexDocsPath + "\\Cheats\\PS3");
	validateDir(mungPlexDocsPath + "\\Cheats\\Mega Drive");
	validateDir(mungPlexDocsPath + "\\Cheats\\Mega-CD");
	validateDir(mungPlexDocsPath + "\\Cheats\\32X");
}

void MungPlex::Settings::validateDir(const std::string dir)
{
	if (!std::filesystem::is_directory(dir))
		std::filesystem::create_directory(dir);
}

MungPlex::GeneralSettings& MungPlex::Settings::GetGeneralSettings()
{
	return GetInstance()._generalSettings;
}

MungPlex::SearchSettings& MungPlex::Settings::GetSearchSettings()
{
	return GetInstance()._searchSettings;
}

MungPlex::CheatsSettings& MungPlex::Settings::GetCheatsSettings()
{
	return GetInstance()._cheatsSettings;
}
