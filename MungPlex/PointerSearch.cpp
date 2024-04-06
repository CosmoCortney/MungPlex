#include "PointerSearch.h"
#include"ImGuiFileDialog.h"
#include"Log.h"
#include "Settings.h"
#include"HelperFunctions.h"
#include<Windows.h>
#include <fstream>
#include <chrono>
#include <thread>
#include <future>

MungPlex::PointerSearch::PointerSearch()
{
    _systemPresets.emplace_back("Nintendo Entertainment Systen / Famicom", 1, false);
    _systemPresets.emplace_back("Super Nintendo Entertainment Systen / Super Famicom", 1, false);
    _systemPresets.emplace_back("Nintendo 64", 2, true);
    _systemPresets.emplace_back("Nintendo GameCube", 2, true);
    _systemPresets.emplace_back("Triforce Arcade", 2, true);
    _systemPresets.emplace_back("Nintendo Wii", 2, true);
    _systemPresets.emplace_back("Nintendo Wii U", 2, true);
    _systemPresets.emplace_back("Nintendo Switch", 3, false);
    _systemPresets.emplace_back("Nintendo GameBoy", 1, false);
    _systemPresets.emplace_back("Nintendo GameBoy Color", 1, false);
    _systemPresets.emplace_back("Nintendo GameBoy Advance", 2, false);
    _systemPresets.emplace_back("Nintendo DS", 2, false);
    _systemPresets.emplace_back("Nintendo 3DS", 2, false);
    _systemPresets.emplace_back("Sony Playstation", 2, false);
    _systemPresets.emplace_back("Sony Playstation 2", 2, false);
    _systemPresets.emplace_back("Sony Playstation 3", 2, true);
    _systemPresets.emplace_back("Sony Playstation 4", 3, false);
    _systemPresets.emplace_back("Sony Playstation 5", 3, false);
    _systemPresets.emplace_back("Sony Playstation Portable", 2, false);
    _systemPresets.emplace_back("Sony Playstation Vita", 2, false);
    _systemPresets.emplace_back("Sega Master System", 1, false);
    _systemPresets.emplace_back("Sega Mega Drive / Genesis", 1, true);
    _systemPresets.emplace_back("Sega Mega 32X", 1, true);
    _systemPresets.emplace_back("Sega Mega CD", 1, true);
    _systemPresets.emplace_back("Sega Saturn", 2, true);
    _systemPresets.emplace_back("Sega Dreamcast", 2, false);
    _systemPresets.emplace_back("Sega GameGear", 1, false);
    _systemPresets.emplace_back("Microsoft XBOX", 2, false);
    _systemPresets.emplace_back("Microsoft XBOX 360", 2, true);
    _systemPresets.emplace_back("Microsoft XBOX One", 3, false);
    _systemPresets.emplace_back("Microsoft XBOX Series", 3, false);
    _systemPresets.emplace_back("Microsoft Windows x86", 2, false);
    _systemPresets.emplace_back("Microsoft Windows x64", 3, false);

    _defaultPath = Settings::GetGeneralSettings().DocumentsPath;
    std::replace(_defaultPath.begin(), _defaultPath.end(), '\\', '/');
    _defaultPath.append("/MungPlex/Dumps/");
    _minOffset = "0";
    _minOffset.resize(17);
    _maxOffset = "1000";
    _maxOffset.resize(17);
    const auto temporary_directory_path = std::filesystem::temp_directory_path();
    const auto pointers_output_file_path = temporary_directory_path / "Pointers.txt";
    _resultsPath = pointers_output_file_path.string();
    _resultsPath.resize(512);
}

void MungPlex::PointerSearch::DrawWindow()
{
    static bool stateSet = false;

    if (ImGui::Begin("Pointer Search"))
    {
        if (Connection::IsConnected() && Settings::GetGeneralSettings().EnableRichPresence && !stateSet)
        {
            Connection::SetRichPresenceState("Scanning for Pointers");
            stateSet = true;
        }

        GetInstance().drawList();
        GetInstance().drawSettings();
        ImGui::SameLine();
        GetInstance().drawResults();
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
        if (SetUpCombo("System Preset:", _systemPresets, _presetSelect, 1.0f, 0.3f))
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

        static std::vector<std::string> addressWidthSelect = { "1 Byte", "2 Bytes", "4 Bytes", "8 Bytes" };
        if (SetUpCombo("Address Width:", addressWidthSelect, _addressWidthIndex, 1.0f, 0.3f, true, "Address width of the dump's system."))
            _addressWidth = 1 << _addressWidthIndex;

        SetUpInputText("Results File:", _resultsPath.data(), _resultsPath.size(), 1.0f, 0.3f, true, "Where to save the results file.");
        SetUpSliderFloat("Max. Memory Utilization Fraction:", &_maxMemUtilizationFraction, 0.1f, 0.95f, "%2f", 1.0f, 0.5f);
        SetUpInputInt("Max. Pointer Count:", &_maxPointerCount, 100, 1000, 1.0f, 0.3f, 0, true, "Maximum amount of pointers to be generated. Smaller values may decrease scan time and but also the likeability to find working pointer paths.");

        
        SetUpCombo("Input Filetype:", _inputTypeSelect, _selectedInputType, 1.0f, 0.3f);

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
                std::string filePathName = std::string("\"") + ImGuiFileDialog::Instance()->GetFilePathName() + "\"";
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
        SetUpCombo("Region:", _regions, _regionSelect, 0.5f, 0.4f);
        
        ImGui::SameLine();
        
        if (ImGui::Button("Dump"))
        {
            SystemRegion& region = _regions[_regionSelect];
            std::wstring path = MorphText::Utf8_To_Utf16LE(Settings::GetGeneralSettings().DocumentsPath) + L"\\MungPlex\\Dumps\\" + std::to_wstring(region.Base) + L".bin";
            Xertz::SystemInfo::GetProcessInfo(ProcessInformation::GetPID()).DumpMemory(region.BaseLocationProcess, path, region.Size);
        }
    }
    ImGui::EndChild();
}


