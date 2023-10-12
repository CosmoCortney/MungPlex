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
#include <utility>


namespace MungPlex
{
    class DataConversion
    {
    private:
        DataConversion();
        ~DataConversion() {}
        DataConversion(const DataConversion&) = delete;
        DataConversion(DataConversion&&) = delete;
        void operator=(const DataConversion) = delete;
        void operator=(DataConversion&&) = delete;
        static DataConversion& GetInstance()
        {
            static DataConversion Instance;
            return Instance;
        }

        enum FloatTypes
        {
            FLOAT, DOUBLE
        };

        std::vector<std::pair<std::string, int>> _specializedColorTypes;
        std::vector<std::pair<std::string, int>> _floatTypes;
        ImVec2 _verticalSpacing;

        void drawPrimitiveConversion();
        void drawColorConversion();
        void drawTextConversion();


    public:
        static void DrawWindow();
        
    };
}