#pragma once
#include<iostream>
#include<string>
#include<stdio.h>
#include<sstream>
#include <Windows.h>
#pragma once
#include <vector>
#include <functional>
#include "GLFW/glfw3.h"
#include "imgui.h"
#include"Connection.h"
#include<any>
#include<cstdint>
#include <filesystem>
#include"Settings.h"
#include"LitColor.h"
#include"MorphText.h"
#include <stdlib.h>
#include"OperativeArray.h"

namespace MungPlex
{
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
            process.ReadExRAM(dest + i, reorderedAddress, 1);
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
            process.WriteExRAM(dest + i, reorderedAddress, 1);
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

    static std::wstring GetStringFromID(const std::vector<EMUPAIR>& pairs, const int ID)
    {
        auto tmpPair = std::ranges::find_if(pairs.begin(), pairs.end(),
            [&](const auto& pair) { return pair.second == ID; }
        );

        return (tmpPair != pairs.end()) ? tmpPair->first : L"";
    }

    static bool WriteTextEx(const uint32_t pid, const char* text, const uint64_t address)
    {
        uint32_t textLength = strlen(text);

        if (text[textLength - 1] == '\n')
            --textLength;

        Xertz::SystemInfo::GetProcessInfo(pid).WriteExRAM(text, reinterpret_cast<void*>(address), textLength);
        return true;
    }

    static bool WriteTextEx(const uint32_t pid, const wchar_t* text, const uint64_t address)
    {
        uint32_t textLength = wcslen(text);

        if (text[textLength - 1] == '\n')
            --textLength;

        Xertz::SystemInfo::GetProcessInfo(pid).WriteExRAM(text, reinterpret_cast<void*>(address), textLength*2);
        return true;
    }

