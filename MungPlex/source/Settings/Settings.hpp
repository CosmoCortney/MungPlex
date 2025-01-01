#pragma once
#include <boost/asio.hpp>
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
        static inline std::vector<StringIdCombo::VecType> Windows =
        {
            {
                { "Search", 0 },
                { "Cheats", 0 },
                { "Pointer Search", 0 },
                { "Device Control", 0 },
                { "Process Information", 0 },
                { "Watch & Control", 0 },
                { "Settings", 0 }
            }
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
        InputText _documentsPathInput = InputText("Documents Path:", true, "", 512);
        InputText _lovenseTokenInput = InputText("Lovense Token:", true, "", 128, ImGuiInputTextFlags_Password);
        InputInt<uint32_t> _defaultAlignmentInput = InputInt<uint32_t>("Default Alignment:", true, 4, 1, 1);
        InputInt<uint32_t> _cheatsDefaultIntervalInput = InputInt<uint32_t>("Default Intervsl:", true, 60, 1, 10);
        StringIdCombo _defaultActiveWindowCombo = StringIdCombo("Deefault Active Window:", true, GeneralSettings::Windows);

        const std::vector<std::string> _featureDirs
        {
             "Search" , "Dumps", "PointerSearch", "WatchControl", "Cheats", "DeviceControl"
        }; 

        static const std::vector<DoubleStringIdCombo::VecType> _colorSettings;
        DoubleStringIdCombo _colorSetSelectCombo = DoubleStringIdCombo("Set Color:", true, _colorSettings);

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
        nlohmann::json generateColorsJson();
        std::vector<float> imVec4ToStdVector(const ImVec4 imVec) const;
	};
}