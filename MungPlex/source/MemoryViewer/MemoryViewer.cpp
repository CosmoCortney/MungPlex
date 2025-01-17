#include "MemoryViewer.hpp"
#include "Connection.hpp"
#include <iostream>
#include "Settings.hpp"

MungPlex::MemoryViewer::MemoryViewer(const uint32_t id)
{
	SetIndex(id);
    _isOpen = true;
    _rereorder = ProcessInformation::GetRereorderFlag();
    _hexView = std::string("", _readSizeInput.GetValue());
    _dummy = std::string("Invalid Memory Region");
    _regionSelectCombo.SetItems(ProcessInformation::GetSystemRegionList());
    SetUpByRegionSelect(0);

    if (Connection::IsConnected())
        refreshMemory();
}

void MungPlex::MemoryViewer::SetIndex(const uint32_t id)
{
	_id = id;
	_windowTitle = "Memory Viewer " + std::to_string(id);

}

void MungPlex::MemoryViewer::DrawWindow()
{
    static bool stateSet = false;

    if (_id < 1)
        return;

    ImGui::Begin(_windowTitle.c_str(), &_isOpen, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing);
    {
        if (ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows))
        {
            if (!stateSet && Settings::GetGeneralSettings().EnableRichPresence)
                Connection::GetDiscordRichPresence().SetRichPresenceState("Memory Viewer");

            stateSet = true;
        }
        else
            stateSet = false;

        ImGui::SetWindowSize(ImVec2(1280.0f, 720.0f));
        drawControlPanel();
        ImGui::SameLine();
        drawHexEditor();
    }
	ImGui::End();
}

bool MungPlex::MemoryViewer::IsOpen()
{
    return _isOpen;
}

void MungPlex::MemoryViewer::drawControlPanel()
{
    ImVec2 childXY = ImGui::GetContentRegionAvail();
    childXY.x *= 0.333f;

    ImGui::BeginChild("child_MemoryViewerControlPanel", childXY);
    {
        if (_regionSelectCombo.Draw(1.0f, 0.4f))
        {
            SetUpByRegionSelect(_regionSelectCombo.GetSelectedIndex());
        }

        static bool refresh = false;
        static int processType = 0;
        processType = ProcessInformation::GetProcessType();

        if (processType != ProcessInformation::NATIVE) ImGui::BeginDisabled();

        if (ImGui::Checkbox("Write", ProcessInformation::GetRangeFlagWrite()))
            refresh = true;

        ImGui::SameLine();
        if (ImGui::Checkbox("Execute", ProcessInformation::GetRangeFlagExecute()))
            refresh = true;

        if (refresh)
            ProcessInformation::RefreshRegionlistPC();

        if (processType != ProcessInformation::NATIVE) ImGui::EndDisabled();

        if (_targetAddressInput.Draw(1.0f, 0.4f, true))
        {
            processBufferAddress();
        }

        if (_readSizeInput.Draw(1.0f, 0.4f))
        {
            if (_readSizeInput.GetValue() < 0x10)
                _readSizeInput.SetValue(0x10);

            _hexView = std::string(_hexView.data(), _readSizeInput.GetValue());
            refreshMemory();
        }

        if (processType == ProcessInformation::CONSOLE)
        {
            switch (ProcessInformation::GetConsoleConnectionType())
            {
                case ProcessInformation::CON_USBGecko:
                {
                    if (ImGui::Button("Refresh Memory"))
                    {
                        refreshMemory();
                    }
                } break;
            }
        }
    }
    ImGui::EndChild();
}

void MungPlex::MemoryViewer::drawHexEditor()
{
    ImGui::BeginChild("child_hexeditor");
    {
        if (_validAddress)
        {
            if(ProcessInformation::GetProcessType() != ProcessInformation::CONSOLE)
                _memEdit.DrawContents(_hexView.data(), _readSizeInput.GetValue(), _targetAddressInput.GetValue(), ProcessInformation::GetHandle(), _readAddressEx, _rereorder);
            else
            {
                switch (ProcessInformation::GetConsoleConnectionType())
                {
                case ProcessInformation::CON_USBGecko:
                {
                    static uint64_t byteWriteOffset = 0;

                    if (_memEdit.DrawContents(_hexView.data(), _readSizeInput.GetValue(), _targetAddressInput.GetValue(), NULL, nullptr, false, 0, &byteWriteOffset))
                    {
                        USBGecko* gecko = ProcessInformation::GetUsbGecko();
                        gecko->Poke<char>(_hexView[byteWriteOffset], byteWriteOffset + _targetAddressInput.GetValue());
                        refreshMemory();
                    }
                } break;
                default:
                    _memEdit.DrawContents(_dummy.data(), _dummy.size(), 0, 0, 0);
                }
            }
        }
        else
            _memEdit.DrawContents(_dummy.data(), _dummy.size(), 0, 0, 0);
    }
    ImGui::EndChild();
}

void MungPlex::MemoryViewer::SetUpByRegionSelect(const int index)
{
    _targetAddressInput.SetValue(_regionSelectCombo.GetRegionAt(index).Base);
    processBufferAddress();
}

void MungPlex::MemoryViewer::processBufferAddress()
{
    _validAddress = false;

    if (ProcessInformation::GetProcessType() == ProcessInformation::CONSOLE)
    {
        if (ProcessInformation::GetRegionIndex(_targetAddressInput.GetValue()) == -1)
            return;

        _validAddress = true;
        refreshMemory();
        return;
    }

    for (auto& region : ProcessInformation::GetSystemRegionList())
    {
        if (_targetAddressInput.GetValue() >= region.Base && _targetAddressInput.GetValue() < (region.Base + region.Size))
        {
            _readAddressEx = (void*)((char*)region.BaseLocationProcess + (_targetAddressInput.GetValue() - region.Base));
            _handle = ProcessInformation::GetHandle();
            _validAddress = true;
            return;
        }
    }
}

void MungPlex::MemoryViewer::refreshMemory()
{
    if (ProcessInformation::GetProcessType() != ProcessInformation::CONSOLE)
        return;

    switch (ProcessInformation::GetConsoleConnectionType())
    {
        case ProcessInformation::CON_USBGecko:
        {
            USBGecko* gecko = ProcessInformation::GetUsbGecko();
            gecko->Read(_hexView.data(), _targetAddressInput.GetValue(), _readSizeInput.GetValue());
        } break;
    }
}