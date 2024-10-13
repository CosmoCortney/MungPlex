#pragma once
#include <boost/asio.hpp>
#include <array>
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "examples/libs/emscripten/emscripten_mainloop_stub.h"
#include "IView.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "GLFW/glfw3.h"
#include <iostream>
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

        class IntegralView : public IView
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

        class FloatView : public IView
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

        class BoolView : public IView
        {
        private:
            bool _val = false;

        public:
            BoolView(const int id);
            BoolView(const int id, const nlohmann::json elem);
            void Draw();
            nlohmann::json GetJSON();
        };

        class MousePiano : public IView
        {
        private: 
            std::vector<std::pair<std::string, bool>> _switches;
            std::array<std::string, 32> _checkBoxIDs;
            std::array<std::string, 32> _checkBoxTextIDs;
            std::string _idText;

            void assignCheckBoxIDs();

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
    };
}