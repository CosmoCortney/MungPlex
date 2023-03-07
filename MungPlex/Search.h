#pragma once
#include <iostream>
#include <stdio.h>
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "examples/libs/emscripten/emscripten_mainloop_stub.h"
#include<string>
#include "Xertz.h"
#include <Windows.h>
#include "MungPlexConfig.h"
#include "Connection.h"
#include"HelperFunctions.h"
#include <chrono>
#include <thread>
#include<functional>
#include<any>

namespace MungPlex
{
    template<typename T> struct SearchResult
    {
        uint64_t _address;
        T _currentValue;
        T _previousValue;
        T _difference;
    };

    class Search
    {
    private:
        Search()
        {
            _searchValueTypes.push_back(std::pair<std::string, int>("Int 8 (Byte)", INT8));
            _searchValueTypes.push_back(std::pair<std::string, int>("Int 16 (Byte)", INT16));
            _searchValueTypes.push_back(std::pair<std::string, int>("Int 32 (Byte)", INT32));
            _searchValueTypes.push_back(std::pair<std::string, int>("Int 64 (Byte)", INT64));
            _searchValueTypes.push_back(std::pair<std::string, int>("Float Single", FLOAT));
            _searchValueTypes.push_back(std::pair<std::string, int>("Float Double", DOUBLE));

            _searchArrayTypes = _searchValueTypes;
            _searchValueTypes.push_back(std::pair<std::string, int>("Array", ARRAY));

            _searchValueTypes.push_back(std::pair<std::string, int>("Text", TEXT));
            _searchValueTypes.push_back(std::pair<std::string, int>("Color RGB (3 Bytes)", RGB_BYTE));
            _searchValueTypes.push_back(std::pair<std::string, int>("Color RGBA (4 Bytes)", RGBA_BYTE));
            _searchValueTypes.push_back(std::pair<std::string, int>("Color RGB (3 Floats)", RGB_FLOAT));
            _searchValueTypes.push_back(std::pair<std::string, int>("Color RGBA (4 Floats)", RGBA_FLOAT));

            _searchTextTypes.push_back(std::pair<std::string, int>("ASCII", ASCII));
            _searchTextTypes.push_back(std::pair<std::string, int>("UTF-8", UTF8));
            _searchTextTypes.push_back(std::pair<std::string, int>("UTF-16", UTF16));
            _searchTextTypes.push_back(std::pair<std::string, int>("UTF-32", UTF32));
            _searchTextTypes.push_back(std::pair<std::string, int>("Shift JIS", SHIFT_JIS));

            _searchConditionTypes.push_back(std::pair<std::string, int>("Equal (==)", EQUAL));
            _searchConditionTypes.push_back(std::pair<std::string, int>("Unequal (!=)", UNEQUAL));
            _searchConditionTypes.push_back(std::pair<std::string, int>("Greater (>)", GREATER));
            _searchConditionTypes.push_back(std::pair<std::string, int>("Greater or Equal (>=)", GREATER_EQUAL));
            _searchConditionTypes.push_back(std::pair<std::string, int>("Lower (<)", LOWER));
            _searchConditionTypes.push_back(std::pair<std::string, int>("Lower or Equal (<=)", LOWER_EQUAL));
            _searchConditionTypes.push_back(std::pair<std::string, int>("AND (has all true bits)", AND));
            _searchConditionTypes.push_back(std::pair<std::string, int>("OR (has at least 1 true bit)", OR));

            _searchComparasionType.push_back(std::pair<std::string, int>("Unknown/Initial", UNKNOWN));
            _searchComparasionType.push_back(std::pair<std::string, int>("Known Value", KNOWN));

            _RegionSelectSignalCombo.ConnectOnIndexChanged(Slot_IndexChanged);
            _RegionSelectSignalCombo.ConnectOnItemCountChanged(Slot_ItemCountChanged);
            _RegionSelectSignalCombo.ConnectOnTextChanged(Slot_TextChanged);
        }

        ~Search() {};
        Search(const Search&) = delete;
        Search(Search&&) = delete;
        void operator=(const Search&) = delete;
        void operator=(Search&&) = delete;
        static Search& GetInstance()
        {
            static Search Instance;
            return Instance;
        }

        void DrawValueTypeOptions();
        void DrawRangeOptions();
        void DrawSearchOptions();
        void DrawResultsArea();
        void PickColorFromScreen();

        std::vector<std::pair<std::string, int>> _searchValueTypes{};
        std::vector<std::pair<std::string, int>> _searchTextTypes{};
        std::vector<std::pair<std::string, int>> _searchArrayTypes{};
        std::vector<std::pair<std::string, int>> _searchConditionTypes{};
        std::vector<std::pair<std::string, int>> _searchComparasionType{};
        std::vector<MungPlex::SystemRegion> _regions{};
        std::vector<SearchResult<std::any>> _searchResults{};
        int _currentRegionSelect = 0;
        int _currentValueTypeSelect = 0;
        int _currentArrayTypeSelect = 0;
        int _currentTextTypeSelect = 0;
        int _currentComparisionTypeSelect = 0;
        int _currentConditionTypeSelect = 0;
        bool _signed = false;
        bool _hex = false;
        bool _cached = false;
        int _precision = 100;
        bool _disableBecauseNoInt = false;
        bool _disableBecauseNoText = false;
        bool _disableBecauseNoColor = false;
        bool _caseSensitive = false;
        bool _disableBecauseNoArray = false;
        int _lastRangeSelect = -1;
        ImVec4 _colorVec;

        uint64_t _resultCount = 0;
        uint16_t _resultsPerPage = 1000;
        uint32_t _pages = 0;
        uint32_t _currentPage = 0;
        char _rangeStartText[256] = { "" };
        uint64_t _rangeStartValue = 0;
        char _rangeEndText[256] = { "" };
        uint64_t _rangeEndValue = 0;
        char _knownValueText[2048] = { "" };
        std::any _knownValueValue; //the name is intended
        char _alignmentText[8] = { "4" };
        int _alignmentValue = 4;

    public:
        static void DrawWindow();
        static void Refresh();
        MungPlex::SignalCombo<MungPlex::SystemRegion> _RegionSelectSignalCombo;

        std::function<void()> Slot_IndexChanged = []()
        {
            
        };

        std::function<void()> Slot_ItemCountChanged = []()
        {
            
        };

        std::function<void()> Slot_TextChanged = []()
        {
            std::stringstream stream;
            stream << std::hex << GetInstance()._regions[GetInstance()._currentRegionSelect].Base;
            std::string hexBegStr = stream.str();
            std::strcpy(GetInstance()._rangeStartText, hexBegStr.c_str());

            std::string hexEndStr = MungPlex::ToHexString(GetInstance()._regions[GetInstance()._currentRegionSelect].Base + GetInstance()._regions[GetInstance()._currentRegionSelect].Size -1, 0).c_str();
            std::strcpy(GetInstance()._rangeEndText, hexEndStr.c_str());
        };
    };
}