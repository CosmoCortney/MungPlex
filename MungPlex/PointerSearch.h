#pragma once

#include <string>
#include <Windows.h>
#include <string>
#include "HelperFunctions.h"

namespace MungPlex
{
    class PointerSearch
    {
    public:
        static void DrawWindow();
        static void SelectPreset(int presetIndex);

        PointerSearch();
        PointerSearch(const PointerSearch&) = delete;
        PointerSearch(PointerSearch&&) = delete;
        void operator=(const PointerSearch&) = delete;
        void operator=(PointerSearch&&) = delete;
        static PointerSearch& GetInstance()
        {
            static PointerSearch Instance;
            return Instance;
        }

    private:
        void drawList();
        void drawSettings();
        void drawResults();
        bool performScan();
        void generateArgument();
        void waitAndLoadResults(PROCESS_INFORMATION pi);
        bool loadResults();
        static bool comparePairs(std::pair<std::string, std::array<uint64_t, 4>>& a, std::pair<std::string, std::array<uint64_t, 4>>& b);

        std::string _arg;
        std::string _defaultPath;
        std::string _results;
        std::vector<std::pair<std::string, std::array<uint64_t, 4>>> _memDumps{}; //0: starting address, 1: target address, 2: reserved, 3: correspondence
        std::vector<std::string> _bufStartingAddress{};
        std::vector<std::string> _bufTargetAddress{};
        bool _isBigEndian = false;
        std::string _minOffset;
        std::string _maxOffset;
        std::string _resultsPath;
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
        std::vector<SystemRegion> _regions{};
        int _regionSelect = 0;
    };
}