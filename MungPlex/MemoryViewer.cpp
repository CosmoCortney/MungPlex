#include "MemoryViewer.h"

MungPlex::MemoryViewer::MemoryViewer(const uint32_t id)
{
	SetIndex(id);
    _isOpen = true;
    strcpy_s(_bufAddress, 17, "0");
    _hexView = new char[32*16+1];
    memset(_hexView, '0', 32 * 16 + 1);
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
        SetUpCombo("Region:", ProcessInformation::GetRegions(), _regionSelect, 1.0f, 0.4f);

        if (SetUpInputText("Jump to Address:", _bufAddress, 17, 1.0f, 0.4f))
        {
            std::stringstream stream;
            stream << std::hex << _bufAddress;
            stream >> _viewAddress;
        }
    }
    ImGui::EndChild();
}

void MungPlex::MemoryViewer::drawHexEditor()
{
    //_memEdit.DrawContents(_hexView, 32 * 16 + 1, _viewAddress);
    _memEdit.DrawContents(_hexView, 32 * 16 + 1,
        reinterpret_cast<size_t>(ProcessInformation::GetRegions()[0].BaseLocationProcess),
        ProcessInformation::GetHandle());
}