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
#include"imgui_memory_editor.h"

namespace MungPlex
{
    class MemoryViewer
    {
    private:
        uint32_t _id = 0;
        std::string _windowTitle;
        bool _isOpen = false;
        int _regionSelect = 0;
        std::string _bufAddress;
        uint64_t _address;
        std::string _hexView;
        MemoryEditor _memEdit;
        uint64_t _viewAddress = 0;
        uint32_t _readSize = 256;
        uint64_t _displayAddress = 0;
        void* _readAddressEx = nullptr;
        std::string _dummy;
        HANDLE _handle;
        bool _validAddress = false;

        void drawControlPanel();
        void drawHexEditor();

    public:
        MemoryViewer(const uint32_t id);
        void DrawWindow();
        void SetIndex(const uint32_t id);
        bool IsOpen();
        void setUpByRegionSelect(const int index);
        void processBufferAddress();
    };
}