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
            _lua.open_libraries(sol::lib::base);
            _lua.set("INT8", INT8);
            _lua.set("INT16", INT16);
            _lua.set("INT32", INT32);
            _lua.set("INT64", INT64);
            _lua.set("FLOAT", FLOAT);
            _lua.set("DOUBLE", DOUBLE);
            _lua.set("BOOL", BOOL);

            _lua.set_function("ReadFromRAM", &readFromRAM);
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

        void DrawCheatList(); //top-left
        void DrawCheatInformation(); //top-right
        void DrawControl(); //bottom left

        static double readFromRAM(int type, uint64_t address)
        {
            bool bigEndian = Connection::IsBE();
            int64_t readValue = 0;
            double returnValue = 0.0;
            int pid = Connection::GetCurrentPID();
            std::vector<SystemRegion> regions = Connection::GetRegions();

            for (int i = 0; i < regions.size(); ++i)
            {
                if (address < regions[i].Base || address >= regions[i].Base + regions[i].Size)
                    continue;

                void* readAddress = (char*)regions[i].BaseLocationProcess + address - regions[i].Base;

                switch (type)
                {
                case INT8:
                    Xertz::SystemInfo::GetProcessInfo(pid).ReadExRAM(&readValue, readAddress, 1);
                    returnValue = (double)*(int8_t*)&readValue;
                    break;
                case BOOL:
                    Xertz::SystemInfo::GetProcessInfo(pid).ReadExRAM(&readValue, readAddress, 1);
                    returnValue = readValue != 0;
                    break;
                case INT16:
                    Xertz::SystemInfo::GetProcessInfo(pid).ReadExRAM(&readValue, readAddress, 2);
                    if (bigEndian) readValue = Xertz::SwapBytes<int16_t>(readValue);
                    returnValue = (double)*(int16_t*)&readValue;
                    break;
                case INT32:
                    Xertz::SystemInfo::GetProcessInfo(pid).ReadExRAM(&readValue, readAddress, 4);
                    if (bigEndian) readValue = Xertz::SwapBytes<int32_t>(readValue);
                    returnValue = (double)*(int32_t*)&readValue;
                    break;
                case INT64:
                    Xertz::SystemInfo::GetProcessInfo(pid).ReadExRAM(&readValue, readAddress, 8);
                    if (bigEndian) readValue = Xertz::SwapBytes<int64_t>(readValue);
                    returnValue = (double)*(int64_t*)&readValue;
                    break;
                case FLOAT:
                    Xertz::SystemInfo::GetProcessInfo(pid).ReadExRAM(&readValue, readAddress, 4);
                    if (bigEndian) readValue = Xertz::SwapBytes<int32_t>(readValue);
                    returnValue = (double)*(float*)&readValue;
                    break;
                default://DOUBLE
                    Xertz::SystemInfo::GetProcessInfo(pid).ReadExRAM(&readValue, readAddress, 8);
                    if (bigEndian) readValue = Xertz::SwapBytes<int64_t>(readValue);
                    returnValue = *(double*)&readValue;
                }

                break;
            }

            return returnValue;
        }


    public:
        static void DrawWindow();
	};
}