#pragma once
#include "Connection.hpp"
#include <filesystem>
#include <functional>
#include "HelperFunctions.hpp"
#include "nlohmann/json.hpp"
#include <string>
#include <vector>

namespace MungPlex
{
    struct GeneralSettings
    {
        char DocumentsPath[512] = "";
        float Scale = 1.2f;
        std::vector<std::string> Windows;
        int DefaultWindowSelect = 0;
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
        static void InitSettings();

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
        ImGuiStyle _defaultStyle;
        const std::vector<std::string> _featureDirs
        {
             "Search" , "Dumps", "PointerSearch", "WatchControl", "Cheats"
        }; 
        const std::vector<std::pair<int, std::pair<std::string, std::string>>> _colorSettings
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

        void drawGeneralSettings();
        void drawSearchSettings();
        void drawCheatSettings();
        bool saveSettings();
        void createDocFolders();
        void validateDir(const std::string dir);
        void setUi(const nlohmann::json& uiJson);
        void resetSettings();
        ImVec4 getColorVec(const nlohmann::json& colorJson) const;
        nlohmann::json generateColorsJson() const;
        std::vector<float> imVec4ToStdVector(const ImVec4 imVec) const;
	};
}