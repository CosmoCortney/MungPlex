#pragma once
#include "IWidget.hpp"
#include <sstream>

namespace MungPlex
{
	template <typename intType> class InputVectorInt : public IWidget
	{
	public:
		typedef std::function<void()> Slot;
		InputVectorInt() = default;
		InputVectorInt(const std::string& label, const bool printLabel = true, const std::vector<intType>& values = {}, const uint64_t stepLow = 1, const uint64_t stepHigh = 10, const ImGuiInputTextFlags textFlags = ImGuiInputTextFlags_None)
			: IWidget(label, printLabel)
		{
			_values = values;
			_stepLow = stepLow;
			_stepHigh = stepHigh;
			_textFlags = textFlags;
		}

		InputVectorInt(const InputVectorInt& other)
		{
			assign(other);
		}

		InputVectorInt& operator=(const InputVectorInt& other)
		{
			assign(other);
			return *this;
		}

		InputVectorInt(InputVectorInt&& other) noexcept
		{
			assign(other);
		}

		InputVectorInt& operator=(InputVectorInt&& other) noexcept
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
				edited = ImGui::InputScalar(_id.c_str(), hex ? ImGuiDataType_U8 : ImGuiDataType_S8, &_values[_selectedIndex], stepLow, stepHigh, hex ? "%02X" : nullptr, flags);
			else if constexpr (std::is_same_v<intType, uint8_t>)
				edited = ImGui::InputScalar(_id.c_str(), ImGuiDataType_U8, &_values[_selectedIndex], stepLow, stepHigh, hex ? "%02X" : nullptr, flags);
			else if constexpr (std::is_same_v<intType, int16_t>)
				edited = ImGui::InputScalar(_id.c_str(), hex ? ImGuiDataType_U16 : ImGuiDataType_S16, &_values[_selectedIndex], stepLow, stepHigh, hex ? "%04X" : nullptr, flags);
			else if constexpr (std::is_same_v<intType, uint16_t>)
				edited = ImGui::InputScalar(_id.c_str(), ImGuiDataType_U16, &_values[_selectedIndex], stepLow, stepHigh, hex ? "%04X" : nullptr, flags);
			else if constexpr (std::is_same_v<intType, int32_t>)
				edited = ImGui::InputScalar(_id.c_str(), hex ? ImGuiDataType_U32 : ImGuiDataType_S32, &_values[_selectedIndex], stepLow, stepHigh, hex ? "%08X" : nullptr, flags);
			else if constexpr (std::is_same_v<intType, uint32_t>)
				edited = ImGui::InputScalar(_id.c_str(), ImGuiDataType_U32, &_values[_selectedIndex], stepLow, stepHigh, hex ? "%08X" : nullptr, flags);
			else if constexpr (std::is_same_v<intType, int64_t>)
				edited = ImGui::InputScalar(_id.c_str(), hex ? ImGuiDataType_U64 : ImGuiDataType_S64, &_values[_selectedIndex], stepLow, stepHigh, hex ? "%llX" : nullptr, flags);
			else if constexpr (std::is_same_v<intType, uint64_t>)
				edited = ImGui::InputScalar(_id.c_str(), ImGuiDataType_U64, &_values[_selectedIndex], stepLow, stepHigh, hex ? "%llX" : nullptr, flags);
			else
				ImGui::Text("Incorrect data type specified.");

			ImGui::PopItemWidth();

			if (edited)
				callOnValueChangedSlots();

			return edited;
		}

		void SelectByIndex(const uint64_t index)
		{
			isInRange(index);
			_selectedIndex = index;
		}

		void SetSelectedValue(const intType value)
		{
			_values[_selectedIndex] = value;
		}

		void SetValueAt(const intType value, const uint64_t index)
		{
			isInRange(index);
			_values[index] = value;
		}

		intType GetSelectedValue() const
		{
			return _values[_selectedIndex];
		}

		intType GetValueAt(const uint64_t index)
		{
			isInRange(index);
			return _values[index];
		}

		std::vector<intType> GetAll() const
		{
			return _values;
		}

		std::string ToStringSelected(const bool hex = false)
		{
			std::stringstream stream;

			if (hex)
				stream << std::uppercase << std::hex << _values[_selectedIndex];
			else
				stream << _values[_selectedIndex];

			return stream.str();
		}

		void ConnectOnValueChanged(const Slot slot)
		{
			_slotsOnValueChanged.emplace_back(slot);
		}

		void PushBack(const intType value)
		{
			_values.push_back(value);
		}

		void Clear()
		{
			_values.clear();
			_selectedIndex = 0;
			_stepLow = 0;
			_stepHigh = 0;
			_textFlags = ImGuiInputTextFlags_None;
		}

		void DeleteItemAt(const int64_t index)
		{
			isInRange(index);
			_values.erase(_values.begin() + index);

			if (_selectedIndex >= _values.size())
				_selectedIndex = _values.size() - 1;
		}

	private:
		std::vector<intType> _values;
		uint64_t _selectedIndex = 0;
		uint64_t _stepLow = 0;
		uint64_t _stepHigh = 0;
		ImGuiInputTextFlags _textFlags = ImGuiInputTextFlags_None;
		std::vector<Slot> _slotsOnValueChanged{};

		void assign(const InputVectorInt& other)
		{
			_values = other._values;
			_helpText = other._helpText;
			_id = other._id;
			_label = other._label;
			_printLabel = other._printLabel;
			_showHelpText = other._showHelpText;
			_textFlags = other._textFlags;
			_stepLow = other._stepLow;
			_stepHigh = other._stepHigh;
			_slotsOnValueChanged = other._slotsOnValueChanged;
			_selectedIndex = other._selectedIndex;
		}

		void callOnValueChangedSlots()
		{
			for (Slot& slot : _slotsOnValueChanged)
				slot();
		}

		void isInRange(const uint64_t index)
		{
			if (index >= _values.size())
				throw "Error: Index out of range!";
		}
	};
}