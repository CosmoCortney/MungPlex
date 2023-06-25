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

        ~Cheats()
        {
            delete[] _textCheatTitle;
            delete[] _textCheatHacker;
            delete[] _textCheatLua;
            delete[] _textCheatDescription;
        }

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
        bool _rememberEnabled = false;
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



        static int luaExceptionHandler(lua_State* L, sol::optional<const std::exception&> exception, sol::string_view description);
        void copyCheatToInformationBox(const int index);
        void copyCheatToList(const int index);
        bool saveCheatList();
        void deleteCheat(const uint16_t index);
        void refreshModuleList();
        void DrawCheatList(); //top-left
        void DrawCheatInformation(); //top-right
        void DrawControl(); //bottom left
        void updateConnectionInfo()
        {
            _isBigEndian = Connection::IsBE();
            _pid = Connection::GetCurrentPID();
            _regions = Connection::GetRegions();
            refreshModuleList();
        }

        void initCheatFile();

        int getRangeIndex(uint64_t address);

        void cheatRoutine();

        //legacy function to keep older cheats functioning
        static double readFromRAM(int type, uint64_t address)
        {
            int64_t readValue = 0;
            double returnValue = 0.0;
            int rangeIndex = GetInstance().getRangeIndex(address);

            if (rangeIndex == -1)
                return returnValue = NAN;

            void* readAddress = (char*)GetInstance()._regions[rangeIndex].BaseLocationProcess + address - GetInstance()._regions[rangeIndex].Base;

            switch (type)
            {
            case INT8:
                GetInstance()._processInfo.ReadExRAM(&readValue, readAddress, 1);
                returnValue = (double)*(int8_t*)&readValue;
                break;
            case BOOL:
                GetInstance()._processInfo.ReadExRAM(&readValue, readAddress, 1);
                returnValue = readValue != 0;
                break;
            case INT16:
                GetInstance()._processInfo.ReadExRAM(&readValue, readAddress, 2);
                if (GetInstance()._isBigEndian) readValue = Xertz::SwapBytes<int16_t>(readValue);
                returnValue = (double)*(int16_t*)&readValue;
                break;
            case INT32:
                GetInstance()._processInfo.ReadExRAM(&readValue, readAddress, 4);
                if (GetInstance()._isBigEndian) readValue = Xertz::SwapBytes<int32_t>(readValue);
                returnValue = (double)*(int32_t*)&readValue;
                break;
            case INT64:
                GetInstance()._processInfo.ReadExRAM(&readValue, readAddress, 8);
                if (GetInstance()._isBigEndian) readValue = Xertz::SwapBytes<int64_t>(readValue);
                returnValue = (double)*(int64_t*)&readValue;
                break;
            case FLOAT:
                GetInstance()._processInfo.ReadExRAM(&readValue, readAddress, 4);
                if (GetInstance()._isBigEndian) readValue = Xertz::SwapBytes<int32_t>(readValue);
                returnValue = (double)*(float*)&readValue;
                break;
            default://DOUBLE
                GetInstance()._processInfo.ReadExRAM(&readValue, readAddress, 8);
                if (GetInstance()._isBigEndian) readValue = Xertz::SwapBytes<int64_t>(readValue);
                returnValue = *(double*)&readValue;
            }

            return returnValue;
        }

        static bool readBool(uint64_t address)
        {
            return readInt8(address) != 0;
        }

        static int8_t readInt8(uint64_t address)
        {
            return GetInstance()._isBigEndian ? readInt64(address) >> 56 : readInt64(address);
        }

        static uint8_t readUInt8(uint64_t address)
        {
            return readInt8(address);
        }

        static int16_t readInt16(uint64_t address)
        {
            return GetInstance()._isBigEndian ? readInt64(address) >> 48 : readInt64(address);
        }

        static uint16_t readUInt16(uint64_t address)
        {
            return readInt16(address);
        }

        static int32_t readInt32(uint64_t address)
        {
            return GetInstance()._isBigEndian ? readInt64(address) >> 32 : readInt64(address);
        }

        static uint32_t readUInt32(uint64_t address)
        {
            return readInt32(address);
        }

        static int64_t readInt64(uint64_t address)
        {
            int64_t readValue = 0;
            int rangeIndex = GetInstance().getRangeIndex(address);

            if (rangeIndex == -1)
                return 0;

            void* readAddress = (char*)GetInstance()._regions[rangeIndex].BaseLocationProcess + address - GetInstance()._regions[rangeIndex].Base;
            GetInstance()._processInfo.ReadExRAM(&readValue, readAddress, 8);
            if (GetInstance()._isBigEndian) readValue = Xertz::SwapBytes<int64_t>(readValue);
            return readValue;
        }

        static uint64_t readUInt64(uint64_t address)
        {
            return readInt64(address);
        }

        static float readFloat(uint64_t address)
        {
            float readValue = 0.0f;
            int rangeIndex = GetInstance().getRangeIndex(address);

            if (rangeIndex == -1)
                return 0.0f;

            void* readAddress = (char*)GetInstance()._regions[rangeIndex].BaseLocationProcess + address - GetInstance()._regions[rangeIndex].Base;
            GetInstance()._processInfo.ReadExRAM(&readValue, readAddress, 4);
            if (GetInstance()._isBigEndian) readValue = Xertz::SwapBytes<float>(readValue);
            return readValue;
        }

        static double readDouble(uint64_t address)
        {
            double readValue = 0.0;
            int rangeIndex = GetInstance().getRangeIndex(address);

            if (rangeIndex == -1)
                return 0.0;

            void* readAddress = (char*)GetInstance()._regions[rangeIndex].BaseLocationProcess + address - GetInstance()._regions[rangeIndex].Base;
            GetInstance()._processInfo.ReadExRAM(&readValue, readAddress, 8);
            if (GetInstance()._isBigEndian) readValue = Xertz::SwapBytes<double>(readValue);
            return readValue;
        }

        //legacy function to keep older cheats functioning
        static void writeToRAM(int type, uint64_t address, double value)
        {
            uint64_t writeValue;
            int rangeIndex = GetInstance().getRangeIndex(address);

            if (rangeIndex == -1)
                return;

            void* writeAddress = (char*)GetInstance()._regions[rangeIndex].BaseLocationProcess + address - GetInstance()._regions[rangeIndex].Base;

            switch (type)
            {
            case BOOL:
                writeValue = value != 0;
                GetInstance()._processInfo.WriteExRAM(&writeValue, writeAddress, 1);
                return;
            case INT8:
                writeValue = (int8_t)value;
                GetInstance()._processInfo.WriteExRAM(&writeValue, writeAddress, 1);
                return;
            case INT16:
                writeValue = (int16_t)value;
                if (GetInstance()._isBigEndian) writeValue = Xertz::SwapBytes<int16_t>(writeValue);
                GetInstance()._processInfo.WriteExRAM(&writeValue, writeAddress, 2);
                return;
            case INT32:
                writeValue = (int32_t)value;
                if (GetInstance()._isBigEndian) writeValue = Xertz::SwapBytes<int32_t>(writeValue);
                GetInstance()._processInfo.WriteExRAM(&writeValue, writeAddress, 4);
                return;
            case INT64:
                writeValue = (int64_t)value;
                if (GetInstance()._isBigEndian) writeValue = Xertz::SwapBytes<int64_t>(writeValue);
                GetInstance()._processInfo.WriteExRAM(&writeValue, writeAddress, 8);
                return;
            case FLOAT: {
                float temp = (float)value;
                writeValue = *(int32_t*)&temp;
                    if (GetInstance()._isBigEndian) writeValue = Xertz::SwapBytes<int32_t>(writeValue);
                    GetInstance()._processInfo.WriteExRAM(&writeValue, writeAddress, 4);
                } return;
            case DOUBLE: {
                writeValue = *(int64_t*)&value;
                if (GetInstance()._isBigEndian) writeValue = Xertz::SwapBytes<int64_t>(writeValue);
                GetInstance()._processInfo.WriteExRAM(&writeValue, writeAddress, 8);
            } return;
            }
        }

        static void writeBool(uint64_t address, bool value)
        {
            int rangeIndex = GetInstance().getRangeIndex(address);

            if (rangeIndex == -1)
                return;

            void* writeAddress = (char*)GetInstance()._regions[rangeIndex].BaseLocationProcess + address - GetInstance()._regions[rangeIndex].Base;
            GetInstance()._processInfo.WriteExRAM(&value, writeAddress, 1);
        }

        static void writeInt8(uint64_t address, int8_t value)
        {
            int rangeIndex = GetInstance().getRangeIndex(address);

            if (rangeIndex == -1)
                return;

            void* writeAddress = (char*)GetInstance()._regions[rangeIndex].BaseLocationProcess + address - GetInstance()._regions[rangeIndex].Base;
            GetInstance()._processInfo.WriteExRAM(&value, writeAddress, 1);
        }

        static void writeInt16(uint64_t address, int16_t value)
        {
            int rangeIndex = GetInstance().getRangeIndex(address);

            if (rangeIndex == -1)
                return;

            void* writeAddress = (char*)GetInstance()._regions[rangeIndex].BaseLocationProcess + address - GetInstance()._regions[rangeIndex].Base;
            if (GetInstance()._isBigEndian) value = Xertz::SwapBytes<int16_t>(value);
            GetInstance()._processInfo.WriteExRAM(&value, writeAddress, 2);
        }

        static void writeInt32(uint64_t address, int32_t value)
        {
            int rangeIndex = GetInstance().getRangeIndex(address);

            if (rangeIndex == -1)
                return;

            void* writeAddress = (char*)GetInstance()._regions[rangeIndex].BaseLocationProcess + address - GetInstance()._regions[rangeIndex].Base;
            if (GetInstance()._isBigEndian) value = Xertz::SwapBytes<int32_t>(value);
            GetInstance()._processInfo.WriteExRAM(&value, writeAddress, 4);
        }

        static void writeInt64(uint64_t address, int64_t value)
        {
            int rangeIndex = GetInstance().getRangeIndex(address);

            if (rangeIndex == -1)
                return;

            void* writeAddress = (char*)GetInstance()._regions[rangeIndex].BaseLocationProcess + address - GetInstance()._regions[rangeIndex].Base;
            if (GetInstance()._isBigEndian) value = Xertz::SwapBytes<int64_t>(value);
            GetInstance()._processInfo.WriteExRAM(&value, writeAddress, 8);
        }

        static void writeFloat(uint64_t address, float value)
        {
            int rangeIndex = GetInstance().getRangeIndex(address);

            if (rangeIndex == -1)
                return;

            void* writeAddress = (char*)GetInstance()._regions[rangeIndex].BaseLocationProcess + address - GetInstance()._regions[rangeIndex].Base;
            if (GetInstance()._isBigEndian) value = Xertz::SwapBytes<float>(value);
            GetInstance()._processInfo.WriteExRAM(&value, writeAddress, 4);
        }

        static void writeDouble(uint64_t address, double value)
        {
            int rangeIndex = GetInstance().getRangeIndex(address);

            if (rangeIndex == -1)
                return;

            void* writeAddress = (char*)GetInstance()._regions[rangeIndex].BaseLocationProcess + address - GetInstance()._regions[rangeIndex].Base;
            if (GetInstance()._isBigEndian) value = Xertz::SwapBytes<double>(value);
            GetInstance()._processInfo.WriteExRAM(&value, writeAddress, 8);
        }

    public:
        static void DrawWindow();
        static void SetGameID(const char* ID);
	};
}