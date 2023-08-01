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
#include<fstream>
#include <iomanip>
#include "Xertz.h"
#include <Windows.h>
#include "MungPlexConfig.h"
#include"ProcessInformation.h"

namespace MungPlex
{
    class MemoryViewer
    {
    private:

        uint32_t _id = 0;
        std::string _windowTitle;
        bool _isOpen = false;

    public:
        MemoryViewer(const uint32_t id);
        ~MemoryViewer() { }

        void DrawWindow();
        void SetIndex(const uint32_t id);
        bool IsOpen();
    };
}