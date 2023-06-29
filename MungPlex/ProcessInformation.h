#pragma once
#define EMUPAIR std::pair<const std::wstring, const int>
#include <iostream>
#include <stdio.h>
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "examples/libs/emscripten/emscripten_mainloop_stub.h"
#include<string>
#include "Xertz.h"
#include <Windows.h>
#include "MungPlexConfig.h"
#include "Connection.h"
#include"HelperFunctions.h"
#include <nlohmann/json.hpp>


namespace MungPlex
{
    struct GameEntity
    {
        std::string Entity;
        int Location;
        std::string Datatype;
        int Size;
        bool Hex;
        std::string Value;
    };
    
    struct SystemRegion
    {
        std::string Label;
        uint64_t Base;
        uint64_t Size;
        void* BaseLocationProcess = nullptr;
    };

    class ProcessInformation
    {
    private:
        ProcessInformation()
        {
            _emulators.push_back(EMUPAIR(L"Dolphin", Emulators::DOLPHIN));
            _emulators.push_back(EMUPAIR(L"Project64", Emulators::PROJECT64));
        }

        ~ProcessInformation(){};
        ProcessInformation(const ProcessInformation&) = delete;
        ProcessInformation(ProcessInformation&&) = delete;
        void operator=(const ProcessInformation&) = delete;
        void operator=(ProcessInformation&&) = delete;
        static ProcessInformation& GetInstance()
        {
            static ProcessInformation Instance;
            return Instance;
        }

        int32_t _processType = 0;
        int32_t _pid = -1;
        MODULE_LIST _modules{};
        REGION_LIST _regions{};
        bool _isX64 = false;
        bool _underlyingIsBigEndian = false;
        std::string _gameID;
        std::string _processName;
        int32_t _addressWidth = 8;
        HANDLE _handle = 0;
        std::vector<GameEntity> _gameEntities{};
        std::vector<SystemRegion> _systemRegions{};
        std::vector<std::pair<std::string, size_t>> _labeledEmulatorRegions{};
        int32_t _currentEmulatorNumber = -1;
        std::vector<EMUPAIR> _emulators{};
        std::wstring _exePath;

        void DrawModuleList();
        void DrawRegionList();
        void DrawMiscInformation();
        void DrawGameInformation();
        bool InitEmulator(const int emulatorIndex);
        bool InitProcess(const std::wstring& processName);
        bool InitDolphin();
        bool InitProject64();
        void ObtainGameEntities(const void* baseLocation);

    public:
        enum ProcessType
        {
            EMULATOR, NATIVE, CONSOLE
        };

        enum Emulators
        {
            PROJECT64, DOLPHIN
        };

        static void DrawWindow();
        static void RefreshData(const int32_t PID);
        static bool ConnectToEmulator(const int EmulatorIndex);
        static void SetProcessType(const int32_t processType);
        static std::vector<EMUPAIR>& GetEmulatorList();
        static int32_t GetProcessType();
        static void SetPID(const int32_t pid);
        static int32_t GetPID();
        static void SetX64Flag(const bool isX64);
        static bool IsX64();
        static void SetUnderlyingBigEndianFlag(const bool isBigEndian);
        static bool UnderlyingIsBigEndian();
        static HANDLE GetHandle();
        static int32_t GetAddressWidth();
        static bool LoadSystemInformationJSON(const int emulatorIndex);
        static std::vector<SystemRegion>& GetRegions();
    };
}