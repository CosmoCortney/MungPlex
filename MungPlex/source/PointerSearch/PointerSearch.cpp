#include "ImGuiFileDialog.h"
#include "../../Log.hpp"
#include "PointerSearch.hpp"
#include "Settings.hpp"

inline const std::vector<MungPlex::StringIdBoolCombo::VecType> MungPlex::PointerSearch::_systemPresets =
{
    {
        { "NES", 2, false },
        { "SNES", 2, false },
        { "N64", 4, true },
        { "GCN", 4, true },
        { "Triforce", 4, true },
        { "RVL", 4, true },
        { "Cafe", 4, true },
        { "Switch", 8, false },
        { "GB", 2, false },
        { "GBC", 2, false },
        { "GBA", 4, false },
        { "NDS", 4, false },
        { "CTR", 4, false },
        { "PS1", 4, false },
        { "PS2", 4, false },
        { "PS3", 4, true },
        { "PS4", 8, false },
        { "PS5", 8, false },
        { "PSP", 4, false },
        { "PSV", 4, false },
        { "Master System", 2, false },
        { "Genesis", 2, true },
        { "32X", 2, true },
        { "Mega CD", 2, true },
        { "Saturn", 4, true },
        { "DC", 4, false },
        { "GGr", 2, false },
        { "XBOX", 4, false },
        { "XBOX 360", 4, true },
        { "XBOX One", 8, false },
        { "Series", 8, false },
        { "x86", 4, false },
        { "x64", 8, false },
    }
};

inline const std::vector<MungPlex::StringIdCombo::VecType> MungPlex::PointerSearch::_addressWidthTypes =
{
    {
        { "1 Byte", 1 },
        { "2 Bytes", 2 },
        { "4 Bytes", 4 },
        { "8 Bytes", 8 }
    }
};

inline const std::vector<MungPlex::StringIdCombo::VecType> MungPlex::PointerSearch::_inputFileTypes =
{
    {
        { "Memory Dump", 0 },
        { "Pointer Map", 0 }
    }
};

