#pragma once
#include "GLFW/glfw3.h"
#include "imgui.h"
#include <string>
#include <utility>
#include <vector>

namespace MungPlex
{
	static void HelpMarker(const char* desc) //© ImGui devs
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

	static void PrepareWidgetLabel(const char* label, const float paneWidth, const float labelPortion, bool printLabel, const char* helpText = nullptr)
	{
		const float absoluteWidth = ImGui::GetContentRegionAvail().x * paneWidth;
		const float curserPos = ImGui::GetCursorPos().x;

		if (printLabel)
		{
			ImGui::Text(label);
			ImGui::SameLine();
			if (helpText != nullptr)
			{
				HelpMarker(helpText);
				ImGui::SameLine();
			}

			ImGui::SetCursorPosX(curserPos + absoluteWidth * labelPortion);
			ImGui::PushItemWidth(absoluteWidth * (1.0f - labelPortion));
		}
		else
			ImGui::PushItemWidth(absoluteWidth);
	}

	class StringIdPairCombo
	{
	public:
		StringIdPairCombo(const std::vector<std::pair<std::string, int>>& pairs);
		StringIdPairCombo operator=(const StringIdPairCombo& other);

		bool Draw(const std::string& label, int* currentSelect, const float paneWidth = 0.25f, const float labelPortion = 0.4f, bool printLabel = true, const char* helpText = nullptr) const;
		const char* GetCString(const uint32_t index) const;
		const char* const* GetData() const;
		const std::string& GetStdString(const uint32_t index) const;
		int GetId(const uint32_t index) const;
		uint32_t GetIndexById(const int id) const;
		uint32_t GetCount() const;

	private:
		std::vector<std::pair<std::string, int>> _pairs;
		std::vector<const char*> _stringsPointers;
	};



}