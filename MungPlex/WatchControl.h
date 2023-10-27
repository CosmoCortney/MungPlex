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
            std::wstring _moduleW = std::wstring(32, '\0');//
            std::string _module = std::string(32, '\0');//
            std::string _pointerPathText = std::string(128, '\0');//
            std::vector<int64_t> _pointerPath;//
            bool _useModulePath;//
            bool _plotting;
            bool _freeze;//
            bool _active = false;
            bool _hideAdvancedSettings;
            int _id;//
            std::string _idText;//

            void* GetCurrentPointer();
        };

        class IntegralView : View
        {
        public:
            static const std::vector<std::pair<std::string, int>> s_IntTypes;

        private: 
            int64_t _val;//
            int _typeSelect = ImGuiDataType_S8;
            bool _signed = false;//
            bool _hex = false;//
            int _plotCount = 64;
            std::vector<float> _plotVals;

        public:
            IntegralView(const int id, const std::string& pointerPathText, const uint8_t subtype, const int64_t min, const int64_t max, const int64_t val = 0, const int32_t integralViewFlags = 0, const std::string& module = "", const Xertz::ProcessInfo* process = nullptr);
            void Draw();
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


        void drawList();

    public:
        static void DrawWindow();
       
    };
}