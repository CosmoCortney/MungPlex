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
            _emulators.emplace_back(L"No$psx", NO$PSX);
            _emulators.emplace_back(L"pcsx2", PCSX2);
            _emulators.emplace_back(L"Rpcs3", RPCS3);
            _emulators.emplace_back(L"PPSSPP", PPSSPP);
            _emulators.emplace_back(L"Fusion", FUSION);
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

        int32_t _processType;
        DWORD _pid;
        MODULE_LIST _modules;
        REGION_LIST _regions;
        Xertz::ProcessInfo _process;
        bool _isX64;
        bool _underlyingIsBigEndian;
        bool _rereorderRegion;
        int _alignment;
        std::string _gameID;
        std::string _platform;
        std::string _processName;
        std::string _gameName;
        std::string _gameRegion;
        int32_t _addressWidth;
        HANDLE _handle;
        std::vector<GameEntity> _gameEntities;
        std::vector<SystemRegion> _systemRegions;
        std::vector<std::pair<std::string, size_t>> _labeledEmulatorRegions;
        int32_t _currentEmulatorNumber;
        std::vector<EMUPAIR> _emulators;
        std::wstring _exePath;
        bool _read = true;
        bool _write = true;
        bool _execute = true;

        void drawModuleList();
        void refreshData(int32_t PID);
        void drawRegionList();
        void drawMiscInformation();
        void drawGameInformation();
        bool initEmulator(int emulatorIndex);
        bool initProcess(const std::wstring& processName);
        bool initDolphin();
        bool initProject64();
        bool initMesen();
        bool initCemu();
        bool initMelonDS();
        bool initPPSSPP();
        bool initYuzu();
        bool initNo$psx();
        bool initPcsx2();
        bool initRpcs3();
        bool initFusion();
        void obtainGameEntities(void* baseLocation);
        void setupSearch();
        void setupCheats();
        bool connectToProcessFR();
        void setMiscProcessInfo(const std::string processName, const bool bigEndian, const bool rereorder, const int addressWidth, const int alignment);

    public:
        enum ProcessType
        {
            NONE, EMULATOR, NATIVE, CONSOLE
        };

        enum Emulators
        {
            MESEN, PROJECT64, DOLPHIN, CEMU, YUZU, MELONDS, NO$PSX, PCSX2, RPCS3, PPSSPP, FUSION
        };

        enum Systems
        {
	        NES, SNES, N64, GAMECUBE, TRIFORCE, WII, WIIU, SWITCH, GB, GBC, GBA, NDS, N3DS, PS1, PS2, PS3,
            PS4, PS5, PSP, PSV, SMS, GENESIS, S32X, SMCD, SATURN, DREAMCAST, GG, XBOX, XBOX360, XBOXONE,
            XBOXSERIES, X86, X64
        };

        static std::string& GetGameID();
        static void DrawWindow();
        static void RefreshRegionlistPC();
        static bool ConnectToEmulator(int EmulatorIndex);
        static bool ConnectToProcess(int processIndex);
        static bool ConnectToApplicationProcess(int applicationProcessIndex);
        static std::vector<EMUPAIR>& GetEmulatorList();
        static int32_t GetProcessType();
        static void SetPID(int32_t pid);
        static int32_t GetPID();
        static bool IsX64();
        static bool UnderlyingIsBigEndian();
        static HANDLE GetHandle();
        static int32_t GetAddressWidth();
        static bool GetRereorderFlag();
        static bool LoadSystemInformationJSON(int emulatorIndex);
        static std::vector<SystemRegion>& GetRegions();
        static std::string& GetProcessName();
        static std::string& GetTitle();
        static std::string& GetRegion();
        static std::string& GetPlatform();
        static bool* GetRangeFlagRead();
        static bool* GetRangeFlagWrite();
        static bool* GetRangeFlagExecute();
        static int GetRegionIndex(const uint64_t baseAddress);

        template<typename addressType> static addressType EmuAddrToProcessAddr(addressType address)
        {
            int regionIndex = GetRegionIndex(address);

            if (regionIndex < 0)
                return 0;

            return (addressType)(
                reinterpret_cast<uint64_t>(GetInstance()._systemRegions[regionIndex].BaseLocationProcess)
                + (uint64_t)address
                - GetInstance()._systemRegions[regionIndex].Base);
        }

        template<typename dataType> static dataType ReadValue(const uint64_t address)
        {
            void* readAddress = nullptr;
            dataType readValue = (dataType)0;

            if (GetInstance()._processType == EMULATOR)
            {
                if (GetRegionIndex(address) == -1)
                    return 0;

                readAddress = reinterpret_cast<void*>(EmuAddrToProcessAddr<uint64_t>(address));
            }
            else
                readAddress = reinterpret_cast<void*>(address);

            if constexpr (std::is_same_v<dataType, uint64_t> || std::is_same_v<dataType, int64_t>)
            {
                if (GetInstance()._rereorderRegion)
                    ReadFromReorderedRangeEx<dataType>(GetInstance()._process, &readValue, readAddress);
                else
                    GetInstance()._process.ReadExRAM(reinterpret_cast<void*>(&readValue), readAddress, 8);

                if (GetInstance()._underlyingIsBigEndian)
                    readValue = Xertz::SwapBytes<int64_t>(readValue);
            }
            else if constexpr (std::is_same_v<dataType, uint32_t> || std::is_same_v<dataType, int32_t>)
            {
                if (GetInstance()._rereorderRegion)
                    ReadFromReorderedRangeEx<dataType>(GetInstance()._process, &readValue, readAddress);
                else
                    GetInstance()._process.ReadExRAM(reinterpret_cast<void*>(&readValue), readAddress, 4);

                if (GetInstance()._underlyingIsBigEndian)
                    readValue = Xertz::SwapBytes<int32_t>(readValue);
            }
            else if constexpr (std::is_same_v<dataType, uint16_t> || std::is_same_v<dataType, int16_t>)
            {
                if (GetInstance()._rereorderRegion)
                    ReadFromReorderedRangeEx<dataType>(GetInstance()._process, &readValue, readAddress);
                else
                    GetInstance()._process.ReadExRAM(reinterpret_cast<void*>(&readValue), readAddress, 2);

                if (GetInstance()._underlyingIsBigEndian)
                    readValue = Xertz::SwapBytes<int16_t>(readValue);
            }
            else if constexpr (std::is_same_v<dataType, uint8_t> || std::is_same_v<dataType, int8_t>)
            {
                if (GetInstance()._rereorderRegion)
                    ReadFromReorderedRangeEx<dataType>(GetInstance()._process, &readValue, readAddress);
                else
                    GetInstance()._process.ReadExRAM(reinterpret_cast<void*>(&readValue), readAddress, 1);
            }
            else if constexpr (std::is_same_v<dataType, double>)
            {
                int64_t temp = ReadValue<int64_t>(address);
                readValue = *reinterpret_cast<dataType*>(&temp);
            }
            else if constexpr (std::is_same_v<dataType, float>)
            {
                int32_t temp = ReadValue<int32_t>(address);
                readValue = *reinterpret_cast<dataType*>(&temp);
            }
            else if constexpr (std::is_same_v<dataType, bool>)
            {
                readValue = ReadValue<int8_t>(address) != 0;
            }

            return readValue;
        }

        template<typename dataType> static void WriteValue(const uint64_t address, const dataType value)
        {
            void* writeAddress = nullptr;
            dataType writeValue = value;

            if (GetInstance()._processType == EMULATOR)
            {
                if (GetRegionIndex(address) == -1)
                    return;

                writeAddress = reinterpret_cast<void*>(EmuAddrToProcessAddr<uint64_t>(address));
            }
            else
                writeAddress = reinterpret_cast<void*>(address);

            if constexpr (std::is_same_v<dataType, uint64_t> || std::is_same_v<dataType, int64_t>)
            {
                if (GetInstance()._underlyingIsBigEndian)
                    writeValue = Xertz::SwapBytes<int64_t>(writeValue);

                if (GetInstance()._rereorderRegion)
                    WriteToReorderedRangeEx<dataType>(GetInstance()._process, &writeValue, writeAddress);
                else
                    GetInstance()._process.WriteExRAM(reinterpret_cast<void*>(&writeValue), writeAddress, 8);
            }
            else if constexpr (std::is_same_v<dataType, uint32_t> || std::is_same_v<dataType, int32_t>)
            {
                if (GetInstance()._underlyingIsBigEndian)
                    writeValue = Xertz::SwapBytes<int32_t>(writeValue);

                if (GetInstance()._rereorderRegion)
                    WriteToReorderedRangeEx<dataType>(GetInstance()._process, &writeValue, writeAddress);
                else
                    GetInstance()._process.WriteExRAM(reinterpret_cast<void*>(&writeValue), writeAddress, 4);
            }
            else if constexpr (std::is_same_v<dataType, uint16_t> || std::is_same_v<dataType, int16_t>)
            {
                if (GetInstance()._underlyingIsBigEndian)
                    writeValue = Xertz::SwapBytes<int16_t>(writeValue);

                if (GetInstance()._rereorderRegion)
                    WriteToReorderedRangeEx<dataType>(GetInstance()._process, &writeValue, writeAddress);
                else
                    GetInstance()._process.WriteExRAM(reinterpret_cast<void*>(&writeValue), writeAddress, 2);
            }
            else if constexpr (std::is_same_v<dataType, uint8_t> || std::is_same_v<dataType, int8_t>)
            {
                if (GetInstance()._rereorderRegion)
                    WriteToReorderedRangeEx<dataType>(GetInstance()._process, &writeValue, writeAddress);
                else
                    GetInstance()._process.WriteExRAM(reinterpret_cast<void*>(&writeValue), writeAddress, 1);
            }
            else if constexpr (std::is_same_v<dataType, double>)
            {
                int64_t temp = *reinterpret_cast<int64_t*>(&writeValue);
                WriteValue<int64_t>(address, temp);
            }
            else if constexpr (std::is_same_v<dataType, float>)
            {
                int32_t temp = *reinterpret_cast<int32_t*>(&writeValue);
                WriteValue<int32_t>(address, temp);
            }
            else if constexpr (std::is_same_v<dataType, bool>)
            {
                WriteValue<int8_t>(address, writeValue);
            }
        }
    };
}