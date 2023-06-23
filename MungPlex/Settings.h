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
	class Settings
	{
    private:
        Settings();

        ~Settings()
        {
            delete[] _documentsPath;
        }

        Settings(const Settings&) = delete;
        Settings(Settings&&) = delete;
        void operator=(const Settings&) = delete;
        void operator=(Settings&&) = delete;
        static Settings& GetInstance()
        {
            static Settings Instance;
            return Instance;
        }

        nlohmann::json _settingsJSON;

        //general
        char _documentsPath[512];
        float _scale = 1.2f;

        //search settings


        //cheats settings


        void drawGeneralSettings();
        void drawSearchSettings();
        void drawCheatSettings();

        public:
            static void DrawWindow();
	};
}