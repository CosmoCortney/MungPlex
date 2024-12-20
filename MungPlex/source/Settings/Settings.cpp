#include "Settings.hpp"
#include "../../Log.hpp"
#include "MungPlexConfig.hpp"
#include <shlobj_core.h>

inline const MungPlex::DoubleStringIdPairs MungPlex::Settings::_colorSettings =
{
	{ "Text",        "TextDisabled",        "TextSelectedBg",           "Background",      "ChildBG",          "PopupBG",          "FrameBG",          "FrameBGHovered",           "FrameBGActive",           "TitleBG",              "TitleBGActive",               "TitleBGCollapsed",               "ScrollbarBg",          "ScrollbarGrab",        "ScrollbarGrabHovered",        "ScrollbarGrabActive",        "CheckMark",        "SliderGrab",        "SliderGrabActive",        "Button",        "ButtonHovered",        "ButtonActive",        "Header",        "HeaderHovered",        "HeaderActive",        "Separator",	    "SeparatorHovered",       "SeparatorActive",         "ResizeGrip",        "ResizeGripHovered",        "ResizeGripActive",        "Tab",        "TabHovered",        "TabActive",        "TabUnfocused",        "TabUnfocusedActive",        "DockingPreview",        "DockingEmptyBG",           "TableHeaderBg",           "TableBorderStrong",         "TableBorderLight",        "TableRowBg",           "TableRowBgAlt",                  "WindowDim",                "PlotLines",        "PlotLinesHovered",        "PlotHistogram",        "PlotHistogramHovered" },
	{ "Text",        "Text Disabled",       "Selected Text Background", "Background",      "Panel Background", "Popup Background", "Frame Background", "Frame Background Hovered", "Frame Background Active", "Title Bar Background", "Title Bar Background Active", "Title Bar Background Collapsed", "Scrollbar Background", "Scrollbar Grab",       "Scrollbar Grab Hovered",      "Scrollbar Grab Active",      "Check Mark",       "Slider Grab",       "Slider Grab Active",      "Button",        "Button Hovered",       "Button Active",       "Header" ,       "Header Hovered",       "Header Active",       "Separator" ,       "Separator Hovered",	  "Separator Active",        "Grip Resize",       "Grip Resize Hovered",      "Grip Resize Active",      "Tab",        "Tab Hovered",       "Tab Active",       "Tab Unfocused",       "Tab Unfocused Active",      "Docking Preview",       "Docking Empty Background", "Table Header Background", "Table Border Strong",       "Table Border Light" ,     "Table Row Background", "Table Row Background Alternate", "Window Dim",               "Plot Lines",       "Plot Lines Hovered",      "Plot Histogram",       "Plot Histogram Hovered" },
	{ ImGuiCol_Text, ImGuiCol_TextDisabled, ImGuiCol_TextSelectedBg,    ImGuiCol_WindowBg, ImGuiCol_ChildBg,   ImGuiCol_PopupBg,   ImGuiCol_FrameBg,   ImGuiCol_FrameBgHovered,    ImGuiCol_FrameBgActive, 	  ImGuiCol_TitleBg,       ImGuiCol_TitleBgActive,         ImGuiCol_TitleBgCollapsed,       ImGuiCol_ScrollbarBg,   ImGuiCol_ScrollbarGrab, ImGuiCol_ScrollbarGrabHovered, ImGuiCol_ScrollbarGrabActive, ImGuiCol_CheckMark,	ImGuiCol_SliderGrab, ImGuiCol_SliderGrabActive, ImGuiCol_Button, ImGuiCol_ButtonHovered, ImGuiCol_ButtonActive, ImGuiCol_Header, ImGuiCol_HeaderHovered, ImGuiCol_HeaderActive, ImGuiCol_Separator,	ImGuiCol_SeparatorHovered, ImGuiCol_SeparatorActive, ImGuiCol_ResizeGrip, ImGuiCol_ResizeGripHovered, ImGuiCol_ResizeGripActive, ImGuiCol_Tab, ImGuiCol_TabHovered, ImGuiCol_TabActive, ImGuiCol_TabUnfocused, ImGuiCol_TabUnfocusedActive, ImGuiCol_DockingPreview, ImGuiCol_DockingEmptyBg,    ImGuiCol_TableHeaderBg, 	 ImGuiCol_TableBorderStrong, ImGuiCol_TableBorderLight, ImGuiCol_TableRowBg,    ImGuiCol_TableRowBgAlt,            ImGuiCol_ModalWindowDimBg, ImGuiCol_PlotLines, ImGuiCol_PlotLinesHovered, ImGuiCol_PlotHistogram, ImGuiCol_PlotHistogramHovered },
	"Set Color:"
};


