#pragma once
#include <boost/asio.hpp>
#include "GameEntity.hpp"
#include <GLFW/glfw3.h>
#include "HelperFunctions.hpp"
#include <nlohmann/json.hpp>
#include <stdio.h>
#include "USBGecko.hpp"
#include "WidgetHelpers.hpp"

namespace MungPlex
{
    struct GameEntity;

    class ProcessInformation
    {
    public:
        enum ProcessType
        {
            NONE, EMULATOR, NATIVE, CONSOLE
        };

        enum Emulators
        {
            MESEN, PROJECT64, DOLPHIN, CEMU, YUZU, mGBA, MELONDS, LIME3DS, NO$PSX, PCSX2, RPCS3, PPSSPP, FUSION
        };

        enum Systems
        {
            NES, SNES, N64, GAMECUBE, TRIFORCE, WII, WIIU, SWITCH, GB, GBC, GBA, NDS, N3DS, PS1, PS2, PS3,
            PS4, PS5, PSP, PSV, SMS, GENESIS, S32X, SMCD, SATURN, DREAMCAST, GG, XBOX, XBOX360, XBOXONE,
            XBOXSERIES, X86, X64, UNK = -1
        };

        enum ConnectionTypeIDs
        {
            CON_UNDEF = -1,
            CON_USBGecko
        };

        static void DrawWindow();
        static void RefreshRegionlistPC();
        static bool ConnectToEmulator(int EmulatorIndex);
        static bool ConnectToProcess(int processIndex);
        static bool ConnectToApplicationProcess(int applicationProcessIndex);
        static bool ConnectToRealConsole(const int type);
        static const std::vector<StringIdCombo::VecType>& GetEmulatorList();
        static const std::vector<StringIdCombo::VecType>& GetConsoleConnectionTypeList();
        static int32_t GetProcessType();
        static int32_t GetConsoleConnectionType();
        static int32_t GetPID();
        static bool IsX64();
        static bool UnderlyingIsBigEndian();
        static HANDLE GetHandle();
        static int32_t GetAddressWidth();
        static bool GetRereorderFlag();
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
        static const std::vector<SystemRegion>& GetSystemRegionList();
        static bool IsConnectionValid();
        static const std::vector<StringIdCombo::VecType>& GetSystemList();
        static void SetWindowRef(GLFWwindow* window);
        static void ResetWindowTitle();
        static void* GetPointerFromPointerPathExpression(const std::vector<int64_t>& pointerPath, const bool useModule = false, const int64_t moduleAddress = 0);
        static void SetMiscProcessInfo(const std::string processName, const bool bigEndian, const bool rereorder, const int addressWidth, const int alignment);
        static USBGecko* GetUsbGecko();

        template<typename addressType> static addressType GetModuleAddress(const std::wstring& moduleName)
        {
            return (addressType)GetInstance()._process.GetModuleAddress(moduleName);
        }

        template<typename ptrType> static void DumpMemory(ptrType buf, const void* processBaseAddress, const uint64_t sizeInBytes)
        {
            if (GetInstance()._processType == (int32_t)CONSOLE)
            {
                switch (GetInstance()._currentConsoleConnectionType)
                {
                case CON_USBGecko:
                    if (!GetInstance()._usbGecko->IsConnectedAndReady())
                    {
                        return;
                    }

                    GetInstance()._usbGecko->Read(reinterpret_cast<char*>(buf), (uint64_t)processBaseAddress, sizeInBytes);
                    return;
                default:
                    return;
                }
            }
            else
                ProcessInformation::GetProcess().ReadMemorySafe(buf, processBaseAddress, sizeInBytes, 0);
        }

