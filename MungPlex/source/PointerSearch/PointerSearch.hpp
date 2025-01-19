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

        static void DrawWindow();
        static void SelectPreset(int presetIndex);
        static void SetMemoryRegions(const std::vector<SystemRegion>& regions);

    private:
        void drawList();
        void drawSettings();
        void drawResults();
        bool performScan();
        void generateArgument();
        void waitAndLoadResults();
        bool loadResults();
        std::string execute_external_process(const std::string& command, const std::vector<std::string>& arguments);
        static bool comparePairs(const std::pair<InputText, std::array<InputInt<uint64_t>, 4>>& a, const std::pair<InputText, std::array<InputInt<uint64_t>, 4>>& b);

        std::vector<std::string> _args;
        std::string _defaultPath;
        InputTextMultiline _resultsInput = InputTextMultiline("Results:", true, "", 0, ImGuiInputTextFlags_ReadOnly);
        std::vector<std::pair<InputText, std::array<InputInt<uint64_t>, 4>>> _memDumps{}; //0: starting address, 1: target address, 2: reserved, 3: correspondence
        bool _isBigEndian = false;
        InputText _minOffsetInput = InputText("Min Offset:", true, "0", 16); //type Text required for preceeding minus
        InputText _maxOffsetInput = InputText("Max Offset:", true, "1000", 16); //type Text required for preceeding minus
        InputText _resultsPathInput = InputText("Results File Path:", true, "", 512);
        InputInt<uint32_t> _minPointerDepthInput = InputInt<uint32_t>("Min. Pointer Depth:", true, 1, 1, 1);
        InputInt<uint32_t> _maxPointerDepthInput = InputInt<uint32_t>("Max. Pointer Depth:", true, 1, 1, 1);
        bool _printVisitedAddresses = false;
        bool _printModuleNames = false;
		Slider<float> _maxMemUtilizationFraction = Slider<float>("Max. Memory Utilization Fraction:", false, 0.95f, 0.1f, 1.0f, ImGuiSliderFlags_None);
        InputInt<uint64_t> _maxPointerCountInput = InputInt<uint64_t>("Max. Pointer Count:", true, 100000, 1, 20);
        static const std::vector<StringIdBoolCombo::VecType> _systemPresets;
        StringIdBoolCombo _systemPresetSelectCombo = StringIdBoolCombo("System Preset:", true, _systemPresets);
        static const std::vector<StringIdCombo::VecType> _addressWidthTypes;
        StringIdCombo _addressWidthSelectCombo = StringIdCombo("Address Width:", true, _addressWidthTypes);
        int _selectedInputType = 0;
        static const std::vector<StringIdCombo::VecType> _inputFileTypes;
        StringIdCombo _inputFileTypeSelectCombo = StringIdCombo("Input File Type:", true, _inputFileTypes);
        RegionCombo _regionSelectCombo = RegionCombo("Region:", true);
        bool _disableUI = false;
        // TODO This can probably be removed
        std::shared_ptr<bp::child> _pointerSearcherProcess;
        //std::shared_ptr<bp::ipstream> _pointerSearcherLog;
        //std::shared_ptr<bp::ipstream> _pointerSearcherErrorLog;
    };
}