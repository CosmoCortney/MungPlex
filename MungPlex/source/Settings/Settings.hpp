#pragma once
#include "Connection.hpp"
#include <filesystem>
#include <functional>
#include "HelperFunctions.hpp"
#include "nlohmann/json.hpp"
#include "WidgetHelpers.hpp"

namespace MungPlex
{
    struct GeneralSettings
    {
        FloorString DocumentsPath = FloorString("", 512);
        float Scale = 1.2f;
        StringIdPairs Windows =
        {
            { "Search", "Cheats", "Pointer Search", "Device Control", "Process Information", "Settings" },
            { 0,        0,        0,                0,                0,                     0 },
            "Default Active Window:"
        };
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
        bool DefaultDisableUndo = false;
    };

    struct CheatsSettings
    {
        bool DefaultCheatList = true;
        int DefaultInterval = 60;
    };

    struct DeviceControlSettings
    {
        FloorString LovenseToken = FloorString("", 128);
    };

	class Settings
	{
    public:
        static void DrawWindow();
        static GeneralSettings& GetGeneralSettings();
        static SearchSettings& GetSearchSettings();
        static CheatsSettings& GetCheatsSettings();
        static DeviceControlSettings& GetDeviceControlSettings();
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
        DeviceControlSettings _deviceControlSettings;
        ImGuiStyle _defaultStyle;
        InputText _documentsPathInput = InputText("Documents Path:", "", 512);
        InputText _lovenseTokenInput = InputText("Lovense Token:", "", 128, true, ImGuiInputTextFlags_Password);
        const std::vector<std::string> _featureDirs
        {
             "Search" , "Dumps", "PointerSearch", "WatchControl", "Cheats", "DeviceControl"
        }; 
        static const DoubleStringIdPairs _colorSettings;

        void drawGeneralSettings();
        void drawSearchSettings();
        void drawCheatSettings();
        void drawDeviceControlSettings();
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