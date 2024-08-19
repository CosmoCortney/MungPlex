#pragma once
#include <boost/asio.hpp>
#include <boost/process.hpp>
#include <memory>
#include "ProcessInformation.hpp"
#include <string>

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
        static bool comparePairs(const std::pair<std::string, std::array<uint64_t, 4>>& a,
            const std::pair<std::string, std::array<uint64_t, 4>>& b);

        std::vector<std::string> _args;
        std::string _defaultPath;
        std::string _results;
        std::vector<std::pair<std::string, std::array<uint64_t, 4>>> _memDumps{}; //0: starting address, 1: target address, 2: reserved, 3: correspondence
        std::vector<std::string> _bufStartingAddress{};
        std::vector<std::string> _bufTargetAddress{};
        bool _isBigEndian = false;
        std::string _minOffset = std::string(17, '\0');
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
        const std::vector<std::tuple< std::string, int, bool>> _systemPresets
        {
            { "Nintendo Entertainment Systen / Famicom", 1, false },
            { "Super Nintendo Entertainment Systen / Super Famicom", 1, false },
            { "Nintendo 64", 2, true },
            { "Nintendo GameCube", 2, true },
            { "Triforce Arcade", 2, true },
            { "Nintendo Wii", 2, true },
            { "Nintendo Wii U", 2, true },
            { "Nintendo Switch", 3, false },
            { "Nintendo GameBoy", 1, false },
            { "Nintendo GameBoy Color", 1, false },
            { "Nintendo GameBoy Advance", 2, false },
            { "Nintendo DS", 2, false },
            { "Nintendo 3DS", 2, false },
            { "Sony Playstation", 2, false },
            { "Sony Playstation 2", 2, false },
            { "Sony Playstation 3", 2, true },
            { "Sony Playstation 4", 3, false },
            { "Sony Playstation 5", 3, false },
            { "Sony Playstation Portable", 2, false },
            { "Sony Playstation Vita", 2, false },
            { "Sega Master System", 1, false },
            { "Sega Mega Drive / Genesis", 1, true },
            { "Sega Mega 32X", 1, true },
            { "Sega Mega CD", 1, true },
            { "Sega Saturn", 2, true },
            { "Sega Dreamcast", 2, false },
            { "Sega GameGear", 1, false },
            { "Microsoft XBOX", 2, false },
            { "Microsoft XBOX 360", 2, true },
            { "Microsoft XBOX One", 3, false },
            { "Microsoft XBOX Series", 3, false },
            { "Microsoft Windows x86", 2, false },
            { "Microsoft Windows x64", 3, false },
        };
        int _presetSelect = 0;
        int _selectedInputType = 0;
        const std::vector<std::string> _inputTypeSelect = { "Memory Dump", "Pointer Map" };
        std::vector<SystemRegion> _regions{};
        int _regionSelect = 0;
        bool _disableUI = false;
        // TODO This can probably be removed
        std::shared_ptr<bp::child> _pointerSearcherProcess;
        //std::shared_ptr<bp::ipstream> _pointerSearcherLog;
        //std::shared_ptr<bp::ipstream> _pointerSearcherErrorLog;
    };
}