void MungPlex::PointerSearch::drawList()
{
    ImVec2 childXY = { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y * 0.5f };

    ImGui::BeginChild("child_MemDumpList", childXY, true);
    {
        //const float width = childXY.x;
        
        /*ImGui::Text("Dump File");

        ImGui::SameLine();

        ImGui::SetCursorPosX(width * 0.51f);
        ImGui::Text("Starting Address");

        ImGui::SameLine();
        ImGui::SetCursorPosX(width * 0.64f);
        ImGui::Text("Target Address");

        ImGui::SameLine();
        ImGui::SetCursorPosX(width * 0.765f);
        ImGui::Text("File Type");

        ImGui::SameLine();
        ImGui::SetCursorPosX(width * 0.89f);
        ImGui::Text("Correspondence");*/

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
            
            for (int row = 0; row < _memDumps.size(); ++row)
            {
                ImGui::TableNextRow();

                for (int col = 0; col < 4; ++col)
                {
                    ImGui::TableSetColumnIndex(col);

                    switch(col)
                    {
                    case 0:
                        if (SetUpInputText((label + std::to_string(row)).c_str(), _memDumps[row].first.data(), _memDumps[row].first.size(), 1.0f, 0.0f, false))
                        {
                        }
                        break;
                    case 1:
                        if (SetUpInputText((label + "s" + std::to_string(row)).c_str(), _bufStartingAddress[row].data(), _bufStartingAddress[row].size(), 1.0f, 0.0f, false))
                        {
                            std::stringstream stream;
                            stream << std::hex << _bufStartingAddress[row];
                            stream >> _memDumps[row].second[0];
                        }
                        break;
                    case 2:
                        if (SetUpInputText((label + "t" + std::to_string(row)).c_str(), _bufTargetAddress[row].data(), _bufTargetAddress[row].size(), 1.0f, 0.0f, false))
                        {
                            std::stringstream stream;
                            stream << std::hex << _bufTargetAddress[row];
                            stream >> _memDumps[row].second[1];
                        }
                        break;
                    case 3: 
                        if (SetUpInputInt((label + "correspondence" + std::to_string(row)).c_str(), reinterpret_cast<int*>(&_memDumps[row].second[3]), 1, 1, 1.0f, 0.0f, 0, false))
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


bool MungPlex::PointerSearch::performScan()
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    std::promise<PROCESS_INFORMATION> piPromise;
    std::future<PROCESS_INFORMATION> piFuture = piPromise.get_future();
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    const auto pointerSearcherFilePath = GetResourcesFilePath("Universal-Pointer-Searcher-Engine\\UniversalPointerSearcher.exe");
    std::cout << "Starting pointer searcher with arguments: " << _arg;

    const bool success = CreateProcess(pointerSearcherFilePath.string().data(),
        const_cast<LPSTR>(std::string("UniversalPointerSearcher.exe ").append(_arg).c_str()),
        NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

    if (success) 
    {
        std::thread waitNload = std::thread([&](std::future<PROCESS_INFORMATION> future)
            {
            PROCESS_INFORMATION pi = future.get();
			waitAndLoadResults(pi);
            }, std::move(piFuture));

    	piPromise.set_value(pi);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        waitNload.join();
    }
    else
    {
        std::cerr << "Pointer searcher exited with an error" << std::endl;
    }

    return success;
}

void MungPlex::PointerSearch::waitAndLoadResults(PROCESS_INFORMATION pi)
{
    bool stillActive = true;
    DWORD exitCode;
    const auto millisecondsToWait = std::chrono::seconds(1);
    
    while(stillActive)
    {
        std::this_thread::sleep_for(millisecondsToWait);
        WaitForSingleObject(pi.hProcess, 0);
        GetExitCodeProcess(pi.hProcess, &exitCode);
        stillActive = exitCode == STILL_ACTIVE;
    }

    Log::LogInformation("Finished Pointer Scan");
    loadResults();
}

void MungPlex::PointerSearch::generateArgument()
{
    std::stringstream stream;
    _arg = "--pointer-offset-range ";
    _arg.append(_minOffset.c_str()).append(",");
    _arg.append(_maxOffset.c_str()).append(" ");

    _arg.append("--pointer-depth-range " + 
        std::to_string(_minPointerDepth) + "," + 
        std::to_string(_maxPointerDepth) + " ");

    int highestCorrespondence = 0;
    std::string initialFilePaths("--initial-file-path ");
    std::string readPointermapsFilePaths("--read-pointer-maps-file-paths ");
    std::string writePointermapsFilePaths("--write-pointer-maps-file-paths ");
    std::string targetPointermaps("--target-pointer-maps ");
    std::string comparisonFilePaths("--comparison-file-path ");
    std::string initialStartingAddresses("--initial-starting-address ");
    std::string comparisonStartingAddresses("--comparison-starting-address ");
    std::string targetAdresses("--target-address ");

    for (auto memDump : _memDumps)
        if (memDump.second[3] > highestCorrespondence)
            highestCorrespondence = memDump.second[3];

    int previousCorrespondence = -1;
    bool separator = true;
    for(int currentCorrespondence = 0; currentCorrespondence <= highestCorrespondence; ++currentCorrespondence)
    {
        separator = true;

    	for (auto memDump : _memDumps)
        {
            stream.str(std::string());

            if (_selectedInputType)
            {
                readPointermapsFilePaths.append(memDump.first).append(" ");

                if (memDump.second[3] == 0)
                    targetPointermaps.append("Initial ");
                else
                    targetPointermaps.append("\"Comparison ").append(std::to_string(memDump.second[3])).append("\" ");
            }
            else
            {
                if (memDump.second[3] == currentCorrespondence && currentCorrespondence == 0)
                {
                    initialFilePaths.append(memDump.first.c_str()).append(" "); //file path
                    stream << std::hex << memDump.second[0]; //starting address
                    initialStartingAddresses.append("0x").append(stream.str()).append(" ");
                }
                else if (memDump.second[3] == currentCorrespondence && currentCorrespondence > 0)
                {
                    comparisonFilePaths.append(memDump.first.c_str()).append(" "); //file path

                    stream << std::hex << memDump.second[0]; //starting address
                    comparisonStartingAddresses.append("0x").append(stream.str()).append(" ");
                }
            }
            
            if(memDump.second[3] == currentCorrespondence && previousCorrespondence != currentCorrespondence)
            {
                stream.str(std::string()); //target address
                stream << std::hex << memDump.second[1];
                targetAdresses.append("0x").append(stream.str()).append(" ");
                previousCorrespondence = currentCorrespondence;
            }

            if(currentCorrespondence > 1 && currentCorrespondence <= highestCorrespondence && separator)
            {
                comparisonFilePaths.append("%% ");
                comparisonStartingAddresses.append("%% ");
                separator = false;
            }
        }

        if (_selectedInputType)
            break;
    }
    
    if (_selectedInputType)
    {
        _arg.append(readPointermapsFilePaths);
        _arg.append(targetPointermaps);
    }
    else
    {
        _arg.append(initialFilePaths);
        _arg.append(initialStartingAddresses);
        _arg.append(comparisonFilePaths);
        _arg.append(comparisonStartingAddresses);
    }

    _arg.append(targetAdresses);
    _arg.append("--endian ").append(_isBigEndian ? "big " : "little ");
    _arg.append("--address-size ").append(std::to_string(_addressWidth) + " ");
    _arg.append("--store-memory-pointers-file-path \"").append(_resultsPath).append("\" ");
    _arg.append("--maximum-memory-utilization-fraction ").append(std::to_string(_maxMemUtilizationFraction) + " ");
    _arg.append("--file-extensions .bin .raw .dmp");
    _arg.append("--maximum-pointer-count ").append(std::to_string(_maxPointerCount) + " ");
    _arg.append("--maximum-pointers-printed-count ").append(std::to_string(_maxPointerCount) + " ");
    _arg.append("--disable-printing-memory-pointers-to-console ");

    if(_printModuleNames)
        _arg.append("--print-module-file-names ");

    if(_printVisitedAddresses)
		_arg.append("--print-visited-addresses ");

    Log::LogInformation("Pointer scan arguments: " + _arg);

#ifndef NDEBUG
    std::cout << "pointer scan args:\n" << _arg << "\n";
#endif
}

void MungPlex::PointerSearch::SelectPreset(const int presetIndex)
{
    GetInstance()._presetSelect = presetIndex;
    GetInstance()._addressWidthIndex = std::get<int>(GetInstance()._systemPresets[presetIndex]);
    GetInstance()._isBigEndian = std::get<bool>(GetInstance()._systemPresets[presetIndex]);
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
    return static_cast<bool>(_results.size());
}
