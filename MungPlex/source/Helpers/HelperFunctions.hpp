#pragma once
#include <algorithm>
#include <chrono>
#include "Connection.hpp"
#include <cstdint>
#include <ctime>
#include <filesystem>
#include <functional>
#include "GLFW/glfw3.h"
#include "imgui.h"
#include <iomanip>
#include <iostream>
#include "LitColor.hpp"
#include "MorphText.hpp"
#include "OperativeArray.hpp"
#include "Pairs.hpp"
#include <sstream>
#include <stdio.h>
#include <stdlib.h>

typedef MorphText MT;

namespace MungPlex
{
    struct SystemRegion;

    inline static const MungPlex::StringIdPairs TextTypes =
    {
        { "UTF-8",  "UTF-16 Little Endian", "UTF-16 Big Endian", "UTF-32 Little Endian", "UTF-32 Big Endian", "ASCII",  "ISO-8859-1 (Latin 1)", "ISO-8859-2 (Latin 2)", "ISO-8859-3 (Latin 3)", "ISO-8859-4 (Latin 4)", "ISO-8859-5 (Cyrillic)", "ISO-8859-6 (Arabic)", "ISO-8859-7 (Greek)",  "ISO-8859-8 (Hebrew)", "ISO-8859-9 (Turkish)", "ISO-8859-10 (Nordic)", "ISO-8859-11 (Thai)", "ISO-8859-13 (Baltic)", "ISO-8859-14 (Celtic)", "ISO-8859-15 (West European)", "ISO-8859-16 (South-East European)", "Shift-Jis CP932",  "KS X 1001 (EUC-KR)", "Jis X 0201 Full Width",  "Jis X 0201 Half Width",  "PKMN Gen I English",     "PKMN Gen I French/German",     "PKMN Gen I Italian/Spanish",     "PKMN Gen I Japanese" },
        { MT::UTF8, MT::UTF16LE,            MT::UTF16BE,         MT::UTF32LE,            MT::UTF32BE,         MT::ASCII, MT::ISO_8859_1,        MT::ISO_8859_2,         MT::ISO_8859_3,         MT::ISO_8859_4,         MT::ISO_8859_5,          MT::ISO_8859_6,        MT::ISO_8859_7,        MT::ISO_8859_8,        MT::ISO_8859_9,         MT::ISO_8859_10,        MT::ISO_8859_11,      MT::ISO_8859_13,        MT::ISO_8859_14,        MT::ISO_8859_15,               MT::ISO_8859_16,                     MT::SHIFTJIS_CP932, MT::KS_X_1001,        MT::JIS_X_0201_FULLWIDTH, MT::JIS_X_0201_HALFWIDTH, MT::POKEMON_GEN1_ENGLISH, MT::POKEMON_GEN1_FRENCH_GERMAN, MT::POKEMON_GEN1_ITALIAN_SPANISH, MT::POKEMON_GEN1_JAPANESE },
        "Text Types:"
    };

    template<typename addressType> static addressType TranslatePtrTo4BytesReorderingPtr(addressType ptr)
    {
        uint64_t tempPtr;

        if constexpr (std::is_same_v<uint64_t, addressType>)
            tempPtr = ptr;
        else
            tempPtr = reinterpret_cast<uint64_t>(ptr);

        switch (tempPtr & 0xF)
        {
        case 1: case 5: case 9: case 0xD:
            ++tempPtr;
            break;
        case 2: case 6: case 0xA: case 0xE:
            --tempPtr;
            break;
        case 3: case 7: case 0xB: case 0xF:
            tempPtr -= 3;
            break;
        default: //0, 4, 8, C
            tempPtr += 3;
        }

        if constexpr (std::is_same_v<uint64_t, addressType>)
            return tempPtr;
        else
            return reinterpret_cast<addressType>(tempPtr);
    }

    template<typename dataType> static void ReadFromReorderedRangeEx(const Xertz::ProcessInfo& process, dataType* out, void* readAddress)
    {
        char* address = reinterpret_cast<char*>(readAddress);
        char* dest = reinterpret_cast<char*>(out);

        for (int i = 0; i < sizeof(dataType); ++i)
        {
            char* reorderedAddress = TranslatePtrTo4BytesReorderingPtr<char*>(address + i);
            process.ReadMemoryFast(dest + i, reorderedAddress, 1);
        }

        return;
    }

