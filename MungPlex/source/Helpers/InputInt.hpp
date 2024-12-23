#pragma once
#include "IWidget.hpp"

namespace MungPlex
{
	template <typename intType> class InputInt : public IWidget
	{
	public:
		typedef std::function<void()> Slot;
		InputInt() = default;
		InputInt(const std::string& label, const bool printLabel = true, const intType value = 0, const uint64_t stepLow = 1, const uint64_t stepHigh = 10, const ImGuiInputTextFlags textFlags = ImGuiInputTextFlags_None)
			: IWidget(label, printLabel)
		{
			_val = value;
			_stepLow = stepLow;
			_stepHigh = stepHigh;
			_textFlags = textFlags;
		}

		InputInt(const InputInt& other)
		{
			assign(other);
		}

		InputInt& operator=(const InputInt& other)
		{
			assign(other);
			return *this;
		}

		InputInt(InputInt&& other) noexcept
		{
			assign(other);
		}

		InputInt& operator=(InputInt&& other) noexcept
		{
			assign(other);
			return *this;
		}


		bool Draw(const float paneWidth = 0.25f, const float labelPortion = 0.4f, const bool hex = false)
		{
			static bool edited = false;
			static ImGuiInputTextFlags flags = ImGuiInputTextFlags_None;
			static void* stepLow = nullptr;
			static void* stepHigh = nullptr;
			stepLow = _stepLow > 0 ? &_stepLow : nullptr;
			stepHigh = _stepHigh > 0 ? &_stepHigh : nullptr;

			DrawLabel(_label.c_str(), paneWidth, labelPortion, _printLabel, _showHelpText ? _helpText.c_str() : nullptr);

			if (hex)
				flags = _textFlags | ImGuiInputTextFlags_CharsHexadecimal;
			else
				flags = _textFlags & ~ImGuiInputTextFlags_CharsHexadecimal;

			if constexpr (std::is_same_v<intType, int8_t> || std::is_same_v<intType, char>)
				edited = ImGui::InputScalar(_id.c_str(), hex ? ImGuiDataType_U8 : ImGuiDataType_S8, &_val, stepLow, stepHigh, hex ? "%02X" : nullptr, flags);
			else if constexpr (std::is_same_v<intType, uint8_t>)
				edited = ImGui::InputScalar(_id.c_str(), ImGuiDataType_U8, &_val, stepLow, stepHigh, hex ? "%02X" : nullptr, flags);
			else if constexpr (std::is_same_v<intType, int16_t>)
				edited = ImGui::InputScalar(_id.c_str(), hex ? ImGuiDataType_U16 : ImGuiDataType_S16, &_val, stepLow, stepHigh, hex ? "%04X" : nullptr, flags);
			else if constexpr (std::is_same_v<intType, uint16_t>)
				edited = ImGui::InputScalar(_id.c_str(), ImGuiDataType_U16, &_val, stepLow, stepHigh, hex ? "%04X" : nullptr, flags);
			else if constexpr (std::is_same_v<intType, int32_t>)
				edited = ImGui::InputScalar(_id.c_str(), hex ? ImGuiDataType_U32 : ImGuiDataType_S32, &_val, stepLow, stepHigh, hex ? "%08X" : nullptr, flags);
			else if constexpr (std::is_same_v<intType, uint32_t>)
				edited = ImGui::InputScalar(_id.c_str(), ImGuiDataType_U32, &_val, stepLow, stepHigh, hex ? "%08X" : nullptr, flags);
			else if constexpr (std::is_same_v<intType, int64_t>)
				edited = ImGui::InputScalar(_id.c_str(), hex ? ImGuiDataType_U64 : ImGuiDataType_S64, &_val, stepLow, stepHigh, hex ? "%llX" : nullptr, flags);
			else if constexpr (std::is_same_v<intType, uint64_t>)
				edited = ImGui::InputScalar(_id.c_str(), ImGuiDataType_U64, &_val, stepLow, stepHigh, hex ? "%llX" : nullptr, flags);
			else
				ImGui::Text("Incorrect data type specified.");

			ImGui::PopItemWidth();

			if (edited)
				callOnValueChangedSlots();

			return edited;
		}

		void SetValue(const intType value)
		{
			_val = value;
		}

		intType GetValue() const
		{
			return _val;
		}

		void ConnectOnValueChanged(const Slot slot)
		{
			_slotsOnValueChanged.emplace_back(slot);
		}

	private:
		intType _val = 0;
		uint64_t _stepLow = 0;
		uint64_t _stepHigh = 0;
		ImGuiInputTextFlags _textFlags = ImGuiInputTextFlags_None;
		std::vector<Slot> _slotsOnValueChanged{};

		void assign(const InputInt& other)
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
			_slotsOnValueChanged = other._slotsOnValueChanged;
		}

		void callOnValueChangedSlots()
		{
			for (Slot& slot : _slotsOnValueChanged)
				slot();
		}
	};
}