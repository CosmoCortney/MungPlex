#pragma once
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "InputFloat.hpp"
#include "InputInt.hpp"
#include "InputText.hpp"
#include "StringIdBoolCombo.hpp"
#include "StringIdCombo.hpp"


namespace MungPlex
{
	static void DrawHelpMarker(const char* desc) //� ImGui devs
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

	static void DrawLabel(const char* label, const float paneWidth, const float labelPortion, bool printLabel, const char* helpText = nullptr)
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
}