    template<typename dataType> static void WriteToReorderedRangeEx(const Xertz::ProcessInfo& process, dataType* in, void* writeAddress)
    {
        char* address = reinterpret_cast<char*>(writeAddress);
        char* dest = reinterpret_cast<char*>(in);

        for (int i = 0; i < sizeof(dataType); ++i)
        {
            char* reorderedAddress = TranslatePtrTo4BytesReorderingPtr<char*>(address + i);
            process.WriteMemoryFast(dest + i, reorderedAddress, 1);
        }

        return;
    }

    static void Rereorder4BytesReorderedMemory(void* ptr, const uint64_t size)
    {
        uint32_t* swapPtr = reinterpret_cast<uint32_t*>(ptr);

        for (uint64_t offset = 0; offset < (size >> 2); ++offset)
        {
            swapPtr[offset] = Xertz::SwapBytes<uint32_t>(swapPtr[offset]);
        }
    }

    static bool WriteTextEx(const uint32_t pid, char* text, const uint64_t address)
    {
        uint32_t textLength = strlen(text);

        if (text[textLength - 1] == '\n')
            --textLength;

        Xertz::SystemInfo::GetProcessInfo(pid).WriteMemoryFast(text, reinterpret_cast<void*>(address), textLength);
        return true;
    }

    static bool WriteTextEx(const uint32_t pid, wchar_t* text, const uint64_t address)
    {
        uint32_t textLength = wcslen(text);

        if (text[textLength - 1] == '\n')
            --textLength;

        Xertz::SystemInfo::GetProcessInfo(pid).WriteMemoryFast(text, reinterpret_cast<void*>(address), textLength * 2);
        return true;
    }

    static bool WriteTextEx(const uint32_t pid, char32_t* text, const uint64_t address)
    {
        uint32_t textLength = std::char_traits<char32_t>::length(text);

        if (text[textLength - 1] == '\n')
            --textLength;

        Xertz::SystemInfo::GetProcessInfo(pid).WriteMemoryFast(text, reinterpret_cast<void*>(address), textLength * 4);
        return true;
    }

    static ImVec4 PackedColorToImVec4(const uint8_t* packedColor)
    {
        const float r = static_cast<float>(*(packedColor + 3)) / 255.0f;
        const float g = static_cast<float>(*(packedColor + 2)) / 255.0f;
        const float b = static_cast<float>(*(packedColor + 1)) / 255.0f;
        const float a = static_cast<float>(*packedColor) / 255.0f;

        return ImVec4(r, g, b, a);
    }

    static ImU32 ImVec4ToPackedColor(const ImVec4& colorVec)
    {
        return IM_COL32(int(colorVec.x * 255), int(colorVec.y * 255), int(colorVec.z * 255), int(colorVec.w * 255));
    }