    static bool WriteTextEx(const uint32_t pid, const char32_t* text, const uint64_t address)
    {
        uint32_t textLength = std::char_traits<char32_t>::length(text);

        if (text[textLength - 1] == '\n')
            --textLength;

        Xertz::SystemInfo::GetProcessInfo(pid).WriteExRAM(text, reinterpret_cast<void*>(address), textLength * 4);
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
        case LitColor::RGB565:{
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
        
        strcpy_s(destination, 48,cstream.str().c_str());
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

    static const char* GetStringLiteral(const int valueType, const bool isSigned, const bool hex)
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
    struct SystemRegion; //forward declaration for compiler
    struct ColorScheme; //forward declaration for compiler
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

    static void PrepareWidgetLabel(const std::string& name, const float paneWidth, const float labelPortion, bool printLabel, const char* helpText = nullptr)
    {
    	const float absoluteWidth = ImGui::GetContentRegionAvail().x * paneWidth;
        const float curserPos = ImGui::GetCursorPos().x;

        if(printLabel)
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

    template<typename T> static bool SetUpCombo(const std::string& name, const std::vector<T>& items, int& select, const float paneWidth = 0.25f, const float labelPortion = 0.4f, bool printLabel = true, const char* helpText = nullptr)
    {
        std::vector<std::string> items_str;
        items_str.reserve(items.size());
        for (const auto& item : items)
        {
            if constexpr (std::is_same_v<T, std::string>)
                items_str.emplace_back(item.c_str());
            else if constexpr (std::is_same_v<T, const char*>)
                items_str.emplace_back(item);
            else if constexpr (std::is_same_v<T, std::pair<std::string, int>>)
                items_str.emplace_back(item.first.c_str());
            else if constexpr (std::is_same_v<T, std::pair<std::string, ColorScheme>>)
                items_str.emplace_back(item.first.c_str());
            else if constexpr (std::is_same_v<T, std::wstring>)
                items_str.emplace_back(std::string(item.begin(), item.end()).c_str());
            else if constexpr (std::is_same_v<T, std::pair<std::wstring, int>> || std::is_same_v<T, EMUPAIR>)
                items_str.emplace_back(std::string(item.first.begin(), item.first.end()).c_str());
            else if constexpr (std::is_same_v<T, std::tuple<std::string, int, bool>>)
                items_str.emplace_back(std::get<std::string>(item).c_str());
            else if constexpr (std::is_same_v<T, Xertz::ProcessInfo>)
            {
                Xertz::ProcessInfo x = item;
                items_str.emplace_back(x.GetProcessName());
            }
            else if constexpr (std::is_same_v<T, SystemRegion>)
            {
                items_str.emplace_back(std::string(item.Label).append(": ").append(ToHexString(item.Base, 0)).c_str());
            }
        }

		PrepareWidgetLabel(name, paneWidth, labelPortion, printLabel, helpText);
        const bool indexChanged = ImGui::Combo(("##" + name).c_str(), &select, [](void* data, int idx, const char** out_text)
                                            {
                                                auto& items = *static_cast<std::vector<std::string>*>(data);
                                                *out_text = items[idx].c_str();
                                                return true;
                                            }, static_cast<void*>(&items_str), items.size());
        ImGui::PopItemWidth();
        return indexChanged;
    }

    static void SetUpSliderFloat(const std::string& name, float* val, const float min, const float max, const char* format = "%3f", const float paneWidth = 0.25f, const float labelPortion = 0.4f, bool printLabel = true, const char* helpText = nullptr)
    {
        PrepareWidgetLabel(name, paneWidth, labelPortion, printLabel, helpText);
        ImGui::SliderFloat(("##" + name).c_str(), val, min, max, format, NULL);
        ImGui::PopItemWidth();
    }

    static void SetUpSliderInt(const std::string& name, int* val, const int min, const int max, const char* format = "%d", const float paneWidth = 0.25f, const float labelPortion = 0.4f, bool printLabel = true, const char* helpText = nullptr)
    {
        PrepareWidgetLabel(name, paneWidth, labelPortion, printLabel, helpText);
        ImGui::SliderInt(("##" + name).c_str(), val, min, max, format, NULL);
        ImGui::PopItemWidth();
    }

    static bool SetUpInputText(const std::string& name, char* text, const size_t bufSize, const float paneWidth = 0.25f, const float labelPortion = 0.4f, bool printLabel = true, const char* helpText = nullptr)
    {
        PrepareWidgetLabel(name, paneWidth, labelPortion, printLabel, helpText);
        const bool edited = ImGui::InputText(("##" + name).c_str(), text, bufSize);
        ImGui::PopItemWidth();
        return edited;
    }

    static bool SetUpInputInt(const std::string& name, int* val, const int step = 1, const int stepFast = 100, const float paneWidth = 0.25f, const float labelPortion = 0.4f, const ImGuiInputTextFlags flags = 0, bool printLabel = true, const char* helpText = nullptr)
    {
        PrepareWidgetLabel(name, paneWidth, labelPortion, printLabel, helpText);
        const bool edited = ImGui::InputInt(("##" + name).c_str(), val, step, stepFast, flags);
        ImGui::PopItemWidth();
        return edited;
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

    static bool SetUpInputTextMultiline(const std::string& name, char* text, const size_t bufSize, const float paneWidth = 0.25f, const float paneHeight = 0.25f, const ImGuiInputTextFlags flags = 0, bool printLabel = true, const char* helpText = nullptr)
    {
        const float absoluteWidth = ImGui::GetContentRegionAvail().x * paneWidth;
        const float absoluteHeight = ImGui::GetContentRegionAvail().y * paneHeight;
        ImGui::Text(name.c_str());
        const bool edited = ImGui::InputTextMultiline(("##" + name).c_str(), text, bufSize, ImVec2(absoluteWidth, absoluteHeight), flags);
        return edited;
    }

    static void SetUpLableText(const std::string& name, const char* text, const size_t bufSize, const float paneWidth = 0.25f, const float labelPortion = 0.4f)
    {
        PrepareWidgetLabel(name, paneWidth, labelPortion, true);
        ImGui::Text(text, bufSize);
        ImGui::PopItemWidth();
    }

    template<typename T> class SignalCombo //yes I know this is against the purpose of ImGui. But it makes my life easier here. Please don't call the code cops
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
        SignalCombo<T>(){}
        void Draw(const std::string& name, const std::vector<T>& items, int& select, const float paneWidth = 0.25f, const float labelPortion = 0.4f)
        {
            if (items.size() > 0)
            {
                if (_slotsOnIndexChanged.size() > 0 && _index != select)
                {
                    for (const auto& slot : _slotsOnIndexChanged)
                        slot();

                    _index = select;
                }
 
                if (_slotsOnItemCountChanged.size() > 0 && _itemCount != items.size())
                {
                    for (const auto& slot : _slotsOnItemCountChanged)
                        slot();

                    _itemCount = items.size();
                }

                if (_slotsOnTextChanged.size() > 0 && _text.compare(items[_index].Label) != 0)//todo: make this using a boolean flag instead
                {
                    for (const auto& slot : _slotsOnTextChanged)
                        slot();

                    _text = items[select].Label;
                }

            }
            SetUpCombo(name, items, select, paneWidth, labelPortion);
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

    class SignalInputText //ImGui's InputTexts don't return true if the text was changed due to accessing the variable instead of keyboard input
    {
    public:
        typedef std::function<void()> Slot;

    private:
        char _text[1024] = "";
        std::vector<Slot> _slotsOnTextChanged{};

    public:
        SignalInputText() {}
        bool Draw(const char* name, char* buf, const size_t bufSize, const float paneWidth = 0.25f, const float labelPortion = 0.4f)
        {
            bool changedByFlow = false;
            if (std::strcmp(_text, buf) != 0)
            {
                for (const auto& slot : _slotsOnTextChanged)
                    slot();

                strcpy_s(_text, buf);
                changedByFlow = true;
            }

            return SetUpInputText(name, buf, bufSize, paneWidth, labelPortion) || changedByFlow;
        }

        void ConnectOnTextChanged(const Slot slot)
        {
            _slotsOnTextChanged.emplace_back(slot);
        }
    };

    inline std::filesystem::path GetResourcesFilePath(const std::filesystem::path &resourceFileName)
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
}