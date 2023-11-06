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
            INTEGRAL, FLOAT, BOOL, MOUSEPIANO, MAP2D, MAP3D, COLOR
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
            bool _delete = false;
            std::string _idText;
            uint64_t _rangeMin = 0;
            uint64_t _rangeMax = 0;
            int _typeSelect = 0;
            static const std::vector<std::pair<std::string, int>> s_IntTypes;
            static const std::vector<std::pair<std::string, int>> s_FloatTypes;
            static const std::vector<std::pair<std::string, int>> s_SuperiorTypes;

            int GetID();
            void* GetCurrentPointer();
            nlohmann::json GetBasicJSON();
            void SetBasicMembers(const nlohmann::json elem);
            void DrawSetup(const float itemWidth, const float itemHeight, const int type);
        };

        class IntegralView : public View
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

        class FloatView : public View
        {
        private:
            double _val = 0.0;
            bool DoublePrecision;
            bool _useSlider = false;
            int64_t _min;
            int64_t _max;
            int _plotCount = 128;
            std::vector<float> _plotVals;
            float _plotMin = 0.0f;
            float _plotMax = 0.0f;
            std::string _plotBuf = std::string(64, '\0');

        public:
            FloatView(const int id);
            FloatView(const int id, const nlohmann::json elem);
            void Draw();
            nlohmann::json GetJSON();
        };

        class BoolView : public View
        {
        private:
            bool _val = false;

        public:
            BoolView(const int id);
            BoolView(const int id, const nlohmann::json elem);
            void Draw();
            nlohmann::json GetJSON();
        };

        class MousePiano : public View
        {
        private: 
            std::vector<std::pair<std::string, bool>> _switches;
            std::string _idText;

        public:
            MousePiano(const int id);
            MousePiano(const int id, const nlohmann::json elem);
            void Draw();
            nlohmann::json GetJSON();
        };

        std::vector<std::pair<int, std::variant<IntegralView, FloatView, BoolView, MousePiano>>> _views;
        std::vector<int> _ids;
        std::wstring _currentFile;
        std::string _placeholderFile = "{\"Watchlist\":[]}";

        void drawList();
        bool saveList();

    public:
        static void DrawWindow();
        static void InitWatchFile();
        static void DeleteItem(const int id);
    };
}