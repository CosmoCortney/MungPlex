﻿#pragma once
#include <boost/asio.hpp>
#include <algorithm>
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <boost/thread.hpp>
#include <boost/atomic.hpp>
#include <chrono>
#include "Connection.hpp"
#include <cstdio>
#include "examples/libs/emscripten/emscripten_mainloop_stub.h"
#include <functional>
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_internal.h"
#include <iostream>
#include "HelperFunctions.hpp"
#include "LitColor.hpp"
#include "../../Log.hpp"
#include "MemoryCompare.hpp"
#include "MemoryDump.hpp"
#include "MorphText.hpp"
#include "MungPlexConfig.hpp"
#include "OperativeArray.hpp"
#include "Settings.hpp"
#include <stdio.h>
#include <thread>
#include "WidgetHelpers.hpp"

typedef MemoryCompare::MemCompare MC;

namespace MungPlex
{
    class RegionPairs;

    class Search
    {
    public:
        static void DrawWindow();
        static void SetUnderlyingBigEndianFlag(const bool isBigEndian);
        static void SetRereorderRegion(const bool rereorder);
        static void SetAlignment(const int alignment);
        static void SetNativeAppSearchSettings();
        static void SetDefaultSearchSettings();
        static bool IsBusySearching();
        static void SetMemoryRegions(const std::vector<SystemRegion>& regions);

        std::function<void()> Slot_IndexChanged = []()
        {
            auto& region = ProcessInformation::NEWGetSystemRegionList()[GetInstance()._currentRegionSelect];
            GetInstance()._rangeStartInput.SetValue(region.Base);
            GetInstance()._rangeEndInput.SetValue(region.Base + region.Size - 1);
        };

    private:
        Search();
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

        //value options
        InputInt<uint32_t> _alignmentValueInput = InputInt<uint32_t>("Alignment:", true, 4, 1, 1);
        int _lastRangeSelect = -1;
        bool _signed = false;
        bool _hex = false;
        bool _cached = false;
        bool _disableUndo = false;
        float _precision = 100.0f;
        bool _diableBecauseUnknownAndNotRangebased = false;
        bool _disableBecauseNoText = false;
        bool _caseSensitive = true;
        static const std::vector<StringIdCombo::VecType> _searchValueTypes;
        StringIdCombo _searchValueTypesCombo = StringIdCombo("Value Type:", true, _searchValueTypes);
        static const std::vector<StringIdCombo::VecType> _searchPrimitiveTypes;
        StringIdCombo _primitiveTypesCombo = StringIdCombo("Primitive Type:", true, _searchPrimitiveTypes);
        static const std::vector<StringIdCombo::VecType> _searchArrayTypes;
        StringIdCombo _arrayTypesCombo = StringIdCombo("Array Type:", true, _searchArrayTypes);
        static const std::vector<StringIdCombo::VecType> _searchColorTypes;
        StringIdCombo _colorTypesCombo = StringIdCombo("Color Type:", true, _searchColorTypes);
        StringIdCombo _textTypesCombo = StringIdCombo("Text Type:", true, s_TextTypes);
        bool _forceAlpha = false;
        bool _useColorWheel = false;
        std::wstring _resultsPath = {};
        bool _searchActive = false;
        boost::thread _searchThread;
        bool _busySearching = false;

        //search settings
        static const std::vector<StringIdCombo::VecType> _searchComparasionTypes;
        StringIdCombo _searchComparasionTypeCombo = StringIdCombo("Comparison:", true, _searchComparasionTypes);
        static const std::vector<StringIdCombo::VecType> _intSearchConditionTypes;
        StringIdCombo _subsidiaryTypeSearchConditionsCombo = StringIdCombo("Condition:", true, _intSearchConditionTypes);
        static const std::vector<StringIdCombo::VecType> _floatSearchConditionTypes;
        static const std::vector<StringIdCombo::VecType> _arraySearchConditionTypes;
        static const std::vector<StringIdCombo::VecType> _colorSearchConditionTypes;
        static const std::vector<StringIdCombo::VecType> _textSearchConditionTypes;
        InputText _knownValueInput = InputText("Value:", true, "", 256); //No InputInt because a string param to be parsed is needed
        InputTextMultiline _knownValueTextInput = InputTextMultiline("Text:", true, "", 512);
        InputText _secondaryKnownValueInput = InputText("Not applicable", true, "", 256); //same
        StringIdCombo _iterationsCombo = StringIdCombo("Counter Iteration:", true); 
        uint32_t _iterationCount = 0;
        bool _updateLabels = true;
        ImVec4 _searchColorVec = { 0.0f, 0.0f, 0.0f, 1.0f };
        ImVec4 _pokeColorVec = { 0.0f, 0.0f, 0.0f, 1.0f };

