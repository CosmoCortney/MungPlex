#pragma once
#include "GLFW/glfw3.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "examples/libs/emscripten/emscripten_mainloop_stub.h"
#include "HelperFunctions.hpp"

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

        static const StringIdPairs _specializedColorTypes;
        static const StringIdPairs _floatTypes;
        static const StringIdPairs _intTypes;
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