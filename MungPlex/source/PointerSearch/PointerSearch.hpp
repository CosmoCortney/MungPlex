#pragma once
#include <boost/asio.hpp>
#include <boost/process.hpp>
#include <memory>
#include "ProcessInformation.hpp"
#include <string>
#include "WidgetHelpers.hpp"

namespace MungPlex
{
    namespace bp = boost::process;

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
        void waitAndLoadResults();
        bool loadResults();
        std::string execute_external_process(const std::string& command, const std::vector<std::string>& arguments);
        static bool comparePairs(const std::pair<InputText, std::array<uint64_t, 4>>& a, const std::pair<InputText, std::array<uint64_t, 4>>& b);

        std::vector<std::string> _args;
        std::string _defaultPath;
        InputTextMultiline _resultsInput = InputTextMultiline("Results:", true, "", 0, ImGuiInputTextFlags_ReadOnly);
        std::vector<std::pair<InputText, std::array<uint64_t, 4>>> _memDumps{}; //0: starting address, 1: target address, 2: reserved, 3: correspondence
        std::vector<InputText> _bufStartingAddress{};
        std::vector<InputText> _bufTargetAddress{};
        bool _isBigEndian = false;
        InputText _minOffsetInput = InputText("Min Offset:", true, "0", 16);
        InputText _maxOffsetInput = InputText("Max Offset:", true, "1000", 16);
        InputText _resultsPathInput = InputText("Results File Path:", true, "", 512);
        int _minPointerDepth = 1;
        int _maxPointerDepth = 1;
        int _addressWidth = 4;
        int _addressWidthIndex = 2;
        bool _printVisitedAddresses = false;
        bool _printModuleNames = false;
        float _maxMemUtilizationFraction = 0.9f;
        int _maxPointerCount = 100000;
        static const StringIdBoolPairs _systemPresets;
        int _presetSelect = 0;
        int _selectedInputType = 0;
        inline static const StringIdPairs _inputTypeSelect = { { "Memory Dump", "Pointer Map" }, { 0, 0}, "Input File Type:" };
        std::vector<SystemRegion> _regions{};
        int _regionSelect = 0;
        bool _disableUI = false;
        // TODO This can probably be removed
        std::shared_ptr<bp::child> _pointerSearcherProcess;
        //std::shared_ptr<bp::ipstream> _pointerSearcherLog;
        //std::shared_ptr<bp::ipstream> _pointerSearcherErrorLog;
    };
}