        //range options
        std::vector<SystemRegion> _regions{};
        std::vector<SystemRegion> _dumpRegions{};
        int _currentRegionSelect = 0;
        bool _crossRegion = false;
        bool _rereorderRegion = false;
        static const std::vector<StringIdCombo::VecType> _endiannesses;
        StringIdCombo _endiannessCombo = StringIdCombo("Endianness:", true, _endiannesses);
        InputInt<uint64_t> _rangeStartInput = InputInt<uint64_t>("Start at:", true, 0, 0, 0);
        InputInt<uint64_t> _rangeEndInput = InputInt<uint64_t>("End at:", true, 0, 0, 0);
        RegionCombo _regionSelectCombo = RegionCombo("Region:", true);

        //results
        uint32_t _pagesAmountValue = 0;
        InputInt<int32_t> _currentPageInput = InputInt<int32_t>("Page:", true, 1, 1, 10);;
        uint32_t _pagesAmount = 0;
        std::string _pagesAmountText = "0";
        std::vector<bool> _selectedIndices{};
        InputInt<uint32_t> _maxResultsPerPageInput = InputInt<uint32_t>("Max. results per page:", true, 256, 16, 128);
        bool _multiPoke = false;
        bool _pokePrevious = false;
        std::vector<char> _pokeValue;
        InputText  _pokeValueInput = InputText("Value:", true, "", 256); //No InputInt because a string param to be parsed is needed
        InputTextMultiline _pokeValueTextInput = InputTextMultiline("Value:", true, "", 512);
        uint64_t _pokeAddress = 0;
        InputText  _pokeAddressInput = InputText("Address:", true, "", 16); //No InputInt because non-numeral information might be displayed
        std::tuple<uint64_t, int> _searchStats;
        bool _deselectedIllegalSelection = false;
        std::vector<MemoryCompare::MemDump> _currentMemoryDumps{};
        bool _scanAllRegions = false;
        std::vector<uint8_t> _updateValues;
        boost::thread _updateThread;
        boost::atomic<bool> _updateThreadFlag = false;
        uint64_t _lastPageResultCount = 0;
        uint16_t _arrayItemCount = 0;
        uint32_t _liveUpdateMilliseconds = 16;
        std::string _formatting;

        //value type options
        void drawValueTypeOptions();
        void setFormatting();

        //range options
        void drawRangeOptions();

        //setup search
        void setUpAndIterate();
        void performSearch();
        void primitiveTypeSearchLog();
        void arrayTypeSearchLog();
        void textTypeSearchLog();
        void colorTypeSearchLog();
        void generateDumpRegionMap();
        void emplaceDumpRegion(const uint16_t index);
        void setRecommendedValueSettings(const int valueType);

        //search options
        void drawSearchOptions();
        void drawPrimitiveSearchOptions();
        void drawArraySearchOptions();
        void drawColorSelectOptions();
        void drawColorSearchOptions();
        void drawTextSearchOptions();
        void setUpIterationSelect();