MungPlex::PointerSearch::PointerSearch()
{
    _defaultPath = Settings::GetGeneralSettings().DocumentsPath.StdStrNoLeadinZeros();
    std::ranges::replace(_defaultPath, '\\', '/');
    _defaultPath.append("/MungPlex/Dumps/");
    _minOffsetInput.SetHelpText("Smallest offset value to be considered. Negative values allowed. A lower value may increase results count but also the scan time.");
    _maxOffsetInput.SetHelpText("Biggest offset value to be considered. A bigger value may increase results count but also increase scan time.");
    _resultsPathInput.SetText(Settings::GetGeneralSettings().DocumentsPath.StdStrNoLeadinZeros() + R"(\MungPlex\PointerSearch\Pointers.txt)");
    _resultsPathInput.SetHelpText("Where to save the results file.");
    _minPointerDepthInput.SetHelpText("Minimum pointer level depth. A value of 1 means a single pointer redirection is considered. Values bigger than 1 mean that pointers may redirect to other pointers. This value is usually always 1.", true);
    _maxPointerDepthInput.SetHelpText("Maximum pointer level depth. A value of 1 means a single pointer redirection is considered. Values bigger than 1 mean that pointers may redirect to other pointers. This value can be the same as \"Minimum Pointer Depth\" if you don't want any extra depth. A higher value will increase the results count but also scan time.", true);
    _maxPointerCountInput.SetHelpText("Maximum amount of pointers to be generated. Smaller values may decrease scan time and but also the likeability to find working pointer paths.", true);
    _addressWidthSelectCombo.SetHelpText("Address width of the dump's system.", true);
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
        if (_systemPresetSelectCombo.Draw(1.0f, 0.3f))
            SelectPreset(_systemPresetSelectCombo.GetSelectedIndex());

        ImGui::Checkbox("Big Endian", &_isBigEndian); ImGui::SameLine(); HelpMarker("Whether the memory dumps to be scanned are big endian or not.");
        ImGui::SameLine();
        ImGui::Checkbox("Print Visited Addresses", &_printVisitedAddresses); ImGui::SameLine(); HelpMarker("Additionally print visited addresses.");
        ImGui::SameLine();
        ImGui::Checkbox("Print Module Names", &_printModuleNames); ImGui::SameLine(); HelpMarker("Whether to print file/module name instead of initial address.");
        _minOffsetInput.Draw(1.0f, 0.3f);
    	_maxOffsetInput.Draw(1.0f, 0.3f);

        if (_minPointerDepthInput.Draw(1.0f, 0.3f))
        {
            if (_minPointerDepthInput.GetValue() < 1)
                _minPointerDepthInput.SetValue(1);

            if (_minPointerDepthInput.GetValue() > _maxPointerDepthInput.GetValue())
                _minPointerDepthInput.SetValue(_maxPointerDepthInput.GetValue());
        }

        if (_maxPointerDepthInput.Draw(1.0f, 0.3f))
        {
            if (_maxPointerDepthInput.GetValue() > 4)
                _maxPointerDepthInput.SetValue(4);

            if (_maxPointerDepthInput.GetValue() < _minPointerDepthInput.GetValue())
                _maxPointerDepthInput.SetValue(_minPointerDepthInput.GetValue());
        }

        _addressWidthSelectCombo.Draw(1.0f, 0.3f);
        _resultsPathInput.Draw(1.0f, 0.3f);
        SetUpSliderFloat("Max. Memory Utilization Fraction:", &_maxMemUtilizationFraction, 0.1f, 0.95f, "%2f", 1.0f, 0.5f);
        _maxPointerCountInput.Draw(1.0f, 0.3f);
        //_inputFileTypeSelectCombo.Draw(1.0f, 0.3f); todo

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
                _memDumps.emplace_back(InputText("path" + std::to_string(_memDumps.size()), false, filePathName, 512), std::array<InputInt<uint64_t>, 4>(
                {
                    InputInt<uint64_t>("starting" + std::to_string(_memDumps.size()), false, 0, 0, 0),
                    InputInt<uint64_t>("target" + std::to_string(_memDumps.size()), false, 0, 0, 0),
                    InputInt<uint64_t>("reserved" + std::to_string(_memDumps.size()), false, 0, 0, 0),
                    InputInt<uint64_t>("corres" + std::to_string(_memDumps.size()), false, 0, 1, 1) 
                }));
            }

            ImGuiFileDialog::Instance()->Close();
        }

        ImGui::SameLine();

        if (ImGui::Button("Clear List"))
        {
            _memDumps.clear();
            Log::LogInformation("Pointer list cleared");
        }

        ImGui::SameLine();

        if (ImGui::Button("Scan"))
        {
            Log::LogInformation("Pointer scan started");
            generateArgument();
            performScan();
        }

        ImGui::Dummy(ImVec2(0.0f, ImGui::GetContentRegionAvail().y - 40.0f));

        _regionSelectCombo.Draw(0.5f, 0.4f);

        ImGui::SameLine();
        
        if (ImGui::Button("Dump"))
        {
            std::stringstream stream(Settings::GetGeneralSettings().DocumentsPath.StdStrNoLeadinZeros(), std::ios_base::app | std::ios_base::out);
            stream << R"(\MungPlex\Dumps\)";
            stream << ProcessInformation::GetPlatform() << '\\';
            stream << ProcessInformation::GetGameID() << '\\';
            auto& [Label, Base, Size, BaseLocationProcess] = _regionSelectCombo.GetSelectedRegion();// _regions[_regionSelectCombo.GetSelectedIndex()];
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
                            _memDumps[row].first.Draw(1.0f, 0.0f);
	                        break;
	                    case 1:
                            _memDumps[row].second[0].Draw(1.0f, 0.0f, true);
	                        break;
	                    case 2:
                            _memDumps[row].second[1].Draw(1.0f, 0.0f, true);
	                        break;
	                    case 3: 
	                        if (_memDumps[row].second[3].Draw(1.0f, 0.0f))
	                        {
	                            if (_memDumps[row].second[3].GetValue() < 0)
	                                _memDumps[row].second[3].SetValue(0);
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
        _resultsInput.Draw(1.0f, 0.925f);
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

void MungPlex::PointerSearch::generateArgument() // TODO Implement the missing flags
{
    _args.clear();
    auto memDumpsSorted = _memDumps; //this is needed because sorting _memDumps messes up the table
    std::ranges::sort(memDumpsSorted, comparePairs);
    std::stringstream stream;

    if (!memDumpsSorted.empty())
    {
        int lastCorrespondence = 1;
        bool initStartingAddrFlagRequired = true;
        bool compStartingAddrFlagRequired = true;
        for (auto [first, second] : memDumpsSorted)
        {
            const auto& starting_address = second[0];

            if (second[3].GetValue() == 0) // 0 correspondence (aka initial)
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
            
            if (second[3].GetValue() != lastCorrespondence && second[3].GetValue() > 1)
                _args.push_back("%%");

            _args.push_back(ToHexString(starting_address.GetValue(), true, true));
            lastCorrespondence = second[3].GetValue();
        }

        lastCorrespondence = -1;

        _args.emplace_back("--target-address");
        for (auto [first, second] : memDumpsSorted)
        {
            if (lastCorrespondence == second[3].GetValue())
                continue;

            const auto target_address = second[1];
            _args.push_back(ToHexString(target_address.GetValue(), true, true));
            lastCorrespondence = second[3].GetValue();
        }

        lastCorrespondence = 1;

        bool initPathFlagRequired = true;
        bool compPathFlagRequired = true;
        for (auto [first, second] : memDumpsSorted)
        {
            if (second[3].GetValue() == 0)
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

            if (second[3].GetValue() != lastCorrespondence && second[3].GetValue() > 1)
                _args.push_back("%%");

            std::string first_copy = first.GetCString();
            _args.push_back(first_copy);
            lastCorrespondence = second[3].GetValue();
        }
    }

    _args.emplace_back("--verbose");
    _args.emplace_back("--endian");
    _args.emplace_back(_isBigEndian ? "big" : "little");
    _args.emplace_back("--address-size");
    _args.push_back(std::to_string(_addressWidthSelectCombo.GetSelectedId()));
    _args.emplace_back("--pointer-offset-range");
    _args.push_back(_minOffsetInput.GetStdStringNoZeros() + "," + _maxOffsetInput.GetStdStringNoZeros());
	_args.emplace_back("--pointer-depth-range");
    _args.push_back(_minPointerDepthInput.ToString() + "," + _maxPointerDepthInput.ToString());
    _args.emplace_back("--store-memory-pointers-file-path");
    _args.push_back(_resultsPathInput.GetStdStringNoZeros());
    _args.emplace_back("--maximum-memory-utilization-fraction");
    _args.push_back(std::to_string(_maxMemUtilizationFraction));
    _args.emplace_back("--file-extensions");
    _args.emplace_back(".bin");
    _args.emplace_back(".raw");
    _args.emplace_back(".dmp");
    _args.emplace_back("--maximum-pointer-count");
    _args.push_back(_maxPointerCountInput.ToString());
    _args.emplace_back("--maximum-pointers-printed-count");
    _args.push_back(_maxPointerCountInput.ToString());
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
    GetInstance()._systemPresetSelectCombo.SetSelectedByIndex(presetIndex);
    GetInstance()._addressWidthSelectCombo.SetSelectedById(GetInstance()._systemPresetSelectCombo.GetSelectedId());
    GetInstance()._isBigEndian = GetInstance()._systemPresetSelectCombo.GetSelectedBool();
}

void MungPlex::PointerSearch::SetMemoryRegions(const std::vector<SystemRegion>& regions)
{
    GetInstance()._regionSelectCombo.SetItems(regions);
}

bool MungPlex::PointerSearch::loadResults()
{
    std::ifstream resultsFile;
    resultsFile.open(_resultsPathInput.GetStdStringNoZeros());

    if (!resultsFile) {
        Log::LogInformation("Error loading pointer results");
        return false;
    }

    std::stringstream buffer;
    buffer << resultsFile.rdbuf();
    std::string temp = buffer.str();
    _resultsInput.SetMaxLength(temp.size());
    _resultsInput.SetText(temp);
    Log::LogInformation("Pointer Scan results loaded");
    return !temp.empty();
}

bool MungPlex::PointerSearch::comparePairs(const std::pair<InputText, std::array<InputInt<uint64_t>, 4>>& a, const std::pair<InputText, std::array<InputInt<uint64_t>, 4>>& b)
{
    if (a.second[3].GetValue() != b.second[3].GetValue())
        return a.second[3].GetValue() < b.second[3].GetValue();

    return a.second[0].GetValue() < b.second[0].GetValue();
}