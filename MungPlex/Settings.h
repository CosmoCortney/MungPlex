#pragma once

#include "Connection.h"
#include <filesystem>
#include<string>
#include"nlohmann/json.hpp"
#include <vector>
#include <functional>
#include"HelperFunctions.h"

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
        std::vector<std::pair<int, std::pair<std::string, std::string>>> _colorSettings;
        ImGuiStyle _defaultStyle;

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