        //results
        void setUpResultPaging();
        void drawResultsArea();
        void drawResultsTable();
        bool isSelectionOrIndexOurOfBounds(const uint64_t row, const uint64_t resultCount);
        void drawPrimitiveTableRow(const int col, const uint64_t row, const uint64_t pageIndexWithRowCount, FloorString& buf, FloorString& tempValue);
        void drawArrayTableRow(const int col, const uint64_t pageIndexWithRowCount, FloorString& buf, FloorString& tempValue);
        void drawColorTableRow(const int col, const uint64_t row, const uint64_t pageIndexWithRowCount, FloorString& buf, FloorString& tempValue);
        void drawTextTableRow(const int col, const uint64_t row, const uint64_t pageIndexWithRowCount, FloorString& buf, FloorString& tempValue);
        void drawSelectedTableRowColor(const uint64_t row, FloorString& buf);
        void selectRows(const uint64_t row, bool& rowClicked, FloorString& tempAddress);
        void prepareLiveUpdateValueList();
        void setLiveUpdateRefreshRate();
        void updateLivePreview();
        void updateLivePreviewOnce();
        void updateLivePreviewConditional();

        //value poke
        void performValuePoke();

        template<typename addressType> bool pokeText()
        {
            const int pid = ProcessInformation::GetPID();
            const std::string pokeTextp = _pokeValueTextInput.GetStdStringNoZeros();
            MorphText pokeValue(pokeTextp);
            auto& results = MemoryCompare::MemCompare::GetResults();

            if (_multiPoke)
            {
                const uint64_t resultIndex = (_currentPageInput.GetValue() - 1) * _maxResultsPerPageInput.GetValue();

                for (int index = 0; index < _selectedIndices.size(); ++index)
                {
                    if (_selectedIndices[index] == false)
                        continue;

                    uint64_t address = results.GetAddressAllRanges<addressType>(resultIndex + index);
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

                    switch (_colorTypesCombo.GetSelectedId())
                    {
                        case MT::UTF16LE: case MT::UTF16BE:
                            ProcessInformation::WriteTextEx(pid, pokeValue.GetString<wchar_t*>(_textTypesCombo.GetSelectedId()), address);
                        break;
                        case MT::UTF32LE: case MT::UTF32BE:
                            ProcessInformation::WriteTextEx(pid, pokeValue.GetString<char32_t*>(_textTypesCombo.GetSelectedId()), address);
                            break;
                        default:
                            ProcessInformation::WriteTextEx(pid, pokeValue.GetString<char*>(_textTypesCombo.GetSelectedId()), address);
                    }
                }

                Log::LogInformation((std::string("Multi-poked ") + _textTypesCombo.GetSelectedStdString() + " text values").c_str());
                return true;
            }

            uint64_t address = _pokeAddress;

            for (int i = 0; i < _regions.size(); ++i)
            {
                if (_pokeAddress >= _regions[i].Base && _pokeAddress <= _regions[i].Base + _regions[i].Size)
                {
                    address -= _regions[i].Base;
                    address += reinterpret_cast<uint64_t>(_regions[i].BaseLocationProcess);
                    bool success;

                    switch (_textTypesCombo.GetSelectedId())
                    {
                        case MT::UTF16LE: case MT::UTF16BE:
                            success = ProcessInformation::WriteTextEx(pid, pokeValue.GetString<wchar_t*>(_textTypesCombo.GetSelectedId()), address);
                        break;
                        case MT::UTF32LE: case MT::UTF32BE:
                            success = ProcessInformation::WriteTextEx(pid, pokeValue.GetString<char32_t*>(_textTypesCombo.GetSelectedId()), address);
                        break;
                        default:
                            success = ProcessInformation::WriteTextEx(pid, pokeValue.GetString<char*>(_textTypesCombo.GetSelectedId()), address);
                    }

                    Log::LogInformation((std::string("Poked ") + _textTypesCombo.GetSelectedStdString() + " text value").c_str());
                    return success;
                }
            }
            
            return false;
        }

