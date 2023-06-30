#pragma once
#define SOL_ALL_SAFETIES_ON 1
#include "lua.hpp"
#include "sol/sol.hpp"
#include <cassert>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "luaconf.h"
#include<iostream>
#include <stdio.h>
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "examples/libs/emscripten/emscripten_mainloop_stub.h"
#include"Settings.h"
#include"Connection.h"
#include<thread>
#include<chrono>
#include <future>
#include<Windows.h>
#include <shlobj.h>
#include <filesystem>
#include<string>
#include"nlohmann/json.hpp"

namespace MungPlex
{
    struct LuaCheat
    {
        int ID = -1;
        bool Checked = false;
        std::string Title;
        std::string Hacker;
        std::string Lua;
        std::string Description;
    };


    class Cheats
	{
    private:
        Cheats();

        ~Cheats(){}

        Cheats(const Cheats&) = delete;
        Cheats(Cheats&&) = delete;
        void operator=(const Cheats&) = delete;
        void operator=(Cheats&&) = delete;
        static Cheats& GetInstance()
        {
            static Cheats Instance;
            return Instance;
        }

        char _textCheatTitle[256];
        char _textCheatHacker[128];
        char _textCheatLua[1024 * 128];
        char _textCheatDescription[1024 * 16];
        std::wstring _cheatListPath;
        sol::state _lua{};
        bool _isBigEndian = false;
        int _pid = 0;
        std::vector<SystemRegion> _regions{};
        int _perSecond = 60;
        bool _executeCheats = false;
        bool _cheatList = false;
        std::thread _cheatThread;
        PROCESS_INFO _processInfo;
        bool _cheatError = false;
        std::wstring _documentsPath;
        std::wstring _currentGameID;
        std::wstring _currentCheatFile;
        std::string _currentCheatListFile;
        std::string _placeholderCheatFile = "{\"Cheats\":[]}";
        std::vector<LuaCheat> _luaCheats{};
        std::vector<bool> _markedCheats{};
        std::vector<std::string> _checkBoxIDs{};
        bool _unsavedChangesTextCheat = false;
        bool _unsavedChangesCheatList = false;
        int _selectedID = -1;
        bool _disableEditButtons = false;

        static int luaExceptionHandler(lua_State* L, const sol::optional<const std::exception&> exception, const sol::string_view description);
        void copyCheatToInformationBox(const int index);
        void copyCheatToList(const int index);
        bool saveCheatList() const;
        void deleteCheat(const uint16_t index);
        void refreshModuleList();
        void DrawCheatList(); //top-left
        void DrawCheatInformation(); //top-right
        void DrawControl(); //bottom left
        void updateConnectionInfo()
        {
            _isBigEndian = ProcessInformation::UnderlyingIsBigEndian();
            _pid = ProcessInformation::GetPID();
            _regions = ProcessInformation::GetRegions();
            refreshModuleList();
        }

        void initCheatFile();
        int getRangeIndex(const uint64_t address) const;
        void cheatRoutine();

        //legacy function to keep older cheats functioning
        static double readFromRAM(const int type, const uint64_t address)
        {
            int64_t readValue = 0;
            double returnValue = 0.0;
            const int rangeIndex = GetInstance().getRangeIndex(address);

            if (rangeIndex == -1)
                return returnValue = NAN;

            const void* readAddress = static_cast<char*>(GetInstance()._regions[rangeIndex].BaseLocationProcess) + address - GetInstance()._regions[rangeIndex].Base;

            switch (type)
            {
            case INT8:
                GetInstance()._processInfo.ReadExRAM(&readValue, readAddress, 1);
                returnValue = static_cast<double>(*reinterpret_cast<int8_t*>(&readValue));
                break;
            case BOOL:
                GetInstance()._processInfo.ReadExRAM(&readValue, readAddress, 1);
                returnValue = readValue != 0;
                break;
            case INT16:
                GetInstance()._processInfo.ReadExRAM(&readValue, readAddress, 2);
                if (GetInstance()._isBigEndian) readValue = Xertz::SwapBytes<int16_t>(readValue);
                returnValue = static_cast<double>(*reinterpret_cast<int16_t*>(&readValue));
                break;
            case INT32:
                GetInstance()._processInfo.ReadExRAM(&readValue, readAddress, 4);
                if (GetInstance()._isBigEndian) readValue = Xertz::SwapBytes<int32_t>(readValue);
            	returnValue = static_cast<double>(*reinterpret_cast<int32_t*>(&readValue));
                break;
            case INT64:
                GetInstance()._processInfo.ReadExRAM(&readValue, readAddress, 8);
                if (GetInstance()._isBigEndian) readValue = Xertz::SwapBytes<int64_t>(readValue);
                returnValue = static_cast<double>(*&readValue);
                break;
            case FLOAT:
                GetInstance()._processInfo.ReadExRAM(&readValue, readAddress, 4);
                if (GetInstance()._isBigEndian) readValue = Xertz::SwapBytes<int32_t>(readValue);
                returnValue = static_cast<double>(*reinterpret_cast<float*>(&readValue));
                break;
            default://DOUBLE
                GetInstance()._processInfo.ReadExRAM(&readValue, readAddress, 8);
                if (GetInstance()._isBigEndian) readValue = Xertz::SwapBytes<int64_t>(readValue);
                returnValue = *reinterpret_cast<double*>(&readValue);
            }

            return returnValue;
        }

