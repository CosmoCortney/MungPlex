#pragma once
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
#include "json5.hpp"

namespace MungPlex
{
	static class Cheats
	{
    private:
        Cheats()
        {
            _lua.open_libraries(sol::lib::base);
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


    public:
        static void DrawWindow();
	};
}