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

namespace MungPlex
{
    template <typename dataType, typename addressType> struct SearchResult
    {
        addressType _address;
        dataType _currentValue;
        dataType _previousValue;
        dataType _difference;
    };

    class Search
    {
    private:
Search()
        {
            _searchValueTypes.push_back(std::pair<std::string, int>("Primitive", PRIMITIVE));
            _searchValueTypes.push_back(std::pair<std::string, int>("Array", ARRAY));
            _searchValueTypes.push_back(std::pair<std::string, int>("Text", TEXT));
            _searchValueTypes.push_back(std::pair<std::string, int>("Color", COLOR));

            _searchPrimitiveTypes.push_back(std::pair<std::string, int>("Int 8 (1 Byte)", INT8));
            _searchPrimitiveTypes.push_back(std::pair<std::string, int>("Int 16 (2 Bytes)", INT16));
            _searchPrimitiveTypes.push_back(std::pair<std::string, int>("Int 32 (4 Bytes)", INT32));
            _searchPrimitiveTypes.push_back(std::pair<std::string, int>("Int 64 (8 Bytes)", INT64));
            _searchArrayTypes = _searchPrimitiveTypes;
            _searchPrimitiveTypes.push_back(std::pair<std::string, int>("Float Single", FLOAT));
            _searchPrimitiveTypes.push_back(std::pair<std::string, int>("Float Double", DOUBLE));

            _searchTextTypes.push_back(std::pair<std::string, int>("ASCII", ASCII));
            _searchTextTypes.push_back(std::pair<std::string, int>("UTF-8", UTF8));
            _searchTextTypes.push_back(std::pair<std::string, int>("UTF-16", UTF16));
            _searchTextTypes.push_back(std::pair<std::string, int>("UTF-32", UTF32));
            _searchTextTypes.push_back(std::pair<std::string, int>("Shift JIS", SHIFT_JIS));

            _searchColorTypes.push_back(std::pair<std::string, int>("Color RGB (3 Bytes)", RGB_BYTE));
            _searchColorTypes.push_back(std::pair<std::string, int>("Color RGBA (4 Bytes)", RGBA_BYTE));
            _searchColorTypes.push_back(std::pair<std::string, int>("Color RGB (3 Floats)", RGB_FLOAT));
            _searchColorTypes.push_back(std::pair<std::string, int>("Color RGBA (4 Floats)", RGBA_FLOAT));

            _searchConditionTypes.push_back(std::pair<std::string, int>("Equal (==)", Xertz::EQUAL));
            _searchConditionTypesText = _searchConditionTypes;
            _searchConditionTypes.push_back(std::pair<std::string, int>("Unequal (!=)", Xertz::UNEQUAL));
            _searchConditionTypesArray = _searchConditionTypes;
            _searchConditionTypesColor = _searchConditionTypes;
            _searchConditionTypes.push_back(std::pair<std::string, int>("Greater (>)", Xertz::GREATER));
            _searchConditionTypes.push_back(std::pair<std::string, int>("Greater or Equal (>=)", Xertz::GREATER_EQUAL));
            _searchConditionTypes.push_back(std::pair<std::string, int>("Lower (<)", Xertz::LOWER));
            _searchConditionTypes.push_back(std::pair<std::string, int>("Lower or Equal (<=)", Xertz::LOWER_EQUAL));
            _searchConditionTypes.push_back(std::pair<std::string, int>("Increased by", Xertz::INCREASED_BY));
            _searchConditionTypes.push_back(std::pair<std::string, int>("Decreased by", Xertz::DECREASED_BY));
            _searchConditionTypes.push_back(std::pair<std::string, int>("Value Between", Xertz::BETWEEN));
            _searchConditionTypes.push_back(std::pair<std::string, int>("Value Not Between", Xertz::NOT_BETWEEN));
            _searchConditionTypesFloat = _searchConditionTypes;
            _searchConditionTypes.push_back(std::pair<std::string, int>("AND (has all true bits)", Xertz::AND));
            _searchConditionTypes.push_back(std::pair<std::string, int>("OR (has at least 1 true bit)", Xertz::OR));

            _searchComparasionType.push_back(std::pair<std::string, int>("Unknown/Initial", Xertz::UNKNOWN));
            _searchComparasionType.push_back(std::pair<std::string, int>("Known Value", Xertz::KNOWN));

            _RegionSelectSignalCombo.ConnectOnIndexChanged(Slot_IndexChanged);
            _RegionSelectSignalCombo.ConnectOnItemCountChanged(Slot_ItemCountChanged);
            _RegionSelectSignalCombo.ConnectOnTextChanged(Slot_TextChanged);
            _SignalInputTextRangeStart.ConnectOnTextChanged(std::bind(Slot_RangeTextChanged, _rangeStartText, std::ref(_rangeStartValue)));
            _SignalInputTextRangeEnd.ConnectOnTextChanged(std::bind(Slot_RangeTextChanged, _rangeEndText, std::ref(_rangeEndValue)));

            _selectedIndices.resize(_maxResultsPerPage);
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
        void ResetSearch();

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
        std::vector<MungPlex::SystemRegion> _regions{};
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
        bool _caseSensitive = false;
        bool _disableBecauseNoArray = false;
        int _currentValueTypeSelect = 0;
        int _currentPrimitiveTypeSelect = 0;
        int _currentArrayTypeSelect = 0;
        int _currentTextTypeSelect = 0;
        int _currentColorTypeSelect = 0;
        bool _useColorWheel = false;

        //value options
        char _knownValueText[2048] = { "" };
        std::any _knownValueValue = 0;
        char _secondaryKnownValueText[2048] = { "" };
        std::any _secondaryKnownValueValue = 0;

        //range options
        char _rangeStartText[256] = { "" };
        uint64_t _rangeStartValue = 0;
        char _rangeEndText[256] = { "" };
        uint64_t _rangeEndValue = 0;

        std::wstring _dir = L"F:\\test\\";

        //results table
        std::any _searchResults = NULL;
        int _currentRegionSelect = 0;
        uint32_t _pagesAmountValue = 0;
        char _pagesAmountText[17] = {"0"};
        uint32_t _currentPageValue = 1;
        char _currentPageText[8] = { "1" };
        uint64_t _resultCount = 0;
        std::vector<bool> _selectedIndices{};
        int _maxResultsPerPage = 256;
        bool _multiPoke = false;
        bool _pokePrevious = false;
        char _pokeValue[1024] = { };
        char _pokeValueText[1024] = { "0" };
        uint64_t _pokeAddress = 0;
        char _pokeAddressText[17] = { "0" };
        uint64_t _resultcount = 0;
        int _iterationCount = 0;

        void ResetCurrentPage()
        {
            _currentPageValue = 1;
            strncpy(_currentPageText, "1", 2);
        }

        template <typename dataType> uint64_t SetUpAndIterate(dataType valKnown = 0, dataType valKnownSecondary = 0)
        {
            uint64_t offset = _rangeStartValue - MungPlex::Connection::GetRegions()[_currentRegionSelect].Base;
            void* baseAddressEx = MungPlex::Connection::GetRegions()[_currentRegionSelect].BaseLocationProcess;
            uint64_t size = _rangeEndValue - _rangeStartValue + 1;
            const int addressWidth = *Connection::GetAddressWidth();
            const bool isWideAddress = addressWidth > 4;
            bool isKnown = _currentComparisionTypeSelect == 1;

            if (isWideAddress)
            {
                Xertz::MemCompare<dataType, uint64_t>::SetUp(Connection::GetCurrentPID(), _dir, _cached, Connection::IsBE(), _alignmentValue);
                return Xertz::MemCompare<dataType, uint64_t>::Iterate(baseAddressEx, size, _currentConditionTypeSelect, isKnown, _precision/100.0f, valKnown, valKnownSecondary);
            }
            else
            {
                Xertz::MemCompare<dataType, uint32_t>::SetUp(Connection::GetCurrentPID(), _dir, _cached, Connection::IsBE(), _alignmentValue);
                return Xertz::MemCompare<dataType, uint32_t>::Iterate(baseAddressEx, size, _currentConditionTypeSelect, isKnown, _precision/100.0f, valKnown, valKnownSecondary);
            }
        }

        template<typename uType, typename addressType> bool PokeArray()
        {
            uint64_t itemCount = OperativeArray<uType>(std::string(_knownValueText)).ItemCount();
            int pid = Connection::GetCurrentPID();
            std::string arrayString(_pokeValueText);
            OperativeArray<uType> pokeArray(arrayString);
               
            if (_multiPoke)
            {
                int regionIndex;
                auto results = Xertz::MemCompare<OperativeArray<uType>, addressType>::GetResults();
                uint64_t resultIndex = (_currentPageValue - 1) * _maxResultsPerPage;

                for (int index = 0; index < _selectedIndices.size(); ++index)
                {
                    if (_selectedIndices[index] == false)
                        continue;

                    uint64_t address = *(results->at(_iterationCount - 1)->GetResultOffsets() + resultIndex + index) + _regions[_currentRegionSelect].Base;

                    for (int i = 0; i < _regions.size(); ++i)
                    {
                        if (address >= _regions[i].Base && address <= _regions[i].Base + _regions[i].Size)
                        {
                            regionIndex = i;
                            break;
                        }
                        return false;
                    }

                    if (_pokePrevious)
                    {
                        uType* arr = (uType*)(results->at(_iterationCount - 1)->GetResultPreviousValues() + resultIndex + index);
                        pokeArray = OperativeArray<uType>(arr, itemCount);
                    }
                    
                    
                    if (Connection::IsBE() && (index == 0 || _pokePrevious))
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
            else
            {
                if (Connection::IsBE())
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
            }
            return false;
        }

        template<typename dataType, typename addressType> bool PokeValue()
        {
            int pid = Connection::GetCurrentPID();
            if (_multiPoke)
            {
                int regionIndex;
                auto results = Xertz::MemCompare<dataType, addressType>::GetResults();
                uint64_t resultIndex = (_currentPageValue - 1) * _maxResultsPerPage;

                for (int index = 0; index < _selectedIndices.size(); ++index)
                {
                    if (_selectedIndices[index] == false)
                        continue;

                    uint64_t address = *(results->at(_iterationCount - 1)->GetResultOffsets() + resultIndex + index) + _regions[_currentRegionSelect].Base;

                    for (int i = 0; i < _regions.size(); ++i)
                    {
                        if (address >= _regions[i].Base && address <= _regions[i].Base + _regions[i].Size)
                        {
                            regionIndex = i;
                            break;
                        }
                        return false;
                    }

                    if (_pokePrevious)
                        *(dataType*)_pokeValue = *(results->at(_iterationCount - 1)->GetResultPreviousValues() + resultIndex + index);

                    if (Connection::IsBE() && (index == 0 || _pokePrevious))
                        *(dataType*)_pokeValue = Xertz::SwapBytes<dataType>(*(dataType*)_pokeValue);

                    address -= _regions[regionIndex].Base;
                    address += reinterpret_cast<uint64_t>(_regions[regionIndex].BaseLocationProcess);
                    Xertz::SystemInfo::GetProcessInfo(pid).WriteExRAM(_pokeValue, reinterpret_cast<void*>(address), sizeof(dataType));
                }

                return true;
            }
            else
            {
                if (Connection::IsBE())
                    *(dataType*)_pokeValue = Xertz::SwapBytes<dataType>(*(dataType*)_pokeValue);

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

            if (!_cached && _iterationCount > 0)
            {
                if (_iterationCount > 1 && results->at(_iterationCount - 2)->HasResults())
                    results->at(_iterationCount - 2)->FreeData(false);

                if (!results->at(_iterationCount - 1)->HasResults())
                    if (!results->at(_iterationCount - 1)->LoadResults(false))
                        return;
            }
                    
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

            for (int row = 0; row < _resultCount; ++row)
            {
                if (row >= _maxResultsPerPage || row >= _resultCount || (_currentPageValue >= _pagesAmountValue && row >= _resultCount % _maxResultsPerPage))
                    break;

                static char tempAddress[1024];
                static char tempValue[1024];
                static char buf[1024];
                int addressTextWidth = *Connection::GetAddressWidth() > 4 ? 16 : 8;
                uint64_t pageIndex = (_currentPageValue-1) * _maxResultsPerPage;
                uint64_t address = *(results->at(_iterationCount - 1)->GetResultOffsets() + pageIndex + row) + _regions[_currentRegionSelect].Base;
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
                            auto currentValue = *(results->at(_iterationCount - 1)->GetResultValues() + resultsIndex);
                            auto previousValue = *(results->at(_iterationCount - 1)->GetResultPreviousValues() + resultsIndex);
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
                    }
                    else
                    {
                        sprintf(buf, addressTextWidth == 16 ? "%016X" : "%08X", address);
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

        template<typename T> void DrawArrayValues(int col, auto results, uint64_t itemCount, uint64_t resultIndexWithItemCount, char* buf, char* tempValue, const char* literal)
        {
            T* value;
            const bool copyTempValue = (col == 1 && !_pokePrevious) || (col == 2 && _pokePrevious);

            if(col == 1)
                value = (T*)results->at(_iterationCount - 1)->GetResultValues() + resultIndexWithItemCount;
            else if (col ==  2)
                value = (T*)results->at(_iterationCount - 1)->GetResultPreviousValues() + resultIndexWithItemCount;
            else
            {
                sprintf(buf, "");
                return;
            }

            PrintTableArray<T>(buf, literal, itemCount, value);

            if(copyTempValue)
                std::memcpy(tempValue, buf, 1024);
        }

        template<typename T> void PrintTableArray(char* buf, const char* literal, uint32_t itemCount, T* vals)
        {
            std::strcpy(buf, "");
            char temp[18];

            for (uint32_t i = 0; i < itemCount; ++i)
            {
                sprintf(temp, literal, vals[i]);
                if(_hex)
                    std::strcat(buf, "0x");
                std::strcat(buf, temp);
                if(i < itemCount-1)
                    std::strcat(buf, ", ");
            }

            std::puts(buf);
        }

    public:
        static void DrawWindow();
        static void Refresh();
        MungPlex::SignalCombo<MungPlex::SystemRegion> _RegionSelectSignalCombo;
        MungPlex::SignalInputText _SignalInputTextRangeStart;
        MungPlex::SignalInputText _SignalInputTextRangeEnd;

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
            std::string hexBegStr = stream.str();
            std::strcpy(GetInstance()._rangeStartText, hexBegStr.c_str());
            std::string hexEndStr = MungPlex::ToHexString(GetInstance()._regions[GetInstance()._currentRegionSelect].Base + GetInstance()._regions[GetInstance()._currentRegionSelect].Size -1, 0).c_str();
            std::strcpy(GetInstance()._rangeEndText, hexEndStr.c_str());
        };
    };
}