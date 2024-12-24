#pragma once
#include "GLFW/glfw3.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "examples/libs/emscripten/emscripten_mainloop_stub.h"
#include "HelperFunctions.hpp"
#include "WidgetHelpers.hpp"

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
        InputText _specializedColorValueInput = InputText("Specializes Color Val:", false, "", 24);
        InputInt<uint32_t> _hexFloatInput = InputInt<uint32_t>("Hex Float:", true, 0x3F800000, 0x10000, 0x1000000);
        InputInt<uint64_t> _hexDoubleInput = InputInt<uint64_t>("Hex Double:", true, 0x3FF0000000000000, 0x1000000000000, 0x100000000000000);
        InputText _littleEndianInput = InputText("Little Endian:", true, "", 16);
        InputText _bigEndianInput = InputText("Big Endian:", true, "", 16);
        InputTextMultiline _plainTextInput = InputTextMultiline("Plain Text:", true, "", 512);
        InputFloat<float> _floatSingleConvertInput = InputFloat<float>("Float:", true, 1.0f, 0.1f, 1.0f, 6);
        InputFloat<double> _floatDoubleConvertInput = InputFloat<double>("Double:", true, 1.0, 0.1, 1.0, 10);

        void drawPrimitiveConversion();
        void drawHexFloatConversion();
        void drawEndiannessConversion();
        std::string swapBytes(const std::string& in, const int select);
        void drawColorConversion();
        void drawTextConversion();
        void convertText(std::string& in, std::string& out, const int textTypeSelect);
        void convertHexText(std::string& in, std::string& out, const int textTypeSelect);
    };
}