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
#include "MungPlexConfig.h"
#include "Connection.h"
#include"HelperFunctions.h"
#include <chrono>
#include <thread>
#include<functional>
#include<algorithm>
#include"MemCompareResult.h"
#include <cstdio>
#include"Settings.h"
#include"Log.h"
#include"LitColor.h"
#include"MorphText.h"
#include"OperativeArray.h"
#include"MemDump.h"
#include"LitColor.h"
#include"MorphText.h"
#include"OperativeArray.h"
#include"MemCompare.h"

namespace MungPlex
{
    class Search
    {
    public:
        static void DrawWindow();
        static void SetUnderlyingBigEndianFlag(const bool isBigEndian);
        static void SetRereorderRegion(const bool rereorder);
        static void SetAlignment(const int alignment);

        SignalCombo<SystemRegion> _RegionSelectSignalCombo;
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
                std::stringstream stream;
                stream << std::hex << GetInstance()._regions[GetInstance()._currentRegionSelect].Base;
                const std::string hexBegStr = stream.str();
                GetInstance()._rangeStartText = hexBegStr;
                const std::string hexEndStr = ToHexString(GetInstance()._regions[GetInstance()._currentRegionSelect].Base + GetInstance()._regions[GetInstance()._currentRegionSelect].Size - 1, 0);
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
        int _currentComparisionTypeSelect = 0;
        int _currentConditionTypeSelect = 0;
        ImVec4 _colorVec;

        //search settings
        int _alignmentValue = 4;
        int _lastRangeSelect = -1;
        bool _signed = false;
        bool _hex = false;
        bool _cached = false;
        float _precision = 100.0f;
        bool _diableBecauseUnknownAndNotRangebased = false;
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
        bool _forceAlpha = false;
        bool _useColorWheel = false;
        std::wstring _resultsPath = {};
        bool _searchActive = false;

        //value options
        FloorString _knownValueText = FloorString("", 256);
        FloorString _secondaryKnownValueText = FloorString("", 256);
        std::vector<std::string> _iterations;
        int _iterationIndex = 0;
        bool _updateLabels = true;

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
        bool _underlyingBigEndian = false;
        bool _deselectedIllegalSelection = false;
        std::vector<MemoryCompare::MemDump> _currentMemoryDumps{};
        bool _scanAllRegions = false;