        template<typename addressType> bool pokeColor()
        {
            const std::string colorString = _pokeValueInput.GetStdStringNoZeros();
            LitColor pokeValue(colorString);
            const int selectedType = pokeValue.GetSelectedType();
            auto& results = MemoryCompare::MemCompare::GetResults();

            if (_multiPoke)
            {
                const uint64_t resultIndex = (_currentPageInput.GetValue() - 1) * _maxResultsPerPageInput.GetValue();

                for (int index = 0; index < _selectedIndices.size(); ++index)
                {
                    if (_selectedIndices[index] == false)
                        continue;

                    uint64_t address = results.GetAddressAllRanges<addressType>(resultIndex + index);

                    switch (selectedType) //RGB888, RGBA8888
                    {
                    case LitColor::RGB565: case LitColor::RGB5A3:
                    {
                        uint16_t val;

                        if (_pokePrevious)
                            val = results.GetPreviousValueAllRanges<uint16_t>(resultIndex + index);
                        else
                        {
                            if (selectedType == LitColor::RGB565)
                                val = pokeValue.GetRGB565();
                            else
                                val = pokeValue.GetRGB5A3();
                        }

                        ProcessInformation::WriteValue<uint16_t>(address, val);
                    }break;
                    case LitColor::RGBF: case LitColor::RGBAF:
                    {
                        for (int item = 0; item < (selectedType == LitColor::RGBF ? 3 : 4); ++item)
                        {
                            float val = _pokePrevious ? results.GetPreviousValueAllRanges<float>(resultIndex + index + item) : pokeValue.GetColorValue<float>(item);
                            ProcessInformation::WriteValue<float>(address + item * sizeof(float), val);
                        }
                    } break;
                    default: //LitColor::RGB888: LitColor::RGBA8888: 
                    {
                        uint32_t val = _pokePrevious ? results.GetPreviousValueAllRanges<uint32_t>(resultIndex + index) : pokeValue.GetRGBA();
                        ProcessInformation::WriteValue<uint32_t>(address, val);
                    } break;
                    }
                }

                Log::LogInformation((std::string("Multi-poked ") + _colorTypesCombo.GetSelectedStdString() + " color values").c_str());
                return true;
            }

            switch (selectedType) //RGB888, RGBA8888
            {
            case LitColor::RGB565: case LitColor::RGB5A3:
            {
                uint16_t val;

                if (selectedType == LitColor::RGB565)
                    val = pokeValue.GetRGB565();
                else
                    val = pokeValue.GetRGB5A3();
                
                ProcessInformation::WriteValue<uint16_t>(_pokeAddress, val);
            }break;
            case LitColor::RGBF: case LitColor::RGBAF:
            {
                for (int item = 0; item < (selectedType == LitColor::RGBF ? 3 : 4); ++item)
                {
                    float val = pokeValue.GetColorValue<float>(item);
                    ProcessInformation::WriteValue<float>(_pokeAddress + item * sizeof(float), val);
                }
            } break;
            default: //LitColor::RGB888: LitColor::RGBA8888: 
            {
                uint32_t val = pokeValue.GetRGBA();
                ProcessInformation::WriteValue<uint32_t>(_pokeAddress, val);
            } break;
            }

            return true;
        }

        template<typename uType, typename addressType> bool pokeArray()
        {
            const uint64_t itemCount = OperativeArray<uType>(_knownValueInput.GetStdStringNoZeros()).ItemCount();
            std::string arrayString = _pokeValueInput.GetStdStringNoZeros();
            OperativeArray<uType> pokeArray(arrayString);
            auto& results = MemoryCompare::MemCompare::GetResults();
            
            if (_multiPoke)
            {
                int regionIndex = -1;
                bool swapBytes = _endiannessCombo.GetSelectedId() == BIG;
                const uint64_t resultIndex = (_currentPageInput.GetValue() - 1) * _maxResultsPerPageInput.GetValue();

                for (int index = 0; index < _selectedIndices.size(); ++index)
                {
                    if (_selectedIndices[index] == false)
                        continue;

                    uint64_t address = results.GetAddressAllRanges<addressType>(resultIndex + index);

                    if (_pokePrevious)
                    {
                        if (MemoryCompare::MemCompare::GetIterationCount() < 0)
                        {
                            uType* buf = new uType[itemCount];
                            std::fill(buf, buf + itemCount-1, 0);
                            pokeArray = OperativeArray<uType>(buf, itemCount);
                            delete[] buf;
                        }
                        else
                            pokeArray = OperativeArray<uType>(results.GetSpecificPreviousValuePtrAllRanges<uType>(resultIndex + index), itemCount);
                    }
                    
                    for (int i = 0; i < itemCount; ++i)
                        if (!pokeArray.IsIgnoredIndex(i))
                            ProcessInformation::WriteValue<uType>(address + sizeof(uType) * i, pokeArray[i]);
                }

                Log::LogInformation((std::string("Multi-poked ") + _arrayTypesCombo.GetSelectedStdString() + " array values").c_str());
                return true;
            }
            
            for (int i = 0; i < itemCount; ++i)
                if (!pokeArray.IsIgnoredIndex(i))
                    ProcessInformation::WriteValue<uType>(_pokeAddress + sizeof(uType) * i, pokeArray[i]);

            Log::LogInformation((std::string("Poked ") + _arrayTypesCombo.GetSelectedStdString() + " array value").c_str());
            return true;
        }

