#pragma once

#include <string>
#include <Windows.h>
#include <string>
#include "HelperFunctions.h"

namespace MungPlex
{
    class PointerSearch
    {
        PointerSearch();
        ~PointerSearch()
        {
	        delete[] _results;

            for(int i = 0; i < _memDumps.size(); ++i)
            {
                delete[] _memDumps[i].first;
                delete[] _bufStartingAddress[i];
                delete[] _bufTargetAddress[i];
            }
        }
        PointerSearch(const PointerSearch&) = delete;
        PointerSearch(PointerSearch&&) = delete;
        void operator=(const PointerSearch&) = delete;
        void operator=(PointerSearch&&) = delete;
        static PointerSearch& GetInstance()
        {
            static PointerSearch Instance;
            return Instance;
        }

        void drawList();
        void drawSettings();
        void drawResults();
        bool performScan();
        void generateArgument();
        void waitAndLoadResults(PROCESS_INFORMATION pi);
        bool loadResults();

        std::string _arg;
        std::string _defaultPath;
        char* _results = nullptr;
        std::vector<std::pair<char*, std::array<uint64_t, 4>>> _memDumps{};
        std::vector<char*> _bufStartingAddress{};
        std::vector<char*> _bufTargetAddress{};
        bool _isBigEndian = false;
        char* _minOffset = nullptr;
        char* _maxOffset = nullptr;
        // TODO Always use std::string instead of char*, it's possible to pre-allocate std::strings as well!
        char* _resultsPath = nullptr;
        int _minPointerDepth = 1;
        int _maxPointerDepth = 1;
        int _addressWidth = 4;
        int _addressWidthIndex = 2;
        bool _printVisitedAddresses = false;
        bool _printModuleNames = false;
        float _maxMemUtilizationFraction = 0.9f;
        int _maxPointerCount = 100000;
        std::vector<std::tuple< std::string, int, bool>> _systemPresets{};
        int _presetSelect = 0;
        int _selectedInputType = 0;
        const std::vector<std::string> _inputTypeSelect = { "Memory Dump", "Pointer Map" };

    public:
        static void DrawWindow();
        static void SelectPreset(int presetIndex);
    };
}