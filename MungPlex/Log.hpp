#pragma once
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "examples/libs/emscripten/emscripten_mainloop_stub.h"
#include "imgui.h"
#include "imgui_internal.h"
#include <iostream>
#include "GLFW/glfw3.h"
#include <string>
#include "WidgetHelpers.hpp"

namespace MungPlex
{
    class Log
    {
    public:
        static void DrawWindow();
        static void LogInformation(const char* text, const bool appendToLast = false, const int indentation = 0);
        static void LogInformation(const std::string& text, const bool appendToLast = false, const int indentation = 0);

    private:
        Log() {}
        ~Log();
        Log(const Log&) = delete;
        Log(Log&&) = delete;
        void operator=(const Log&) = delete;
        void operator=(Log&&) = delete;
        static Log& GetInstance()
        {
            static Log Instance;
            return Instance;
        }

        InputTextMultiline _logMessage = InputTextMultiline("Log Msg:", false, "", 0x10000, ImGuiInputTextFlags_ReadOnly);
        bool _logToFile = true;
        std::fstream* _logFile = nullptr;
        bool _initialized = false;
        std::string _logPath;

        bool init();
        void clear(const bool deleteFileOnly = false);
    };
}