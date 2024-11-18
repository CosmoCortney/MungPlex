#include "ImGuiFileDialog.h"
#include "../../Log.hpp"
#include "PointerSearch.hpp"
#include "Settings.hpp"

inline const MungPlex::StringIdBoolPairs MungPlex::PointerSearch::_systemPresets =
{
    { "NES", "SNES", "N64", "GCN", "Triforce", "RVL", "Cafe", "Switch", "GB", "GBC", "GBA", "NDS", "CTR", 
      "PS1", "PS2", "PS3", "PS4", "PS5", "PSP", "PSV", 
      "SMS", "Genesis", "32X", "Mega CD", "Saturn", "Dreamcast", "GG", 
      "XBOX", "360", "XBone", "Series", "PC x86", "PC x64" },
    { 1, 1, 2, 2, 2, 2, 2, 3, 1, 1, 2, 2, 2,
      2, 2, 2, 3, 3, 2, 2,
      1, 1, 1, 1, 2, 2, 1,
      2, 2, 3, 3, 2, 3 },
    { false, false, true, true, true, true, true, false, false, false, false, false, false,
      false, false, true, false, false, false, false,
      false, true, true, true, true, false, false,
      false, true, false, false, false, false },
    "System Preset:"
};



MungPlex::PointerSearch::PointerSearch()
{
    _defaultPath = Settings::GetGeneralSettings().DocumentsPath;
    std::ranges::replace(_defaultPath, '\\', '/');
    _defaultPath.append("/MungPlex/Dumps/");
    _maxOffset = "1000";
    _maxOffset.resize(17);
    _resultsPath = std::string(Settings::GetGeneralSettings().DocumentsPath) + R"(\MungPlex\PointerSearch\Pointers.txt)";
    _resultsPath.resize(512);
}

void MungPlex::PointerSearch::DrawWindow()
{
    static bool stateSet = false;

    if (ImGui::Begin("Pointer Search"))
    {
	    const bool waitForDisable = GetInstance()._disableUI;
        if (waitForDisable) ImGui::BeginDisabled();
        {
            if (Connection::IsConnected() && Settings::GetGeneralSettings().EnableRichPresence && !stateSet)
            {
                Connection::GetDiscordRichPresence().SetRichPresenceState("Scanning for Pointers");
                stateSet = true;
            }

            GetInstance().drawList();
            GetInstance().drawSettings();
            ImGui::SameLine();
            GetInstance().drawResults();
        }
        if (waitForDisable) ImGui::EndDisabled();
    }
    else
        stateSet = false;

	ImGui::End();
}

