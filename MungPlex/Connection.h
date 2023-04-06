#pragma once
#include <iostream>
#include <stdio.h>
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "examples/libs/emscripten/emscripten_mainloop_stub.h"
#include<string>
#include "json5.hpp"
#include<fstream>
#include <iomanip>
#include "Xertz.h"
#include <Windows.h>
#include "MungPlexConfig.h"
#include <sstream>
#include"ProcessInformation.h"
#include"HelperFunctions.h"

namespace MungPlex
{
    struct GameEntity
    {
        std::string Entity = "";
        int Location;
        std::string Datatype = "";
        int Size;
        bool Hex;
        std::string Value = "";
    };

    struct SystemRegion
    {
        std::string Label = "";
        uint64_t Base;
        uint64_t Size;
        void* BaseLocationProcess = nullptr;

    };

    class Connection
    {
    private:
        Connection()
        {
            _emulators.push_back(std::pair<std::wstring, int>(L"Dolphin", 0));
            _emulators.push_back(std::pair<std::wstring, int>(L"Project64", 1));
        }

        ~Connection() {};
        Connection(const Connection&) = delete;
        Connection(Connection&&) = delete;
        void operator=(const Connection&) = delete;
        void operator=(Connection&&) = delete;
        static Connection& GetInstance()
        {
            static Connection Instance;
            return Instance;
        }

        bool BE = false;
        bool s_connected = false;
        int32_t _addressWidth = 8;
        std::string ConnectionStatus = NO_CONNECTION;
        int _currentPID;
        HANDLE _handle;
        REGION_LIST _regions;
        void InitProcess(std::wstring& processName, int connectionType, std::pair<std::wstring, int> emulator = std::pair<std::wstring, int>());
        void InitDolphin();
        void InitProject64();
        void DrawGameInformation();
        void DrawConnectionSelect();
        std::vector<std::pair<std::wstring, int>> _emulators{};
        void LoadSystemInformationJSON(std::wstring& emuName);
        void ParseJsonToEntities();
        void ObtainGameEntities(void* location);
        std::vector<GameEntity> _gameEntities{};
        std::vector<SystemRegion> _systemRegions{};
        int _currentEmulatorNumber = 0;
        std::vector<std::pair<std::string, size_t>> _labeledEmulatorRegions;

    public:
        static void DrawWindow();
        static void SystemInformations();
        static int GetCurrentPID();
        static HANDLE GetCurrentHandle();
        static std::vector<SystemRegion>& GetRegions();
        static bool* IsBE() { return &GetInstance().BE; }
        static int32_t* GetAddressWidth() { return &GetInstance()._addressWidth; }
    };
}