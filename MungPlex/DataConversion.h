#pragma once
#include "GLFW/glfw3.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "examples/libs/emscripten/emscripten_mainloop_stub.h"
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

        std::vector<std::pair<std::string, int>> _specializedColorTypes;
        std::vector<std::pair<std::string, int>> _floatTypes;
        std::vector<std::pair<std::string, int>> _intTypes;
        const std::vector<std::pair<std::string, int>> _textTypes = 
        {
            { "UTF-8", MorphText::UTF8 }, 
            { "UTF-16 Little Endian", MorphText::UTF16LE }, 
            { "UTF-16 Big Endian", MorphText::UTF16BE }, 
            { "UTF-32 Little Endian", MorphText::UTF32LE }, 
            { "UTF-32 Big Endian", MorphText::UTF32BE }, 
            { "ASCII", MorphText::ASCII }, 
            { "ISO-8859-1 (Latin 1)", MorphText::ISO_8859_1 }, 
            { "ISO-8859-2 (Latin 2)", MorphText::ISO_8859_2 }, 
            { "ISO-8859-3 (Latin 3)", MorphText::ISO_8859_3 }, 
            { "ISO-8859-4 (Latin 4)", MorphText::ISO_8859_4 }, 
            { "ISO-8859-5 (Cyrillic)", MorphText::ISO_8859_5 }, 
            { "ISO-8859-6 (Arabic)", MorphText::ISO_8859_6 }, 
            { "ISO-8859-7 (Greek)", MorphText::ISO_8859_7 }, 
            { "ISO-8859-8 (Hebrew)", MorphText::ISO_8859_8 }, 
            { "ISO-8859-9 (Turkish)", MorphText::ISO_8859_9 }, 
            { "ISO-8859-10 (Nordic)", MorphText::ISO_8859_10 }, 
            { "ISO-8859-11 (Thai)", MorphText::ISO_8859_11 }, 
            { "ISO-8859-13 (Baltic)", MorphText::ISO_8859_13 }, 
            { "ISO-8859-14 (Celtic", MorphText::ISO_8859_14 }, 
            { "ISO-8859-15 (West European)", MorphText::ISO_8859_15 }, 
            { "ISO-8859-16 (South-East European)", MorphText::ISO_8859_16 },
            { "Shift-Jis CP932", MorphText::SHIFTJIS_CP932 },
            { "KS X 1001 (EUC-KR)", MorphText::KS_X_1001 },
            { "Jis X 0201 Full Width", MorphText::JIS_X_0201_FULLWIDTH }, 
            { "Jis X 0201 Half Width", MorphText::JIS_X_0201_HALFWIDTH }
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