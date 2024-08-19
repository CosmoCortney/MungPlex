#pragma once
#include "GLFW/glfw3.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "examples/libs/emscripten/emscripten_mainloop_stub.h"
#include "LitColor.hpp"
#include <string>
#include <vector>

namespace MungPlex
{
    class DataConversion
    {
    public:
        static void DrawWindow();

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

        enum IntTypes
        {
            INT16, INT32, INT64
        };

        const std::vector<std::pair<std::string, int>> _specializedColorTypes
        {
            { "RGBF (3 Floats)", LitColor::RGBF },
            { "RGBAF (4 Floats)", LitColor::RGBAF },
            { "RGB 565 (2 Bytes)", LitColor::RGB565 },
            { "RGB 5A3 (2 Bytes)", LitColor::RGB5A3 }
        };

        const std::vector<std::pair<std::string, int>> _floatTypes
        {
            { "Float Single", FloatTypes::FLOAT },
            { "Float Double", FloatTypes::DOUBLE }
        };

        const std::vector<std::pair<std::string, int>> _intTypes
        {
            { "Int 16", IntTypes::INT16 },
            { "Int 32", IntTypes::INT32 },
            { "Int 64", IntTypes::INT64 }
        };

        ImVec2 _verticalSpacing;

        void drawPrimitiveConversion();
        void drawHexFloatConversion();
        void drawEndiannessConversion();
        std::string swapBytes(std::string& in, const int select);
        void drawColorConversion();
        void drawTextConversion();
        void convertText(std::string& in, std::string& out, const int textTypeSelect);
        void convertHexText(std::string& in, std::string& out, const int textTypeSelect);
    };
}