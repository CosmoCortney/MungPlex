#include"Settings.h"
#include "Log.h"
#include <shlobj_core.h>
#include"MungPlexConfig.h"

MungPlex::Settings::Settings()
{
	_colorSettings =
	{
		{ ImGuiCol_Text, { "Text", "Text" }},
		{ ImGuiCol_TextDisabled, { "TextDisabled", "Text Disabled" }},
		{ ImGuiCol_TextSelectedBg, { "TextSelectedBg", "Selected Text Background" }},
		{ ImGuiCol_WindowBg, { "Background", "Background" }},
		{ ImGuiCol_ChildBg, { "ChildBG", "Panel Background" }},
		{ ImGuiCol_PopupBg, { "PopupBG", "Popup Background" }},
		{ ImGuiCol_FrameBg, { "FrameBG", "Frame Background" }},
		{ ImGuiCol_FrameBgHovered, { "FrameBGHovered", "Frame Background Hovered" }},
		{ ImGuiCol_FrameBgActive, { "FrameBGActive", "Frame Background Active" }},
		{ ImGuiCol_TitleBg, { "TitleBG", "Title Bar Background" }},
		{ ImGuiCol_TitleBgActive, { "TitleBGActive", "Title Bar Background Active" }},
		{ ImGuiCol_TitleBgCollapsed, { "TitleBGCollapsed", "Title Bar Background Collapsed" }},
		{ ImGuiCol_ScrollbarBg, { "ScrollbarBg", "Scrollbar Background" }},
		{ ImGuiCol_ScrollbarGrab, { "ScrollbarGrab", "Scrollbar Grab" }},
		{ ImGuiCol_ScrollbarGrabHovered, { "ScrollbarGrabHovered", "Scrollbar Grab Hovered" }},
		{ ImGuiCol_ScrollbarGrabActive, { "ScrollbarGrabActive", "Scrollbar Grab Active" }},
		{ ImGuiCol_CheckMark, { "CheckMark", "Check Mark" }},
		{ ImGuiCol_SliderGrab, { "SliderGrab", "Slider Grab" }},
		{ ImGuiCol_SliderGrabActive, { "SliderGrabActive", "Slider Grab Active" }},
		{ ImGuiCol_Button, { "Button", "Button" }},
		{ ImGuiCol_ButtonHovered, { "ButtonHovered", "Button Hovered" }},
		{ ImGuiCol_ButtonActive, { "ButtonActive", "Button Active" }},
		{ ImGuiCol_Header, { "Header", "Header" }},
		{ ImGuiCol_HeaderHovered, { "HeaderHovered", "Header Hovered" }},
		{ ImGuiCol_HeaderActive, { "HeaderActive", "Header Active" }},
		{ ImGuiCol_Separator, { "Separator", "Separator" }},
		{ ImGuiCol_SeparatorHovered, { "SeparatorHovered", "Separator Hovered" }},
		{ ImGuiCol_SeparatorActive, { "SeparatorActive", "Separator Active" }},
		{ ImGuiCol_ResizeGrip, { "ResizeGrip", "Resize Grip" }},
		{ ImGuiCol_ResizeGripHovered, { "ResizeGripHovered", "Resize Grip Hovered" }},
		{ ImGuiCol_ResizeGripActive, { "ResizeGripActive", "Resize Grip Active" }},
		{ ImGuiCol_Tab, { "Tab", "Tab" }},
		{ ImGuiCol_TabHovered, { "TabHovered", "Tab Hovered" }},
		{ ImGuiCol_TabActive, { "TabActive", "Tab Active" }},
		{ ImGuiCol_TabUnfocused, { "TabUnfocused", "Tab Unfocused" }},
		{ ImGuiCol_TabUnfocusedActive, { "TabUnfocusedActive", "Tab Unfocused Active" }},
		{ ImGuiCol_DockingPreview, { "DockingPreview", "Docking Preview" }},
		{ ImGuiCol_DockingEmptyBg, { "DockingEmptyBG", "Docking Empty Background" }},
		{ ImGuiCol_TableHeaderBg, { "TableHeaderBg", "Table Header Background" }},
		{ ImGuiCol_TableBorderStrong, { "TableBorderStrong", "Table Border Strong" }},
		{ ImGuiCol_TableBorderLight, { "TableBorderLight", "Table Border Light" }},
		{ ImGuiCol_TableRowBg, { "TableRowBg", "Table Row Background" }},
		{ ImGuiCol_TableRowBgAlt, { "TableRowBgAlt", "Table Row Background Alternate" }},
		{ ImGuiCol_ModalWindowDimBg, { "WindowDim", "Window Dim" }},
		{ ImGuiCol_PlotLines, { "PlotLines", "Plot Lines" }},
		{ ImGuiCol_PlotLinesHovered, { "PlotLinesHovered", "Plot Lines Hovered" }},
		{ ImGuiCol_PlotHistogram, { "PlotHistogram", "Plot Histogram" }},
		{ ImGuiCol_PlotHistogramHovered, { "PlotHistogramHovered", "Plot Histogram Hovered" }}
	};

	_defaultStyle.Colors[ImGuiCol_WindowBg] = { 0.1f, 0.1f, 0.1f, 1.0f };
	_defaultStyle.Colors[ImGuiCol_Text] = { 1.0f, 1.0f, 1.0f, 1.0f };
	_defaultStyle.Colors[ImGuiCol_TextDisabled] = { 0.5f, 0.5f, 0.5f, 1.0f };
	_defaultStyle.Colors[ImGuiCol_ChildBg] = { 0.12f, 0.12f, 0.12f, 1.0f };
	_defaultStyle.Colors[ImGuiCol_TableHeaderBg] = _defaultStyle.Colors[ImGuiCol_PopupBg] = { 0.15f, 0.15f, 0.15f, 1.0f };
	_defaultStyle.Colors[ImGuiCol_TitleBgCollapsed] = _defaultStyle.Colors[ImGuiCol_FrameBg] = { 0.2f, 0.17f, 0.35f, 1.0f };
	_defaultStyle.Colors[ImGuiCol_FrameBgHovered] = { 0.25f, 0.19f, 0.4f, 1.0f };
	_defaultStyle.Colors[ImGuiCol_FrameBgActive] = { 0.3f, 0.2f, 0.45f, 1.0f };
	_defaultStyle.Colors[ImGuiCol_ScrollbarGrab] =
		_defaultStyle.Colors[ImGuiCol_DockingPreview] =
		_defaultStyle.Colors[ImGuiCol_ResizeGrip] =
		_defaultStyle.Colors[ImGuiCol_Header] =
		_defaultStyle.Colors[ImGuiCol_SliderGrab] =
		_defaultStyle.Colors[ImGuiCol_Button] = 
		_defaultStyle.Colors[ImGuiCol_TitleBg] = { 0.40f, 0.2f, 0.8f, 1.0f };
	_defaultStyle.Colors[ImGuiCol_HeaderActive] =
		_defaultStyle.Colors[ImGuiCol_ScrollbarGrabActive] =
		_defaultStyle.Colors[ImGuiCol_ResizeGripActive] =
		_defaultStyle.Colors[ImGuiCol_ButtonActive] =
		_defaultStyle.Colors[ImGuiCol_SliderGrabActive] =
		_defaultStyle.Colors[ImGuiCol_TitleBgActive] = { 0.6f, 0.3f, 0.9f, 1.0f };
	_defaultStyle.Colors[ImGuiCol_ScrollbarBg] = _defaultStyle.Colors[ImGuiCol_FrameBg];
	_defaultStyle.Colors[ImGuiCol_ResizeGripHovered] =
		_defaultStyle.Colors[ImGuiCol_HeaderHovered] =
		_defaultStyle.Colors[ImGuiCol_ScrollbarGrabHovered] =
		_defaultStyle.Colors[ImGuiCol_ButtonHovered] = { 0.5f, 0.25f, 0.85f, 1.0f };
	_defaultStyle.Colors[ImGuiCol_CheckMark] = { 0.8f, 0.8f, 0.8f, 1.0f };
	_defaultStyle.Colors[ImGuiCol_Separator] = { 0.3f, 0.3f, 0.3f, 1.0f };
	_defaultStyle.Colors[ImGuiCol_SeparatorHovered] = { 0.4f, 0.4f, 0.4f, 1.0f };
	_defaultStyle.Colors[ImGuiCol_SeparatorActive] = { 0.5f, 0.5f, 0.5f, 1.0f };
	_defaultStyle.Colors[ImGuiCol_TabUnfocused] = _defaultStyle.Colors[ImGuiCol_Tab] = { 0.15f, 0.15f, 0.3f, 1.0f };
	_defaultStyle.Colors[ImGuiCol_TabHovered] = { 0.25f, 0.25f, 0.4f, 1.0f };
	_defaultStyle.Colors[ImGuiCol_TabUnfocusedActive] = _defaultStyle.Colors[ImGuiCol_TabActive] = { 0.35f, 0.35f, 0.5f, 1.0f };
	_defaultStyle.Colors[ImGuiCol_DockingEmptyBg] = _defaultStyle.Colors[ImGuiCol_FrameBg];
	_defaultStyle.Colors[ImGuiCol_ModalWindowDimBg] = { 0.10f, 0.08f, 0.2f, 0.5f };
	_defaultStyle.Colors[ImGuiCol_PlotHistogram] = _defaultStyle.Colors[ImGuiCol_PlotLines] = { 0.0f, 0.7f, 0.9f, 1.0f };
	_defaultStyle.Colors[ImGuiCol_PlotHistogramHovered] = _defaultStyle.Colors[ImGuiCol_PlotLinesHovered] = { 0.0f, 0.9f, 0.75f, 1.0f };
}