        static bool readBool(const uint64_t address)
        {
            return readInt8(address) != 0;
        }

        static int8_t readInt8(const uint64_t address)
        {
            return GetInstance()._isBigEndian ? readInt64(address) >> 56 : readInt64(address);
        }

        static uint8_t readUInt8(const uint64_t address)
        {
            return readInt8(address);
        }

        static int16_t readInt16(const uint64_t address)
        {
            return GetInstance()._isBigEndian ? readInt64(address) >> 48 : readInt64(address);
        }

        static uint16_t readUInt16(const uint64_t address)
        {
            return readInt16(address);
        }

        static int32_t readInt32(const uint64_t address)
        {
            return GetInstance()._isBigEndian ? readInt64(address) >> 32 : readInt64(address);
        }

        static uint32_t readUInt32(const uint64_t address)
        {
            return readInt32(address);
        }

        static int64_t readInt64(const uint64_t address)
        {
            int64_t readValue = 0;
            const int rangeIndex = GetInstance().getRangeIndex(address);

            if (rangeIndex == -1)
                return 0;

            const void* readAddress = static_cast<char*>(GetInstance()._regions[rangeIndex].BaseLocationProcess) + address - GetInstance()._regions[rangeIndex].Base;
            GetInstance()._processInfo.ReadExRAM(&readValue, readAddress, 8);
            if (GetInstance()._isBigEndian) readValue = Xertz::SwapBytes<int64_t>(readValue);
            return readValue;
        }

        static uint64_t readUInt64(const uint64_t address)
        {
            return readInt64(address);
        }

        static float readFloat(const uint64_t address)
        {
            float readValue = 0.0f;
            const int rangeIndex = GetInstance().getRangeIndex(address);

            if (rangeIndex == -1)
                return 0.0f;

            const void* readAddress = static_cast<char*>(GetInstance()._regions[rangeIndex].BaseLocationProcess) + address - GetInstance()._regions[rangeIndex].Base;
            GetInstance()._processInfo.ReadExRAM(&readValue, readAddress, 4);
            if (GetInstance()._isBigEndian) readValue = Xertz::SwapBytes<float>(readValue);
            return readValue;
        }

        static double readDouble(const uint64_t address)
        {
            double readValue = 0.0;
            const int rangeIndex = GetInstance().getRangeIndex(address);

            if (rangeIndex == -1)
                return 0.0;

            const void* readAddress = static_cast<char*>(GetInstance()._regions[rangeIndex].BaseLocationProcess) + address - GetInstance()._regions[rangeIndex].Base;
            GetInstance()._processInfo.ReadExRAM(&readValue, readAddress, 8);
            if (GetInstance()._isBigEndian) readValue = Xertz::SwapBytes<double>(readValue);
            return readValue;
        }

        //legacy function to keep older cheats functioning
        static void writeToRAM(const int type, const  uint64_t address, double value)
        {
            uint64_t writeValue;
            const int rangeIndex = GetInstance().getRangeIndex(address);

            if (rangeIndex == -1)
                return;

            void* writeAddress = static_cast<char*>(GetInstance()._regions[rangeIndex].BaseLocationProcess) + address - GetInstance()._regions[rangeIndex].Base;

            switch (type)
            {
            case BOOL:
                writeValue = static_cast<int64_t>(value) != 0;
                GetInstance()._processInfo.WriteExRAM(&writeValue, writeAddress, 1);
                return;
            case INT8:
                writeValue = static_cast<int8_t>(value);
                GetInstance()._processInfo.WriteExRAM(&writeValue, writeAddress, 1);
                return;
            case INT16:
                writeValue = static_cast<int16_t>(value);
                if (GetInstance()._isBigEndian) writeValue = Xertz::SwapBytes<int16_t>(writeValue);
                GetInstance()._processInfo.WriteExRAM(&writeValue, writeAddress, 2);
                return;
            case INT32:
                writeValue = static_cast<int32_t>(value);
                if (GetInstance()._isBigEndian) writeValue = Xertz::SwapBytes<int32_t>(writeValue);
                GetInstance()._processInfo.WriteExRAM(&writeValue, writeAddress, 4);
                return;
            case INT64:
                writeValue = static_cast<int64_t>(value);
                if (GetInstance()._isBigEndian) writeValue = Xertz::SwapBytes<int64_t>(writeValue);
                GetInstance()._processInfo.WriteExRAM(&writeValue, writeAddress, 8);
                return;
            case FLOAT: {
                float temp = static_cast<float>(value);
                writeValue = *reinterpret_cast<int32_t*>(&temp);
                    if (GetInstance()._isBigEndian) writeValue = Xertz::SwapBytes<int32_t>(writeValue);
                    GetInstance()._processInfo.WriteExRAM(&writeValue, writeAddress, 4);
                } return;
            case DOUBLE: {
                writeValue = *reinterpret_cast<int64_t*>(&value);
                if (GetInstance()._isBigEndian) writeValue = Xertz::SwapBytes<int64_t>(writeValue);
                GetInstance()._processInfo.WriteExRAM(&writeValue, writeAddress, 8);
            } return;
            }
        }

