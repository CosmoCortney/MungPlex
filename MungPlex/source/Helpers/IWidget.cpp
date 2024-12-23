#include "imgui.h"
#include "IWidget.hpp"

MungPlex::IWidget::IWidget(const std::string& label, const bool printLabel)
{
	_label = label;
	_id = "##" + label;
	_printLabel = printLabel;
}

void MungPlex::IWidget::SetLabel(const std::string& label)
{
	_label = label;
	_id = "##" + label;
}

void MungPlex::IWidget::SetHelpText(const std::string& helpText, const bool show)
{
	_helpText = helpText;
	SetShowHelpText(show);
}

void MungPlex::IWidget::SetShowHelpText(const bool show)
{
	_showHelpText = show;
}

void MungPlex::IWidget::DrawHelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

void MungPlex::IWidget::DrawLabel(const char* label, const float paneWidth, const float labelPortion, bool printLabel, const char* helpText)
{
	const float absoluteWidth = ImGui::GetContentRegionAvail().x * paneWidth;
	const float curserPos = ImGui::GetCursorPos().x;

	if (printLabel)
	{
		ImGui::Text(label);
		ImGui::SameLine();
		if (helpText != nullptr)
		{
			DrawHelpMarker(helpText);
			ImGui::SameLine();
		}

		ImGui::SetCursorPosX(curserPos + absoluteWidth * labelPortion);
		ImGui::PushItemWidth(absoluteWidth * (1.0f - labelPortion));
	}
	else
		ImGui::PushItemWidth(absoluteWidth);
}