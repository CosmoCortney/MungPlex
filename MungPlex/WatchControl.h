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
#include <vector>
#include <variant>
#include<utility>
#include"MungPlexConfig.h"
#include"Xertz.h"
#include"nlohmann/json.hpp"

namespace MungPlex
{

    class WatchControl
    {
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

        enum ViewTypes
        {
            INTEGRAL, FLOAT, BOOL, MOUSEPIANO, COLOR, STRING
        };

        enum DefaultViewFlags
        {
            USE_MODULEPATH = 1, PLOTTING = 1 << 2, FREEZE = 1 << 3, ACTIVE = 1 << 4 
        };

        enum IntegralViewFlags
        {
           HEX = 1 << 5
        };

        class View
        {
        public:
            static Xertz::ProcessInfo s_Process;
            std::wstring _moduleW = std::wstring(32, '\0');
            std::string _module = std::string(32, '\0');
            std::string _pointerPathText = std::string(128, '\0');
            std::string _label = std::string(128, '\0');
            std::vector<int64_t> _pointerPath;
            bool _useModulePath = false;
            bool _freeze = false;
            bool _active = false;
            int _id;
            std::string _idText;
            uint64_t _rangeMin = 0;
            uint64_t _rangeMax = 0;
            int _typeSelect = 0;
            static const std::vector<std::pair<std::string, int>> s_IntTypes;

            int GetID();
            void* GetCurrentPointer();
            nlohmann::json GetBasicJSON();
            void SetBasicMembers(const nlohmann::json elem);
            void DrawSetup(const float itemWidth, const float itemHeight, const int tyoe);
        };

        class IntegralView : View
        {
        private: 
            int64_t _val = 0;
            bool _hex = false;
            int _plotCount = 128;
            std::vector<float> _plotVals;
            uint64_t _plotMin = 0;
            uint64_t _plotMax = 255;
            float _plotMinF = 0.0f;
            float _plotMaxF = 0.0f;
            std::string _plotBuf = std::string(64, '\0');
            std::string _formatPlot;

        public:
            IntegralView(const int id);
            IntegralView(const int id, const nlohmann::json elem);
            void Draw();
            nlohmann::json GetJSON();
        };

        class FloatView : View
        {
            double Val;
            bool DoublePrecision;
            bool _useSlider = false;
            int64_t _min;
            int64_t _max;
        };

        class MousePiano : View
        {
            std::vector<bool> Switches;
            std::vector<bool> Freeze;
        };

        //void drawControl();

        std::vector<std::pair<int, std::variant<IntegralView, MousePiano>>> _views;
        std::vector<int> _ids;
        std::wstring _currentFile;
        std::string _placeholderFile = "{\"Watchlist\":[]}";

        void drawList();
        bool saveList();

    public:
        static void DrawWindow();
        static void InitWatchFile();
    };
}