    static ImVec4 PickColorFromScreen()
    {
        POINT point;
        ImVec4 colorVec;
        std::atomic_bool buttonPressed(false);

        std::thread mouseThread([&buttonPressed]()
            {
                while (!buttonPressed)
                {
                    if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
                    {
                        buttonPressed = true;
                        break;
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            });

        mouseThread.join();

        HDC hdc = GetDC(NULL);
        if (hdc == NULL)
            return colorVec;

        if (!GetCursorPos(&point))
            return colorVec;

        COLORREF color = GetPixel(hdc, point.x, point.y);
        if (color == CLR_INVALID)
            return colorVec;

        ReleaseDC(GetDesktopWindow(), hdc);

        colorVec.x = static_cast<float>(GetRValue(color)) / 255.0f;
        colorVec.y = static_cast<float>(GetGValue(color)) / 255.0f;
        colorVec.z = static_cast<float>(GetBValue(color)) / 255.0f;
        colorVec.w = 1.0f;
        return colorVec;
    }

    static void ColorValuesToCString(const ImVec4& rgba, const int type, char* destination, const bool forceAlpha = false)
    {
        std::stringstream cstream;

        switch (type)
        {
        case LitColor::RGBA8888:
            cstream << "#" << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(rgba.x * 255.0f) << std::setw(2) << static_cast<int>(rgba.y * 255.0f) << std::setw(2) << static_cast<int>(rgba.z * 255.0f) << std::setw(2) << static_cast<int>(rgba.w * 255.0f);
            break;
        case LitColor::RGBF:
            cstream << rgba.x << ", " << rgba.y << ", " << rgba.z;
            break;
        case LitColor::RGBAF:
            cstream << rgba.x << ", " << rgba.y << ", " << rgba.z << ", " << rgba.w;
            break;
        case LitColor::RGB565: {
            LitColor color((float*)&rgba);
            cstream << "#" << std::hex << std::setfill('0') << std::setw(4) << color.GetRGB565();
        }break;
        case LitColor::RGB5A3: {
            LitColor color((float*)&rgba, forceAlpha);
            cstream << "@" << std::hex << std::setfill('0') << std::setw(4) << color.GetRGB5A3();
        }break;
        default: //RGB888
            cstream << "#" << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(rgba.x * 255.0f) << std::setw(2) << static_cast<int>(rgba.y * 255.0f) << std::setw(2) << static_cast<int>(rgba.z * 255.0f);
        }

        strcpy_s(destination, 48, cstream.str().c_str());
    }

    template<typename uType> static void SwapBytesArray(OperativeArray<uType>& arr)
    {
        for (int i = 0; i < arr.ItemCount(); ++i)
        {
            arr[i] = Xertz::SwapBytes<uType>(arr[i]);
        }
    }

    static void RemoveChars(std::string& stringToEdit, const char* charsToRemove)
    {
        for (int i = 0; i < strlen(charsToRemove); ++i)
        {
            stringToEdit.erase(remove(stringToEdit.begin(), stringToEdit.end(), charsToRemove[i]), stringToEdit.end());
        }
    }

    static const std::string GetStringFormatting(const int valueType, const bool isSigned, const bool hex)
    {
        if (valueType == FLOAT || valueType == DOUBLE)
            return "%.4f";

        if (hex)
        {
            switch (valueType)
            {
            case INT8:
                return "%02X";
            case INT16:
                return "%04X";
            case INT64:
                return "%016llX";
            default:
                return "%08X";
            }
        }

        switch (valueType)
        {
        case INT8:
            return isSigned ? "%hhi" : "%hhu";
        case INT16:
            return isSigned ? "%hi" : "%hu";
        case INT64:
            return isSigned ? "%lli" : "%llu";
        default:
            return isSigned ? "%li" : "%lu";
        }
    }

    static void CenteredText(std::string text)
    {
        float windowWidth = ImGui::GetContentRegionAvail().x;
        float textWidth = ImGui::CalcTextSize(text.c_str()).x;

        ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
        ImGui::Text(text.c_str());
    }

    static void HelpMarker(const char* desc) //© ImGui devs
    {
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(desc);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }

    static int s_globalWindowFlag = 0;

    static void SetWindowToForeground(HWND hWnd)// todo: make this work ):
    {
        SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        SetForegroundWindow(hWnd);

        if (IsIconic(hWnd))
            ShowWindow(hWnd, SW_RESTORE);

        LONG_PTR style = GetWindowLongPtr(hWnd, GWL_STYLE);
        style &= ~WS_EX_TOPMOST;
        SetWindowLongPtr(hWnd, GWL_STYLE, style);
        SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_FRAMECHANGED);
        SetForegroundWindow(hWnd);
        ImGui::SetWindowFocus();
    }

    static std::string ToHexString(const uint64_t val, const int leftJustify, const bool prepend_0x = false)
    {
        std::stringstream stream;
        stream << std::hex << val;

        std::string result;
        if (leftJustify > static_cast<int>(stream.str().size()))
        {
            result = std::string(leftJustify - stream.str().size(), '0');
        }

        result += stream.str();
        result = "0x" + result;

        if (prepend_0x)
            return result;

        return result.substr(2);
    }

    static bool IsValidHexString(std::string input)
    {
        input = input.c_str();
        return std::all_of(input.begin(), input.end(), ::isxdigit);
    }

    static std::string RemoveSpacePadding(const std::string& input, const bool removeAll = false)
    {
        std::string output;

        if (removeAll)
        {
            for (int i = 0; i < input.size(); ++i)
            {
                if (input[i] != 0x20)
                    output += input[i];
            }
        }
        else
        {
            for (int i = 0; i < input.size() - 1; ++i)
            {
                if (input[i] != 0x20 || (input[i] == 0x20 && input[i + 1] != 0x20))
                {
                    output += input[i];
                    continue;
                }
            }

            if (output.back() == 0x20)
                output.resize(output.size() - 1);
        }


        return output;
    }

    static void PrepareWidgetLabel(const std::string& name, const float paneWidth, const float labelPortion, bool printLabel, const char* helpText = nullptr)
    {
        const float absoluteWidth = ImGui::GetContentRegionAvail().x * paneWidth;
        const float curserPos = ImGui::GetCursorPos().x;

        if (printLabel)
        {
            ImGui::Text(name.c_str());
            ImGui::SameLine();
            if (helpText != nullptr)
            {
                HelpMarker(helpText);
                ImGui::SameLine();
            }

            ImGui::SetCursorPosX(curserPos + absoluteWidth * labelPortion);
            ImGui::PushItemWidth(absoluteWidth * (1.0f - labelPortion));
        }
        else
            ImGui::PushItemWidth(absoluteWidth);
    }

    static void PrepareWidgetLabel(const char* name, const float paneWidth, const float labelPortion, bool printLabel, const char* helpText = nullptr)
    {
        const float absoluteWidth = ImGui::GetContentRegionAvail().x * paneWidth;
        const float curserPos = ImGui::GetCursorPos().x;

        if (printLabel)
        {
            ImGui::Text(name);
            ImGui::SameLine();
            if (helpText != nullptr)
            {
                HelpMarker(helpText);
                ImGui::SameLine();
            }

            ImGui::SetCursorPosX(curserPos + absoluteWidth * labelPortion);
            ImGui::PushItemWidth(absoluteWidth * (1.0f - labelPortion));
        }
        else
            ImGui::PushItemWidth(absoluteWidth);
    }

    static bool SetUpPairCombo(const IPairs& pairs, int* currentSelect, const float paneWidth = 0.25f, const float labelPortion = 0.4f, bool printLabel = true, const char* helpText = nullptr)
    {
        PrepareWidgetLabel(pairs.GetLabelCString(), paneWidth, labelPortion, printLabel, helpText);
        return ImGui::Combo(pairs.GetComboLabelCString(), currentSelect, pairs.GetData(), pairs.GetCount());
    }

    static void SetUpSliderFloat(const std::string& name, float* val, const float min, const float max, const char* format = "%3f", const float paneWidth = 0.25f, const float labelPortion = 0.4f, bool printLabel = true, const char* helpText = nullptr)
    {
        PrepareWidgetLabel(name, paneWidth, labelPortion, printLabel, helpText);
        ImGui::SliderFloat(("##" + name).c_str(), val, min, max, format, NULL);
        ImGui::PopItemWidth();
    }

    static bool SetUpSliderInt(const std::string& name, int* val, const int min, const int max, const char* format = "%d", const float paneWidth = 0.25f, const float labelPortion = 0.4f, bool printLabel = true, const char* helpText = nullptr)
    {
        PrepareWidgetLabel(name, paneWidth, labelPortion, printLabel, helpText);
        bool ret = ImGui::SliderInt(("##" + name).c_str(), val, min, max, format, NULL);
        ImGui::PopItemWidth();
        return ret;
    }

    static bool SetUpInputFloat(const std::string& name, float* val, const float step = 1.0f, const float stepFast = 5.0f, const char* format = "%3f", const float paneWidth = 0.25f, const float labelPortion = 0.4f, const ImGuiInputTextFlags flags = 0, bool printLabel = true, const char* helpText = nullptr)
    {
        PrepareWidgetLabel(name, paneWidth, labelPortion, printLabel, helpText);
        const bool edited = ImGui::InputFloat(("##" + name).c_str(), val, step, stepFast, format, flags);
        ImGui::PopItemWidth();
        return edited;
    }

    static bool SetUpInputDouble(const std::string& name, double* val, const double step = 1.0, const double stepFast = 5.0, const char* format = "%3f", const float paneWidth = 0.25f, const float labelPortion = 0.4f, const ImGuiInputTextFlags flags = 0, bool printLabel = true, const char* helpText = nullptr)
    {
        PrepareWidgetLabel(name, paneWidth, labelPortion, printLabel, helpText);
        const bool edited = ImGui::InputDouble(("##" + name).c_str(), val, step, stepFast, format, flags);
        ImGui::PopItemWidth();
        return edited;
    }

    static void SetUpLableText(const std::string& name, const char* text, const size_t bufSize, const float paneWidth = 0.25f, const float labelPortion = 0.4f)
    {
        PrepareWidgetLabel(name, paneWidth, labelPortion, true);
        ImGui::Text(text, bufSize);
        ImGui::PopItemWidth();
    }

    class FloorString
    {
    private:
        std::string _str;
        int _floor;

    public:
        FloorString(const std::string& str, const int lowerLimit)
        {
            _str = str;
            _floor = lowerLimit;

            if (_str.size() < lowerLimit)
                _str.resize(lowerLimit);
        }

        /*FloorString(const char* str, const int lowerLimit)
        {
            FloorString(str, lowerLimit);
        }*/

        std::string StdStr()
        {
            return _str;
        }

        std::string StdStrNoLeadinZeros()
        {
            return _str.c_str();
        }

        const char* CStr()
        {
            return _str.c_str();
        }

        char* Data()
        {
            return _str.data();
        }

        int Size()
        {
            return _str.size();
        }

        void operator = (const std::string& other)
        {
            _str = other;

            if (_str.size() < _floor)
                _str.resize(_floor);
        }

        void operator = (const char* other)
        {
            _str = other;

            if (_str.size() < _floor)
                _str.resize(_floor);
        }
    };

    class SignalCombo //yes I know this is against the purpose of ImGui. But it makes my life easier here. Please don't call the code cops
    {
    public:
        typedef std::function<void()> Slot;

    private:
        int _index = 0;
        int _itemCount = 0;
        std::string _text = "";
        std::vector<Slot> _slotsOnIndexChanged{};
        std::vector<Slot> _slotsOnItemCountChanged{};
        std::vector<Slot> _slotsOnTextChanged{};

    public:
        SignalCombo() {}
        void Draw(const IPairs& pair, int& select, const float paneWidth = 0.25f, const float labelPortion = 0.4f)
        {
            if (pair.GetCount() > 0)
            {
                if (_slotsOnIndexChanged.size() > 0 && _index != select)
                {
                    for (const auto& slot : _slotsOnIndexChanged)
                        slot();

                    _index = select;
                }

                if (_slotsOnItemCountChanged.size() > 0 && _itemCount != pair.GetCount())
                {
                    for (const auto& slot : _slotsOnItemCountChanged)
                        slot();

                    _itemCount = pair.GetCount();
                }

                if (_slotsOnTextChanged.size() > 0 && _text.compare(pair.GetData()[_index]) != 0)//todo: make this using a boolean flag instead
                {
                    for (const auto& slot : _slotsOnTextChanged)
                        slot();

                    _text = pair.GetData()[_index];
                }

            }
            SetUpPairCombo(pair, &select, paneWidth, labelPortion);
        }

        void ConnectOnIndexChanged(const Slot slot)
        {
            _slotsOnIndexChanged.emplace_back(slot);
        }

        void ConnectOnItemCountChanged(const Slot slot)
        {
            _slotsOnItemCountChanged.emplace_back(slot);
        }

        void ConnectOnTextChanged(const Slot slot)
        {
            _slotsOnTextChanged.emplace_back(slot);
        }
    };

    inline std::filesystem::path GetResourcesFilePath(const std::filesystem::path& resourceFileName)
    {
        const auto resourcesDirectory = "resources" / resourceFileName;

#ifdef _DEBUG
        const auto currentPath = std::filesystem::current_path();
        const auto projectPath = currentPath / ".." / ".." / ".." / "..";
        auto fontPath = projectPath / "MungPlex" / resourcesDirectory;
        fontPath = absolute(fontPath);

        return fontPath;
#endif

        return resourcesDirectory;
    }

    static bool LoadFonts(ImGuiIO& io, ImFontConfig& cfg, const float scale)
    {
        static const ImWchar ranges[] =
        {
            0x0020, 0xFFFF, //Just get all of it. IDK if this is a good idea, but it works
            0,
        };

        return io.Fonts->AddFontFromFileTTF(GetResourcesFilePath("NotoSansJP-KR-SC-TH_black.ttf").string().data(), scale, nullptr, ranges);
    }

    static void PopBackTrailingChars(std::string& str, const char ch)
    {
        while (str.back() == ch)
            str.pop_back();
    }

    static void SetQuotationmarks(std::string& str)
    {
        if (str.front() != '"')
            str = '"' + str;

        if (str.back() != '"')
            str.append("\"");
    }

    enum TimeFlags
    {
        YEAR = 1,
        MONTH = 1 << 1,
        DAY = 1 << 2,
        HOUR = 1 << 3,
        MINUTE = 1 << 4,
        SECOND = 1 << 5
    };

    static std::string GetCurrentTimeString(const int flags = HOUR | MINUTE | SECOND)
    {
        auto now = std::chrono::system_clock::now();
        std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);

        std::tm* nowTm = std::localtime(&now_time_t);

        if (nowTm == nullptr)
            throw "Error: Could not get local time.";

        std::string timeArgs;

        if (flags & YEAR)
            timeArgs.append("%Y-");

        if (flags & MONTH)
            timeArgs.append("%m-");

        if (flags & DAY)
            timeArgs.append("%d-");

        if (flags & HOUR)
            timeArgs.append("%H-");

        if (flags & MINUTE)
            timeArgs.append("%M-");

        if (flags & SECOND)
            timeArgs.append("%S");

        PopBackTrailingChars(timeArgs, '-');
        std::ostringstream stream;
        stream << std::put_time(nowTm, timeArgs.c_str());
        return stream.str();
    }

