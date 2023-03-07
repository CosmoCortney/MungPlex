#pragma once
#include <iostream>
#include <stdio.h>
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "examples/libs/emscripten/emscripten_mainloop_stub.h"
#include<string>
#include "Xertz.h"
#include <Windows.h>
#include "MungPlexConfig.h"
#include "Connection.h"
#include"HelperFunctions.h"

namespace MungPlex
{
    class ProcessInformation
    {
    private:
        ProcessInformation()
        {
        }
        ~ProcessInformation() {};
        ProcessInformation(const ProcessInformation&) = delete;
        ProcessInformation(ProcessInformation&&) = delete;
        void operator=(const ProcessInformation&) = delete;
        void operator=(ProcessInformation&&) = delete;
        static ProcessInformation& GetInstance()
        {
            static ProcessInformation Instance;
            return Instance;
        }

        int _currentPID;
        MODULE_LIST _modules;
        REGION_LIST _regions;

        static void DrawModuleList();
        static void DrawRegionList();
        static void DrawMiscInformation();

    public:
        static void DrawWindow();
        static void RefreshData(int PID);

    };
}