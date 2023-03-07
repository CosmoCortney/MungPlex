#pragma once
#include<iostream>
#include<string>
#include<stdio.h>
#include<sstream>
#include <Windows.h>
#include <vector>
#include <functional>
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "examples/libs/emscripten/emscripten_mainloop_stub.h"
#include"Connection.h"

namespace MungPlex
{
    struct SystemRegion; //forward declaration for the compiler
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

    template<typename T> static void SetUpCombo(const std::string& name, std::vector<T>& items, int& select)
    {
        std::vector<std::string> items_str;
        items_str.reserve(items.size());
        for (const auto& item : items)
        {
            if constexpr (std::is_same_v<T, std::string>)                           //std::string>
                items_str.push_back(item.c_str());
            else if constexpr (std::is_same_v<T, std::pair<std::string, int>>)      //std::pair<std::string, int>>
                items_str.push_back(item.first.c_str());
            else if constexpr (std::is_same_v<T, std::wstring>)                     //std::wstring>
                items_str.push_back(std::string(item.begin(), item.end()).c_str());
            else if constexpr (std::is_same_v<T, std::pair<std::wstring, int>>)     //std::pair<std::wstring, int>>
                items_str.push_back(std::string(item.first.begin(), item.first.end()).c_str());
            else if constexpr (std::is_same_v < T, MungPlex::SystemRegion>)         //MungPlex::SystemRegion>
            {
                items_str.push_back(std::string(item.Label).append(": ").append(ToHexString(item.Base, 0)).c_str());
            }
        }
        ImGui::Combo(name.c_str(), &select, [](void* data, int idx, const char** out_text)
                                            {
                                                auto& items = *static_cast<std::vector<std::string>*>(data);
                                                *out_text = items[idx].c_str();
                                                return true;
                                            }, static_cast<void*>(&items_str), items.size());
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
        void Draw(const std::string& name, std::vector<T>& items, int& select)
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

            if (_slotsOnTextChanged.size() > 0 && _text.compare(items[_index].Label) != 0)
            {
                for (const auto& slot : _slotsOnTextChanged)
                    slot();

                _text = items[select].Label;
            }

            }
            SetUpCombo(name, items, select);
        }

        void ConnectOnIndexChanged(Slot slot)
        {
            _slotsOnIndexChanged.push_back(slot);
        }
            

        void ConnectOnItemCountChanged(Slot slot)
        {
            _slotsOnItemCountChanged.push_back(slot);
        }
            

        void ConnectOnTextChanged(Slot slot)
        {
            _slotsOnTextChanged.push_back(slot);
        }
    };
}