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
#include"LitColor.h"


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

        std::vector<std::pair<std::string, int>> _specializedColorTypes;

        void drawPrimitiveConversion();
        void drawColorConversion();
        void drawTextConversion();
        ImVec4 specializedColorToPackedColor();


    public:
        static void DrawWindow();
        
    };
}