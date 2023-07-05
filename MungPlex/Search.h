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
#include<algorithm>
#include"MemCompareResult.h"
#include <cstdio>
#include<string>
#include<tuple>
#include"Settings.h"

namespace MungPlex
{
    class Search
    {
    private:
        Search()
        {
            _searchValueTypes.emplace_back("Primitive", PRIMITIVE);
            _searchValueTypes.emplace_back("Array", ARRAY);
            _searchValueTypes.emplace_back("Text", TEXT);
            _searchValueTypes.emplace_back("Color", COLOR);

            _searchPrimitiveTypes.emplace_back("Int 8 (1 Byte)", INT8);
            _searchPrimitiveTypes.emplace_back("Int 16 (2 Bytes)", INT16);
            _searchPrimitiveTypes.emplace_back("Int 32 (4 Bytes)", INT32);
            _searchPrimitiveTypes.emplace_back("Int 64 (8 Bytes)", INT64);
            _searchArrayTypes = _searchPrimitiveTypes;
            _searchPrimitiveTypes.emplace_back("Float Single", FLOAT);
            _searchPrimitiveTypes.emplace_back("Float Double", DOUBLE);

            _searchTextTypes.emplace_back("UTF-8", MorphText::UTF8);
            _searchTextTypes.emplace_back("UTF-16 Little Endian", MorphText::UTF16LE);
            _searchTextTypes.emplace_back("UTF-16 Big Endian", MorphText::UTF16BE);
            _searchTextTypes.emplace_back("UTF-32 Little Endian", MorphText::UTF32LE);
            _searchTextTypes.emplace_back("UTF-32 Big Endian", MorphText::UTF32BE);
            _searchTextTypes.emplace_back("ASCII", MorphText::ASCII);
            _searchTextTypes.emplace_back("ISO-8859-1 (Latin-1)", MorphText::ISO_8859_1);
            _searchTextTypes.emplace_back("ISO-8859-2 (Latin-2)", MorphText::ISO_8859_2);
            _searchTextTypes.emplace_back("ISO-8859-3 (Latin-3)", MorphText::ISO_8859_3);
            _searchTextTypes.emplace_back("ISO-8859-4 (Latin-4)", MorphText::ISO_8859_4);
            _searchTextTypes.emplace_back("ISO-8859-5 (Cyrillic)", MorphText::ISO_8859_5);
            _searchTextTypes.emplace_back("ISO-8859-6 (Arabic)", MorphText::ISO_8859_6);
            _searchTextTypes.emplace_back("ISO-8859-7 (Greek)", MorphText::ISO_8859_7);
            _searchTextTypes.emplace_back("ISO-8859-8 (Hebrew)", MorphText::ISO_8859_8);
            _searchTextTypes.emplace_back("ISO-8859-9 (Turkish, Latin-5)", MorphText::ISO_8859_9);
            _searchTextTypes.emplace_back("ISO-8859-10 (Nordic, Latin-6)", MorphText::ISO_8859_10);
            _searchTextTypes.emplace_back("ISO-8859-11 (Thai)", MorphText::ISO_8859_11);
            _searchTextTypes.emplace_back("ISO-8859-13 (Baltic, Latin-7)", MorphText::ISO_8859_13);
            _searchTextTypes.emplace_back("ISO-8859-14 (Celtic, Latin-8)", MorphText::ISO_8859_14);
            _searchTextTypes.emplace_back("ISO-8859-15 (West European, Latin-9)", MorphText::ISO_8859_15);
            _searchTextTypes.emplace_back("ISO-8859-16 (South-East European, Latin-10)", MorphText::ISO_8859_16);
            _searchTextTypes.emplace_back("Shift-Jis", MorphText::SHIFTJIS);

            _searchColorTypes.emplace_back("RGB 888 (3 Bytes)", LitColor::RGB888);
            _searchColorTypes.emplace_back("RGBA 888 (4 Bytes)", LitColor::RGBA8888);
            _searchColorTypes.emplace_back("RGBF (3 Floats)", LitColor::RGBF);
            _searchColorTypes.emplace_back("RGBAF (4 Floats)", LitColor::RGBAF);
            _searchColorTypes.emplace_back("RGB 565 (2 Bytes)", LitColor::RGB565);

            _searchConditionTypes.emplace_back("Equal (==)", Xertz::EQUAL);
            _searchConditionTypesText = _searchConditionTypes;
            _searchConditionTypes.emplace_back("Unequal (!=)", Xertz::UNEQUAL);
            _searchConditionTypesArray = _searchConditionTypes;
            _searchConditionTypes.emplace_back("Greater (>)", Xertz::GREATER);
            _searchConditionTypes.emplace_back("Greater or Equal (>=)", Xertz::GREATER_EQUAL);
            _searchConditionTypes.emplace_back("Lower (<)", Xertz::LOWER);
            _searchConditionTypes.emplace_back("Lower or Equal (<=)", Xertz::LOWER_EQUAL);
            _searchConditionTypesColor = _searchConditionTypes;
            _searchConditionTypes.emplace_back("Increased by", Xertz::INCREASED_BY);
            _searchConditionTypes.emplace_back("Decreased by", Xertz::DECREASED_BY);
            _searchConditionTypes.emplace_back("Value Between", Xertz::BETWEEN);
            _searchConditionTypes.emplace_back("Value Not Between", Xertz::NOT_BETWEEN);
            _searchConditionTypesFloat = _searchConditionTypes;
            _searchConditionTypes.emplace_back("AND (has all true bits)", Xertz::AND);
            _searchConditionTypes.emplace_back("OR (has at least 1 true bit)", Xertz::OR);

            _searchComparasionType.emplace_back("Unknown/Initial", Xertz::UNKNOWN);
            _searchComparasionType.emplace_back("Known Value", Xertz::KNOWN);

            _RegionSelectSignalCombo.ConnectOnIndexChanged(Slot_IndexChanged);
            _RegionSelectSignalCombo.ConnectOnItemCountChanged(Slot_ItemCountChanged);
            _RegionSelectSignalCombo.ConnectOnTextChanged(Slot_TextChanged);
            _SignalInputTextRangeStart.ConnectOnTextChanged(std::bind(Slot_RangeTextChanged, _rangeStartText, std::ref(_rangeStartValue)));
            _SignalInputTextRangeEnd.ConnectOnTextChanged(std::bind(Slot_RangeTextChanged, _rangeEndText, std::ref(_rangeEndValue)));

            _selectedIndices.resize(_maxResultsPerPage);
            _alignmentValue = Settings::GetSearchSettings().DefaultAlignment;
            _cached = Settings::GetSearchSettings().DefaultCached;
            _caseSensitive = Settings::GetSearchSettings().DefaultCaseSensitive;
            _useColorWheel = Settings::GetSearchSettings().DefaultColorWheel;
            _hex = Settings::GetSearchSettings().DefaultValuesHex;
            _dumpPath = MorphText::Utf8_To_Utf16LE(Settings::GetGeneralSettings().DocumentsPath) + L"\\MungPlex\\Dump";
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
        void PerformSearch();
        void PrimitiveTypeSearch();
        void ArrayTypeSearch();
        void TextTypeSearch();
        void ColorTypeSearch();

        std::vector<std::pair<std::string, int>> _searchValueTypes{};
        std::vector<std::pair<std::string, int>> _searchPrimitiveTypes{};
        std::vector<std::pair<std::string, int>> _searchTextTypes{};
        std::vector<std::pair<std::string, int>> _searchArrayTypes{}; //remove once Arrays support floats
        std::vector<std::pair<std::string, int>> _searchColorTypes{};
        std::vector<std::pair<std::string, int>> _searchConditionTypes{};
        std::vector<std::pair<std::string, int>> _searchConditionTypesArray{};
        std::vector<std::pair<std::string, int>> _searchConditionTypesFloat{};
        std::vector<std::pair<std::string, int>> _searchConditionTypesColor{};
        std::vector<std::pair<std::string, int>> _searchConditionTypesText{};
        std::vector<std::pair<std::string, int>> _searchComparasionType{};
        std::vector<SystemRegion> _regions{};
        int _currentComparisionTypeSelect = 0;
        int _currentConditionTypeSelect = 0;
        std::vector<Xertz::MemDump> _memDumps{};
        ImVec4 _colorVec;

        //search settings
        char _alignmentText[8] = { "4" };
        int _alignmentValue = 4;
        int _lastRangeSelect = -1;
        bool _signed = false;
        bool _hex = false;
        bool _cached = false;
        float _precision = 100.0f;
        bool _disableBecauseNoPrimitive = false;
        bool _disableBecauseNoInt = false;
        bool _disableBecauseNoText = false;
        bool _disableBecauseNoColor = false;
        bool _caseSensitive = true;
        bool _disableBecauseNoArray = false;
        int _currentValueTypeSelect = 0;
        int _currentPrimitiveTypeSelect = 0;
        int _currentArrayTypeSelect = 0;
        int _currentTextTypeSelect = 0;
        int _currentColorTypeSelect = 0;
        bool _useColorWheel = false;
        std::wstring _dumpPath = {};

        //value options
        char _knownValueText[2048] = { "" };
        char _secondaryKnownValueText[2048] = { "" };
        std::vector<const char*> _iterations;
        int _iterationIndex = 0;

        //range options
        char _rangeStartText[256] = { "" };
        uint64_t _rangeStartValue = 0;
        char _rangeEndText[256] = { "" };
        uint64_t _rangeEndValue = 0;

        //results table
        int _currentRegionSelect = 0;
        uint32_t _pagesAmountValue = 0;
        char _pagesAmountText[17] = {"0"};
        uint32_t _currentPageValue = 1;
        char _currentPageText[8] = { "1" };
        std::vector<bool> _selectedIndices{};
        int _maxResultsPerPage = 256;
        bool _multiPoke = false;
        bool _pokePrevious = false;
        char _pokeValue[1024] = { };
        char _pokeValueText[1024] = { "0" };
        uint64_t _pokeAddress = 0;
        char _pokeAddressText[17] = { "0" };
        std::tuple<uint64_t, int> _searchStats;
        bool _underlyingBigEndian = false;

        void ResetCurrentPage()
        {
            _currentPageValue = 1;
            strncpy_s(_currentPageText, "1", 1);
        }

        template <typename dataType> std::tuple<uint64_t, int> SetUpAndIterate(const dataType valKnown = 0, const dataType valKnownSecondary = 0)
        {
            uint64_t offset = _rangeStartValue - ProcessInformation::GetRegions()[_currentRegionSelect].Base;
            void* baseAddressEx = ProcessInformation::GetRegions()[_currentRegionSelect].BaseLocationProcess;
            uint64_t size = _rangeEndValue - _rangeStartValue + 1;
            const int addressWidth = ProcessInformation::GetAddressWidth();
            const bool isWideAddress = addressWidth > 4;
            bool isKnown = _currentComparisionTypeSelect == 1;

            if (isWideAddress)
            {
                Xertz::MemCompare<dataType, uint64_t>::SetUp(ProcessInformation::GetPID(), _dumpPath, _cached, _underlyingBigEndian, _alignmentValue);
                return Xertz::MemCompare<dataType, uint64_t>::Iterate(baseAddressEx, size, _currentConditionTypeSelect, isKnown, _precision/100.0f, valKnown, valKnownSecondary, _iterationIndex+1);
            }
            else
            {
                Xertz::MemCompare<dataType, uint32_t>::SetUp(ProcessInformation::GetPID(), _dumpPath, _cached, _underlyingBigEndian, _alignmentValue);
                return Xertz::MemCompare<dataType, uint32_t>::Iterate(baseAddressEx, size, _currentConditionTypeSelect, isKnown, _precision/100.0f, valKnown, valKnownSecondary, _iterationIndex +1);
            }
        }
        
        template<typename addressType> bool PokeText()
        {
            const int pid = ProcessInformation::GetPID();
            const std::string pokeTextp(_pokeValueText);
            MorphText pokeValue(pokeTextp);

            if (_multiPoke)
            {
                auto results = Xertz::MemCompare<MorphText, addressType>::GetResults();
                uint64_t resultIndex = (_currentPageValue - 1) * _maxResultsPerPage;

                for (int index = 0; index < _selectedIndices.size(); ++index)
                {
                    if (_selectedIndices[index] == false)
                        continue;

                    uint64_t address = *(results->at(std::get<1>(_searchStats) - 1)->GetResultOffsets() + resultIndex + index) + _regions[_currentRegionSelect].Base;
                    int regionIndex = -1;

                    for (int i = 0; i < _regions.size(); ++i)
                    {
                        if (address >= _regions[i].Base && address <= _regions[i].Base + _regions[i].Size)
                        {
                            regionIndex = i;
                            break;
                        }
                    }

                    if (regionIndex == -1)
                        return false;

                    address -= _regions[regionIndex].Base;
                    address += reinterpret_cast<uint64_t>(_regions[regionIndex].BaseLocationProcess);

                    switch (_currentTextTypeSelect)
                    {
                    case MorphText::ASCII:
                        WriteTextEx(pid, pokeValue.GetASCII(), address);
                    break;
                    case MorphText::SHIFTJIS:
                        WriteTextEx(pid, pokeValue.GetShiftJis(), address);
                    break;
                    case MorphText::UTF8:
                        WriteTextEx(pid, pokeValue.GetUTF8().c_str(), address);
                    break;
                    case MorphText::UTF16LE: case MorphText::UTF16BE:
                        WriteTextEx(pid, pokeValue.GetUTF16(_currentTextTypeSelect == MorphText::UTF16BE ? true : false).c_str(), address);
                        break;
                    case MorphText::UTF32LE: case MorphText::UTF32BE:
                        WriteTextEx(pid, pokeValue.GetUTF32(_currentTextTypeSelect == MorphText::UTF32BE ? true : false).c_str(), address);
                        break;
                    default: //ISO-8859-X
                        WriteTextEx(pid, pokeValue.GetISO8859X(_currentTextTypeSelect), address);
                    }
                }
                return true;
            }


            uint64_t address = _pokeAddress;

            for (int i = 0; i < _regions.size(); ++i)
            {
                if (_pokeAddress >= _regions[i].Base && _pokeAddress <= _regions[i].Base + _regions[i].Size)
                {
                    address -= _regions[i].Base;
                    address += reinterpret_cast<uint64_t>(_regions[i].BaseLocationProcess);

                    switch (_currentTextTypeSelect)
                    {
                    case MorphText::ASCII:
                        return WriteTextEx(pid, pokeValue.GetASCII(), address);
                    case MorphText::SHIFTJIS:
                        return WriteTextEx(pid, pokeValue.GetShiftJis(), address);
                    case MorphText::UTF8:
                        return WriteTextEx(pid, pokeValue.GetUTF8().c_str(), address);
                    case MorphText::UTF16LE: case MorphText::UTF16BE:
                        return WriteTextEx(pid, pokeValue.GetUTF16(_currentTextTypeSelect == MorphText::UTF16BE ? true : false).c_str(), address);
                    case MorphText::UTF32LE: case MorphText::UTF32BE:
                        return WriteTextEx(pid, pokeValue.GetUTF32(_currentTextTypeSelect == MorphText::UTF32BE ? true : false).c_str(), address);
                    default: //ISO-8859-X
                        return WriteTextEx(pid, pokeValue.GetISO8859X(_currentTextTypeSelect), address);
                    }
                }
            }
            
            return false;
        }

        template<typename addressType> bool PokeColor()
        {
            const int pid = ProcessInformation::GetPID();
            const std::string colorString(_pokeValueText);
            LitColor pokeValue(colorString);
            const int pokeValueWidth = pokeValue.GetSelectedType() == LitColor::RGB888 ? 3 : 4;

            if (_multiPoke)
            {
                auto results = Xertz::MemCompare<LitColor, addressType>::GetResults();
                uint64_t resultIndex = (_currentPageValue - 1) * _maxResultsPerPage;

                for (int index = 0; index < _selectedIndices.size(); ++index)
                {
                    if (_selectedIndices[index] == false)
                        continue;

                    uint64_t address = *(results->at(std::get<1>(_searchStats) - 1)->GetResultOffsets() + resultIndex + index) + _regions[_currentRegionSelect].Base;
                    int regionIndex = -1;

                    for (int i = 0; i < _regions.size(); ++i)
                    {
                        if (address >= _regions[i].Base && address <= _regions[i].Base + _regions[i].Size)
                        {
                            regionIndex = i;
                            break;
                        }
                    }

                    if (regionIndex == -1)
                        return false;

                    address -= _regions[regionIndex].Base;
                    address += reinterpret_cast<uint64_t>(_regions[regionIndex].BaseLocationProcess);

                        if (pokeValue.GetSelectedType() < LitColor::RGBF) //RGB888, RGBA8888
                        {
                            uint32_t val = _pokePrevious ? *reinterpret_cast<uint32_t*>(results->at(std::get<1>(_searchStats) - 1)->GetResultPreviousValues() + resultIndex + index) : pokeValue.GetRGBA();

                            if (_underlyingBigEndian)
                                val = Xertz::SwapBytes<uint32_t>(val);

                            Xertz::SystemInfo::GetProcessInfo(pid).WriteExRAM(&val, reinterpret_cast<void*>(address), pokeValueWidth);//todo check if LE pokes work too!
                        }
                        else if (pokeValue.GetSelectedType() == LitColor::RGB565)
                        {
                            uint16_t val = _pokePrevious ? *reinterpret_cast<uint16_t*>(results->at(std::get<1>(_searchStats) - 1)->GetResultPreviousValues() + resultIndex + index) : pokeValue.GetRGB565();

                            if (_underlyingBigEndian)
                                val = Xertz::SwapBytes<uint16_t>(val);

                            Xertz::SystemInfo::GetProcessInfo(pid).WriteExRAM(&val, reinterpret_cast<void*>(address), sizeof(uint16_t));
                        }
                        else //RGBF, RGBAF
                        {
                            for (int item = 0; item < (pokeValue.GetSelectedType() == LitColor::RGBF ? 3 : 4); ++item)
                            {
                                float val = _pokePrevious ? *reinterpret_cast<float*>(results->at(std::get<1>(_searchStats) - 1)->GetResultPreviousValues() + resultIndex + index + item * sizeof(float)) : pokeValue.GetColorValue<float>(item);

                                if (_underlyingBigEndian)
                                    val = Xertz::SwapBytes<float>(val);

                                Xertz::SystemInfo::GetProcessInfo(pid).WriteExRAM(&val, reinterpret_cast<void*>(address + item * sizeof(float)), sizeof(float));
                            }
                        }
                }
                return true;
            }

            uint64_t address = _pokeAddress;

            for (int i = 0; i < _regions.size(); ++i)
            {
                if (_pokeAddress >= _regions[i].Base && _pokeAddress <= _regions[i].Base + _regions[i].Size)
                {
                    address -= _regions[i].Base;
                    address += reinterpret_cast<uint64_t>(_regions[i].BaseLocationProcess);

                    if (pokeValue.GetSelectedType() < LitColor::RGBF) //RGB888, RGBA8888
                    {
                        uint32_t val = pokeValue.GetRGBA();

                        if (_underlyingBigEndian)
                            val = Xertz::SwapBytes<uint32_t>(val);

                        Xertz::SystemInfo::GetProcessInfo(pid).WriteExRAM(&val, reinterpret_cast<void*>(address), pokeValueWidth);//todo check if LE pokes work too!
                    }
                    else if (pokeValue.GetSelectedType() == LitColor::RGB565)
                    {
                        uint16_t val = pokeValue.GetRGB565();

                        if (_underlyingBigEndian)
                            val = Xertz::SwapBytes<uint16_t>(val);

                        Xertz::SystemInfo::GetProcessInfo(pid).WriteExRAM(&val, reinterpret_cast<void*>(address), 2);
                    }
                    else //RGBF, RGBAF
                    {
                        for (int item = 0; item < (pokeValue.GetSelectedType() == LitColor::RGBF ? 3 : 4); ++item)
                        {
                            float val = pokeValue.GetColorValue<float>(item);

                            if (_underlyingBigEndian)
                                val = Xertz::SwapBytes<float>(val);

                            Xertz::SystemInfo::GetProcessInfo(pid).WriteExRAM(&val, reinterpret_cast<void*>(address + item * sizeof(float)), sizeof(float));
                        }
                    }
                    return true;
                }
            }
            
            return false;
        }

        template<typename uType, typename addressType> bool PokeArray()
        {
            uint64_t itemCount = OperativeArray<uType>(std::string(_knownValueText)).ItemCount();
            const int pid = ProcessInformation::GetPID();
            std::string arrayString(_pokeValueText);
            OperativeArray<uType> pokeArray(arrayString);
               
            if (_multiPoke)
            {
                int regionIndex = -1;
                auto results = Xertz::MemCompare<OperativeArray<uType>, addressType>::GetResults();
                uint64_t resultIndex = (_currentPageValue - 1) * _maxResultsPerPage;

                for (int index = 0; index < _selectedIndices.size(); ++index)
                {
                    if (_selectedIndices[index] == false)
                        continue;

                    uint64_t address = *(results->at(std::get<1>(_searchStats) - 1)->GetResultOffsets() + resultIndex + index) + _regions[_currentRegionSelect].Base;

                    for (int i = 0; i < _regions.size(); ++i)
                    {
                        if (address >= _regions[i].Base && address <= _regions[i].Base + _regions[i].Size)
                        {
                            regionIndex = i;
                            break;
                        }
                    }

                    if(regionIndex == -1)
                        return false;

                    if (_pokePrevious)
                    {
                        uType* arr = reinterpret_cast<uType*>(results->at(std::get<1>(_searchStats) - 1)->GetResultPreviousValues() + resultIndex + index);
                        pokeArray = OperativeArray<uType>(arr, itemCount);
                    }
                    
                    if (_underlyingBigEndian && (index == 0 || _pokePrevious))
                        MungPlex::SwapBytesArray<uType>(pokeArray);
                    
                    address -= _regions[regionIndex].Base;
                    address += reinterpret_cast<uint64_t>(_regions[regionIndex].BaseLocationProcess);

                    for (int i = 0; i < itemCount; ++i)
                    {
                        if(!pokeArray.IsIgnoredIndex(i))
                            Xertz::SystemInfo::GetProcessInfo(pid).WriteExRAM(&pokeArray[i], reinterpret_cast<void*>(address + sizeof(uType) * i), sizeof(uType));
                    }
                }
                return true;
            }

            if (_underlyingBigEndian)
                MungPlex::SwapBytesArray<uType>(pokeArray);

            uint64_t address = _pokeAddress;
            for (int i = 0; i < _regions.size(); ++i)
            {
                if (_pokeAddress >= _regions[i].Base && _pokeAddress <= _regions[i].Base + _regions[i].Size)
                {
                    address -= _regions[i].Base;
                    address += reinterpret_cast<uint64_t>(_regions[i].BaseLocationProcess);
                    
                    for (int i = 0; i < itemCount; ++i)
                    {
                        if (!pokeArray.IsIgnoredIndex(i))
                            Xertz::SystemInfo::GetProcessInfo(pid).WriteExRAM(&pokeArray[i], reinterpret_cast<void*>(address + sizeof(uType) * i), sizeof(uType));
                    }
                    return true;
                }
            }
            
            return false;
        }

        template<typename dataType, typename addressType> bool PokeValue()
        {
            const int pid = ProcessInformation::GetPID();

            if (_multiPoke)
            {
                int regionIndex = -1;
                auto results = Xertz::MemCompare<dataType, addressType>::GetResults();
                uint64_t resultIndex = (_currentPageValue - 1) * _maxResultsPerPage;

                for (int index = 0; index < _selectedIndices.size(); ++index)
                {
                    if (_selectedIndices[index] == false)
                        continue;

                    uint64_t address = *(results->at(std::get<1>(_searchStats) - 1)->GetResultOffsets() + resultIndex + index) + _regions[_currentRegionSelect].Base;

                    for (int i = 0; i < _regions.size(); ++i)
                    {
                        if (address >= _regions[i].Base && address <= _regions[i].Base + _regions[i].Size)
                        {
                            regionIndex = i;
                            break;
                        }
                    }

                    if(regionIndex == -1)
                        return false;

                    if (_pokePrevious)
                        *reinterpret_cast<dataType*>(_pokeValue) = *(results->at(std::get<1>(_searchStats) - 1)->GetResultPreviousValues() + resultIndex + index);

                    if (_underlyingBigEndian && (index == 0 || _pokePrevious))
                        *reinterpret_cast<dataType*>(_pokeValue) = Xertz::SwapBytes<dataType>(*reinterpret_cast<dataType*>(_pokeValue));

                    address -= _regions[regionIndex].Base;
                    address += reinterpret_cast<uint64_t>(_regions[regionIndex].BaseLocationProcess);
                    Xertz::SystemInfo::GetProcessInfo(pid).WriteExRAM(_pokeValue, reinterpret_cast<void*>(address), sizeof(dataType));
                }
                return true;
            }

            if (_underlyingBigEndian)
                *reinterpret_cast<dataType*>(_pokeValue) = Xertz::SwapBytes<dataType>(*reinterpret_cast<dataType*>(_pokeValue));

            uint64_t address = _pokeAddress;

            for (int i = 0; i < _regions.size(); ++i)
            {
                if (_pokeAddress >= _regions[i].Base && _pokeAddress <= _regions[i].Base + _regions[i].Size)
                {
                    address -= _regions[i].Base;
                    address += reinterpret_cast<uint64_t>(_regions[i].BaseLocationProcess);
                    Xertz::SystemInfo::GetProcessInfo(pid).WriteExRAM(_pokeValue, reinterpret_cast<void*>(address), sizeof(dataType));
                    return true;
                }
            }
            
            return false;
        }

        template<typename dataType, typename addressType> void DrawResultsTable()
        {
            static ImGuiSelectableFlags selectableFlags = ImGuiSelectableFlags_SpanAllColumns;
            static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

            if (!ImGui::BeginTable("Results", 4, flags, ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 16.0f)))
                return;
                    
            auto results = Xertz::MemCompare<dataType, addressType>::GetResults();
            const int iterationCount = std::get<1>(_searchStats);
            if (!_cached && iterationCount > 0)
            {
                if (iterationCount > 1 && results->at(iterationCount - 2)->HasResults())
                    results->at(iterationCount - 2)->FreeData(false);

                if (!results->at(iterationCount - 1)->HasResults())
                    if (!results->at(iterationCount - 1)->LoadResults(false))
                        return;
            }
                    
            const uint64_t resultCount = std::get<0>(_searchStats);
            ImGui::TableSetupColumn("Address");
            ImGui::TableSetupColumn("Value");
            ImGui::TableSetupColumn("Previous");
            ImGui::TableSetupColumn("Difference (a - b)");
            ImGui::TableHeadersRow();
            const char* literal;
            if (_currentValueTypeSelect == PRIMITIVE)
                literal = GetStringLiteral(_currentPrimitiveTypeSelect, _signed, _hex);
            else
                literal = GetStringLiteral(_currentArrayTypeSelect, _signed, _hex);

            for (int row = 0; row < resultCount; ++row)
            {
                if (row >= _maxResultsPerPage || row >= resultCount || (_currentPageValue >= _pagesAmountValue && row >= resultCount % _maxResultsPerPage))
                    break;

                static char tempAddress[1024];
                static char tempValue[1024];
                static char buf[1024];
                const int addressTextWidth = ProcessInformation::GetAddressWidth() > 4 ? 16 : 8;
                uint64_t pageIndex = (_currentPageValue-1) * _maxResultsPerPage;
                const uint64_t address = *(results->at(iterationCount - 1)->GetResultOffsets() + pageIndex + row) + _regions[_currentRegionSelect].Base;
                bool rowClicked = false;
                uint64_t resultsIndex = (pageIndex + row);
                ImGui::TableNextRow(selectableFlags);

                ImGui::TableNextRow();
                for (int col = 0; col < 4; ++col)
                {
                    ImGui::TableSetColumnIndex(col);

                    if (col > 0)
                    {
                        if constexpr (std::is_integral_v<dataType> || std::is_floating_point_v<dataType>)
                        {
                            auto currentValue = *(results->at(iterationCount - 1)->GetResultValues() + resultsIndex);
                            auto previousValue = *(results->at(iterationCount - 1)->GetResultPreviousValues() + resultsIndex);
                            auto difference = currentValue - previousValue;

                            switch (col)
                            {
                            case 1:
                                sprintf(buf, literal, currentValue);
                                if (!_pokePrevious)
                                    std::memcpy(tempValue, buf, 1024);
                                break;
                            case 2:
                                sprintf(buf, literal, previousValue);
                                if (_pokePrevious)
                                    std::memcpy(tempValue, buf, 1024);
                                break;
                            case 3:
                                sprintf(buf, literal, difference);
                                break;
                            }
                        }
                        else if constexpr(Xertz::is_instantiation_of<dataType, OperativeArray>::value)
                        {
                            uint64_t itemCount = Xertz::MemCompare<dataType, addressType>::GetValueItemCount();
                            uint64_t resultIndexWithItemCount = resultsIndex * itemCount;

                            switch (_currentArrayTypeSelect)
                            {
                            case INT8:
                                DrawArrayValues<uint8_t>(col, results, itemCount, resultIndexWithItemCount, buf, tempValue, literal);
                            break;
                            case INT16: 
                                DrawArrayValues<uint16_t>(col, results, itemCount, resultIndexWithItemCount, buf, tempValue, literal);
                            break;
                            case INT32:
                                DrawArrayValues<uint32_t>(col, results, itemCount, resultIndexWithItemCount, buf, tempValue, literal);
                            break;
                            case INT64:
                                DrawArrayValues<uint64_t>(col, results, itemCount, resultIndexWithItemCount, buf, tempValue, literal);
                            break;
                            /*case FLOAT:
                                DrawArrayValues<float>(col, results, itemCount, resultIndexWithItemCount, buf, tempValue, literal);
                            break;
                            case DOUBLE: 
                                DrawArrayValues<double>(col, results, itemCount, resultIndexWithItemCount, buf, tempValue, literal);
                            break;*/
                            }
                        }
                        else if constexpr (std::is_same_v<dataType, LitColor>)
                        {
                            static ImU32 rectColor;
                            static ImVec4 vecCol;

                            if (_currentColorTypeSelect != LitColor::RGBF && _currentColorTypeSelect != LitColor::RGBAF)//RGB888, RGBA8888, RGB565
                            {
                                if (col == 1)
                                {
                                    rectColor = *(reinterpret_cast<uint32_t*>(results->at(iterationCount - 1)->GetResultValues()) + resultsIndex);

                                    if (!_pokePrevious)
                                        vecCol = PackedColorToImVec4(reinterpret_cast<uint8_t*>(&rectColor));
                                }
                                else if (col == 2)
                                {
                                    rectColor = *(reinterpret_cast<uint32_t*>(results->at(iterationCount - 1)->GetResultPreviousValues()) + resultsIndex);

                                    if (_pokePrevious)
                                        vecCol = PackedColorToImVec4(reinterpret_cast<uint8_t*>(&rectColor));
                                }
                                else
                                    break;
                            }
                            else //RGBF, RGBAF
                            {
                                static bool usesAlpha = _currentColorTypeSelect == LitColor::RGBAF;
                                static int colorValueCount = _currentColorTypeSelect == LitColor::RGBAF ? 4 : 3;

                                if (col == 1)
                                {
                                    rectColor = LitColor(reinterpret_cast<float*>(results->at(iterationCount - 1)->GetResultValues()) + resultsIndex * colorValueCount, usesAlpha).GetRGBA();

                                    if (!_pokePrevious)
                                        vecCol = PackedColorToImVec4(reinterpret_cast<uint8_t*>(&rectColor));
                                }
                                else if (col == 2)
                                {
                                    rectColor = LitColor(reinterpret_cast<float*>(results->at(iterationCount - 1)->GetResultPreviousValues()) + resultsIndex * colorValueCount, usesAlpha).GetRGBA();

                                    if (_pokePrevious)
                                        vecCol = PackedColorToImVec4(reinterpret_cast<uint8_t*>(&rectColor));
                                }
                                else
                                    break;
                            }

                            ColorValuesToCString(vecCol, _currentColorTypeSelect, buf);
                            std::memcpy(tempValue, buf, 1024);
                            strcpy_s(buf, "");
                            ImDrawList* drawList = ImGui::GetWindowDrawList();
                            ImVec2 rectMin = ImGui::GetCursorScreenPos();
                            ImVec2 rectMax = ImVec2(rectMin.x + 124, rectMin.y + 30);
                            if (_currentColorTypeSelect == LitColor::RGB888 || _currentColorTypeSelect == LitColor::RGB565)
                                rectColor |= 0x000000FF;
                            drawList->AddRectFilled(rectMin, rectMax, Xertz::SwapBytes<uint32_t>(rectColor));
                        }
                        else if constexpr (std::is_same_v<dataType, MorphText>)
                        {
                            static int strLength = 0;

                            if (col == 1)
                            {
                                switch (_currentTextTypeSelect)
                                {
                                case MorphText::ASCII:
                                    if (!strLength)
                                        strLength = strlen(Xertz::MemCompare<dataType, addressType>::GetPrimaryKnownValue().GetASCII()) + 1;
                                    sprintf_s(buf, "%s", reinterpret_cast<char*>(results->at(iterationCount - 1)->GetResultValues()) + resultsIndex * strLength);
                                    std::memcpy(tempValue, buf, 1024);
                                    break;
                                case MorphText::SHIFTJIS: {
                                    static std::string temputf8 = MorphText::ShiftJis_To_Utf8(reinterpret_cast<char*>(results->at(iterationCount - 1)->GetResultValues()) + resultsIndex * strLength);
                                    if (!strLength)
                                        strLength = strlen(temputf8.c_str());
                                    sprintf_s(buf, "%s", temputf8.c_str());
                                    std::memcpy(tempValue, buf, 1024);
                                } break;
                                case MorphText::UTF8:
                                    if (!strLength)
                                        strLength = strlen(Xertz::MemCompare<dataType, addressType>::GetPrimaryKnownValue().GetUTF8().c_str())+1;
                                    sprintf_s(buf, "%s", reinterpret_cast<char*>(results->at(iterationCount - 1)->GetResultValues()) + resultsIndex * strLength);
                                    std::memcpy(tempValue, buf, 1024);
                                    break;
                                case MorphText::UTF16LE: case MorphText::UTF16BE: {//todo: fix this - strings won`t be rendered properly
                                    if (!strLength)
                                        strLength = strlen((char*)Xertz::MemCompare<dataType, addressType>::GetPrimaryKnownValue().GetUTF16(_currentTextTypeSelect == MorphText::UTF16BE ? true : false).c_str()) + 1;

                                    static std::string temp = _currentTextTypeSelect == MorphText::UTF16BE
                                        ? MorphText::Utf16BE_To_Utf8(reinterpret_cast<wchar_t*>(reinterpret_cast<char*>(results->at(iterationCount - 1)->GetResultValues()) + resultsIndex * strLength))
                                        : MorphText::Utf16LE_To_Utf8(reinterpret_cast<wchar_t*>(reinterpret_cast<char*>(results->at(iterationCount - 1)->GetResultValues()) + resultsIndex * strLength));

                                    sprintf_s(buf, "%s", temp.c_str());
                                    std::memcpy(tempValue, buf, 1024);
                                } break;
                                case MorphText::UTF32LE: case MorphText::UTF32BE: {//todo: fix this - strings won`t be rendered properly
                                    if (!strLength)
                                        strLength = strlen((char*)Xertz::MemCompare<dataType, addressType>::GetPrimaryKnownValue().GetUTF32(_currentTextTypeSelect == MorphText::UTF32BE ? true : false).c_str()) + 1;

                                    static std::string temp = _currentTextTypeSelect == MorphText::UTF32BE
                                        ? MorphText::Utf32BE_To_Utf8(reinterpret_cast<char32_t*>(reinterpret_cast<char*>(results->at(iterationCount - 1)->GetResultValues()) + resultsIndex * strLength))
                                        : MorphText::Utf32LE_To_Utf8(reinterpret_cast<char32_t*>(reinterpret_cast<char*>(results->at(iterationCount - 1)->GetResultValues()) + resultsIndex * strLength));

                                    sprintf_s(buf, "%s", temp.c_str());
                                    std::memcpy(tempValue, buf, 1024);
                                } break;
                                default: { //ISO-8859-X
                                    static std::string temputf8 = MorphText::ISO8859X_To_Utf8(reinterpret_cast<char*>(results->at(iterationCount - 1)->GetResultValues()) + resultsIndex * strLength, _currentTextTypeSelect);
                                    if (!strLength)
                                        strLength = strlen(temputf8.c_str())+1;
                                    sprintf_s(buf, "%s", temputf8.c_str(), strLength);
                                    std::memcpy(tempValue, buf, 1024);
                                } break;
                                }
                            }
                            else
                                break;
                        }
                    }
                    else
                    {
                        sprintf_s(buf, addressTextWidth == 16 ? "%016X" : "%08X", address);
                        std::memcpy(tempAddress, buf, 17);
                    }

                    if (_selectedIndices[row])
                    {
                        ImGui::Selectable(buf, true, selectableFlags);
                        for (int i = 0; i < 4; ++i)
                        {
                            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImU32(0xFFFF0080));
                            ImGui::TableSetColumnIndex(i);
                        }
                    }
                    else
                    {
                        ImGui::Selectable(buf, false, selectableFlags);
                    }

                    if (_multiPoke //multi-select
                        && ImGui::IsItemClicked()
                        && (GetAsyncKeyState(VK_LCONTROL) || GetAsyncKeyState(VK_RCONTROL)))
                    {
                        strncpy(tempAddress, "Multi-Poke", 17);

                        if (_selectedIndices[row] == true)
                            _selectedIndices[row] = false;
                        else
                            _selectedIndices[row] = true;

                        rowClicked = true;
                    }
                    else if (_multiPoke
                        && ImGui::IsItemClicked() //range-select
                        && (GetAsyncKeyState(VK_LSHIFT) || GetAsyncKeyState(VK_RSHIFT)))
                    {
                        strncpy(tempAddress, "Multi-Poke", 17);
                        int rangeBegin = -1;
                        int rangeEnd;

                        for (int selIndex = 0; selIndex < _selectedIndices.size(); ++selIndex)
                        {
                            if (_selectedIndices[selIndex] == true)
                            {
                                if (selIndex == row)
                                    break;

                                rangeBegin = selIndex;
                                break;
                            }
                        }

                        if (rangeBegin == -1)
                            break;

                        if (rangeBegin > row)
                        {
                            rangeEnd = rangeBegin;
                            rangeBegin = row;
                        }
                        else
                        {
                            rangeEnd = row;
                        }

                        for (int markupIndex = rangeBegin; markupIndex <= rangeEnd; ++markupIndex)
                        {
                            _selectedIndices[markupIndex] = true;
                        }

                        rowClicked = true;
                    }
                    else if (ImGui::IsItemClicked() //single selection and multi-select reset
                        && !(GetAsyncKeyState(VK_LCONTROL) || GetAsyncKeyState(VK_RCONTROL))
                        && !(GetAsyncKeyState(VK_LSHIFT) || GetAsyncKeyState(VK_RSHIFT)))
                    {
                        for (int selIndex = 0; selIndex < _selectedIndices.size(); ++selIndex)
                        {
                            _selectedIndices[selIndex] = row == selIndex ? true : false;
                        }

                        rowClicked = true;
                    }
                }

                if (rowClicked)
                {
                    std::memcpy(_pokeAddressText, tempAddress, 17);
                    std::memcpy(_pokeValueText, tempValue, 1024);
                }
            }