MungPlex::Settings::Settings()
{
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

	_documentsPathInput.SetHelpText("Where you want MungPlex to save data. An SSD is recommended for fast memory dump streeaming. Changes take effect after restarting!", true);
}

void MungPlex::Settings::InitSettings()
{
	bool save = false;
	GeneralSettings& generalSettings = GetInstance()._generalSettings;
	SearchSettings& searchSettings = GetInstance()._searchSettings;
	CheatsSettings& cheatsSettings = GetInstance()._cheatsSettings;
	DeviceControlSettings& deviceControlSettings = GetInstance()._deviceControlSettings;

	try
	{
		auto save = false;
		std::ifstream file_reader(GetResourcesFilePath("settings.json"));
		auto doc = nlohmann::json::parse(file_reader);
		auto& settings = doc["Settings"];

		//set general settings
		generalSettings.DocumentsPath = settings["General"]["DocumentsPath"].get<std::string>();
		generalSettings.Scale = settings["General"]["Scale"].get<float>();
		generalSettings.DefaultWindowSelect = settings["General"]["DefaultWindowSelect"].get<int>();

		if (settings["General"].contains("EnableRichPresence"))
			generalSettings.EnableRichPresence = settings["General"]["EnableRichPresence"].get<bool>();
		else
			generalSettings.EnableRichPresence = false;


		GetInstance().setUi(settings["General"]);


		if (std::filesystem::is_directory(generalSettings.DocumentsPath.StdStrNoLeadinZeros()))
			GetInstance().createDocFolders(); //ensure changes to docfolder also become updated
		else
		{
			auto tmp = new wchar_t[512];
			SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &tmp);
			generalSettings.DocumentsPath = MT::Convert<wchar_t*, std::string>(tmp, MT::UTF16LE, MT::UTF8);
			CoTaskMemFree(tmp);
			GetInstance().createDocFolders();
			save = true;
		}

		GetInstance()._documentsPathInput.SetText(generalSettings.DocumentsPath.StdStrNoLeadinZeros());

		//set search defaults
		searchSettings.DefaultCaseSensitive = settings["Search"]["DefaultCaseSensitive"].get<bool>();
		searchSettings.DefaultColorWheel = settings["Search"]["DefaultColorWheel"].get<bool>();
		searchSettings.DefaultValuesHex = settings["Search"]["DefaultValuesHex"].get<bool>();
		searchSettings.DefaultCached = settings["Search"]["DefaultCached"].get<bool>();
		searchSettings.DefaultDisableUndo = settings["Search"]["DefaultDisableUndo"].get<bool>();
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

		//set device control defaults
		deviceControlSettings.LovenseToken = settings["DeviceControl"]["LovenseToken"].get<std::string>();

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
		static ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;

		if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
		{
			GetInstance().drawGeneralSettings();
			GetInstance().drawSearchSettings();
			GetInstance().drawCheatSettings();
			GetInstance().drawDeviceControlSettings();
			ImGui::EndTabBar();
		}

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
		ImGui::Dummy(ImVec2(0.0f, 30.0f));
	}
	ImGui::End();
}