    static std::string GetWindowTitleBase(const std::string& appendage = "")
    {
        std::string windowTitle("MungPlex ");
        windowTitle.append(std::to_string(MungPlex_VERSION_MAJOR) + "." + std::to_string(MungPlex_VERSION_MINOR) + "." + std::to_string(MungPlex_VERSION_PATCH));
        windowTitle.append(appendage);
        return windowTitle;
    }

    static void ParsePointerPath(std::vector<int64_t>& pointerPath, const std::string& pointerPathStr)
    {
        pointerPath.clear();
        std::string line;

        if (pointerPathStr.find(',') == std::string::npos)
        {
            line = RemoveSpacePadding(pointerPathStr, true);

            if (!line.empty())
                if (line.front() != '\0')
                    if (IsValidHexString(line))
                        pointerPath.push_back(stoll(line, 0, 16));
        }
        else
        {
            std::stringstream stream;
            stream << pointerPathStr;

            while (std::getline(stream, line, ','))
            {
                line = RemoveSpacePadding(line, true);

                if (line.empty())
                    break;

                if (line.front() == '\0')
                    break;

                if (!IsValidHexString(line))
                    break;

                pointerPath.push_back(stoll(line, 0, 16));
            }
        }
    }

    static void DrawBottomLine()
    {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 childMin = ImGui::GetWindowPos();
        ImVec2 childMax = ImVec2(childMin.x + ImGui::GetWindowWidth(), childMin.y + ImGui::GetWindowHeight());
        ImU32 borderColor = ImGui::ColorConvertFloat4ToU32(ImGui::GetStyle().Colors[ImGuiCol_Border]);

        draw_list->AddLine(
            ImVec2(childMin.x, childMax.y - 1.0f),
            ImVec2(childMax.x, childMax.y - 1.0f),
            borderColor,
            1.0f
        );
    }

