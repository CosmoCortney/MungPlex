#include "MemoryViewer.h"

MungPlex::MemoryViewer::MemoryViewer(const uint32_t id)
{
	SetIndex(id);
    _isOpen = true;
    _rereorder = ProcessInformation::GetRereorderFlag();
    _bufAddress = std::string("00000000\0", 17);
    _hexView = std::string("", _readSize);
    _dummy = std::string("Invalid Memory Region");
    setUpByRegionSelect(0);
}

void MungPlex::MemoryViewer::SetIndex(const uint32_t id)
{
	_id = id;
	_windowTitle = "Memory Viewer " + std::to_string(id);

}

void MungPlex::MemoryViewer::DrawWindow()
{
    if (_id < 1)
        return;

    ImGui::Begin(_windowTitle.c_str(), &_isOpen, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings);
    {
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
        if (SetUpCombo("Region:", ProcessInformation::GetSystemRegionList(), _regionSelect, 1.0f, 0.4f))
        {
            setUpByRegionSelect(_regionSelect);
        }

        static bool refresh = false;

        if (ProcessInformation::GetProcessType() != ProcessInformation::NATIVE) ImGui::BeginDisabled();

        if (ImGui::Checkbox("Write", ProcessInformation::GetRangeFlagWrite()))
            refresh = true;

        ImGui::SameLine();
        if (ImGui::Checkbox("Execute", ProcessInformation::GetRangeFlagExecute()))
            refresh = true;

        if (refresh)
            ProcessInformation::RefreshRegionlistPC();

        if (ProcessInformation::GetProcessType() != ProcessInformation::NATIVE) ImGui::EndDisabled();

        if (SetUpInputText("Jump to Address:", _bufAddress.data(), 17, 1.0f, 0.4f))
        {
            processBufferAddress();
        }

        if (SetUpInputInt("Read Size:", reinterpret_cast<int*>(&_readSize), 0x10, 0x100, 1.0f, 0.4f))
        {
            if (_readSize < 0x10)
                _readSize = 0x10;

            _hexView = std::string(_hexView.data(), _readSize);
        }
    }
    ImGui::EndChild();
}

void MungPlex::MemoryViewer::drawHexEditor()
{
    ImGui::BeginChild("child_hexeditor");
    {
        if(_validAddress)
            _memEdit.DrawContents(_hexView.data(), _readSize, _viewAddress, ProcessInformation::GetHandle(), _readAddressEx, _rereorder);
        else
            _memEdit.DrawContents(_dummy.data(), _dummy.size(), 0, 0, 0);
    }
    ImGui::EndChild();
}

void MungPlex::MemoryViewer::setUpByRegionSelect(const int index)
{
    std::stringstream stream;
    stream << std::hex << ProcessInformation::GetSystemRegionList()[index].Base;
    stream >> _bufAddress;
    processBufferAddress();
}

void MungPlex::MemoryViewer::processBufferAddress()
{
    std::stringstream stream;
    stream << std::hex << _bufAddress.data();
    stream >> _viewAddress;

    for (SystemRegion& region : ProcessInformation::GetSystemRegionList())
    {
        if (_viewAddress >= region.Base && _viewAddress < (region.Base + region.Size))
        {
            _readAddressEx = (void*)((char*)region.BaseLocationProcess + (_viewAddress - region.Base));
            _handle = ProcessInformation::GetHandle();
            _validAddress = true;
            return;
        }
    }

    _validAddress = false;
}