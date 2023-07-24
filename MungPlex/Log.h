#pragma once
#include <iostream>
#include <stdio.h>
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "examples/libs/emscripten/emscripten_mainloop_stub.h"
#include <string>

namespace MungPlex
{
    class Log
    {
    private:
        Log() {}
        ~Log() {}
        Log(const Log&) = delete;
        Log(Log&&) = delete;
        void operator=(const Log&) = delete;
        void operator=(Log&&) = delete;
        static Log& GetInstance()
        {
            static Log Instance;
            return Instance;
        }
        
        std::string _logMessage;
        void clearLog();

    public:
        static void DrawWindow();
        static void LogInformation(const char* text);
    };
}