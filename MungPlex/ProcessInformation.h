#pragma once

// TODO No defines like this, define a "proper" class
#define EMUPAIR std::pair<const std::wstring, const int>
#include <boost/asio.hpp>
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
    public:
        enum ProcessType
        {
            NONE, EMULATOR, NATIVE, CONSOLE
        };

        enum Emulators
        {
            MESEN, PROJECT64, DOLPHIN, CEMU, YUZU, VBA, MELONDS, NO$PSX, PCSX2, RPCS3, PPSSPP, FUSION
        };

        enum Systems
        {
            NES, SNES, N64, GAMECUBE, TRIFORCE, WII, WIIU, SWITCH, GB, GBC, GBA, NDS, N3DS, PS1, PS2, PS3,
            PS4, PS5, PSP, PSV, SMS, GENESIS, S32X, SMCD, SATURN, DREAMCAST, GG, XBOX, XBOX360, XBOXONE,
            XBOXSERIES, X86, X64, UNK = -1
        };

        static void DrawWindow();
        static void RefreshRegionlistPC();
        static bool ConnectToEmulator(int EmulatorIndex);
        static bool ConnectToProcess(int processIndex);
        static bool ConnectToApplicationProcess(int applicationProcessIndex);
        static const std::vector<EMUPAIR>& GetEmulatorList();
        static int32_t GetProcessType();
        static int32_t GetPID();
        static bool IsX64();
        static bool UnderlyingIsBigEndian();
        static HANDLE GetHandle();
        static int32_t GetAddressWidth();
        static bool GetRereorderFlag();
        static bool LoadSystemInformationJSON(const std::string& system);
        static std::string& GetGameID();
        static std::string& GetRpcGameID();
        static std::string& GetProcessName();
        static std::string& GetTitle();
        static std::string& GetRegion();
        static std::string& GetPlatform();
        static bool* GetRangeFlagRead();
        static bool* GetRangeFlagWrite();
        static bool* GetRangeFlagExecute();
        static int GetRegionIndex(const uint64_t baseAddress);
        static PROCESS_INFO& GetProcess();
        static MODULE_LIST& GetModuleList();
        static REGION_LIST& GetRegionList();
        static std::vector<SystemRegion>& GetSystemRegionList();
        static bool IsConnectionValid();
        static const std::vector<std::pair<int, std::string>>& GetSystemPairs();
        static std::string GetSystemNameByID(const int id);

        template<typename addressType> static addressType GetModuleAddress(const std::wstring& moduleName)
        {
            return (addressType)GetInstance()._process.GetModuleAddress(moduleName);
        }

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

            if constexpr (std::is_same_v<dataType, uint8_t> || std::is_same_v<dataType, int8_t>)
            {
                if (GetInstance()._rereorderRegion)
                    ReadFromReorderedRangeEx<dataType>(GetInstance()._process, &readValue, readAddress);
                else
                    GetInstance()._process.ReadMemoryFast(reinterpret_cast<void*>(&readValue), readAddress, 1);
            }
            else if constexpr (std::is_integral_v<dataType> || std::is_floating_point_v<dataType>)
            {
                if (GetInstance()._rereorderRegion)
                    ReadFromReorderedRangeEx<dataType>(GetInstance()._process, &readValue, readAddress);
                else
                    GetInstance()._process.ReadMemoryFast(reinterpret_cast<void*>(&readValue), readAddress, sizeof(dataType));

                if (GetInstance()._underlyingIsBigEndian)
                    readValue = Xertz::SwapBytes<dataType>(readValue);
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

            if constexpr (std::is_same_v<dataType, uint8_t> || std::is_same_v<dataType, int8_t>)
            {
                if (GetInstance()._rereorderRegion)
                    WriteToReorderedRangeEx<dataType>(GetInstance()._process, &writeValue, writeAddress);
                else
                    GetInstance()._process.WriteMemoryFast(&writeValue, writeAddress, sizeof(dataType));
            }
            else if constexpr (std::is_integral_v<dataType> || std::is_floating_point_v<dataType>)
            {
                if (GetInstance()._underlyingIsBigEndian)
                    writeValue = Xertz::SwapBytes<dataType>(writeValue);

                if (GetInstance()._rereorderRegion)
                    WriteToReorderedRangeEx<dataType>(GetInstance()._process, &writeValue, writeAddress);
                else
                    GetInstance()._process.WriteMemoryFast(&writeValue, writeAddress, sizeof(dataType));
            }
            else if constexpr (std::is_same_v<dataType, bool>)
            {
                WriteValueInternal(address, static_cast<int8_t>(value), 1);
            }
        }

    private:
        ProcessInformation() = default;
        ~ProcessInformation() = default;
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
        PROCESS_INFO _process;
        bool _underlyingIsBigEndian;
        bool _rereorderRegion;
        int _alignment;
        std::string _gameID;
        std::string _rpcGameID;
        std::string _platform;
        std::string _processName;
        std::string _gameName;
        std::string _gameRegion;
        int32_t _addressWidth;
        std::vector<GameEntity> _gameEntities;
        std::vector<SystemRegion> _systemRegions;
        std::vector<std::pair<std::string, size_t>> _labeledEmulatorRegions;
        int32_t _currentEmulatorNumber;
        static inline const std::vector<EMUPAIR> _emulators = 
        {
            { L"Mesen", MESEN },
            { L"Project64", PROJECT64 },
            { L"Dolphin", DOLPHIN },
            { L"Cemu", CEMU },
            { L"Yuzu", YUZU },
            { L"VisualBoyAdvance", VBA },
            { L"melonDS", MELONDS },
            { L"No$psx", NO$PSX },
            { L"pcsx2", PCSX2 },
            { L"Rpcs3", RPCS3 },
            { L"PPSSPP", PPSSPP },
            { L"Fusion", FUSION }
        };
        std::wstring _exePath;
        bool _read = true;
        bool _write = true;
        bool _execute = true;
        int _platformID = UNK;
        int _connectionCheckValue = 0;
        void* _connectionCheckPtr = nullptr;
        static inline const std::vector<std::pair<int, std::string>> _systemPairs
        {
            { NES, "NES" }, { SNES, "SNES" }, { N64, "Nintendo 64" }, { GAMECUBE, "GameCube" },
            { TRIFORCE, "Triforce" }, { WII, "Wii" }, { WIIU, "Wii U" }, { SWITCH, "Switch"}, 
            { GB, "GameBoy" }, { GBC, "GameBoy Color" }, { NDS, "NDS" }, { N3DS, "3DS" }, 
            { PS1, "PS1" }, { PS2, "PS2" }, { PS3 , "PS3" }, { PS4, "PS4" }, { PS5, "PS5", },
            { PSP, "PSP" }, { PSV, "PS Vita" }, 
            { SMS, "Master System" }, { GENESIS, "Mega Drive" }, { S32X, "32X" }, { SMCD, "Mega-CD" },
            { SATURN, "Saturn" }, { DREAMCAST, "Dreamcast" }, { GG, "GamesGear" },
            { XBOX, "XBOX" }, { XBOX360, "XBOX 360" }, { XBOXONE, "XBOX One" }, { XBOXSERIES, "XBOX Series" },
            { X86, "PC" }, { X64, "PC" }
        };

        void drawModuleList();
        void drawRegionList();
        void drawMiscInformation();
        void drawGameInformation();
        bool initEmulator(int emulatorIndex);
        bool initProcess(const std::wstring& processName);
        bool initDolphin();
        bool initProject64();
        bool initMesen();
        bool initCemu();
        bool initVBA();
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
        void refreshModuleList();
        bool connectToProcessFR();
        void setMiscProcessInfo(const std::string processName, const bool bigEndian, const bool rereorder, const int addressWidth, const int alignment);
        std::string getRegionFromNintendoRegionCode(const char code) const;
    };
}