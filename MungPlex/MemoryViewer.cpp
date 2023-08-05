#include "MemoryViewer.h"

MungPlex::MemoryViewer::MemoryViewer(const uint32_t id)
{
	SetIndex(id);
    _isOpen = true;
    _bufAddress = new char[17];
    strcpy_s(_bufAddress, 16, "0");
    _bufAddress[16] = '\0';
    _hexView = new char[_readSize+1];
    memset(_hexView, '\0', _readSize);

    _dummy = new char[256];
    memset(_dummy, '?', 256);

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
        if (SetUpCombo("Region:", ProcessInformation::GetRegions(), _regionSelect, 1.0f, 0.4f))
        {
            setUpByRegionSelect(_regionSelect);
        }

        if (SetUpInputText("Jump to Address:", _bufAddress, 17, 1.0f, 0.4f))
        {
            processBufferAddress();
        }

        if (SetUpInputInt("Read Size:", reinterpret_cast<int*>(&_readSize), 0x10, 0x100, 1.0f, 0.4f))
        {
            if (_readSize < 0x10)
                _readSize = 0x10;

            delete[] _hexView;
            _hexView = new char[_readSize+1];
            memset(_hexView, '\0', _readSize);
        }
    }
    ImGui::EndChild();
}

void MungPlex::MemoryViewer::drawHexEditor()
{
    ImGui::BeginChild("child_hexeditor");
    {
        if(_validAddress)
            _memEdit.DrawContents(_hexView, _readSize, _viewAddress, _handle, _readAddressEx);
        else
            _memEdit.DrawContents(_dummy, 256, 0, 0, 0);
    }
    ImGui::EndChild();
}

void MungPlex::MemoryViewer::setUpByRegionSelect(const int index)
{
    std::stringstream stream;
    stream << std::hex << ProcessInformation::GetRegions()[index].Base;
    strcpy_s(_bufAddress, 17, stream.str().c_str());
    processBufferAddress();
}

void MungPlex::MemoryViewer::processBufferAddress()
{
    std::stringstream stream;
    stream << std::hex << _bufAddress;
    stream >> _viewAddress;

    for (SystemRegion region : ProcessInformation::GetRegions())
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