void MungPlex::Settings::drawGeneralSettings()
{
	const ImVec2 childYX(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y * 0.5f);

	if (ImGui::BeginTabItem("General"))
	{
		ImGui::Dummy(ImVec2(0.0f, 5.0f));
		static ImGuiStyle& style = ImGui::GetStyle();

		ImGui::BeginGroup();
		{
			static ImVec4 color = style.Colors[ImGuiCol_Text];
			static int colorSelect = ImGuiCol_Text;

			if (SetUpPairCombo(_colorSettings, &colorSelect, 0.2f, 0.5f))
				color = style.Colors[_colorSettings.GetId(colorSelect)];

			ImGui::PushItemWidth(childYX.x * 0.2f);
			ImGui::ColorPicker4("##ColorPickerUi", (float*)&color, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
			style.Colors[_colorSettings.GetId(colorSelect)] = color;
		}
		ImGui::EndGroup();

		ImGui::SameLine();
		ImGui::Dummy(ImVec2(5.0f, 0.0f));
		ImGui::SameLine();

		ImGui::BeginGroup();
		{
			if (_documentsPathInput.Draw(1.0f, 0.2f))
				_generalSettings.DocumentsPath = _documentsPathInput.GetStdStringNoZeros();

			SetUpSliderFloat("UI Scale:", &_generalSettings.Scale, 0.65f, 2.0f, "%3f", 1.0f, 0.2f, true, "If the UI looks off you can change the scale. Changes take effect after restarting!");

			//ImGui::SameLine();
			
			//ImGui::PushItemWidth(childYX.x * 0.1f);
			//if (ImGui::Button("Apply Scale"))
			//	style.ScaleAllSizes(_generalSettings.Scale);

			SetUpPairCombo(_generalSettings.Windows, &_generalSettings.DefaultWindowSelect, 1.0f, 0.2f, true, "Window to be active on startup (Search, Cheats, ...).");
		
		if (ImGui::Checkbox("Enable Discord Rich Presence", &_generalSettings.EnableRichPresence))
			if (Connection::IsConnected() && _generalSettings.EnableRichPresence)
				Connection::GetDiscordRichPresence().InitRichPresence();
			else
				Connection::GetDiscordRichPresence().StopRichPresence();
		}
		ImGui::EndGroup();
		ImGui::EndTabItem();
	}
}

void MungPlex::Settings::drawSearchSettings()
{
	const ImVec2 childYX(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y * 0.5f);

	if (ImGui::BeginTabItem("Search"))
	{
		ImGui::Dummy(ImVec2(0.0f, 5.0f));
		ImGui::Checkbox("Case Senstive by default", &_searchSettings.DefaultCaseSensitive);
		ImGui::Checkbox("Color Wheel by default", &_searchSettings.DefaultColorWheel);

		if (SetUpInputInt("Default Alignment:", &_searchSettings.DefaultAlignment, 1, 1, 1.0f, 0.2f))
		{
			if (_searchSettings.DefaultAlignment < 1)
				_searchSettings.DefaultAlignment = 1;
		}

		ImGui::Checkbox("Values are hex by default", &_searchSettings.DefaultValuesHex);
		ImGui::Checkbox("Cached Searches by default", &_searchSettings.DefaultCached);
		ImGui::Checkbox("Disable Undo by default", &_searchSettings.DefaultDisableUndo);
		ImGui::SameLine();
		HelpMarker("Whether search results shall be kept in RAM (uses more RAM, but faster) or saved to local starage (uses less RAM but slower).");
		ImGui::EndTabItem();
	}
}

void MungPlex::Settings::drawCheatSettings()
{
	const ImVec2 childYX(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y * 0.5f);
	
	if (ImGui::BeginTabItem("Cheats"))
	{
		ImGui::Dummy(ImVec2(0.0f, 5.0f));
		ImGui::Checkbox("Cheat List by default", &_cheatsSettings.DefaultCheatList);

		if (SetUpInputInt("Default Cheat interval", &_cheatsSettings.DefaultInterval, 1, 10, 1.0f, 0.2f))
		{
			if (_cheatsSettings.DefaultInterval < 1)
				_cheatsSettings.DefaultInterval = 1;
			else if (_cheatsSettings.DefaultInterval > 240)
				_cheatsSettings.DefaultInterval = 240;
		}
		ImGui::EndTabItem();
	}
}

void MungPlex::Settings::drawDeviceControlSettings()
{
	const ImVec2 childYX(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y * 0.8f);

	if (ImGui::BeginTabItem("Device Control"))
	{
		ImGui::Dummy(ImVec2(0.0f, 5.0f));
		SetUpInputText("Lovense Token", _deviceControlSettings.LovenseToken.Data(), _deviceControlSettings.LovenseToken.Size(), 1.0f, 0.2f);
		ImGui::EndTabItem();
	}
}

bool MungPlex::Settings::saveSettings()
{
	std::ofstream file(GetResourcesFilePath("settings.json"), std::ios::binary);
	const bool isOpen = file.is_open();

	if (isOpen)
	{
		nlohmann::json jsonData;
		nlohmann::json jsonChunk;

		jsonChunk["DocumentsPath"] = _generalSettings.DocumentsPath.StdStrNoLeadinZeros();
		if (!std::filesystem::is_directory(_generalSettings.DocumentsPath.StdStrNoLeadinZeros()))
		{
			auto path = new wchar_t[256];
			wcscpy(path, MT::Convert<std::string, std::wstring>(_generalSettings.DocumentsPath.StdStrNoLeadinZeros(), MT::UTF8, MT::UTF16LE).c_str());

			if (!SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &path)))
			{
				_generalSettings.DocumentsPath = MT::Convert<wchar_t*, std::string>(path, MT::UTF16LE, MT::UTF8);
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
		jsonChunk["DefaultDisableUndo"] = _searchSettings.DefaultDisableUndo;
		jsonData["Settings"]["Search"] = jsonChunk;
		jsonChunk.clear();

		jsonChunk["DefaultCheatList"] = _cheatsSettings.DefaultCheatList;
		jsonChunk["DefaultInterval"] = _cheatsSettings.DefaultInterval;
		jsonData["Settings"]["Cheats"] = jsonChunk;
		jsonChunk.clear();

		jsonChunk["LovenseToken"] = _deviceControlSettings.LovenseToken.StdStrNoLeadinZeros();
		jsonData["Settings"]["DeviceControl"] = jsonChunk;

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
	std::string mungPlexDocsPath = _generalSettings.DocumentsPath.StdStrNoLeadinZeros() + R"(\MungPlex)";
	validateDir(mungPlexDocsPath);
	
	for (auto& dir : _featureDirs)
	{
		validateDir(mungPlexDocsPath + '\\' + dir);

		if (dir.compare("Search") == 0) // dont consider this one for system-specific subdirs
			continue;

		for(int i = 0; i < ProcessInformation::GetSystemList().GetCount(); ++i)
			validateDir(mungPlexDocsPath + '\\' + dir + '\\' + ProcessInformation::GetSystemList().GetStdString(i));
	}
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

MungPlex::DeviceControlSettings& MungPlex::Settings::GetDeviceControlSettings()
{
	return GetInstance()._deviceControlSettings;
}

void MungPlex::Settings::setUi(const nlohmann::json& uiJson)
{
	ImGuiStyle& style = ImGui::GetStyle();

	if (!uiJson.contains("Colors"))
	{
		style = _defaultStyle;
		return;
	}

	if(uiJson["Colors"].size() < _colorSettings.GetCount())
	{
		style = _defaultStyle;
		return;
	}

	for (int i = 0; i < _colorSettings.GetCount(); ++i)
	{
		const auto& entity = _colorSettings.GetStdStringEntity(i);

		if (uiJson["Colors"].contains(entity))
		{
			style.Colors[_colorSettings.GetId(i)] = getColorVec(uiJson["Colors"][entity][0]);
		}
	}
}

void MungPlex::Settings::resetSettings()
{
	ImGuiStyle& style = ImGui::GetStyle();
	style = _defaultStyle;
	auto path = new wchar_t[256];
	wcscpy(path, MT::Convert<std::string, std::wstring>(_generalSettings.DocumentsPath.StdStrNoLeadinZeros(), MT::UTF8, MT::UTF16LE).c_str());
	_generalSettings.DocumentsPath = MT::Convert<wchar_t*, std::string>(path, MT::UTF16LE, MT::UTF8);
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

	for (int i = 0; i < _colorSettings.GetCount(); ++i)
	{
		const std::string& identifier = _colorSettings.GetStdStringEntity(i);
		const ImVec4& color = style.Colors[_colorSettings.GetId(i)];
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