        template<typename dataType, typename addressType> bool pokeValue()
        {
            dataType* pokeValuePtr = (dataType*)&_pokeValue[0];
            auto& results = MemoryCompare::MemCompare::GetResults();

            if (_multiPoke)
            {
                bool swapBytes = _endiannessCombo.GetSelectedId() == BIG;
                const uint64_t resultIndex = (_currentPageInput.GetValue() - 1) * _maxResultsPerPageInput.GetValue();

                for (int index = 0; index < _selectedIndices.size(); ++index)
                {
                    if (_selectedIndices[index] == false)
                        continue;

                    uint64_t address = results.GetAddressAllRanges<addressType>(resultIndex + index);

                    if (_pokePrevious)
                        if (MemoryCompare::MemCompare::GetIterationCount() < 1)
                            *pokeValuePtr = 0;
                        else
                            *pokeValuePtr = results.GetPreviousValueAllRanges<dataType>(resultIndex + index);

                    ProcessInformation::WriteValue<dataType>(address, *reinterpret_cast<dataType*>(pokeValuePtr));
                }

                Log::LogInformation((std::string("Multi-poked ") + _primitiveTypesCombo.GetSelectedStdString() + " values").c_str());
                return true;
            }

            ProcessInformation::WriteValue<dataType>(_pokeAddress, *reinterpret_cast<dataType*>(_pokeValue.data()));
            Log::LogInformation((std::string("Poked ") + _primitiveTypesCombo.GetSelectedStdString() + " value").c_str());
            return true;
        }

        template<typename T> void drawArrayValues(const int col, const uint16_t itemCount, const uint64_t index, FloorString& buf, FloorString& tempValue, const char* literal)
        {
            //const int iterationIndex = MemoryCompare::MemCompare::GetSearchStats().second-1;
            T* value = nullptr;
            const bool copyTempValue = (col == 1 && !_pokePrevious) || (col == 2 && _pokePrevious);

            if (col == 1)
                value = MemoryCompare::MemCompare::GetResults().GetSpecificValuePtrAllRanges<T>(index * itemCount);
            else if (col == 2)
            {
                if (MemoryCompare::MemCompare::GetIterationCount() > 2)
                    value = MemoryCompare::MemCompare::GetResults().GetSpecificPreviousValuePtrAllRanges<T>(index * itemCount);
                else
                {
                    sprintf_s(buf.Data(), buf.Size(), "");
                    return;
                }
            }
            else if (col == 4)
            {
                value = reinterpret_cast<T*>(_updateValues.data() + (index % _maxResultsPerPageInput.GetValue()) * itemCount * sizeof(T));
            }
            else
            {
                sprintf_s(buf.Data(), buf.Size(),"");
                return;
            }

            printTableArray<T>(buf, literal, itemCount, value);

            if(copyTempValue)
                tempValue = buf;
        }

        template<typename T> void printTableArray(FloorString& buf, const char* literal, const uint32_t itemCount, const T* vals)
        {
            if (vals == nullptr)
                return; 

            buf = "";
            static FloorString temp("", 18);

            for (uint32_t i = 0; i < itemCount; ++i)
            {
                sprintf(temp.Data(), literal, vals[i]);
                if(_hex)
                    strcat_s(buf.Data(), buf.Size(), "0x");
                strcat_s(buf.Data(), buf.Size(), temp.Data());
                if(i < itemCount-1)
                    strcat_s(buf.Data(), buf.Size(), ", ");
            }
        }

    };
}