void MungPlex::PointerSearch::drawSettings()
{
    ImVec2 childXY = ImGui::GetContentRegionAvail();
    childXY.x *= 0.4f;

    ImGui::BeginChild("PointerSearchSettings", childXY);
    {
        if (SetUpPairCombo(_systemPresets, &_presetSelect, 1.0f, 0.3f))
        {
            SelectPreset(_presetSelect);
        }

        ImGui::Checkbox("Big Endian", &_isBigEndian); ImGui::SameLine(); HelpMarker("Whether the memory dumps to be scanned are big endian or not.");
        ImGui::SameLine();
        ImGui::Checkbox("Print Visited Addresses", &_printVisitedAddresses); ImGui::SameLine(); HelpMarker("Additionally print visited addresses.");
        ImGui::SameLine();
        ImGui::Checkbox("Print Module Names", &_printModuleNames); ImGui::SameLine(); HelpMarker("Whether to print file/module name instead of initial address.");
        SetUpInputText("Minimum Offset:", _minOffset.data(), _minOffset.size(), 1.0f, 0.3f, true, "Smallest offset value to be considered. Negative values allowed. A lower value may increase results count but also the scan time.");
    	SetUpInputText("Maximum Offset:", _maxOffset.data(), _maxOffset.size(), 1.0f, 0.3f, true, "Biggest offset value to be considered. A bigger value may increase results count but also increase scan time.");

        if (SetUpInputInt("Min. Pointer Depth:", &_minPointerDepth, 1, 1, 1.0f, 0.3f, 0, true, "Minimum pointer depth level. A value of 1 means a single pointer redirection is considered. Values bigger than 1 mean that pointers may redirect to other pointers. This value is usually always 1."))
        {
            if (_minPointerDepth < 1)
                _minPointerDepth = 1;

            if (_minPointerDepth > _maxPointerDepth)
                _minPointerDepth = _maxPointerDepth;
        }

        if (SetUpInputInt("Max. Pointer Depth:", &_maxPointerDepth, 1, 1, 1.0f, 0.3f, 0, true, "Maximum pointer depth level. A value of 1 means a single pointer redirection is considered. Values bigger than 1 mean that pointers may redirect to other pointers. This value can be the same as \"Minimum Pointer Depth\" if you don't want any extra depth. A higher value will increase the results count but also scan time."))
        {
            if (_maxPointerDepth > 4)
                _maxPointerDepth = 4;

            if (_maxPointerDepth < _minPointerDepth)
                _maxPointerDepth = _minPointerDepth;
        }

        static const StringIdPairs addressWidthSelect = { { "1 Byte", "2 Bytes", "4 Bytes", "8 Bytes" }, { 0, 1, 2, 3 }, "Address Width:" };

        if (SetUpPairCombo(addressWidthSelect, &_addressWidthIndex, 1.0f, 0.3f, true, "Address width of the dump's system."))
            _addressWidth = 1 << _addressWidthIndex;

        SetUpInputText("Results File:", _resultsPath.data(), _resultsPath.size(), 1.0f, 0.3f, true, "Where to save the results file.");
        SetUpSliderFloat("Max. Memory Utilization Fraction:", &_maxMemUtilizationFraction, 0.1f, 0.95f, "%2f", 1.0f, 0.5f);
        SetUpInputInt("Max. Pointer Count:", &_maxPointerCount, 100, 1000, 1.0f, 0.3f, 0, true, "Maximum amount of pointers to be generated. Smaller values may decrease scan time and but also the likeability to find working pointer paths.");
        SetUpPairCombo(_inputTypeSelect, &_selectedInputType, 1.0f, 0.3f);

        if (ImGui::Button("Add File"))
        {
            IGFD::FileDialogConfig config;
            config.path = GetInstance()._defaultPath;
            ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Add File", ".bin,.raw,.dmp,.pointermap", config);
        }

        if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
        {
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                const std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                _memDumps.emplace_back(filePathName, std::array<uint64_t, 4>());
                _memDumps.back().first.resize(512);
                _bufStartingAddress.emplace_back("0");
                _bufStartingAddress.back().resize(17);
                _bufTargetAddress.emplace_back("0");
                _bufTargetAddress.back().resize(17);
            }

            ImGuiFileDialog::Instance()->Close();
        }

        ImGui::SameLine();

        if (ImGui::Button("Clear List"))
        {
            Log::LogInformation("Pointer list cleared");
            _memDumps.clear();
        }

        ImGui::SameLine();

        if (ImGui::Button("Scan"))
        {
            Log::LogInformation("Pointer scan started");
            generateArgument();
            performScan();
        }

        ImGui::Dummy(ImVec2(0.0f, ImGui::GetContentRegionAvail().y - 40.0f));

        _regions = ProcessInformation::GetSystemRegionList();
        SetUpPairCombo(ProcessInformation::GetSystemRegionList_(), &_regionSelect, 0.5f, 0.4f);
        
        ImGui::SameLine();
        
        if (ImGui::Button("Dump"))
        {
            std::stringstream stream(std::string(Settings::GetGeneralSettings().DocumentsPath), std::ios_base::app | std::ios_base::out);
            stream << R"(\MungPlex\Dumps\)";
            stream << ProcessInformation::GetPlatform() << '\\';
            stream << ProcessInformation::GetGameID() << '\\';
            auto& [Label, Base, Size, BaseLocationProcess] = _regions[_regionSelect];
            stream << std::uppercase << std::hex << Base << '_' << GetCurrentTimeString(YEAR | MONTH | DAY | HOUR | MINUTE | SECOND) << ".bin";
            std::wstring path = MT::Convert<std::string, std::wstring>(stream.str(), MT::UTF8, MT::UTF16LE);
            Xertz::SystemInfo::GetProcessInfo(ProcessInformation::GetPID()).DumpMemory(BaseLocationProcess, path, Size);
        }
    }
    ImGui::EndChild();
}


