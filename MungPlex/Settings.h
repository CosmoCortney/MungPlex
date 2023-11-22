#pragma once

#include "Connection.h"
#include <filesystem>
#include<string>
#include"nlohmann/json.hpp"
#include <vector>
#include <functional>

namespace MungPlex
{
    struct ColorScheme
    {
        ImVec4 Background = { 0.0f, 0.0f, 0.0f, 0.0f };
        ImVec4 Text = { 0.0f, 0.0f, 0.0f, 0.0f };
        ImVec4 TextDisabled = { 0.0f, 0.0f, 0.0f, 0.0f };
        ImVec4 ChildBG = { 0.0f, 0.0f, 0.0f, 0.0f };
        ImVec4 PopUpBG = { 0.0f, 0.0f, 0.0f, 0.0f };
        ImVec4 FrameBG = { 0.0f, 0.0f, 0.0f, 0.0f };
        ImVec4 FrameBGHovered = { 0.0f, 0.0f, 0.0f, 0.0f };
        ImVec4 FrameBGActive = { 0.0f, 0.0f, 0.0f, 0.0f };
        ImVec4 TitleBG = { 0.0f, 0.0f, 0.0f, 0.0f };
        ImVec4 TitleBGActive = { 0.0f, 0.0f, 0.0f, 0.0f };
        ImVec4 TitleBGCollapsed = { 0.0f, 0.0f, 0.0f, 0.0f };
        ImVec4 ScrollbarBg = { 0.0f, 0.0f, 0.0f, 0.0f };
    	ImVec4 ScrollbarGrab = { 0.0f, 0.0f, 0.0f, 0.0f };
    	ImVec4 ScrollbarGrabHovered = { 0.0f, 0.0f, 0.0f, 0.0f };
    	ImVec4 ScrollbarGrabActive = { 0.0f, 0.0f, 0.0f, 0.0f };
    	ImVec4 CheckMark = { 0.0f, 0.0f, 0.0f, 0.0f };
    	ImVec4 SliderGrab = { 0.0f, 0.0f, 0.0f, 0.0f };
    	ImVec4 SliderGrabActive = { 0.0f, 0.0f, 0.0f, 0.0f };
    	ImVec4 Button = { 0.0f, 0.0f, 0.0f, 0.0f };
    	ImVec4 ButtonHovered = { 0.0f, 0.0f, 0.0f, 0.0f };
    	ImVec4 ButtonActive = { 0.0f, 0.0f, 0.0f, 0.0f };
        ImVec4 Header = { 0.0f, 0.0f, 0.0f, 0.0f };
    	ImVec4 HeaderHovered = { 0.0f, 0.0f, 0.0f, 0.0f };
    	ImVec4 HeaderActive = { 0.0f, 0.0f, 0.0f, 0.0f };
        ImVec4 Separator = { 0.0f, 0.0f, 0.0f, 0.0f };
        ImVec4 SeparatorHovered = { 0.0f, 0.0f, 0.0f, 0.0f };
        ImVec4 SeparatorActive = { 0.0f, 0.0f, 0.0f, 0.0f };
        ImVec4 ResizeGrip = { 0.0f, 0.0f, 0.0f, 0.0f };
        ImVec4 ResizeGripHovered = { 0.0f, 0.0f, 0.0f, 0.0f };
        ImVec4 ResizeGripActive = { 0.0f, 0.0f, 0.0f, 0.0f };
        ImVec4 Tab = { 0.0f, 0.0f, 0.0f, 0.0f };
        ImVec4 TabHovered = { 0.0f, 0.0f, 0.0f, 0.0f };
        ImVec4 TabActive = { 0.0f, 0.0f, 0.0f, 0.0f };
        ImVec4 TabUnfocused = { 0.0f, 0.0f, 0.0f, 0.0f };
        ImVec4 TabUnfocusedActive = { 0.0f, 0.0f, 0.0f, 0.0f };
        ImVec4 DockingPreview = { 0.0f, 0.0f, 0.0f, 0.0f };
        ImVec4 DockingEmptyBG = { 0.0f, 0.0f, 0.0f, 0.0f };
        ImVec4 TableHeaderBg = { 0.0f, 0.0f, 0.0f, 0.0f };
    	ImVec4 TableBorderStrong = { 0.0f, 0.0f, 0.0f, 0.0f };
    	ImVec4 TableBorderLight = { 0.0f, 0.0f, 0.0f, 0.0f };
    	ImVec4 TableRowBg = { 0.0f, 0.0f, 0.0f, 0.0f };
    	ImVec4 TableRowBgAlt = { 0.0f, 0.0f, 0.0f, 0.0f };
    	ImVec4 TextSelectedBg = { 0.0f, 0.0f, 0.0f, 0.0f };
        ImVec4 WindowDim = { 0.0f, 0.0f, 0.0f, 0.0f };
        ImVec4 PlotLines = { 0.0f, 0.0f, 0.0f, 0.0f };
        ImVec4 PlotLinesHovered = { 0.0f, 0.0f, 0.0f, 0.0f };
        ImVec4 PlotHistogram = { 0.0f, 0.0f, 0.0f, 0.0f };
        ImVec4 PlotHistogramHovered = { 0.0f, 0.0f, 0.0f, 0.0f };
    };

    struct GeneralSettings
    {
        char DocumentsPath[512] = "";
        float Scale = 1.2f;
        std::vector<std::string> Windows;
        int DefaultWindowSelect = 0;
        int Style = 0;
        bool EnableRichPresence = false;
    };

    struct SearchSettings
    {
        bool DefaultCaseSensitive = true;
        bool DefaultColorWheel = false;
        int DefaultAlignment = 4;
        bool DefaultValuesHex = false;
        bool DefaultCached = false;
    };

    struct CheatsSettings
    {
        bool DefaultCheatList = true;
        int DefaultInterval = 60;
    };

	class Settings
	{
    public:
        static void DrawWindow();
        static GeneralSettings& GetGeneralSettings();
        static SearchSettings& GetSearchSettings();
        static CheatsSettings& GetCheatsSettings();
        static ColorScheme& GetColorScheme(const int id);

    private:
        Settings();
        ~Settings(){}
        Settings(const Settings&) = delete;
        Settings(Settings&&) = delete;
        void operator=(const Settings&) = delete;
        void operator=(Settings&&) = delete;
        static Settings& GetInstance()
        {
            static Settings Instance;
            return Instance;
        }

        GeneralSettings _generalSettings;
        SearchSettings _searchSettings;
        CheatsSettings _cheatsSettings;
        std::vector<std::pair<std::string, ColorScheme>> _styles{};

        void drawGeneralSettings();
        void drawSearchSettings();
        void drawCheatSettings();
        bool saveSettings();
        void createDocFolders();
        void validateDir(const std::string dir);

        enum Styles
        {
	        IMGUIDEFAULT, DARK, BRIGHT, CANDY, PASTELL
        };
	};
}