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
#include"MemoryViewer.h"

namespace MungPlex
{
    class MemoryViewer;

    class Connection
    {
    private:
        Connection()
        {
            _memoryViewers.reserve(16);
            strcpy_s(_connectionMessage, "Not connected...");
        }

        ~Connection() {}
        Connection(const Connection&) = delete;
        Connection(Connection&&) = delete;
        void operator=(const Connection&) = delete;
        void operator=(Connection&&) = delete;
        static Connection& GetInstance()
        {
            static Connection Instance;
            return Instance;
        }

        bool _connected = false;
        bool _isOpen = true;
        uint32_t _memViewerCount = 0;
        int _selectedEmulatorIndex = 0;
        char _connectionMessage[256];
        std::vector<MemoryViewer> _memoryViewers;
        void DrawConnectionSelect();
        void drawAdditionalFeatureSelect();

    public:
        static void DrawWindow();
        static bool IsConnected();
        static bool IsOpen();
    };
}