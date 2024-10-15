#pragma once
#include <boost/asio.hpp>
#include "BoolView_Impl.hpp"
#include <array>
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "examples/libs/emscripten/emscripten_mainloop_stub.h"
#include "FloatView_Impl.hpp"
#include "IntegralView_Impl.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "GLFW/glfw3.h"
#include <iostream>
#include "MousePianoView_Impl.hpp"
#include "MungPlexConfig.hpp"
#include "nlohmann/json.hpp"
#include <stdio.h>
#include <string>
#include <utility>
#include <variant>
#include <vector>
#include "Xertz.hpp"

namespace MungPlex
{
    class WatchControl
    {
    public:
        static void DrawWindow();
        static void InitWatchFile();
        static void DeleteItem(const int id);

    private:
        WatchControl() {}
        ~WatchControl() {}
        WatchControl(const WatchControl&) = delete;
        WatchControl(WatchControl&&) = delete;
        void operator=(const WatchControl&) = delete;
        void operator=(WatchControl&&) = delete;
        static WatchControl& GetInstance()
        {
            static WatchControl Instance;
            return Instance;
        }

        std::vector<std::pair<int, std::variant<IntegralView, FloatView, BoolView, MousePianoView>>> _views;
        std::vector<int> _ids;
        std::wstring _currentFile;
        std::string _placeholderFile = "{\"Watchlist\":[]}";

        void drawList();
        bool saveList();
    };
}