    static bool DrawColorPicker(const uint32_t colorTypeSelect, const bool forceAlpha, ImVec4* colorVec, const bool useColorWheel, const float width = 1.0f)
    {
        int colorPickerFlags = ImGuiColorEditFlags_NoOptions;
        colorPickerFlags |= useColorWheel ? ImGuiColorEditFlags_PickerHueWheel : ImGuiColorEditFlags_PickerHueBar;

        switch (colorTypeSelect)
        {
        case LitColor::RGBA8888:
            colorPickerFlags |= ImGuiColorEditFlags_Uint8 | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf;
            break;
        case LitColor::RGBF:
            colorPickerFlags |= ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoAlpha;
            break;
        case LitColor::RGBAF:
            colorPickerFlags |= ImGuiColorEditFlags_Float | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf;
            break;
        default: //RGB888, RGB565, RGB5A3
            if (forceAlpha)
                colorPickerFlags |= ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf;
            else
                colorPickerFlags |= ImGuiColorEditFlags_Uint8 | ImGuiColorEditFlags_NoAlpha;
        }

        bool changed = false;
        const ImVec2 childXY = ImGui::GetContentRegionAvail();
        ImGui::PushItemWidth(childXY.x * width);
        changed = ImGui::ColorPicker4("##SearchColorPicker", (float*)colorVec, colorPickerFlags);
        ImGui::PopItemWidth();
        return changed;
    }

    static void DrawExtraColorPickerOptions(bool* useColorWheel, ImVec4* colorVec)
    {
        ImGui::Checkbox("Color Wheel", useColorWheel);

        if (colorVec == nullptr)
            return;

        ImGui::SameLine();

        if (ImGui::Button("Pick screen color"))
        {
            //HWND windowHandle = GetForegroundWindow(); todo: make this work ): 
            *colorVec = PickColorFromScreen();
            //MungPlex::SetWindowToForeground(windowHandle);
        }
    }

    static void LitColorExpressionToImVec4(const std::string str, ImVec4* colorVec)
    {
        if (colorVec == nullptr)
            return;

        LitColor specialized(str);
        colorVec->x = specialized.GetColorValue<float>(0);
        colorVec->y = specialized.GetColorValue<float>(1);
        colorVec->z = specialized.GetColorValue<float>(2);
        colorVec->w = specialized.GetColorValue<float>(3);
    }
}