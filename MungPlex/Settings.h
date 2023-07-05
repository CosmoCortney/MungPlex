#pragma once

#include "Connection.h"
#include <filesystem>
#include <string>

namespace MungPlex
{
    struct GeneralSettings
    {
        char DocumentsPath[512] = "";
        float Scale = 1.2f;
        std::vector<std::string> Windows;
        int DefaultWindowSelect = 0;
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

        void drawGeneralSettings();
        void drawSearchSettings();
        void drawCheatSettings();
        bool saveSettings();
        void createDocFolders() const;

    public:
        static void DrawWindow();
        static GeneralSettings& GetGeneralSettings();
        static SearchSettings& GetSearchSettings();
        static CheatsSettings& GetCheatsSettings();
	};
}