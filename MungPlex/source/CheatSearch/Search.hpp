#pragma once
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

        SignalCombo _RegionSelectSignalCombo;
        SignalInputText _SignalInputTextRangeStart;
        SignalInputText _SignalInputTextRangeEnd;

        std::function<void(const char*, uint64_t&)> Slot_RangeTextChanged = [](const char* in, uint64_t& out)
            {
                /*if (in == nullptr)
                    return;

                std::stringstream stream;
                stream << std::hex << std::string(in);
                stream >> out;
                std::cout << in << std::endl;*/
            };

        std::function<void()> Slot_IndexChanged = []()
            {

            };

        std::function<void()> Slot_ItemCountChanged = []()
            {

            };

        std::function<void()> Slot_TextChanged = []()
            {
                auto& region = ProcessInformation::GetSystemRegionList_().GetRegion(GetInstance()._currentRegionSelect);
                std::stringstream stream;
                stream << std::hex << region.Base;
                const std::string hexBegStr = stream.str();
                GetInstance()._rangeStartText = hexBegStr;
                const std::string hexEndStr = ToHexString(region.Base + region.Size - 1, 0);
                GetInstance()._rangeEndText = hexEndStr;
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

        static const StringIdPairs _searchValueTypes;
        static const StringIdPairs _endiannesses;
        static const StringIdPairs _searchPrimitiveTypes;
        static const StringIdPairs _searchArrayTypes;
        static const StringIdPairs _searchColorTypes;
        static const StringIdPairs _searchConditionTypes;
        static const StringIdPairs _searchConditionTypesArray;
        static const StringIdPairs _searchConditionTypesFloat;
        static const StringIdPairs _searchConditionTypesColor;
        static const StringIdPairs _searchConditionTypesText;
        static const StringIdPairs _searchComparasionType;

        //search settings
        int _currentcomparisonTypeSelect = 0;
        int _currentConditionTypeSelect = 0;
        int _alignmentValue = 4;
        int _lastRangeSelect = -1;
        bool _signed = false;
        bool _hex = false;
        bool _cached = false;
        bool _disableUndo = false;
        float _precision = 100.0f;
        bool _diableBecauseUnknownAndNotRangebased = false;
        bool _disableBecauseNoText = false;
        bool _caseSensitive = true;
        int _currentValueTypeSelect = 0;
        int _currentPrimitiveTypeSelect = 0;
        int _currentArrayTypeSelect = 0;
        int _currentTextTypeSelect = MT::UTF8;
        int _currentTextTypeIndex = 0;
        int _currentColorTypeSelect = 0;
        bool _forceAlpha = false;
        bool _useColorWheel = false;
        std::wstring _resultsPath = {};
        bool _searchActive = false;
        boost::thread _searchThread;
        bool _busySearching = false;

        //value options
        FloorString _knownValueText = FloorString("", 256);
        FloorString _secondaryKnownValueText = FloorString("", 256);
        StringIdPairs _iterations = { {}, {}, "Counter Iteration:"};
        int _iterationIndex = 0; 
        uint32_t _iterationCount = 0;
        bool _updateLabels = true;
        ImVec4 _searchColorVec = { 0.0f, 0.0f, 0.0f, 1.0f };
        ImVec4 _pokeColorVec = { 0.0f, 0.0f, 0.0f, 1.0f };

        //range options
        std::vector<SystemRegion> _regions{};
        std::vector<SystemRegion> _dumpRegions{};
        int _currentRegionSelect = 0;
        FloorString _rangeStartText = FloorString("", 17);
        uint64_t _rangeStartValue = 0;
        FloorString _rangeEndText = FloorString("", 17);
        uint64_t _rangeEndValue = 0;
        bool _crossRegion = false;
        bool _rereorderRegion = false;
        int _endiannessSelect = 0;

        //results
        uint32_t _pagesAmountValue = 0;
        int32_t _currentPageValue = 1;
        uint32_t _pagesAmount = 0;
        std::string _pagesAmountText = "0";
        std::vector<bool> _selectedIndices{};
        int _maxResultsPerPage = 256;
        bool _multiPoke = false;
        bool _pokePrevious = false;
        std::vector<char> _pokeValue;
        FloorString  _pokeValueText = FloorString("", 256);
        uint64_t _pokeAddress = 0;
        FloorString  _pokeAddressText = FloorString("", 17);
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
            const std::string pokeTextp = _pokeValueText.StdStr();
            MorphText pokeValue(pokeTextp);
            auto& results = MemoryCompare::MemCompare::GetResults();

            if (_multiPoke)
            {
                const uint64_t resultIndex = (_currentPageValue - 1) * _maxResultsPerPage;

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

                    switch (_currentTextTypeSelect)
                    {
                        case MT::UTF16LE: case MT::UTF16BE:
                            WriteTextEx(pid, pokeValue.GetString<wchar_t*>(_currentTextTypeSelect), address);
                        break;
                        case MT::UTF32LE: case MT::UTF32BE:
                            WriteTextEx(pid, pokeValue.GetString<char32_t*>(_currentTextTypeSelect), address);
                            break;
                        default:
                            WriteTextEx(pid, pokeValue.GetString<char*>(_currentTextTypeSelect), address);
                    }
                }

                Log::LogInformation((std::string("Multi-poked ") + TextTypes.GetStdStringById(_currentTextTypeIndex) + " text values").c_str());
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

                    switch (_currentTextTypeSelect)
                    {
                        case MT::UTF16LE: case MT::UTF16BE:
                            success = WriteTextEx(pid, pokeValue.GetString<wchar_t*>(_currentTextTypeSelect), address);
                        break;
                        case MT::UTF32LE: case MT::UTF32BE:
                            success = WriteTextEx(pid, pokeValue.GetString<char32_t*>(_currentTextTypeSelect), address);
                        break;
                        default:
                            success = WriteTextEx(pid, pokeValue.GetString<char*>(_currentTextTypeSelect), address);
                    }

                    Log::LogInformation((std::string("Poked ") + TextTypes.GetStdStringById(_currentTextTypeIndex) + " text value").c_str());
                    return success;
                }
            }
            
            return false;
        }

        template<typename addressType> bool pokeColor()
        {
            const std::string colorString = _pokeValueText.StdStr();
            LitColor pokeValue(colorString);
            const int selectedType = pokeValue.GetSelectedType();
            auto& results = MemoryCompare::MemCompare::GetResults();

            if (_multiPoke)
            {
                const uint64_t resultIndex = (_currentPageValue - 1) * _maxResultsPerPage;

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

                Log::LogInformation((std::string("Multi-poked ") + _searchColorTypes.GetStdStringById(_currentColorTypeSelect) + " color values").c_str());
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
            const uint64_t itemCount = OperativeArray<uType>(_knownValueText.StdStr()).ItemCount();
            std::string arrayString = _pokeValueText.StdStr();
            OperativeArray<uType> pokeArray(arrayString);
            auto& results = MemoryCompare::MemCompare::GetResults();
            
            if (_multiPoke)
            {
                int regionIndex = -1;
                bool swapBytes = _endiannessSelect;
                const uint64_t resultIndex = (_currentPageValue - 1) * _maxResultsPerPage;

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

                Log::LogInformation((std::string("Multi-poked ") + _searchArrayTypes.GetStdStringById(_currentArrayTypeSelect) + " array values").c_str());
                return true;
            }
            
            for (int i = 0; i < itemCount; ++i)
                if (!pokeArray.IsIgnoredIndex(i))
                    ProcessInformation::WriteValue<uType>(_pokeAddress + sizeof(uType) * i, pokeArray[i]);

            Log::LogInformation((std::string("Poked ") + _searchArrayTypes.GetStdStringById(_currentArrayTypeSelect) + " array value").c_str());
            return true;
        }

        template<typename dataType, typename addressType> bool pokeValue()
        {
            dataType* pokeValuePtr = (dataType*)&_pokeValue[0];
            auto& results = MemoryCompare::MemCompare::GetResults();

            if (_multiPoke)
            {
                bool swapBytes = _endiannessSelect;
                const uint64_t resultIndex = (_currentPageValue - 1) * _maxResultsPerPage;

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

                Log::LogInformation((std::string("Multi-poked ") + _searchPrimitiveTypes.GetStdStringById(_currentPrimitiveTypeSelect) + " values").c_str());
                return true;
            }

            ProcessInformation::WriteValue<dataType>(_pokeAddress, *reinterpret_cast<dataType*>(_pokeValue.data()));
            Log::LogInformation((std::string("Poked ") + _searchPrimitiveTypes.GetStdStringById(_currentPrimitiveTypeSelect) + " value").c_str());
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
                value = reinterpret_cast<T*>(_updateValues.data() + (index % _maxResultsPerPage) * itemCount * sizeof(T));
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