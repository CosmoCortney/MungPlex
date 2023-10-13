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
            _emulators.emplace_back(L"Mesen", MESEN);
            _emulators.emplace_back(L"Project64", PROJECT64);
            _emulators.emplace_back(L"Dolphin", DOLPHIN);
            _emulators.emplace_back(L"Cemu", CEMU);
            _emulators.emplace_back(L"Yuzu", YUZU);
            _emulators.emplace_back(L"melonDS", MELONDS);
            _emulators.emplace_back(L"PPSSPP", PPSSPP);
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

        static int32_t _processType;
        static DWORD _pid;
        static MODULE_LIST _modules;
        static REGION_LIST _regions;
        static Xertz::ProcessInfo _process;
        static bool _isX64;
        static bool _underlyingIsBigEndian;
        static bool _rereorderRegion;
        static std::string _gameID;
        static std::string _platform;
        static std::string _processName;
        static std::string _gameName;
        static int32_t _addressWidth;
        static HANDLE _handle;
        static std::vector<GameEntity> _gameEntities;
        static std::vector<SystemRegion> _systemRegions;
        static std::vector<std::pair<std::string, size_t>> _labeledEmulatorRegions;
        static int32_t _currentEmulatorNumber;
        static std::vector<EMUPAIR> _emulators;
        static std::wstring _exePath;

        static void drawModuleList();
        static void drawRegionList();
        static void drawMiscInformation();
        static void drawGameInformation();
        static bool initEmulator(int emulatorIndex);
        static bool initProcess(const std::wstring& processName);
        static bool initDolphin();
        static bool initProject64();
        static bool initMesen();
        static bool initCemu();
        static bool initMelonDS();
        static bool initPPSSPP();
        static bool initYuzu();
        static void obtainGameEntities(void* baseLocation);
        static void setupSearch();
        static bool connectToProcessFR();
        static void setMiscProcessInfo(const std::string processName, const bool bigEndian, const bool rereorder, const int addressWidth);

    public:
        enum ProcessType
        {
            EMULATOR, NATIVE, CONSOLE
        };

        enum Emulators
        {
            MESEN, PROJECT64, DOLPHIN, CEMU, YUZU, MELONDS, PPSSPP
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
        static void RefreshRegionlistPC(bool read = true, const bool write = true, const bool execute = true);
        static bool ConnectToEmulator(int EmulatorIndex);
        static bool ConnectToProcess(int processIndex);
        static bool ConnectToApplicationProcess(int applicationProcessIndex);
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
        static bool GetRereorderFlag();
        static bool LoadSystemInformationJSON(int emulatorIndex);
        static std::vector<SystemRegion>& GetRegions();
        static std::string GetProcessName();
    };
}