#pragma once
#include <boost/asio.hpp>
#include <stdio.h>
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "examples/libs/emscripten/emscripten_mainloop_stub.h"
#include <fstream>
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "Xertz.hpp"
#include "imgui_memory_editor.h"
#include <iomanip>
#include "MungPlexConfig.hpp"
#include "ProcessInformation.hpp"
#include <string>
#include "WidgetHelpers.hpp"

namespace MungPlex
{
    class MemoryViewer
    {
    public:
        MemoryViewer(const uint32_t id);
        void DrawWindow();
        void SetIndex(const uint32_t id);
        bool IsOpen();
        void SetUpByRegionSelect(const int index);

    private:
        uint32_t _id = 0;
        std::string _windowTitle;
        bool _isOpen = false;
        int _regionSelect = 0;
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
        bool _rereorder = false;
        InputText _targetAddressInput = InputText("Go to Address:", true, "", 16);

        void drawControlPanel();
        void drawHexEditor();
        void processBufferAddress();
        void refreshMemory();
    };
}