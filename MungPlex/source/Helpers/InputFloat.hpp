#pragma once
#include "imgui.h"
#include "IWidget.hpp"

namespace MungPlex
{
	template <typename floatType> class InputFloat : public IWidget
	{
	public:
		typedef std::function<void()> Slot;
		InputFloat() = default;
		InputFloat(const std::string& label, const bool printLabel = true, const floatType value = 0, const floatType stepLow = 1, const floatType stepHigh = 10, const uint32_t precision = 4, const ImGuiInputTextFlags textFlags = ImGuiInputTextFlags_None)
			: IWidget(label, printLabel)
		{
			_val = value;
			_stepLow = stepLow;
			_stepHigh = stepHigh;
			_precision = precision;
			_textFlags = textFlags;
			SetPrecision(precision);
		}

		InputFloat(const InputFloat& other)
		{
			assign(other);
		}

		InputFloat& operator=(const InputFloat& other)
		{
			assign(other);
			return *this;
		}

		InputFloat(InputFloat&& other) noexcept
		{
			assign(other);
		}

		InputFloat& operator=(InputFloat&& other) noexcept
		{
			assign(other);
			return *this;
		}

		bool Draw(const float paneWidth = 0.25f, const float labelPortion = 0.4f, const bool scientific = false)
		{
			static bool edited = false;
			static void* stepLow = nullptr;
			static void* stepHigh = nullptr;

			DrawLabel(_label.c_str(), paneWidth, labelPortion, _printLabel, _showHelpText ? _helpText.c_str() : nullptr);

			if constexpr (std::is_same_v<floatType, float>)
				edited = ImGui::InputFloat(_id.c_str(), &_val, _stepLow, _stepHigh, scientific ? _scientific.c_str() : _decimal.c_str(), _textFlags);
			else if constexpr (std::is_same_v<floatType, double>)
				edited = ImGui::InputDouble(_id.c_str(), &_val, _stepLow, _stepHigh, scientific ? _scientific.c_str() : _decimal.c_str(), _textFlags);
			else
				ImGui::Text("Incorrect data type specified.");

			ImGui::PopItemWidth();

			if (edited)
				callOnValueChangedSlots();

			return edited;
		}

		void SetPrecision(const uint32_t precision)
		{
			_decimal = "%." + std::to_string(precision) + "F";
			_scientific = "%." + std::to_string(precision) + "e";
		}

		void SetValue(const floatType value)
		{
			_val = value;
		}

		floatType GetValue() const
		{
			return _val;
		}

		void ConnectOnValueChanged(const Slot slot)
		{
			_slotsOnValueChanged.emplace_back(slot);
		}

	private:
		floatType _val = 0;
		floatType _stepLow = 0;
		floatType _stepHigh = 10;
		uint32_t _precision = 4;
		std::string _scientific = "";
		std::string _decimal = "";
		ImGuiInputTextFlags _textFlags = ImGuiInputTextFlags_None;
		std::vector<Slot> _slotsOnValueChanged{};

		void assign(const InputFloat& other)
		{
			_val = other._val;
			_helpText = other._helpText;
			_id = other._id;
			_label = other._label;
			_printLabel = other._printLabel;
			_showHelpText = other._showHelpText;
			_textFlags = other._textFlags;
			_stepLow = other._stepLow;
			_stepHigh = other._stepHigh;
			_precision = other._precision;
			_slotsOnValueChanged = other._slotsOnValueChanged;
		}

		void callOnValueChangedSlots()
		{
			for (Slot& slot : _slotsOnValueChanged)
				slot();
		}
	};
}