        template<typename dataType> static dataType ReadValue(const uint64_t address)
        {
            void* readAddress = nullptr;
            dataType readValue = (dataType)0;

            switch (GetInstance()._processType)
            {
                case EMULATOR:
                {
                    if (GetRegionIndex(address) == -1)
                        return 0;

                    readAddress = reinterpret_cast<void*>(emuAddrToProcessAddr<uint64_t>(address));
                } break;
                case CONSOLE:
                {
                    if (GetRegionIndex(address) == -1)
                        return 0;

                    readAddress = reinterpret_cast<void*>(address);
                } break;
                default:
                    readAddress = reinterpret_cast<void*>(address);
            }

            if (GetInstance()._processType == CONSOLE)
            {
                if constexpr (std::is_same_v<dataType, uint8_t> || std::is_same_v<dataType, int8_t>)
                {
                    switch (GetInstance()._currentConsoleConnectionType)
                    {
                        case CON_USBGecko:
                            GetInstance()._usbGecko->Read(reinterpret_cast<char*>(&readValue), address, sizeof(dataType));
                            return readValue;
                        default: //CON_UNDEF
                            return 0;
                    }
                }
                else if constexpr (std::is_integral_v<dataType> || std::is_floating_point_v<dataType>)
                {
                    switch (GetInstance()._currentConsoleConnectionType)
                    {
                        case CON_USBGecko:
                            GetInstance()._usbGecko->Read(reinterpret_cast<char*>(&readValue), address, sizeof(dataType));
                            return Xertz::SwapBytes<dataType>(readValue);
                        default: //CON_UNDEF
                            return 0;
                    }
                }
                else
                    return 0;
            }

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

            switch (GetInstance()._processType)
            {
                case EMULATOR:
                {
                    if (GetRegionIndex(address) == -1)
                        return;

                    writeAddress = reinterpret_cast<void*>(emuAddrToProcessAddr<uint64_t>(address));
                } break;
                case CONSOLE:
                {
                    if (GetRegionIndex(address) == -1)
                        return;

                    writeAddress = reinterpret_cast<void*>(address);
                } break;
                default: //PC
                    writeAddress = reinterpret_cast<void*>(address);
            }
                
            if (GetInstance()._processType == CONSOLE)
            {
                if constexpr (std::is_same_v<dataType, uint8_t> || std::is_same_v<dataType, int8_t>)
                {
                    switch (GetInstance()._currentConsoleConnectionType)
                    {
                        case CON_USBGecko:
                            if (GetInstance()._usbGecko->Poke(writeValue, address) != FT_OK)
                                GetInstance()._usbGecko->Disconnect();

                            return;
                        default: //CON_UNDEF
                            return;
                    }
                }
                else if constexpr (std::is_integral_v<dataType> || std::is_floating_point_v<dataType>)
                {
                    switch (GetInstance()._currentConsoleConnectionType)
                    {
                        case CON_USBGecko:
                            writeValue = Xertz::SwapBytes<dataType>(writeValue);

                            if (GetInstance()._usbGecko->Poke(writeValue, address) != FT_OK)
                                GetInstance()._usbGecko->Disconnect();

                            return;
                        default: //CON_UNDEF
                            return;
                    }
                }
                else
                    return;
            }

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

        static void Rereorder4BytesReorderedMemory(void* ptr, const uint64_t size)
        {
            uint32_t* swapPtr = reinterpret_cast<uint32_t*>(ptr);

            for (uint64_t offset = 0; offset < (size >> 2); ++offset)
            {
                swapPtr[offset] = Xertz::SwapBytes<uint32_t>(swapPtr[offset]);
            }
        }

        template<typename StringType> static bool WriteTextEx(const uint32_t pid, StringType text, const uint64_t address)
        {
            static uint32_t textLength = 0;
            
            if constexpr (std::is_same_v<StringType, char*>)
            {
                textLength = strlen(text);
            }
            else if constexpr (std::is_same_v<StringType, wchar_t*>)
            {
                textLength = wcslen(text) * 2;
            }
            else if constexpr (std::is_same_v<StringType, char32_t*>)
            {
                textLength = std::char_traits<char32_t>::length(text) * 4;
            }
            else
                throw "Error: Unsupported string type.";
                
            if (text[textLength - 1] == '\n')
                --textLength;

            Xertz::SystemInfo::GetProcessInfo(pid).WriteMemoryFast(text, reinterpret_cast<void*>(address), textLength);
            return true;
        }

        template<typename addressType> static addressType TranslatePtrTo4BytesReorderingPtr(addressType ptr)
        {
            uint64_t tempPtr;

            if constexpr (std::is_same_v<uint64_t, addressType>)
                tempPtr = ptr;
            else
                tempPtr = reinterpret_cast<uint64_t>(ptr);

            switch (tempPtr & 0xF)
            {
            case 1: case 5: case 9: case 0xD:
                ++tempPtr;
                break;
            case 2: case 6: case 0xA: case 0xE:
                --tempPtr;
                break;
            case 3: case 7: case 0xB: case 0xF:
                tempPtr -= 3;
                break;
            default: //0, 4, 8, C
                tempPtr += 3;
            }

            if constexpr (std::is_same_v<uint64_t, addressType>)
                return tempPtr;
            else
                return reinterpret_cast<addressType>(tempPtr);
        }

        template<typename dataType> static void WriteToReorderedRangeEx(const Xertz::ProcessInfo& process, dataType* in, void* writeAddress)
        {
            char* address = reinterpret_cast<char*>(writeAddress);
            char* dest = reinterpret_cast<char*>(in);

            for (int i = 0; i < sizeof(dataType); ++i)
            {
                char* reorderedAddress = TranslatePtrTo4BytesReorderingPtr<char*>(address + i);
                process.WriteMemoryFast(dest + i, reorderedAddress, 1);
            }

            return;
        }

        template<typename dataType> static void ReadFromReorderedRangeEx(const Xertz::ProcessInfo& process, dataType* out, void* readAddress)
        {
            char* address = reinterpret_cast<char*>(readAddress);
            char* dest = reinterpret_cast<char*>(out);

            for (int i = 0; i < sizeof(dataType); ++i)
            {
                char* reorderedAddress = TranslatePtrTo4BytesReorderingPtr<char*>(address + i);
                process.ReadMemoryFast(dest + i, reorderedAddress, 1);
            }

            return;
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
 
        //Misc.
        InputInt<uint32_t> _currentPageInput = InputInt<uint32_t>("Page:", true, 1, 1, 1);
        GLFWwindow* _window = nullptr;

        //Emulator Connection
        static const std::vector<StringIdCombo::VecType> _emulators;
        bool _rereorderRegion = false;

        //Native Connection
        bool _read = true;
        bool _write = true;
        bool _execute = false;

        //Console Connection
        static const std::vector<StringIdCombo::VecType> _consoleConnectionTypes;
        int _currentConsoleConnectionType = CON_UNDEF;
        std::shared_ptr<USBGecko> _usbGecko;

        //Console and Emulator Connection
        std::vector<GameEntity> _gameEntities;
        bool _underlyingIsBigEndian = false;

        //Native and Emulator Connection
        PROCESS_INFO _process;
        std::string _processName;
        std::wstring _exePath;

        //All
        static const std::vector<StringIdCombo::VecType> _systems;
        int32_t _processType = NONE;
        int _platformID = UNK;
        int32_t _addressWidth = 4;
        int _alignment = 4;
        std::string _gameID;
        std::string _rpcGameID;
        std::string _platform;
        std::string _gameName;
        std::string _gameRegion;
        std::vector<SystemRegion> _systemRegions;
        int _connectionCheckValue = 0;
        void* _connectionCheckPtr = nullptr;

        void drawModuleList();
        void drawRegionList();
        void drawMiscInformation();
        void drawGameInformation();
        bool initEmulator(const int emulatorIndex);
        bool initProcess(const std::wstring& processName);
        bool initConsoleConnection(const int connectionType);
        void setupSearch();
        void setupCheats();
        void refreshModuleList();
        bool connectToProcessFR();
        std::string getPlatformNameFromId(const int id);

        template<typename addressType> static addressType emuAddrToProcessAddr(addressType address)
        {
            int regionIndex = GetRegionIndex(address);

            if (regionIndex < 0)
                return 0;

            return (addressType)(
                reinterpret_cast<uint64_t>(GetInstance()._systemRegions[regionIndex].BaseLocationProcess)
                + (uint64_t)address
                - GetInstance()._systemRegions[regionIndex].Base);
        }
    };
}