        static void writeBool(const uint64_t address, bool value)
        {
            const int rangeIndex = GetInstance().getRangeIndex(address);

            if (rangeIndex == -1)
                return;

            void* writeAddress = static_cast<char*>(GetInstance()._regions[rangeIndex].BaseLocationProcess) + address - GetInstance()._regions[rangeIndex].Base;
            GetInstance()._processInfo.WriteExRAM(&value, writeAddress, 1);
        }

        static void writeInt8(const uint64_t address, int8_t value)
        {
            const int rangeIndex = GetInstance().getRangeIndex(address);

            if (rangeIndex == -1)
                return;

            void* writeAddress = static_cast<char*>(GetInstance()._regions[rangeIndex].BaseLocationProcess) + address - GetInstance()._regions[rangeIndex].Base;
            GetInstance()._processInfo.WriteExRAM(&value, writeAddress, 1);
        }

        static void writeInt16(const uint64_t address, int16_t value)
        {
            const int rangeIndex = GetInstance().getRangeIndex(address);

            if (rangeIndex == -1)
                return;

            void* writeAddress = static_cast<char*>(GetInstance()._regions[rangeIndex].BaseLocationProcess) + address - GetInstance()._regions[rangeIndex].Base;
            if (GetInstance()._isBigEndian) value = Xertz::SwapBytes<int16_t>(value);
            GetInstance()._processInfo.WriteExRAM(&value, writeAddress, 2);
        }

        static void writeInt32(const uint64_t address, int32_t value)
        {
            const int rangeIndex = GetInstance().getRangeIndex(address);

            if (rangeIndex == -1)
                return;

            void* writeAddress = static_cast<char*>(GetInstance()._regions[rangeIndex].BaseLocationProcess) + address - GetInstance()._regions[rangeIndex].Base;
            if (GetInstance()._isBigEndian) value = Xertz::SwapBytes<int32_t>(value);
            GetInstance()._processInfo.WriteExRAM(&value, writeAddress, 4);
        }

        static void writeInt64(const uint64_t address, int64_t value)
        {
            const int rangeIndex = GetInstance().getRangeIndex(address);

            if (rangeIndex == -1)
                return;

            void* writeAddress = static_cast<char*>(GetInstance()._regions[rangeIndex].BaseLocationProcess) + address - GetInstance()._regions[rangeIndex].Base;
            if (GetInstance()._isBigEndian) value = Xertz::SwapBytes<int64_t>(value);
            GetInstance()._processInfo.WriteExRAM(&value, writeAddress, 8);
        }

        static void writeFloat(const uint64_t address, float value)
        {
            const int rangeIndex = GetInstance().getRangeIndex(address);

            if (rangeIndex == -1)
                return;

            void* writeAddress = static_cast<char*>(GetInstance()._regions[rangeIndex].BaseLocationProcess) + address - GetInstance()._regions[rangeIndex].Base;
            if (GetInstance()._isBigEndian) value = Xertz::SwapBytes<float>(value);
            GetInstance()._processInfo.WriteExRAM(&value, writeAddress, 4);
        }

        static void writeDouble(const uint64_t address, double value)
        {
            const int rangeIndex = GetInstance().getRangeIndex(address);

            if (rangeIndex == -1)
                return;

            void* writeAddress = static_cast<char*>(GetInstance()._regions[rangeIndex].BaseLocationProcess) + address - GetInstance()._regions[rangeIndex].Base;
            if (GetInstance()._isBigEndian) value = Xertz::SwapBytes<double>(value);
            GetInstance()._processInfo.WriteExRAM(&value, writeAddress, 8);
        }

    public:
        static void DrawWindow();
        static void SetGameID(const char* ID);
	};
}