            ImGui::EndTable();
        }

        template<typename T> void DrawArrayValues(const int col, const auto results, const uint64_t itemCount, const uint64_t resultIndexWithItemCount, char* buf, char* tempValue, const char* literal)
        {
            const int iterationCount = std::get<1>(_searchStats);
            T* value;
            const bool copyTempValue = (col == 1 && !_pokePrevious) || (col == 2 && _pokePrevious);

            if(col == 1)
                value = (T*)results->at(iterationCount - 1)->GetResultValues() + resultIndexWithItemCount;
            else if (col ==  2)
                value = (T*)results->at(iterationCount - 1)->GetResultPreviousValues() + resultIndexWithItemCount;
            else
            {
                sprintf_s(buf, sizeof(buf),"");
                return;
            }

            PrintTableArray<T>(buf, literal, itemCount, value);

            if(copyTempValue)
                std::memcpy(tempValue, buf, 1024);
        }

        template<typename T> void PrintTableArray(char* buf, const char* literal, const uint32_t itemCount, const T* vals)
        {
            strcpy_s(buf, sizeof(buf),"");
            char temp[18];

            for (uint32_t i = 0; i < itemCount; ++i)
            {
                sprintf_s(temp, literal, vals[i]);
                if(_hex)
                    strcat_s(buf, sizeof(buf), "0x");
                strcat_s(buf, sizeof(buf), temp);
                if(i < itemCount-1)
                    strcat_s(buf, sizeof(buf), ", ");
            }

            std::puts(buf);
        }

    public:
        static void DrawWindow();

        SignalCombo<SystemRegion> _RegionSelectSignalCombo;
        SignalInputText _SignalInputTextRangeStart;
        SignalInputText _SignalInputTextRangeEnd;

        std::function<void(const char*, uint64_t&)> Slot_RangeTextChanged = [](const char* in, uint64_t& out)
        {
            if (in == nullptr)
                return;

            std::stringstream stream;
            stream << std::hex << std::string(in);
            stream >> out;
            std::cout << in << std::endl;
        };

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
            const std::string hexBegStr = stream.str();
            strcpy_s(GetInstance()._rangeStartText, hexBegStr.c_str());
            const std::string hexEndStr = ToHexString(GetInstance()._regions[GetInstance()._currentRegionSelect].Base + GetInstance()._regions[GetInstance()._currentRegionSelect].Size -1, 0);
            strcpy_s(GetInstance()._rangeEndText, hexEndStr.c_str());
        };

        static void SetUnderlyingBigEndianFlag(const bool isBigEndian)
        {
            GetInstance()._underlyingBigEndian = isBigEndian;
        }
    };
}