#pragma once
#include<iostream>
#include <stdio.h>
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "examples/libs/emscripten/emscripten_mainloop_stub.h"
#include"Connection.h"
#include<Windows.h>
#include <shlobj.h>
#include <filesystem>
#include<string>
#include"nlohmann/json.hpp"

namespace MungPlex
{
    struct GeneralSettings
    {
        char DocumentsPath[512];
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

    static struct CheatsSettings
    {
        bool DefaultCheatList = true;
        int DefaultInterval = 60;
    };

	class Settings
	{
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