        void setUpAndIterate();
        void drawValueTypeOptions();
        void drawRangeOptions();
        void drawSearchOptions();
        void drawResultsArea();
        void performSearch();
        void primitiveTypeSearchLog();
        void arrayTypeSearchLog();
        void textTypeSearchLog();
        void colorTypeSearchLog();
        void drawResultsTableNew();
        void generateDumpRegionMap();
        void emplaceDumpRegion(const uint16_t index);
        void setRecommendedValueSettings(const int valueType);
        void setUpIterationSelect();
        void setUpResultPaging();

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
                    case MorphText::ASCII:
                        WriteTextEx(pid, pokeValue.GetASCII().data(), address);
                    break;
                    case MorphText::SHIFTJIS:
                        WriteTextEx(pid, pokeValue.GetShiftJis(), address);
                    break;
                    case MorphText::JIS_X_0201_FULLWIDTH:
                        WriteTextEx(pid, pokeValue.GetJisX0201FullWidth(), address);
                    break;
                    case MorphText::JIS_X_0201_HALFWIDTH:
                        WriteTextEx(pid, pokeValue.GetJisX0201HalfWidth(), address);
                    break;
                    case MorphText::UTF8:
                        WriteTextEx(pid, pokeValue.GetUTF8().data(), address);
                    break;
                    case MorphText::UTF16LE: case MorphText::UTF16BE:
                        WriteTextEx(pid, pokeValue.GetUTF16(_currentTextTypeSelect == MorphText::UTF16BE ? true : false).data(), address);
                        break;
                    case MorphText::UTF32LE: case MorphText::UTF32BE:
                        WriteTextEx(pid, pokeValue.GetUTF32(_currentTextTypeSelect == MorphText::UTF32BE ? true : false).data(), address);
                        break;
                    default: //ISO-8859-X
                        WriteTextEx(pid, pokeValue.GetISO8859X(_currentTextTypeSelect), address);
                    }
                }

                Log::LogInformation((std::string("Multi-poked ") + _searchTextTypes[_currentTextTypeSelect].first + " text values").c_str());
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
                    case MorphText::ASCII: {
                        success = WriteTextEx(pid, pokeValue.GetASCII().data(), address);
                    } break;
                    case MorphText::SHIFTJIS: {
                        success = WriteTextEx(pid, pokeValue.GetShiftJis(), address);
                    } break;
                    case MorphText::JIS_X_0201_FULLWIDTH: {
                        success = WriteTextEx(pid, pokeValue.GetJisX0201FullWidth(), address);
                    } break;
                    case MorphText::JIS_X_0201_HALFWIDTH: {
                        success = WriteTextEx(pid, pokeValue.GetJisX0201HalfWidth(), address);
                    } break;
                    case MorphText::UTF8: {
                        success = WriteTextEx(pid, pokeValue.GetUTF8().data(), address);
                    } break;
                    case MorphText::UTF16LE: case MorphText::UTF16BE: {
                        success = WriteTextEx(pid, pokeValue.GetUTF16(_currentTextTypeSelect == MorphText::UTF16BE ? true : false).data(), address);
                    } break;
                    case MorphText::UTF32LE: case MorphText::UTF32BE: {
                        success = WriteTextEx(pid, pokeValue.GetUTF32(_currentTextTypeSelect == MorphText::UTF32BE ? true : false).data(), address);
                    } break;
                    default: //ISO-8859-X
                        success = WriteTextEx(pid, pokeValue.GetISO8859X(_currentTextTypeSelect), address);
                    }

                    Log::LogInformation((std::string("Poked ") + _searchTextTypes[_currentTextTypeSelect].first + " text value").c_str());
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

                Log::LogInformation((std::string("Multi-poked ") + _searchColorTypes[_currentColorTypeSelect].first + " color values").c_str());
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
                bool swapBytes = _underlyingBigEndian;
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

                Log::LogInformation((std::string("Multi-poked ") + _searchArrayTypes[_currentArrayTypeSelect].first + " array values").c_str());
                return true;
            }
            
            for (int i = 0; i < itemCount; ++i)
                if (!pokeArray.IsIgnoredIndex(i))
                    ProcessInformation::WriteValue<uType>(_pokeAddress + sizeof(uType) * i, pokeArray[i]);

            Log::LogInformation((std::string("Poked ") + _searchArrayTypes[_currentArrayTypeSelect].first + " array value").c_str());
            return true;
        }

        template<typename dataType, typename addressType> bool pokeValue()
        {
            dataType* pokeValuePtr = (dataType*)&_pokeValue[0];
            auto& results = MemoryCompare::MemCompare::GetResults();

            if (_multiPoke)
            {
                bool swapBytes = _underlyingBigEndian;
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

                Log::LogInformation((std::string("Multi-poked ") + _searchPrimitiveTypes[_currentPrimitiveTypeSelect].first + " values").c_str());
                return true;
            }

            ProcessInformation::WriteValue<dataType>(_pokeAddress, *reinterpret_cast<dataType*>(_pokeValue.data()));
            Log::LogInformation((std::string("Poked ") + _searchPrimitiveTypes[_currentPrimitiveTypeSelect].first + " value").c_str());
            return true;
        }

        template<typename T> void drawArrayValues(const int col, const uint16_t itemCount, const uint64_t index, char* buf, char* tempValue, const char* literal)
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
                    sprintf_s(buf, 1024, "");
                    return;
                }
            }
            else
            {
                sprintf_s(buf, 1024,"");
                return;
            }

            printTableArray<T>(buf, literal, itemCount, value);

            if(copyTempValue)
                std::memcpy(tempValue, buf, 1024);
        }

        template<typename T> void printTableArray(char* buf, const char* literal, const uint32_t itemCount, const T* vals)
        {
            if (vals == nullptr)
                return; 

            strcpy_s(buf, 1024,"");
            static FloorString temp("", 18);

            for (uint32_t i = 0; i < itemCount; ++i)
            {
                sprintf(temp.Data(), literal, vals[i]);
                if(_hex)
                    strcat_s(buf, 1024, "0x");
                strcat_s(buf, 1024, temp.Data());
                if(i < itemCount-1)
                    strcat_s(buf, 1024, ", ");
            }

            //std::puts(buf);
        }

    };
}