void MungPlex::Settings::InitSettings()
{
	bool save = false;
	GeneralSettings& generalSettings = GetInstance()._generalSettings;
	SearchSettings& searchSettings = GetInstance()._searchSettings;
	CheatsSettings& cheatsSettings = GetInstance()._cheatsSettings;

	generalSettings.Windows.emplace_back("Search");
	generalSettings.Windows.emplace_back("Cheats");
	generalSettings.Windows.emplace_back("Pointer Search");
	generalSettings.Windows.emplace_back("Process Information");
	generalSettings.Windows.emplace_back("Settings");

	try
	{
		auto save = false;
		std::ifstream file_reader(GetResourcesFilePath("settings.json"));
		auto doc = nlohmann::json::parse(file_reader);
		auto& settings = doc["Settings"];

		//set general settings
		strcpy_s(generalSettings.DocumentsPath, settings["General"]["DocumentsPath"].get<std::string>().c_str());
		generalSettings.Scale = settings["General"]["Scale"].get<float>();
		generalSettings.DefaultWindowSelect = settings["General"]["DefaultWindowSelect"].get<int>();

		if (settings["General"].contains("EnableRichPresence"))
			generalSettings.EnableRichPresence = settings["General"]["EnableRichPresence"].get<bool>();
		else
			generalSettings.EnableRichPresence = false;


		GetInstance().setUi(settings["General"]);


		if (std::filesystem::is_directory(generalSettings.DocumentsPath))
			GetInstance().createDocFolders(); //ensure changes to docfolder also become updated
		else
		{
			auto tmp = new wchar_t[512];
			SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &tmp);
			strcpy_s(generalSettings.DocumentsPath, MorphText::Utf16LE_To_Utf8(tmp).c_str());
			CoTaskMemFree(tmp);
			GetInstance().createDocFolders();
			save = true;
		}

		//set search defaults
		searchSettings.DefaultCaseSensitive = settings["Search"]["DefaultCaseSensitive"].get<bool>();
		searchSettings.DefaultColorWheel = settings["Search"]["DefaultColorWheel"].get<bool>();
		searchSettings.DefaultValuesHex = settings["Search"]["DefaultValuesHex"].get<bool>();
		searchSettings.DefaultCached = settings["Search"]["DefaultCached"].get<bool>();
		searchSettings.DefaultAlignment = settings["Search"]["DefaultAlignment"].get<int>();

		if (searchSettings.DefaultAlignment < 1)
			searchSettings.DefaultAlignment = 1;
		
		//set cheats defaults
		cheatsSettings.DefaultCheatList = settings["Cheats"]["DefaultCheatList"].get<bool>();
		cheatsSettings.DefaultInterval = settings["Cheats"]["DefaultInterval"].get<int>();

		if (cheatsSettings.DefaultInterval < 1)
			cheatsSettings.DefaultInterval = 1;
		else if (cheatsSettings.DefaultInterval > 240)
			cheatsSettings.DefaultInterval = 240;

		if (save)
			GetInstance().saveSettings();
	}
	catch (const nlohmann::json::parse_error& exception)
	{
		Log::LogInformation((std::string("Failed parsing settings file: ") + exception.what()).c_str());
		return;
	}
}

