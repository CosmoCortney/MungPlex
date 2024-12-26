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

        static const StringIdPairs _specializedColorTypes;
        static const StringIdPairs _floatTypes;
        static const std::vector<std::pair<std::string, uint32_t>> _intTypes;
        ImVec2 _verticalSpacing;
        InputText _specializedColorValueInput = InputText("Specializes Color Val:", false, "", 32);
        InputInt<uint32_t> _hexFloatInput = InputInt<uint32_t>("Hex Float:", true, 0x3F800000, 0x10000, 0x1000000);
        InputInt<uint64_t> _hexDoubleInput = InputInt<uint64_t>("Hex Double:", true, 0x3FF0000000000000, 0x1000000000000, 0x100000000000000);
        InputInt<uint16_t> _littleEndianInput16 = InputInt<uint16_t>("Little Endian:", true, 0, 0, 0);
        InputInt<uint32_t> _littleEndianInput32 = InputInt<uint32_t>("Little Endian:", true, 0, 0, 0);
        InputInt<uint64_t> _littleEndianInput64 = InputInt<uint64_t>("Little Endian:", true, 0, 0, 0);
        InputInt<uint16_t> _bigEndianInput16 = InputInt<uint16_t>("Big Endian:", true, 0, 0, 0);
        InputInt<uint32_t> _bigEndianInput32 = InputInt<uint32_t>("Big Endian:", true, 0, 0, 0);
        InputInt<uint64_t> _bigEndianInput64 = InputInt<uint64_t>("Big Endian:", true, 0, 0, 0);
        InputTextMultiline _plainTextInput = InputTextMultiline("Plain Text:", true, "", 512);
        InputFloat<float> _floatSingleConvertInput = InputFloat<float>("Float:", true, 1.0f, 0.1f, 1.0f, 6);
        InputFloat<double> _floatDoubleConvertInput = InputFloat<double>("Double:", true, 1.0, 0.1, 1.0, 10);
        StringIdCombo _intTypesCombo = StringIdCombo("Int Type:", true, _intTypes);

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