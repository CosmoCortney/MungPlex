#pragma once

// TODO No defines like this, define a "proper" class
#define EMUPAIR std::pair<const std::wstring, const int>

#include <stdio.h>
#include<string>
#include "Xertz.h"
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
        ProcessInformation()
        {
            _emulators.emplace_back(L"Dolphin", DOLPHIN);
            _emulators.emplace_back(L"Project64", PROJECT64);
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
        std::string _platform;
        std::string _processName;
        int32_t _addressWidth = 8;
        HANDLE _handle = 0;
        std::vector<GameEntity> _gameEntities{};
        std::vector<SystemRegion> _systemRegions{};
        std::vector<std::pair<std::string, size_t>> _labeledEmulatorRegions{};
        int32_t _currentEmulatorNumber = -1;
        std::vector<EMUPAIR> _emulators{};
        std::wstring _exePath;

        void DrawModuleList() const;
        void DrawRegionList() const;
        void DrawMiscInformation();
        void DrawGameInformation() const;
        bool InitEmulator(int emulatorIndex);
        bool InitProcess(const std::wstring& processName);
        bool InitDolphin();
        bool InitProject64();
        void ObtainGameEntities(void* baseLocation);

    public:
        enum ProcessType
        {
            EMULATOR, NATIVE, CONSOLE
        };

        enum Emulators
        {
            PROJECT64, DOLPHIN
        };

        enum Systems
        {
	        NES, SNES, N64, GAMECUBE, TRIFORCE, WII, WIIU, SWITCH, GB, GBC, GBA, NDS, N3DS, PS1, PS2, PS3,
            PS4, PS5, PSP, PSV, SMS, GENESIS, S32X, SMCD, SATURN, DREAMCAST, GG, XBOX, XBOX360, XBOXONE,
            XBOXSERIES, X86, X64
        };

        static std::string& GetGameID();
        static void DrawWindow();
        static void RefreshData(int32_t PID);
        static bool ConnectToEmulator(int EmulatorIndex);
        static void SetProcessType(int32_t processType);
        static std::vector<EMUPAIR>& GetEmulatorList();
        static int32_t GetProcessType();
        static void SetPID(int32_t pid);
        static int32_t GetPID();
        static void SetX64Flag(bool isX64);
        static bool IsX64();
        static void SetUnderlyingBigEndianFlag(bool isBigEndian);
        static bool UnderlyingIsBigEndian();
        static HANDLE GetHandle();
        static int32_t GetAddressWidth();
        static bool LoadSystemInformationJSON(int emulatorIndex);
        static std::vector<SystemRegion>& GetRegions();
    };
}