void MungPlex::PointerSearch::drawList()
{
	const ImVec2 childXY = { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y * 0.5f };

    ImGui::BeginChild("child_MemDumpList", childXY, true);
    {
        static std::string label("dumpfile");
        static ImGuiSelectableFlags selectableFlags = ImGuiSelectableFlags_SpanAllColumns;
        static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

        if (ImGui::BeginTable("Memory Dumps", 4, flags, ImGui::GetContentRegionAvail()))
        {

            ImGui::TableSetupColumn("Dump File");
            ImGui::TableSetupColumn("Starting Address");
            ImGui::TableSetupColumn("Target Address");
            ImGui::TableSetupColumn("Correspondence (0 = initial, 1 = 1st comp, 2 = 2nd comp, ...)");
            //ImGui::TableSetColumnWidth(0, ImGui::GetContentRegionAvail().x * 0.5f);
            ImGui::TableHeadersRow();
            
            for (size_t row = 0; row < _memDumps.size(); ++row)
            {
                ImGui::TableNextRow();

                for (int col = 0; col < 4; ++col)
                {
                    ImGui::TableSetColumnIndex(col);

                    switch (col)
                    {
	                    case 0:
	                        if (SetUpInputText(label + std::to_string(row), _memDumps[row].first.data(), _memDumps[row].first.size(), 1.0f, 0.0f, false))
	                        {
	                        }
	                        break;
	                    case 1:
	                        if (SetUpInputText(label + "s" + std::to_string(row), _bufStartingAddress[row].data(), _bufStartingAddress[row].size(), 1.0f, 0.0f, false))
	                        {
	                            std::stringstream stream;
	                            stream << std::hex << _bufStartingAddress[row];
	                            stream >> _memDumps[row].second[0];
	                        }
	                        break;
	                    case 2:
	                        if (SetUpInputText(label + "t" + std::to_string(row), _bufTargetAddress[row].data(), _bufTargetAddress[row].size(), 1.0f, 0.0f, false))
	                        {
	                            std::stringstream stream;
	                            stream << std::hex << _bufTargetAddress[row];
	                            stream >> _memDumps[row].second[1];
	                        }
	                        break;
	                    case 3: 
	                        if (SetUpInputInt(label + "correspondence" + std::to_string(row), reinterpret_cast<int*>(&_memDumps[row].second[3]), 1, 1, 1.0f, 0.0f, 0, false))
	                        {
	                            if (static_cast<int>(_memDumps[row].second[3]) < 0)
	                                _memDumps[row].second[3] = 0;
	                        }
	                        break;
                    }
                }
            }
            
        ImGui::EndTable();

        }
    }
    ImGui::EndChild();
}

void MungPlex::PointerSearch::drawResults()
{
    ImGui::BeginChild("PointerSearchResults");
    {
        SetUpInputTextMultiline("Results", _results.data(), _results.size(), 1.0f, 0.925f, ImGuiInputTextFlags_ReadOnly);
    }
    ImGui::EndChild();
}

std::string MungPlex::PointerSearch::execute_external_process(const std::string& command, const std::vector<std::string>& arguments) 
{
    boost::asio::io_context io_context;
    boost::process::ipstream output_stream;
    std::ostringstream result;
    std::future<void> read_output;

    boost::process::child process(command, boost::process::args(arguments),
        (boost::process::std_out & boost::process::std_err) > output_stream, io_context);

    // Read output asynchronously
    read_output = std::async(std::launch::async, [&] {
        std::string line;
        while (output_stream && std::getline(output_stream, line) && !line.empty()) {
            result << line << '\n';
        }
    });

    // Run the I/O context in a separate thread
    std::thread io_thread([&io_context] {
        io_context.run();
    });

    _disableUI = true;
    process.wait();
    _disableUI = false;
    read_output.get();
    io_thread.join();

    if (process.exit_code() != 0) {
        const auto error_message = "Command failed with return value "
            + std::to_string(process.exit_code()) + ": " + result.str();
        throw std::runtime_error(error_message);
    }

    return result.str();
}

bool MungPlex::PointerSearch::performScan()
{
    const auto pointerSearcherFilePath = GetResourcesFilePath(R"(Universal-Pointer-Searcher-Engine\UniversalPointerSearcher.exe)");
    
    try
    {
        const auto process_output = execute_external_process(pointerSearcherFilePath.string(), _args);
        Log::LogInformation("Finished Pointer Scan");
        loadResults();
        return true;
    }
    catch (const std::exception& ex)
    {
        std::string errorMsg = "Pointer searcher failed: " + std::string(ex.what());
#ifndef NDEBUG
        std::cerr << errorMsg << '\n';
#endif
        Log::LogInformation(errorMsg);
        return false;
    }
}

void MungPlex::PointerSearch::waitAndLoadResults()
{
    _disableUI = true;
    _pointerSearcherProcess->wait();
    _pointerSearcherProcess.reset();
    Log::LogInformation("Finished Pointer Scan");
    loadResults();
    _disableUI = false;
}

// TODO Maybe his helper method is redundant if it already exists somewhere else
void remove_trailing_nulls(std::string& str) {
    // Find the position of the last character that is not a null character
    if (const std::size_t end_pos = str.find_last_not_of('\0');
        end_pos != std::string::npos) {
        // Erase characters after the last non-null character
        str.erase(end_pos + 1);
    }
    else {
        // If the entire string is null characters, clear it
        str.clear();
    }
}

