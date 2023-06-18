#pragma once
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
#include"Connection.h"

namespace MungPlex
{
    class Cheats
	{
    private:
        Cheats()
        {
            updateConnectionInfo();
            _lua.open_libraries(sol::lib::base);
            _lua.set("INT8", INT8);
            _lua.set("INT16", INT16);
            _lua.set("INT32", INT32);
            _lua.set("INT64", INT64);
            _lua.set("FLOAT", FLOAT);
            _lua.set("DOUBLE", DOUBLE);
            _lua.set("BOOL", BOOL);

            _lua.set_function("ReadFromRAM", &readFromRAM);
            _lua.set_function("WriteToRAM", &writeToRAM);
        }

        ~Cheats()
        {
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

        char _textCheatLua[1024 * 128];
        char _textCheatDescription[1024 * 16];
        std::wstring _cheatListPath;
        uint16_t _timesPerSecond = 60;
        sol::state _lua{};
        bool _isBigEndian = false;
        int _pid = 0;
        std::vector<SystemRegion> _regions{};

        void DrawCheatList(); //top-left
        void DrawCheatInformation(); //top-right
        void DrawControl(); //bottom left
        void updateConnectionInfo()
        {
            _isBigEndian = Connection::IsBE();
            _pid = Connection::GetCurrentPID();
            _regions = Connection::GetRegions();
        }

        //legacy function to keep older cheats functioning
        static double readFromRAM(int type, uint64_t address)
        {
            int64_t readValue = 0;
            double returnValue = 0.0;

            for (int i = 0; i < GetInstance()._regions.size(); ++i)
            {
                if (address < GetInstance()._regions[i].Base || address >= GetInstance()._regions[i].Base + GetInstance()._regions[i].Size)
                    continue;

                void* readAddress = (char*)GetInstance()._regions[i].BaseLocationProcess + address - GetInstance()._regions[i].Base;

                switch (type)
                {
                case INT8:
                    Xertz::SystemInfo::GetProcessInfo(GetInstance()._pid).ReadExRAM(&readValue, readAddress, 1);
                    returnValue = (double)*(int8_t*)&readValue;
                    break;
                case BOOL:
                    Xertz::SystemInfo::GetProcessInfo(GetInstance()._pid).ReadExRAM(&readValue, readAddress, 1);
                    returnValue = readValue != 0;
                    break;
                case INT16:
                    Xertz::SystemInfo::GetProcessInfo(GetInstance()._pid).ReadExRAM(&readValue, readAddress, 2);
                    if (GetInstance()._isBigEndian) readValue = Xertz::SwapBytes<int16_t>(readValue);
                    returnValue = (double)*(int16_t*)&readValue;
                    break;
                case INT32:
                    Xertz::SystemInfo::GetProcessInfo(GetInstance()._pid).ReadExRAM(&readValue, readAddress, 4);
                    if (GetInstance()._isBigEndian) readValue = Xertz::SwapBytes<int32_t>(readValue);
                    returnValue = (double)*(int32_t*)&readValue;
                    break;
                case INT64:
                    Xertz::SystemInfo::GetProcessInfo(GetInstance()._pid).ReadExRAM(&readValue, readAddress, 8);
                    if (GetInstance()._isBigEndian) readValue = Xertz::SwapBytes<int64_t>(readValue);
                    returnValue = (double)*(int64_t*)&readValue;
                    break;
                case FLOAT:
                    Xertz::SystemInfo::GetProcessInfo(GetInstance()._pid).ReadExRAM(&readValue, readAddress, 4);
                    if (GetInstance()._isBigEndian) readValue = Xertz::SwapBytes<int32_t>(readValue);
                    returnValue = (double)*(float*)&readValue;
                    break;
                default://DOUBLE
                    Xertz::SystemInfo::GetProcessInfo(GetInstance()._pid).ReadExRAM(&readValue, readAddress, 8);
                    if (GetInstance()._isBigEndian) readValue = Xertz::SwapBytes<int64_t>(readValue);
                    returnValue = *(double*)&readValue;
                }

                break;
            }

            return returnValue;
        }

        //legacy function to keep older cheats functioning
        static void writeToRAM(int type, uint64_t address, double value)
        {
            uint64_t writeValue;

            for (int i = 0; i < GetInstance()._regions.size(); ++i)
            {
                if (address < GetInstance()._regions[i].Base || address >= GetInstance()._regions[i].Base + GetInstance()._regions[i].Size)
                    continue;

                void* writeAddress = (char*)GetInstance()._regions[i].BaseLocationProcess + address - GetInstance()._regions[i].Base;
                switch (type)
                {
                case BOOL:
                    writeValue = value != 0;
                    Xertz::SystemInfo::GetProcessInfo(GetInstance()._pid).WriteExRAM(&writeValue, writeAddress, 1);
                    return;
                case INT8:
                    writeValue = (int8_t)value;
                    Xertz::SystemInfo::GetProcessInfo(GetInstance()._pid).WriteExRAM(&writeValue, writeAddress, 1);
                    return;
                case INT16:
                    writeValue = (int16_t)value;
                    if (GetInstance()._isBigEndian) writeValue = Xertz::SwapBytes<int16_t>(writeValue);
                    Xertz::SystemInfo::GetProcessInfo(GetInstance()._pid).WriteExRAM(&writeValue, writeAddress, 2);
                    return;
                case INT32:
                    writeValue = (int32_t)value;
                    if (GetInstance()._isBigEndian) writeValue = Xertz::SwapBytes<int32_t>(writeValue);
                    Xertz::SystemInfo::GetProcessInfo(GetInstance()._pid).WriteExRAM(&writeValue, writeAddress, 4);
                    return;
                case INT64:
                    writeValue = (int64_t)value;
                    if (GetInstance()._isBigEndian) writeValue = Xertz::SwapBytes<int64_t>(writeValue);
                    Xertz::SystemInfo::GetProcessInfo(GetInstance()._pid).WriteExRAM(&writeValue, writeAddress, 8);
                    return;
                case FLOAT: {
                    float temp = (float)value;
                    writeValue = *(int32_t*)&temp;
                        if (GetInstance()._isBigEndian) writeValue = Xertz::SwapBytes<int32_t>(writeValue);
                    Xertz::SystemInfo::GetProcessInfo(GetInstance()._pid).WriteExRAM(&writeValue, writeAddress, 4);
                    } return;
                case DOUBLE: {
                    writeValue = *(int64_t*)&value;
                    if (GetInstance()._isBigEndian) writeValue = Xertz::SwapBytes<int64_t>(writeValue);
                    Xertz::SystemInfo::GetProcessInfo(GetInstance()._pid).WriteExRAM(&writeValue, writeAddress, 8);
                } return;
                }
            }
        }



    public:
        static void DrawWindow();
	};
}