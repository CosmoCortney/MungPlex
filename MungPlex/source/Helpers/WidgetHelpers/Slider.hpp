#pragma once
#include "imgui.h"
#include "IWidget.hpp"

namespace MungPlex
{
	template <typename DataType> class Slider : public IWidget
	{
	public:
		enum SliderFlags
		{
			NONE = 0,
			SCIENTIFIC = 1,
			HEX = 1 << 1
		};

		typedef std::function<void()> Slot;
		Slider() = default;
		Slider(const std::string& label, const bool printLabel = true, const DataType value = 0, const DataType boundaryLow = -1, const DataType boundaryHigh = 1, const ImGuiSliderFlags sliderFlags = ImGuiSliderFlags_None)
			: IWidget(label, printLabel)
		{
			_val = value;
			_boundaryLow = boundaryLow;
			_boundaryHigh = boundaryHigh;
			_sliderFlags = sliderFlags;

			if constexpr (std::is_same_v<DataType, char> || std::is_same_v<DataType, int8_t> || std::is_same_v<DataType, uint8_t>)
			{
				_labelIntDec = "%d";
				_labelIntHex = "%02X";
			}
			else if constexpr (std::is_same_v<DataType, uint8_t>)
			{
				_labelIntDec = "%u";
				_labelIntHex = "%02X";
			}
			else if constexpr (std::is_same_v<DataType, int16_t>)
			{
				_labelIntDec = "%d";
				_labelIntHex = "%04X";
			}
			else if constexpr (std::is_same_v<DataType, uint16_t>)
			{
				_labelIntDec = "%u";
				_labelIntHex = "%04X";
			}
			else if constexpr (std::is_same_v<DataType, int32_t>)
			{
				_labelIntDec = "%d";
				_labelIntHex = "%08X";
			}
			else if constexpr (std::is_same_v<DataType, uint32_t>)
			{
				_labelIntDec = "%u";
				_labelIntHex = "%08X";
			}
			else if constexpr (std::is_same_v<DataType, int64_t>)
			{
				_labelIntDec = "%d";
				_labelIntHex = "%llX";
			}
			else if constexpr (std::is_same_v<DataType, uint64_t>)
			{
				_labelIntDec = "%u";
				_labelIntHex = "%llX";
			}
		}

		Slider(const Slider& other)
		{
			assign(other);
		}

		Slider& operator=(const Slider& other)
		{
			assign(other);
			return *this;
		}

		Slider(Slider&& other) noexcept
		{
			assign(other);
		}

		Slider& operator=(Slider&& other) noexcept
		{
			assign(other);
			return *this;
		}

		bool Draw(const float paneWidth = 0.25f, const float labelPortion = 0.4f, const uint32_t sliderFlags = 0)
		{
			static bool edited = false;
			DrawLabel(_label.c_str(), paneWidth, labelPortion, _printLabel, _showHelpText ? _helpText.c_str() : nullptr);

			if constexpr (std::is_same_v<DataType, float>)
				edited = ImGui::SliderScalar(_id.c_str(), ImGuiDataType_Float, &_val, &_boundaryLow, &_boundaryHigh, (sliderFlags & SCIENTIFIC) == SCIENTIFIC ? _scientific.c_str() : _decimal.c_str(), _sliderFlags);
			else if constexpr (std::is_same_v<DataType, double>)
				edited = ImGui::SliderScalar(_id.c_str(), ImGuiDataType_Double, &_val, &_boundaryLow, &_boundaryHigh, (sliderFlags & SCIENTIFIC) == SCIENTIFIC ? _scientific.c_str() : _decimal.c_str(), _sliderFlags);
			else if constexpr (std::is_same_v<DataType, char> || std::is_same_v<DataType, int8_t>)
				edited = ImGui::SliderScalar(_id.c_str(),
					(sliderFlags & HEX) == HEX ? ImGuiDataType_U8 : ImGuiDataType_S8, &_val,
					&_boundaryLow, &_boundaryHigh,
					(sliderFlags & HEX) == HEX ? _labelIntHex.c_str() : _labelIntDec.c_str(),
					_sliderFlags); 
			else if constexpr (std::is_same_v<DataType, uint8_t>)
				edited = ImGui::SliderScalar(_id.c_str(), ImGuiDataType_U8, &_val, &_boundaryLow, &_boundaryHigh, (sliderFlags & HEX) == HEX ? _labelIntHex.c_str() : _labelIntDec.c_str(), _sliderFlags);
			else if constexpr (std::is_same_v<DataType, int16_t>)
				edited = ImGui::SliderScalar(_id.c_str(), 
					(sliderFlags & HEX) == HEX ? ImGuiDataType_U16 : ImGuiDataType_S16, &_val,
					&_boundaryLow, &_boundaryHigh, 
					(sliderFlags & HEX) == HEX ? _labelIntHex.c_str() : _labelIntDec.c_str(), 
					_sliderFlags);
			else if constexpr (std::is_same_v<DataType, uint16_t>)
				edited = ImGui::SliderScalar(_id.c_str(), ImGuiDataType_U16, &_val, &_boundaryLow, &_boundaryHigh, (sliderFlags & HEX) == HEX ? _labelIntHex.c_str() : _labelIntDec.c_str(), _sliderFlags);
			else if constexpr (std::is_same_v<DataType, int32_t>)
				edited = ImGui::SliderScalar(_id.c_str(),
					(sliderFlags & HEX) == HEX ? ImGuiDataType_U32 : ImGuiDataType_S32, &_val,
					&_boundaryLow, &_boundaryHigh,
					(sliderFlags & HEX) == HEX ? _labelIntHex.c_str() : _labelIntDec.c_str(),
					_sliderFlags); 
			else if constexpr (std::is_same_v<DataType, uint32_t>)
				edited = ImGui::SliderScalar(_id.c_str(), ImGuiDataType_U32, &_val, &_boundaryLow, &_boundaryHigh, (sliderFlags & HEX) == HEX ? _labelIntHex.c_str() : _labelIntDec.c_str(), _sliderFlags);
			else if constexpr (std::is_same_v<DataType, int64_t>)
				edited = ImGui::SliderScalar(_id.c_str(),
					(sliderFlags & HEX) == HEX ? ImGuiDataType_U64 : ImGuiDataType_S64, &_val,
					&_boundaryLow, &_boundaryHigh,
					(sliderFlags & HEX) == HEX ? _labelIntHex.c_str() : _labelIntDec.c_str(),
					_sliderFlags); 
			else if constexpr (std::is_same_v<DataType, uint64_t>)
				edited = ImGui::SliderScalar(_id.c_str(), ImGuiDataType_U64, &_val, &_boundaryLow, &_boundaryHigh, (sliderFlags & HEX) == HEX ? _labelIntHex.c_str() : _labelIntDec.c_str(), _sliderFlags);
			else
				ImGui::Text("Incorrect data type specified.");

			ImGui::PopItemWidth();

			if (edited)
				callOnValueChangedSlots();

			return edited;
		}

		void SetValue(const DataType value)
		{
			_val = value;
		}

		DataType GetValue() const
		{
			return _val;
		}

		void SetLabelDecimal(const std::string& label)
		{
			_decimal = label;
		}

		void SetLabelScientific(const std::string& label)
		{
			_scientific = label;
		}

		void SetLabelIntDec(const std::string& label)
		{
			_labelIntDec = label;
		}

		void SetLabelIntHex(const std::string& label)
		{
			_labelIntHex = label;
		}

		void SetBoundaryLow(const DataType boundaryLow)
		{
			_boundaryLow = boundaryLow;
		}

		void SetBoundaryHigh(const DataType boundaryHigh)
		{
			_boundaryHigh = boundaryHigh;
		}

		void ConnectOnValueChanged(const Slot slot)
		{
			_slotsOnValueChanged.emplace_back(slot);
		}

	private:
		DataType _val = 0.0f;
		DataType _boundaryLow = -1.0f;
		DataType _boundaryHigh = 1.0f;
		std::string _scientific = "%.4e";
		std::string _decimal = "%.4F";
		std::string _labelIntDec = "%u";
		std::string _labelIntHex = "%08X";
		ImGuiSliderFlags _sliderFlags = ImGuiSliderFlags_None;
		std::vector<Slot> _slotsOnValueChanged{};

		void assign(const Slider& other)
		{
			_val = other._val;
			_helpText = other._helpText;
			_id = other._id;
			_label = other._label;
			_printLabel = other._printLabel;
			_showHelpText = other._showHelpText;
			_sliderFlags = other._sliderFlags;
			_boundaryLow = other._boundaryLow;
			_boundaryHigh = other._boundaryHigh;
			_scientific = other._scientific;
			_decimal = other._decimal;
			_labelIntDec = other._labelIntDec;
			_labelIntHex = other._labelIntHex;
			_slotsOnValueChanged = other._slotsOnValueChanged;
		}

		void callOnValueChangedSlots()
		{
			for (Slot& slot : _slotsOnValueChanged)
				slot();
		}
	};
}