void MungPlex::PointerSearch::generateArgument() // TODO Implement the missing flags
{
    _args.clear();
    auto memDumpsSorted = _memDumps; //this is needed because sorting _memDumps messes up the table
    std::ranges::sort(memDumpsSorted, comparePairs);

    auto resultsPath = _resultsPath;
    remove_trailing_nulls(resultsPath);

    auto minOffset = _minOffset;
    remove_trailing_nulls(minOffset);

    auto maxOffset = _maxOffset;
    remove_trailing_nulls(maxOffset);

    std::stringstream stream;

    if (!memDumpsSorted.empty())
    {
        int lastCorrespondence = 1;
        bool initStartingAddrFlagRequired = true;
        bool compStartingAddrFlagRequired = true;
        for (auto [first, second] : memDumpsSorted)
        {
            const auto starting_address = second[0];

            if (second[3] == 0) // 0 correspondence (aka initial)
            {
                if (initStartingAddrFlagRequired)
                {
                    _args.emplace_back("--initial-starting-address");
                    initStartingAddrFlagRequired = false;
                }
            }
            else
            {
                if (compStartingAddrFlagRequired)
                {
                    _args.emplace_back("--comparison-starting-address");
                    compStartingAddrFlagRequired = false;
                }
            }
            
            if (second[3] != lastCorrespondence && second[3] > 1)
                _args.push_back("%%");

            _args.push_back(ToHexString(starting_address, true, true));
            lastCorrespondence = second[3];
        }

        lastCorrespondence = -1;

        _args.emplace_back("--target-address");
        for (auto [first, second] : memDumpsSorted)
        {
            if (lastCorrespondence == second[3])
                continue;

            const auto target_address = second[1];
            _args.push_back(ToHexString(target_address, true, true));
            lastCorrespondence = second[3];
        }

        lastCorrespondence = 1;

        bool initPathFlagRequired = true;
        bool compPathFlagRequired = true;
        for (auto [first, second] : memDumpsSorted)
        {
            if (second[3] == 0)
            {
                if (initPathFlagRequired)
                {
                    _args.emplace_back("--initial-file-path");
                    initPathFlagRequired = false;
                }
            }
            else
            {
                if (compPathFlagRequired)
                {
                    _args.emplace_back("--comparison-file-path");
                    compPathFlagRequired = false;
                }
            }

            if (second[3] != lastCorrespondence && second[3] > 1)
                _args.push_back("%%");

            auto first_copy = first;
            remove_trailing_nulls(first_copy);
            _args.push_back(first_copy);
            lastCorrespondence = second[3];
        }
    }

    _args.emplace_back("--verbose");
    _args.emplace_back("--endian");
    _args.emplace_back(_isBigEndian ? "big" : "little");
    _args.emplace_back("--address-size");
    _args.push_back(std::to_string(_addressWidth));
    _args.emplace_back("--pointer-offset-range");
    _args.push_back(minOffset + "," + maxOffset);
	_args.emplace_back("--pointer-depth-range");
    _args.push_back(std::to_string(_minPointerDepth) + "," + std::to_string(_maxPointerDepth));
    _args.emplace_back("--store-memory-pointers-file-path");
    _args.push_back(resultsPath);
    _args.emplace_back("--maximum-memory-utilization-fraction");
    _args.push_back(std::to_string(_maxMemUtilizationFraction));
    _args.emplace_back("--file-extensions");
    _args.emplace_back(".bin");
    _args.emplace_back(".raw");
    _args.emplace_back(".dmp");
    _args.emplace_back("--maximum-pointer-count");
    _args.push_back(std::to_string(_maxPointerCount));
    _args.emplace_back("--maximum-pointers-printed-count");
    _args.push_back(std::to_string(_maxPointerCount));
    _args.emplace_back("--disable-printing-memory-pointers-to-console");

    if(_printModuleNames)
        _args.emplace_back("--print-module-file-names");

    if(_printVisitedAddresses)
        _args.emplace_back("--print-visited-addresses");

    Log::LogInformation("Pointer scan arguments: ");

#ifndef NDEBUG
    for (const std::string& line : _args)
        std::cout << line << '\n';
#endif
    for (const std::string& line : _args)
        Log::LogInformation(line, true, 2);
}

void MungPlex::PointerSearch::SelectPreset(const int presetIndex)
{
    GetInstance()._presetSelect = presetIndex;
    GetInstance()._addressWidthIndex = _systemPresets.GetId(presetIndex);
    GetInstance()._isBigEndian = _systemPresets.GetFlag(presetIndex);
}

bool MungPlex::PointerSearch::loadResults()
{
    std::ifstream resultsFile;

    resultsFile.open(_resultsPath);
    if (!resultsFile) {
        Log::LogInformation("Error loading pointer results");
        return false;
    }

    std::stringstream buffer;
    buffer << resultsFile.rdbuf();
    _results = buffer.str();
    Log::LogInformation("Pointer Scan results loaded");
    return !_results.empty();
}

bool MungPlex::PointerSearch::comparePairs(const std::pair<std::string, std::array<uint64_t, 4>>& a,
                                           const std::pair<std::string, std::array<uint64_t, 4>>& b)
{
    if (a.second[3] != b.second[3])
        return a.second[3] < b.second[3];

    return a.second[0] < b.second[0];
}