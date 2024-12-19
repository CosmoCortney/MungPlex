#pragma once
#include <functional>
#include "GLFW/glfw3.h"
#include "imgui.h"
#include <string>
#include <utility>
#include <vector>

namespace MungPlex
{
	static void DrawHelpMarker(const char* desc) //© ImGui devs
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

	class InputText
	{
	public:
		typedef std::function<void()> Slot;
		InputText(const std::string& label, const std::string text = "", const uint64_t maxLength = 256, const bool printLabel = true, const ImGuiInputTextFlags flags = ImGuiInputTextFlags_None);
		bool Draw(const float paneWidth = 0.25f, const float labelPortion = 0.4f);
		void SetText(const std::string& text);
		void SetLabel(const std::string& label);
		void SetHelpText(const std::string& helpText, const bool show = true);
		void SetShowHelpText(const bool show);
		void SetMaxLength(const uint64_t maxLength);
		std::string GetStdString() const;
		std::string GetStdStringNoZeros() const;
		const char* GetCString() const;
		char* GetData();
		uint64_t GetMaxLength() const;
		void ConnectOnTextChanged(const Slot slot);

	private:
		std::string _text = std::string(256, '\0');
		std::string _helpText = "";
		std::string _id = "";
		std::string _label = "";
		uint64_t _maxLength = 256;
		bool _printLabel = true;
		bool _showHelpText = false;
		ImGuiInputTextFlags _flags = ImGuiInputTextFlags_None;
		std::vector<Slot> _slotsOnTextChanged{};
	};
}