void MungPlex::Settings::DrawWindow()
{
	if (ImGui::Begin("Settings"))
	{
		GetInstance().drawGeneralSettings();
		GetInstance().drawSearchSettings();
		GetInstance().drawCheatSettings();
		ImGui::Separator();

		if (ImGui::Button("Save"))
		{
			GetInstance().saveSettings();
		}

		ImGui::SameLine();

		if (ImGui::Button("Reset"))
			GetInstance().resetSettings();

		ImGui::SameLine();
		ImGui::Text("Some changes take effect after restarting MungPlex (check (?))");
	}
	ImGui::End();
}

void MungPlex::Settings::drawGeneralSettings()
{
	const ImVec2 childYX(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y * 0.5f);
	ImGui::BeginChild("child", childYX);
	{
		ImGui::SeparatorText("General Settings");
		static ImGuiStyle& style = ImGui::GetStyle();

		ImGui::BeginGroup();
		{
			static ImVec4 color = style.Colors[ImGuiCol_Text];
			static int colorSelect = ImGuiCol_Text;

			if (SetUpCombo("Set Color:", _colorSettings, colorSelect, 0.2f, 0.5f))
				color = style.Colors[_colorSettings[colorSelect].first];

			ImGui::PushItemWidth(childYX.x * 0.2f);
			ImGui::ColorPicker4("##ColorPickerUi", (float*)&color, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
			style.Colors[_colorSettings[colorSelect].first] = color;
		}
		ImGui::EndGroup();

		ImGui::SameLine();
		ImGui::Dummy(ImVec2(5.0f, 0.0f));
		ImGui::SameLine();

		ImGui::BeginGroup();
		{
			SetUpInputText("Documents Path:", _generalSettings.DocumentsPath, 512, 1.0f, 0.2f, true, "Where you want MungPlex to save data. An SSD is recommended for fast memory dump streeaming. Changes take effect after restarting!");
			SetUpSliderFloat("UI Scale:", &_generalSettings.Scale, 0.65f, 2.0f, "%3f", 1.0f, 0.2f, true, "If the UI looks off you can change the scale. Changes take effect after restarting!");

			//ImGui::SameLine();
			
			//ImGui::PushItemWidth(childYX.x * 0.1f);
			//if (ImGui::Button("Apply Scale"))
			//	style.ScaleAllSizes(_generalSettings.Scale);

			SetUpCombo("Default Active Window", _generalSettings.Windows, _generalSettings.DefaultWindowSelect, 1.0f, 0.2f, true, "Window to be active on startup (Search, Cheats, ...).");
		
		if (ImGui::Checkbox("Enable Discord Rich Presence", &_generalSettings.EnableRichPresence))
			if (Connection::IsConnected() && _generalSettings.EnableRichPresence)
				Connection::InitRichPresence();
			else
				Connection::StopRichPresence();
		}
		ImGui::EndGroup();
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
		//jsonChunk["Colors"] = nlohmann::json::array();
		jsonChunk["Colors"] = generateColorsJson();
		jsonChunk["EnableRichPresence"] = _generalSettings.EnableRichPresence;
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
	validateDir(mungPlexDocsPath + "\\WatchControl");
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
	validateDir(mungPlexDocsPath + "\\WatchControl\\Nintendo 64");
	validateDir(mungPlexDocsPath + "\\WatchControl\\SNES");
	validateDir(mungPlexDocsPath + "\\WatchControl\\GameCube");
	validateDir(mungPlexDocsPath + "\\WatchControl\\Wii");
	validateDir(mungPlexDocsPath + "\\WatchControl\\Wii U");
	validateDir(mungPlexDocsPath + "\\WatchControl\\PC");
	validateDir(mungPlexDocsPath + "\\WatchControl\\NDS");
	validateDir(mungPlexDocsPath + "\\WatchControl\\Switch");
	validateDir(mungPlexDocsPath + "\\WatchControl\\PSP");
	validateDir(mungPlexDocsPath + "\\WatchControl\\PS1");
	validateDir(mungPlexDocsPath + "\\WatchControl\\PS2");
	validateDir(mungPlexDocsPath + "\\WatchControl\\PS3");
	validateDir(mungPlexDocsPath + "\\WatchControl\\Mega Drive");
	validateDir(mungPlexDocsPath + "\\WatchControl\\Mega-CD");
	validateDir(mungPlexDocsPath + "\\WatchControl\\32X");
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

void MungPlex::Settings::setUi(const nlohmann::json& uiJson)
{
	ImGuiStyle& style = ImGui::GetStyle();

	if (!uiJson.contains("Colors"))
	{
		style = _defaultStyle;
		return;
	}

	if(uiJson["Colors"].size() < _colorSettings.size())
	{
		style = _defaultStyle;
		return;
	}

	for (const auto& colorSetting : _colorSettings)
	{
		if (uiJson["Colors"].contains(colorSetting.second.first))
		{
			style.Colors[colorSetting.first] = getColorVec(uiJson["Colors"][colorSetting.second.first][0]);
		}
	}
}

void MungPlex::Settings::resetSettings()
{
	ImGuiStyle& style = ImGui::GetStyle();
	style = _defaultStyle;
	auto path = new wchar_t[256];
	wcscpy(path, MorphText::Utf8_To_Utf16LE(_generalSettings.DocumentsPath).c_str());
	strcpy_s(_generalSettings.DocumentsPath, MorphText::Utf16LE_To_Utf8(path).c_str());
	_generalSettings.Scale = 1.2f;
	_generalSettings.DefaultWindowSelect = 0;
	_generalSettings.EnableRichPresence = false;
	saveSettings();
}

ImVec4 MungPlex::Settings::getColorVec(const nlohmann::json& colorJson) const
{
	ImVec4 color;

	color.x = colorJson[0].get<float>();
	color.y = colorJson[1].get<float>();
	color.z = colorJson[2].get<float>();
	color.w = colorJson[3].get<float>();

	return color;
}

nlohmann::json MungPlex::Settings::generateColorsJson() const
{
	nlohmann::json colors;
	ImGuiStyle& style = ImGui::GetStyle();

	for (auto& colorSetting : _colorSettings)
	{
		const std::string& identifier = colorSetting.second.first;
		const ImVec4& color = style.Colors[colorSetting.first];
		nlohmann::json colorArray = { imVec4ToStdVector(color) };
		colors[identifier] = colorArray;
	}

	return colors;
}


std::vector<float> MungPlex::Settings::imVec4ToStdVector(const ImVec4 imVec) const
{
	std::vector<float> vec;
	vec.push_back(imVec.x);
	vec.push_back(imVec.y);
	vec.push_back(imVec.z);
	vec.push_back(imVec.w);
	return vec;
}