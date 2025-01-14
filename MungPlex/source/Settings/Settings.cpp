#include "Settings.hpp"
#include "../../Log.hpp"
#include "MungPlexConfig.hpp"
#include <shlobj_core.h>

inline const std::vector<MungPlex::DoubleStringIdCombo::VecType> MungPlex::Settings::_colorSettings =
{
	{
		{ "Text", "Text", ImGuiCol_Text },
		{ "Text Disabled", "TextDisabled", ImGuiCol_TextDisabled },
		{ "Selected Text Background", "TextSelectedBg", ImGuiCol_TextSelectedBg },
		{ "Background", "Background", ImGuiCol_WindowBg },
		{ "Panel Background", "ChildBG", ImGuiCol_ChildBg },
		{ "Popup Background", "PopupBG", ImGuiCol_PopupBg },
		{ "Frame Background", "FrameBG", ImGuiCol_FrameBg },
		{ "Frame Background Hovered", "FrameBGHovered", ImGuiCol_FrameBgHovered },
		{ "Frame Background Active", "FrameBGActive", ImGuiCol_FrameBgActive },
		{ "Title Bar Background", "TitleBG", ImGuiCol_TitleBg },
		{ "Title Bar Background Active", "TitleBGActive", ImGuiCol_TitleBgActive },
		{ "Title Bar Background Collapsed", "TitleBGCollapsed", ImGuiCol_TitleBgCollapsed },
		{ "Scrollbar Background", "ScrollbarBg", ImGuiCol_ScrollbarBg },
		{ "Scrollbar Grab", "ScrollbarGrab", ImGuiCol_ScrollbarGrab },
		{ "Scrollbar Grab Hovered", "ScrollbarGrabHovered", ImGuiCol_ScrollbarGrabHovered },
		{ "Scrollbar Grab Active", "ScrollbarGrabActive", ImGuiCol_ScrollbarGrabActive },
		{ "Check Mark", "CheckMark", ImGuiCol_CheckMark },
		{ "Slider Grab", "SliderGrab", ImGuiCol_SliderGrab },
		{ "Slider Grab Active", "SliderGrabActive", ImGuiCol_SliderGrabActive },
		{ "Button", "Button", ImGuiCol_Button },
		{ "Button Hovered", "ButtonHovered", ImGuiCol_ButtonHovered },
		{ "Button Active", "ButtonActive", ImGuiCol_ButtonActive },
		{ "Header", "Header", ImGuiCol_Header },
		{ "Header Hovered", "HeaderHovered", ImGuiCol_HeaderHovered },
		{ "Header Active", "HeaderActive", ImGuiCol_HeaderActive },
		{ "Separator", "Separator", ImGuiCol_Separator },
		{ "Separator Hovered", "SeparatorHovered", ImGuiCol_SeparatorHovered },
		{ "Separator Active", "SeparatorActive", ImGuiCol_SeparatorActive },
		{ "Grip Resize", "ResizeGrip", ImGuiCol_ResizeGrip },
		{ "Grip Resize Hovered", "ResizeGripHovered", ImGuiCol_ResizeGripHovered },
		{ "Grip Resize Active", "ResizeGripActive", ImGuiCol_ResizeGripActive },
		{ "Tab", "Tab", ImGuiCol_Tab },
		{ "Tab Hovered", "TabHovered", ImGuiCol_TabHovered },
		{ "Tab Active", "TabActive", ImGuiCol_TabActive },
		{ "Tab Unfocused", "TabUnfocused", ImGuiCol_TabUnfocused },
		{ "Tab Unfocused Active", "TabUnfocusedActive", ImGuiCol_TabUnfocusedActive },
		{ "Docking Preview", "DockingPreview", ImGuiCol_DockingPreview },
		{ "Docking Empty Background", "DockingEmptyBG", ImGuiCol_DockingEmptyBg },
		{ "Table Header Background", "TableHeaderBg", ImGuiCol_TableHeaderBg },
		{ "Table Border Strong", "TableBorderStrong", ImGuiCol_TableBorderStrong },
		{ "Table Border Light", "TableBorderLight", ImGuiCol_TableBorderLight },
		{ "Table Row Background", "TableRowBg", ImGuiCol_TableRowBg },
		{ "Table Row Background Alternate", "TableRowBgAlt", ImGuiCol_TableRowBgAlt },
		{ "Window Dim", "WindowDim", ImGuiCol_ModalWindowDimBg },
		{ "Plot Lines", "PlotLines", ImGuiCol_PlotLines },
		{ "Plot Lines Hovered", "PlotLinesHovered", ImGuiCol_PlotLinesHovered },
		{ "Plot Histogram", "PlotHistogram", ImGuiCol_PlotHistogram },
		{ "Plot Histogram Hovered", "PlotHistogramHovered", ImGuiCol_PlotHistogramHovered }
	}
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
	_defaultActiveWindowCombo.SetHelpText("Window to be active on startup (Search, Cheats, ...).", true);
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
		GetInstance()._defaultActiveWindowCombo.SetSelectedByIndex(generalSettings.DefaultWindowSelect);

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
		
		GetInstance()._defaultAlignmentInput.SetValue(searchSettings.DefaultAlignment);

		//set cheats defaults
		cheatsSettings.DefaultCheatList = settings["Cheats"]["DefaultCheatList"].get<bool>();
		cheatsSettings.DefaultInterval = settings["Cheats"]["DefaultInterval"].get<int>();

		if (cheatsSettings.DefaultInterval < 1)
			cheatsSettings.DefaultInterval = 1;
		else if (cheatsSettings.DefaultInterval > 240)
			cheatsSettings.DefaultInterval = 240;

		GetInstance()._cheatsDefaultIntervalInput.SetValue(cheatsSettings.DefaultInterval);

		//set device control defaults
		deviceControlSettings.LovenseToken = settings["DeviceControl"]["LovenseToken"].get<std::string>();
		std::string tempToken = deviceControlSettings.LovenseToken.StdStrNoLeadinZeros();
		GetInstance()._lovenseTokenInput.SetText(tempToken);
		GetInstance()._deviceControlSettings.LovenseToken = tempToken;

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

			if (_colorSetSelectCombo.Draw(0.2f, 0.5f))
				color = style.Colors[_colorSetSelectCombo.GetSelectedId()];

			ImGui::PushItemWidth(childYX.x * 0.2f);
			ImGui::ColorPicker4("##ColorPickerUi", (float*)&color, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview);
			style.Colors[_colorSetSelectCombo.GetSelectedId()] = color;
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

			if(_defaultActiveWindowCombo.Draw(1.0f, 0.2f))
				_generalSettings.DefaultWindowSelect = _defaultActiveWindowCombo.GetSelectedIndex();
		
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

		if (_defaultAlignmentInput.Draw(1.0f, 0.2f))
		{
			if (_defaultAlignmentInput.GetValue() < 1)
			{
				_defaultAlignmentInput.SetValue(1);
			}

			_searchSettings.DefaultAlignment = _defaultAlignmentInput.GetValue();
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

		if (_cheatsDefaultIntervalInput.Draw(1.0f, 0.2f))
		{
			if (_cheatsDefaultIntervalInput.GetValue() < 1)
				_cheatsDefaultIntervalInput.SetValue(1);
			else if (_cheatsDefaultIntervalInput.GetValue() > 240)
				_cheatsDefaultIntervalInput.SetValue(240);

			_cheatsSettings.DefaultInterval = _cheatsDefaultIntervalInput.GetValue();
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
		
		if (_lovenseTokenInput.Draw(1.0f, 0.2f))
			_deviceControlSettings.LovenseToken = _lovenseTokenInput.GetStdStringNoZeros();

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

		for(int i = 0; i < ProcessInformation::GetSystemList().size(); ++i)
			validateDir(mungPlexDocsPath + '\\' + dir + '\\' + ProcessInformation::GetSystemList()[i].first);
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

	if(uiJson["Colors"].size() < _colorSetSelectCombo.GetCount())
	{
		style = _defaultStyle;
		return;
	}

	for (int i = 0; i < _colorSetSelectCombo.GetCount(); ++i)
	{
		const std::string& entity = _colorSetSelectCombo.GetIdentifierStdStringAt(i);

		if (uiJson["Colors"].contains(entity))
		{
			style.Colors[_colorSetSelectCombo.GetIdAt(i)] = GetColorVecFromJson(uiJson["Colors"][entity][0]);
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
	_defaultActiveWindowCombo.SetSelectedByIndex(0);
	_generalSettings.EnableRichPresence = false;
	saveSettings();
}

nlohmann::json MungPlex::Settings::generateColorsJson()
{
	nlohmann::json colors;
	ImGuiStyle& style = ImGui::GetStyle();

	for (int i = 0; i < _colorSetSelectCombo.GetCount(); ++i)
	{
		const std::string& identifier = _colorSetSelectCombo.GetIdentifierStdStringAt(i);
		const ImVec4& color = style.Colors[_colorSetSelectCombo.GetIdAt(i)];
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