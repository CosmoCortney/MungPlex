#pragma once
#include"Search.h"
#include <Windows.h>

MungPlex::Search::Search()
{
	_searchValueTypes.emplace_back("Primitive", PRIMITIVE);
	_searchValueTypes.emplace_back("Array", ARRAY);
	_searchValueTypes.emplace_back("Text", TEXT);
	_searchValueTypes.emplace_back("Color", COLOR);

	_searchPrimitiveTypes.emplace_back("Int 8 (1 Byte)", INT8);
	_searchPrimitiveTypes.emplace_back("Int 16 (2 Bytes)", INT16);
	_searchPrimitiveTypes.emplace_back("Int 32 (4 Bytes)", INT32);
	_searchPrimitiveTypes.emplace_back("Int 64 (8 Bytes)", INT64);
	_searchArrayTypes = _searchPrimitiveTypes;
	_searchPrimitiveTypes.emplace_back("Float Single", FLOAT);
	_searchPrimitiveTypes.emplace_back("Float Double", DOUBLE);

	_searchTextTypes.emplace_back("UTF-8", MorphText::UTF8);
	_searchTextTypes.emplace_back("UTF-16 Little Endian", MorphText::UTF16LE);
	_searchTextTypes.emplace_back("UTF-16 Big Endian", MorphText::UTF16BE);
	_searchTextTypes.emplace_back("UTF-32 Little Endian", MorphText::UTF32LE);
	_searchTextTypes.emplace_back("UTF-32 Big Endian", MorphText::UTF32BE);
	_searchTextTypes.emplace_back("ASCII", MorphText::ASCII);
	_searchTextTypes.emplace_back("ISO-8859-1 (Latin-1)", MorphText::ISO_8859_1);
	_searchTextTypes.emplace_back("ISO-8859-2 (Latin-2)", MorphText::ISO_8859_2);
	_searchTextTypes.emplace_back("ISO-8859-3 (Latin-3)", MorphText::ISO_8859_3);
	_searchTextTypes.emplace_back("ISO-8859-4 (Latin-4)", MorphText::ISO_8859_4);
	_searchTextTypes.emplace_back("ISO-8859-5 (Cyrillic)", MorphText::ISO_8859_5);
	_searchTextTypes.emplace_back("ISO-8859-6 (Arabic)", MorphText::ISO_8859_6);
	_searchTextTypes.emplace_back("ISO-8859-7 (Greek)", MorphText::ISO_8859_7);
	_searchTextTypes.emplace_back("ISO-8859-8 (Hebrew)", MorphText::ISO_8859_8);
	_searchTextTypes.emplace_back("ISO-8859-9 (Turkish, Latin-5)", MorphText::ISO_8859_9);
	_searchTextTypes.emplace_back("ISO-8859-10 (Nordic, Latin-6)", MorphText::ISO_8859_10);
	_searchTextTypes.emplace_back("ISO-8859-11 (Thai)", MorphText::ISO_8859_11);
	_searchTextTypes.emplace_back("ISO-8859-13 (Baltic, Latin-7)", MorphText::ISO_8859_13);
	_searchTextTypes.emplace_back("ISO-8859-14 (Celtic, Latin-8)", MorphText::ISO_8859_14);
	_searchTextTypes.emplace_back("ISO-8859-15 (West European, Latin-9)", MorphText::ISO_8859_15);
	_searchTextTypes.emplace_back("ISO-8859-16 (South-East European, Latin-10)", MorphText::ISO_8859_16);
	_searchTextTypes.emplace_back("Shift-Jis", MorphText::SHIFTJIS);
	_searchTextTypes.emplace_back("JIS x 0201 Full Width", MorphText::JIS_X_0201_FULLWIDTH);
	_searchTextTypes.emplace_back("JIS x 0201 Half Width", MorphText::JIS_X_0201_FULLWIDTH);

	_searchColorTypes.emplace_back("RGB 888 (3 Bytes)", LitColor::RGB888);
	_searchColorTypes.emplace_back("RGBA 8888 (4 Bytes)", LitColor::RGBA8888);
	_searchColorTypes.emplace_back("RGBF (3 Floats)", LitColor::RGBF);
	_searchColorTypes.emplace_back("RGBAF (4 Floats)", LitColor::RGBAF);
	_searchColorTypes.emplace_back("RGB 565 (2 Bytes)", LitColor::RGB565);
	_searchColorTypes.emplace_back("RGB 5A3 (2 Bytes)", LitColor::RGB5A3);

	_searchConditionTypes.emplace_back("Equal (==)", MemoryCompare::EQUAL);
	_searchConditionTypesText = _searchConditionTypes;
	_searchConditionTypes.emplace_back("Unequal (!=)", MemoryCompare::UNEQUAL);
	_searchConditionTypesArray = _searchConditionTypes;
	_searchConditionTypes.emplace_back("Greater (>)", MemoryCompare::GREATER);
	_searchConditionTypes.emplace_back("Greater or Equal (>=)", MemoryCompare::GREATER_EQUAL);
	_searchConditionTypes.emplace_back("Lower (<)", MemoryCompare::LOWER);
	_searchConditionTypes.emplace_back("Lower or Equal (<=)", MemoryCompare::LOWER_EQUAL);
	_searchConditionTypesColor = _searchConditionTypes;
	_searchConditionTypes.emplace_back("Increased by", MemoryCompare::INCREASED_BY);
	_searchConditionTypes.emplace_back("Decreased by", MemoryCompare::DECREASED_BY);
	_searchConditionTypes.emplace_back("Value Between", MemoryCompare::BETWEEN);
	_searchConditionTypes.emplace_back("Value Not Between", MemoryCompare::NOT_BETWEEN);
	_searchConditionTypesFloat = _searchConditionTypes;
	_searchConditionTypes.emplace_back("AND (has all true bits)", MemoryCompare::AND);
	_searchConditionTypes.emplace_back("OR (has at least 1 true bit)", MemoryCompare::OR);

	_searchComparasionType.emplace_back("Unknown/Initial", 0);
	_searchComparasionType.emplace_back("Known Value", MemoryCompare::KNOWN);

	_RegionSelectSignalCombo.ConnectOnIndexChanged(Slot_IndexChanged);
	_RegionSelectSignalCombo.ConnectOnItemCountChanged(Slot_ItemCountChanged);
	_RegionSelectSignalCombo.ConnectOnTextChanged(Slot_TextChanged);
	_SignalInputTextRangeStart.ConnectOnTextChanged(std::bind(Slot_RangeTextChanged, _rangeStartText.Data(), std::ref(_rangeStartValue)));
	_SignalInputTextRangeEnd.ConnectOnTextChanged(std::bind(Slot_RangeTextChanged, _rangeEndText.Data(), std::ref(_rangeEndValue)));

	_selectedIndices.resize(_maxResultsPerPage);
	_alignmentValue = Settings::GetSearchSettings().DefaultAlignment;
	_cached = Settings::GetSearchSettings().DefaultCached;
	_caseSensitive = Settings::GetSearchSettings().DefaultCaseSensitive;
	_useColorWheel = Settings::GetSearchSettings().DefaultColorWheel;
	_hex = Settings::GetSearchSettings().DefaultValuesHex;
	_resultsPath = MorphText::Utf8_To_Utf16LE(Settings::GetGeneralSettings().DocumentsPath) + L"\\MungPlex\\Search\\";
}

void MungPlex::Search::DrawWindow()
{
	ImGui::Begin("Search");

	if (!Connection::IsConnected())
		ImGui::BeginDisabled();

	ImGui::BeginGroup();
	{
		GetInstance().DrawValueTypeOptions();
		GetInstance().DrawRangeOptions();
	}
	ImGui::EndGroup();

	ImGui::SameLine();
	
	GetInstance().DrawSearchOptions();
	GetInstance().DrawResultsArea();

	if (!Connection::IsConnected())
		ImGui::EndDisabled();

	ImGui::End();
}

void MungPlex::Search::DrawValueTypeOptions()
{
	const ImVec2 childXY = { ImGui::GetContentRegionAvail().x * 0.5f, ImGui::GetContentRegionAvail().y * 0.19f };

	ImGui::BeginChild("child_valueOptions", childXY, true);
	{
		ImGui::SeparatorText("Value Type Options");

		if (_searchActive) ImGui::BeginDisabled();
		{
			ImGui::BeginGroup();
			{
				if (MungPlex::SetUpCombo("Value Type:", _searchValueTypes, _currentValueTypeSelect, 0.5f, 0.4f))
					_updateLabels = true;

				_disableBecauseNoPrimitive = _currentValueTypeSelect != PRIMITIVE;
				_disableBecauseNoArray = _currentValueTypeSelect != ARRAY;
				_disableBecauseNoColor = _currentValueTypeSelect != COLOR;
				_disableBecauseNoText = _currentValueTypeSelect != TEXT;
				_disableBecauseNoInt = (!_disableBecauseNoPrimitive && _currentPrimitiveTypeSelect > INT64)
					|| (!_disableBecauseNoArray && _currentArrayTypeSelect > INT64)
					|| !_disableBecauseNoColor
					|| !_disableBecauseNoText;

				if (_currentValueTypeSelect == TEXT || _currentValueTypeSelect == COLOR)
					_currentComparisionTypeSelect = MemoryCompare::KNOWN;

				ImGui::SameLine();

				ImGui::Checkbox("Big Endian", &_underlyingBigEndian);

				if (_disableBecauseNoPrimitive) ImGui::BeginDisabled();
				{
					if (SetUpCombo("Primitive Type:", _searchPrimitiveTypes, _currentPrimitiveTypeSelect, 0.5f, 0.4f))
						setRecommendedValueSettings(PRIMITIVE);
				}
				if (_disableBecauseNoPrimitive) ImGui::EndDisabled();

				ImGui::SameLine();

				if (_disableBecauseNoInt) ImGui::BeginDisabled();
					ImGui::Checkbox("Signed", &_signed);
				if (_disableBecauseNoInt) ImGui::EndDisabled();

				if (_disableBecauseNoArray) ImGui::BeginDisabled();
				{
					if (SetUpCombo("Array Type:", _searchArrayTypes, _currentArrayTypeSelect, 0.5f, 0.4f)) //use primitived types here once Arrays support floats
						setRecommendedValueSettings(ARRAY);
				}
				if (_disableBecauseNoArray) ImGui::EndDisabled();

				if (_disableBecauseNoText) ImGui::BeginDisabled();
				{
					if (SetUpCombo("Text Type:", _searchTextTypes, _currentTextTypeSelect, 0.5f, 0.4f))
						setRecommendedValueSettings(TEXT);
				}
				if (_disableBecauseNoText) ImGui::EndDisabled();

				if (_disableBecauseNoColor) ImGui::BeginDisabled();
				{
					if (SetUpCombo("Color Type:", _searchColorTypes, _currentColorTypeSelect, 0.5f, 0.4f))
					{
						setRecommendedValueSettings(COLOR);

						if (_currentColorTypeSelect != LitColor::RGB5A3)
							_forceAlpha = false;
					}
				}
				if (_disableBecauseNoColor) ImGui::EndDisabled();

				ImGui::SameLine();

				if (_disableBecauseNoColor || _currentColorTypeSelect != LitColor::RGB5A3) ImGui::BeginDisabled();
					ImGui::Checkbox("Force Alpha", &_forceAlpha);
				if (_disableBecauseNoColor || _currentColorTypeSelect != LitColor::RGB5A3) ImGui::EndDisabled();
			}
			ImGui::EndGroup();
		}
		if (_searchActive) ImGui::EndDisabled();
	}
	ImGui::EndChild();
}

void MungPlex::Search::DrawRangeOptions()
{
	const ImVec2 childXY = { ImGui::GetContentRegionAvail().x * 0.5f, ImGui::GetContentRegionAvail().y * 0.175f };

	ImGui::BeginChild("child_rangeOptions", childXY, true);
	{
		ImGui::SeparatorText("Range Options");

		if (_searchActive) ImGui::BeginDisabled();
		{
			ImGui::BeginGroup();
			{
				static std::stringstream stream;
				_regions = ProcessInformation::GetSystemRegionList();
				_RegionSelectSignalCombo.Draw("Region:", _regions, _currentRegionSelect, 0.5f, 0.4f);

				if(_SignalInputTextRangeStart.Draw("Start at (hex):", _rangeStartText.Data(), _rangeStartText.Size(), 0.5f, 0.4f))
				{
					stream << _rangeStartText.StdStr();
					stream >> std::hex >> _rangeStartValue;
					stream.str(std::string());
				}

				if(_SignalInputTextRangeEnd.Draw("End at (hex):", _rangeEndText.Data(), _rangeEndText.Size(), 0.5f, 0.4f))
				{
					stream << _rangeEndText.StdStr();
					stream >> std::hex >> _rangeEndValue;
					stream.str(std::string());
				}
			}
			ImGui::EndGroup();

			ImGui::SameLine();

			ImGui::BeginGroup();
			{
				if (ImGui::Checkbox("Cross-Region", &_crossRegion))
				{
					std::string hexStartStr;
					std::string hexEndStr;

					if (_crossRegion)
					{
						hexStartStr = ToHexString(_regions.front().Base, 0);
						hexEndStr = ToHexString(_regions.back().Base + _regions.back().Size - 1, 0);
					}
					else
					{
						hexStartStr = ToHexString(_regions[_currentRegionSelect].Base, 0);
						hexEndStr = ToHexString(_regions[_currentRegionSelect].Base + _regions[_currentRegionSelect].Size - 1, 0);
					}

					_rangeStartText = hexStartStr;
					_rangeEndText = hexEndStr;
				}

				ImGui::Checkbox("Re-reorder Region", &_rereorderRegion);
				ImGui::SameLine();
				HelpMarker("Some emulators like Project64 reorder the emulatoed RAM in 4 byte chunks of the opposite endianness which requires re-reordering before scanning. The best option is auto-selected for you, but it might be helpful to set it manually if you encounter a reordered region or structure on another platform.");
		
				if (ProcessInformation::GetProcessType() != ProcessInformation::NATIVE) ImGui::BeginDisabled();
				{
					static bool refresh = false;

					if (ImGui::Checkbox("Write", ProcessInformation::GetRangeFlagWrite()))
						refresh = true;

					ImGui::SameLine();
					if (ImGui::Checkbox("Exec.", ProcessInformation::GetRangeFlagExecute()))
						refresh = true;

					if(refresh)
						ProcessInformation::RefreshRegionlistPC();

					ImGui::SameLine();
					HelpMarker("If Write AND Exec. are unchecked, only read-only ranges are considered. Otherwise readable ranges are always considered.");

				}
				if (ProcessInformation::GetProcessType() != ProcessInformation::NATIVE) ImGui::EndDisabled();
			}
			ImGui::EndGroup();
		}
		if (_searchActive) ImGui::EndDisabled();
	}
	ImGui::EndChild();
}

void MungPlex::Search::DrawSearchOptions()
{
	const ImVec2 childXY = { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y * 0.334f };

	ImGui::BeginChild("child_searchOptions", childXY, true);
	{
		ImGui::SeparatorText("Search Options");
		static std::string knownPrimaryValueLabel;
		static std::string knownSecondaryValueLabel;
		static bool disablePrimaryValueText = false;
		static bool disableSecondaryValueText = true;
		int iterationCount = MemoryCompare::MemCompare::GetIterationCount();

		_diableBecauseUnknownAndNotRangebased = _currentComparisionTypeSelect == 0 && _currentConditionTypeSelect != MemoryCompare::INCREASED_BY && _currentConditionTypeSelect != MemoryCompare::DECREASED_BY;

		if (_updateLabels)
		{
			switch (_currentValueTypeSelect)
			{
			case ARRAY:
				knownPrimaryValueLabel = "Array Expression:";
				knownSecondaryValueLabel = "Not applicable";
				break;
			case COLOR:
				knownPrimaryValueLabel, "Color Expression:";
				knownSecondaryValueLabel, "Not applicable";
				break;
			case TEXT:
				knownPrimaryValueLabel = "Text Value:";
				knownSecondaryValueLabel = "Not applicable";
				break;
			default: //PRIMITIVE
				if (_currentConditionTypeSelect == MemoryCompare::BETWEEN)
				{
					knownPrimaryValueLabel = "Lowest:";
					knownSecondaryValueLabel = "Highest:";
					disableSecondaryValueText = false;
				}
				else if (_currentConditionTypeSelect == MemoryCompare::NOT_BETWEEN)
				{
					knownPrimaryValueLabel = "Below:";
					knownSecondaryValueLabel = "Above:";
					disableSecondaryValueText = false;
				}
				else if (_currentConditionTypeSelect == MemoryCompare::INCREASED_BY)
				{
					knownPrimaryValueLabel = "Increased by:";
					knownSecondaryValueLabel = "Not applicable";
				}
				else if (_currentConditionTypeSelect == MemoryCompare::DECREASED_BY)
				{
					knownPrimaryValueLabel = "Decreased by:";
					knownSecondaryValueLabel = "Not applicable";
				}
				else
				{
					knownPrimaryValueLabel = "Value:";
					knownSecondaryValueLabel = "Not applicable";
				}
			}
			_updateLabels = false;
		}

		ImGui::BeginGroup();

		if (!iterationCount) ImGui::BeginDisabled();
			SetUpCombo("Counter Iteration:", _iterations, _iterationIndex, 0.5f, 0.4f);
		if (!iterationCount) ImGui::EndDisabled();
		
		if(SetUpInputInt("Alignment:", &_alignmentValue, 1, 1, 0.5f, 0.4f, 0, true, "This value specifies the increment of the next address to be scanned. 1 means that the following value to be scanned is the at the current address + 1. Here are some recommendations for each value type: Int8/Text/Color/Array<Int8> - 1, Int16/Color/Array<Int16> - 2, Int32/Int64/Float/Double/Color/Array<Int32>/Array<Int64> - 4. Systems that use less than 4MBs of RAM (PS1, SNES, MegaDrive, ...) should always consider an alignment of 1, despite the value recommendations."))
		{
			if (_alignmentValue < 1)
				_alignmentValue = 1;
		}

		if(!_disableBecauseNoText || !_disableBecauseNoColor) ImGui::BeginDisabled();
		{
			if (MungPlex::SetUpCombo("Comparision Type:", _searchComparasionType, _currentComparisionTypeSelect, 0.5f, 0.4f))
				_updateLabels = true;
		}
		if (!_disableBecauseNoText || !_disableBecauseNoColor) ImGui::EndDisabled();

		std::vector<std::pair<std::string, int>>* conditionTypeItems;

		switch (_currentValueTypeSelect)
		{
		case ARRAY:
			conditionTypeItems = &_searchConditionTypesArray;
		break;
		case COLOR:
			conditionTypeItems = &_searchConditionTypesColor;
		break;
		case TEXT:
			conditionTypeItems = &_searchConditionTypesText;
		break;
		default: //PRIMITIVE
			if (_currentPrimitiveTypeSelect < FLOAT)
				conditionTypeItems = &_searchConditionTypes;
			else
				conditionTypeItems = &_searchConditionTypesFloat;
		}

		if (!_disableBecauseNoText) ImGui::BeginDisabled();
		{
			if (SetUpCombo("Condition Type:", *conditionTypeItems, _currentConditionTypeSelect, 0.5f, 0.4f))
				_updateLabels = true;
		}
		if (!_disableBecauseNoText) ImGui::EndDisabled();

		if (_diableBecauseUnknownAndNotRangebased) ImGui::BeginDisabled();
			SetUpInputText(knownPrimaryValueLabel, _knownValueText.Data(), _knownValueText.Size(), 0.5f, 0.4f);
		if (_diableBecauseUnknownAndNotRangebased) ImGui::EndDisabled();

		if (_disableBecauseNoPrimitive || (!_disableBecauseNoPrimitive && (_currentConditionTypeSelect < MemoryCompare::BETWEEN || _currentConditionTypeSelect > MemoryCompare::NOT_BETWEEN))) ImGui::BeginDisabled();
			SetUpInputText(knownSecondaryValueLabel, _secondaryKnownValueText.Data(), _secondaryKnownValueText.Size(), 0.5f, 0.4f);
		if (_disableBecauseNoPrimitive || (!_disableBecauseNoPrimitive && (_currentConditionTypeSelect < MemoryCompare::BETWEEN || _currentConditionTypeSelect > MemoryCompare::NOT_BETWEEN))) ImGui::EndDisabled();

		if (!_disableBecauseNoInt || !_disableBecauseNoText) ImGui::BeginDisabled();
			SetUpSliderFloat("Precision (%%):", &_precision, 75.0f, 100.0f, "%0.2f", 0.5f, 0.4f);
		if (!_disableBecauseNoInt || !_disableBecauseNoText) ImGui::EndDisabled();
		
		if (iterationCount) ImGui::BeginDisabled();
			ImGui::Checkbox("Cached", &GetInstance()._cached);
		if (iterationCount) ImGui::EndDisabled();

		ImGui::SameLine();
		ImGui::Checkbox("Values are hex", &GetInstance()._hex);
		ImGui::SameLine();

		_disableBecauseNoText = _currentValueTypeSelect != TEXT;

		if (_disableBecauseNoText) ImGui::BeginDisabled();
			ImGui::Checkbox("Case Sensitive", &_caseSensitive);
		if (_disableBecauseNoText) ImGui::EndDisabled();
		

		if (ImGui::Button("Search"))
		{
			_searchActive = true;
			PerformSearch();
		}

		ImGui::SameLine();

		if (iterationCount < 1) ImGui::BeginDisabled();
		if (ImGui::Button("Reset"))
		{
			MemoryCompare::MemCompare::Reset();
			_iterations.clear();
			_iterationIndex = 0;
			_searchActive = false;
		}

		if (iterationCount < 1) ImGui::EndDisabled();

		ImGui::EndGroup();

		ImGui::SameLine();

		ImGui::BeginGroup();
		{
			if (_disableBecauseNoColor)
				ImGui::BeginDisabled();
			
			int colorPickerFlags = ImGuiColorEditFlags_NoOptions;
			colorPickerFlags |= _useColorWheel ? ImGuiColorEditFlags_PickerHueWheel : ImGuiColorEditFlags_PickerHueBar;

			switch (_currentColorTypeSelect)
			{
			case LitColor::RGBA8888:
				colorPickerFlags |= ImGuiColorEditFlags_Uint8 | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview;
				break;
			case LitColor::RGBF:
				colorPickerFlags |= ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoAlpha;
				break;
			case LitColor::RGBAF:
				colorPickerFlags |= ImGuiColorEditFlags_Float | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview;
				break;
			default: //RGB888, RGB565, RGB5A3
				if(_forceAlpha)
					colorPickerFlags |= ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview;
				else
					colorPickerFlags |= ImGuiColorEditFlags_Uint8 | ImGuiColorEditFlags_NoAlpha;
			}

			ImGui::PushItemWidth(childXY.x * 0.275f);
			ImGui::ColorPicker4("##ColorPicker", (float*)&_colorVec, colorPickerFlags);
			ImGui::PopItemWidth();

			if (!_disableBecauseNoColor)
				ColorValuesToCString(_colorVec, _currentColorTypeSelect, _knownValueText.Data(), _forceAlpha);


			ImGui::Checkbox("Color Wheel", &_useColorWheel);
			ImGui::SameLine();

			if (ImGui::Button("Pick color from screen"))
			{
				//HWND windowHandle = GetForegroundWindow(); todo: make this work ): 
				_colorVec = PickColorFromScreen();
				//MungPlex::SetWindowToForeground(windowHandle);
			}

			if (_disableBecauseNoColor)
				ImGui::EndDisabled();
		}
		ImGui::EndGroup();
	}
	ImGui::EndChild();
}

void MungPlex::Search::DrawResultsArea()
{
	const ImVec2 childXY = ImGui::GetContentRegionAvail();
	ImGui::BeginChild("child_searchResults", childXY, true);
	{
		ImGui::SeparatorText("Results Panel");

		drawResultsTableNew();

		ImGui::SameLine();

		ImGui::BeginGroup();
		{
			ImGui::SeparatorText("Value Poke");

			if (MemoryCompare::MemCompare::GetResultCount() == 0) ImGui::BeginDisabled();
			{
				SetUpLableText("Results:", std::to_string(MemoryCompare::MemCompare::GetResultCount()).c_str(), 32, 1.0f, 0.2f);

				ImGui::BeginGroup();
				{
					SetUpInputText("Address:", _pokeAddressText.Data(), _pokeAddressText.Size(), 1.0f, 0.2f);
					SetUpInputText("Value:", _pokeValueText.Data(), _pokeValueText.Size(), 1.0f, 0.2f);
				}
				ImGui::EndGroup();

				ImGui::BeginGroup();
				{
					SetUpInputInt("Page:", &_currentPageValue, 1, 10, 0.5f, 0.4f);
					{
						if (_currentPageValue < 1)
							_currentPageValue = _pagesAmountValue;
						else if (_currentPageValue > _pagesAmountValue && _pagesAmountValue > 0)
							_currentPageValue = 1;

						if (_currentPageValue == _pagesAmountValue)
							_deselectedIllegalSelection = true;
					}

					ImGui::SameLine();
					SetUpLableText("Of", _pagesAmountText.c_str(), _pagesAmountText.size(), 1.0f, 0.125f);
				}
				ImGui::EndGroup();

				if (!_disableBecauseNoText) ImGui::BeginDisabled();
				{
					ImGui::Checkbox("Previous Value", &_pokePrevious);
				}
				if (!_disableBecauseNoText) ImGui::EndDisabled();

				ImGui::SameLine();

				HelpMarker("If \"Multi-Poke\" is checked this will enable poking previous values - no matter what's in the \"Value\" text field. If this is unchecked the expression inside \"Value\" will be written to all selected result addresses.");

				ImGui::SameLine();

				ImGui::Checkbox("Multi-Poke", &_multiPoke);

				if (ImGui::Button("Poke"))
				{
					_pokeValue.clear();
					static std::stringstream stream;

					if (!_multiPoke)
					{
						stream << std::hex << std::string(_pokeAddressText.CStr());
						stream >> _pokeAddress;
						stream.str(std::string());
					}

					switch (_currentValueTypeSelect)
					{
					case ARRAY: {
						switch (_currentArrayTypeSelect)
						{
						case INT8: {
							if (ProcessInformation::GetAddressWidth() > 4)
								PokeArray<uint8_t, uint64_t>();
							else
								PokeArray<uint8_t, uint32_t>();
						} break;
						case INT16: {
							if (ProcessInformation::GetAddressWidth() > 4)
								PokeArray<uint16_t, uint64_t>();
							else
								PokeArray<uint16_t, uint32_t>();
						} break;
						case INT64: {
							if (ProcessInformation::GetAddressWidth() > 4)
								PokeArray<uint64_t, uint64_t>();
							else
								PokeArray<uint64_t, uint32_t>();
						} break;
						case FLOAT: {
							if (ProcessInformation::GetAddressWidth() > 4)
								PokeArray<float, uint64_t>();
							else
								PokeArray<float, uint32_t>();
						} break;
						case DOUBLE: {
							if (ProcessInformation::GetAddressWidth() > 4)
								PokeArray<double, uint64_t>();
							else
								PokeArray<double, uint32_t>();
						} break;
						default: { //OperativeArray<INT32>
							if (ProcessInformation::GetAddressWidth() > 4)
								PokeArray<uint32_t, uint64_t>();
							else
								PokeArray<uint32_t, uint32_t>();
						} break;
						}break;
					} break;
					case TEXT: {
						if (ProcessInformation::GetAddressWidth() > 4)
							PokeText<uint64_t>();
						else
							PokeText<uint32_t>();
					} break;
					case COLOR: {
						if (ProcessInformation::GetAddressWidth() > 4)
							PokeColor<uint64_t>();
						else
							PokeColor<uint32_t>();
					} break;
					default: { //PRIMITIVE
						int64_t tempVal;

						if (_hex && _currentPrimitiveTypeSelect < FLOAT)
						{
							stream << std::hex << _pokeValueText.CStr();
							stream >> tempVal;
						}
						else
							tempVal = std::stoll(_pokeValueText.CStr());

						switch (_currentPrimitiveTypeSelect)
						{
						case INT8:
							_pokeValue.insert(_pokeValue.end(), reinterpret_cast<char*>(&tempVal), reinterpret_cast<char*>(&tempVal) + 1);
							ProcessInformation::GetAddressWidth() > 4 ? PokeValue<uint8_t, uint64_t>() : PokeValue<uint8_t, uint32_t>();
							break;
						case INT16:
							_pokeValue.insert(_pokeValue.end(), reinterpret_cast<char*>(&tempVal), reinterpret_cast<char*>(&tempVal) + 2);
							ProcessInformation::GetAddressWidth() > 4 ? PokeValue<uint16_t, uint64_t>() : PokeValue<uint16_t, uint32_t>();
							break;
						case INT64:
							_pokeValue.insert(_pokeValue.end(), reinterpret_cast<char*>(&tempVal), reinterpret_cast<char*>(&tempVal) + 1);
							ProcessInformation::GetAddressWidth() > 4 ? PokeValue<uint64_t, uint64_t>() : PokeValue<uint64_t, uint32_t>();
							break;
						case FLOAT:
						{
							float temp = std::stof(_pokeValueText.CStr());
							_pokeValue.insert(_pokeValue.end(), reinterpret_cast<char*>(&temp), reinterpret_cast<char*>(&temp) + 4);
							ProcessInformation::GetAddressWidth() > 4 ? PokeValue<float, uint64_t>() : PokeValue<float, uint32_t>();
						}   break;
						case DOUBLE:
						{
							double temp = std::stod(_pokeValueText.CStr());
							_pokeValue.insert(_pokeValue.end(), reinterpret_cast<char*>(&temp), reinterpret_cast<char*>(&temp) + 8);
							ProcessInformation::GetAddressWidth() > 4 ? PokeValue<double, uint64_t>() : PokeValue<double, uint32_t>();
						}
						break;
						default:
							_pokeValue.insert(_pokeValue.end(), reinterpret_cast<char*>(&tempVal), reinterpret_cast<char*>(&tempVal) + 4);
							ProcessInformation::GetAddressWidth() > 4 ? PokeValue<uint32_t, uint64_t>() : PokeValue<uint32_t, uint32_t>();
							break;
						}
					}
					}
				}
			}
			if (MemoryCompare::MemCompare::GetResultCount() == 0) ImGui::EndDisabled();

			ImGui::Dummy(ImVec2(0.0f, ImGui::GetContentRegionAvail().y - 40.f));

			if (MemoryCompare::MemCompare::GetIterationCount() > 0) ImGui::BeginDisabled();
			{
				SetUpInputInt("Max. results per page:", &_maxResultsPerPage, 32, 128, 1.0f, 0.5f);
			}
			if (MemoryCompare::MemCompare::GetIterationCount() > 0) ImGui::EndDisabled();
		}
		ImGui::EndGroup();

	}
	ImGui::EndChild();
}

void MungPlex::Search::PerformSearch()
{
	Log::LogInformation("Search: Iteration " + std::to_string(MemoryCompare::MemCompare::GetIterationCount() + 1));

	switch (_currentValueTypeSelect)
	{
	case ARRAY:
		arrayTypeSearchLog();
		break;
	case TEXT:
		textTypeSearchLog();
		break;
	case COLOR:
		colorTypeSearchLog();
		break;
	default:
		primitiveTypeSearchLog();
		break;
	}

	if(_currentValueTypeSelect != PRIMITIVE)
		_currentComparisionTypeSelect = MemoryCompare::KNOWN;

	SetUpAndIterate();
	setUpIterationSelect();
	setUpResultPaging();
}

void MungPlex::Search::setUpIterationSelect()
{
	int iter = MemoryCompare::MemCompare::GetIterationCount();

	if (iter < _iterations.size())
		_iterations.erase(_iterations.begin() + iter - 1, _iterations.end());

	_iterations.emplace_back(std::to_string(iter).c_str());
	_iterationIndex = --iter;
	_selectedIndices.resize(_maxResultsPerPage);
}

void MungPlex::Search::setUpResultPaging()
{
	_pagesAmountValue = MemoryCompare::MemCompare::GetResultCount() / _maxResultsPerPage;

	if (MemoryCompare::MemCompare::GetResultCount() % _maxResultsPerPage > 0)
		++_pagesAmountValue;

	_pagesAmountText = std::to_string(_pagesAmountValue);
}

void MungPlex::Search::primitiveTypeSearchLog()
{
	Log::LogInformation("Primitive: ", true, 4);

	if (_currentPrimitiveTypeSelect < FLOAT)
	{
		if (_signed)
			Log::LogInformation("signed", true);
		else
			Log::LogInformation("unsigned", true);
	}

	switch (_currentPrimitiveTypeSelect)
	{
	case INT8:
		Log::LogInformation("int 8.", true);
		break;
	case INT16:
		Log::LogInformation("int 16.", true);
		break;
	case INT64:
		Log::LogInformation("int 64.", true);
		break;
	case FLOAT:
		Log::LogInformation("float single", true);
		break;
	case DOUBLE:
		Log::LogInformation("float double", true);
		break;
	default: //INT32
		Log::LogInformation("int 32.", true);
	}

	Log::LogInformation("Comparision Type:", true, 4);

	if (_currentComparisionTypeSelect == MemoryCompare::KNOWN)
	{
		Log::LogInformation("Known: " + _knownValueText.StdStr() + ", " + _secondaryKnownValueText.StdStr(), true);
	}
	else
		Log::LogInformation("Unknown", true);
}

void MungPlex::Search::arrayTypeSearchLog()
{
	Log::LogInformation("Array<" + _searchArrayTypes[_currentArrayTypeSelect].first + ">: " + _knownValueText.StdStr(), true, 4);
}

void MungPlex::Search::textTypeSearchLog()
{
	Log::LogInformation("Text<" + _searchTextTypes[_currentTextTypeSelect].first + ">: " + _knownValueText.StdStr(), true, 4);
}

void MungPlex::Search::colorTypeSearchLog()
{
	Log::LogInformation("Text<" + _searchColorTypes[_currentColorTypeSelect].first + ">: " + _knownValueText.StdStr(), true, 4);
}

void MungPlex::Search::drawResultsTableNew()
{
	static ImGuiSelectableFlags selectableFlags = ImGuiSelectableFlags_SpanAllColumns;
	static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

	if (!ImGui::BeginTable("Results", 4, flags, ImVec2(ImGui::GetContentRegionAvail().x * 0.666f, ImGui::GetContentRegionAvail().y)))
		return;

	const uint64_t resultCount = MemoryCompare::MemCompare::GetResultCount();
	ImGui::TableSetupColumn("Address");
	ImGui::TableSetupColumn("Value");
	ImGui::TableSetupColumn("Previous");
	ImGui::TableSetupColumn("Difference (a - b)");
	ImGui::TableHeadersRow();
	const char* literal;
	if (_currentValueTypeSelect == PRIMITIVE)
		literal = GetStringLiteral(_currentPrimitiveTypeSelect, _signed, _hex);
	else
		literal = GetStringLiteral(_currentArrayTypeSelect, _signed, _hex);

	for (int row = 0; row < resultCount; ++row)
	{
		if (row >= _maxResultsPerPage)
			break;

		if (row >= resultCount)
			break;

		if (_currentPageValue > _pagesAmountValue)
			break;

		
		if (_currentPageValue == _pagesAmountValue)
		{
			static uint32_t lastPageResultCount = resultCount % _maxResultsPerPage;
			if (lastPageResultCount == 0)
				;
			else if (row >= lastPageResultCount)
				break;

			if(_deselectedIllegalSelection && lastPageResultCount != 0)
				for (int i = lastPageResultCount; i < _maxResultsPerPage; ++i)
					_selectedIndices[i] = false;

			_deselectedIllegalSelection = false;
		}

		static FloorString tempAddress("", 18);
		static FloorString tempValue("", 256);
		static FloorString buf("", 256);
		static int addressTextWidth = ProcessInformation::GetAddressWidth() > 4 ? 16 : 8;
		static uint64_t pageIndex;
		pageIndex = (_currentPageValue - 1) * _maxResultsPerPage;
		uint64_t pageIndexWithRowCount = pageIndex + row;
		static uint64_t address;

		switch (ProcessInformation::GetAddressWidth())
		{
		case 1:
			address = MemoryCompare::MemCompare::GetResults().GetAddressAllRanges<uint8_t>(pageIndexWithRowCount);
			break;
		case 2:
			address = MemoryCompare::MemCompare::GetResults().GetAddressAllRanges<uint16_t>(pageIndexWithRowCount);
			break;
		case 8:
			address = MemoryCompare::MemCompare::GetResults().GetAddressAllRanges<uint64_t>(pageIndexWithRowCount);
			break;
		default: //4
			address = MemoryCompare::MemCompare::GetResults().GetAddressAllRanges<uint32_t>(pageIndexWithRowCount);
		}

		bool rowClicked = false;
		uint64_t resultsIndex = (pageIndex + row);
		ImGui::TableNextRow(selectableFlags);
		uint16_t iterationCount = MemoryCompare::MemCompare::GetIterationCount();

		for (int col = 0; col < 4; ++col)
		{
			ImGui::TableSetColumnIndex(col);

			if (col > 0)
			{
				switch (_currentValueTypeSelect)
				{
				case ARRAY:
				{
					static uint32_t itemCount;
					itemCount = MemoryCompare::MemCompare::GetResults().GetValueWidth();

					switch (_currentArrayTypeSelect)
					{
					case INT16:
						itemCount >>= 1;
						break;
					case INT32: case FLOAT:
						itemCount >>= 2;
						break;
					case INT64: case DOUBLE:
						itemCount >>= 3;
						break;
					}


					//uint64_t resultIndexWithItemCount = resultsIndex * itemCount;

					switch (_currentArrayTypeSelect)
					{
					case INT8:
						DrawArrayValues<uint8_t>(col, itemCount, pageIndexWithRowCount, buf.Data(), tempValue.Data(), literal);
						break;
					case INT16:
						DrawArrayValues<uint16_t>(col, itemCount, pageIndexWithRowCount, buf.Data(), tempValue.Data(), literal);
						break;
					case INT32:
						DrawArrayValues<uint32_t>(col, itemCount, pageIndexWithRowCount, buf.Data(), tempValue.Data(), literal);
						break;
					case INT64:
						DrawArrayValues<uint64_t>(col, itemCount, pageIndexWithRowCount, buf.Data(), tempValue.Data(), literal);
						break;
						/*case FLOAT:
							DrawArrayValues<float>(col, itemCount, resultsIndex, buf, tempValue, literal);
						break;
						case DOUBLE:
							DrawArrayValues<double>(col, itemCount, resultsIndex, buf, tempValue, literal);
						break;*/
					}
				}
				break;
				case COLOR:
				{
					static ImU32 rectColor;
					static ImVec4 vecCol;

					switch (_currentColorTypeSelect)
					{
					case LitColor::RGB565:
					{
						static uint16_t rgb565;

						if (col == 1)
						{
							rgb565 = MemoryCompare::MemCompare::GetResults().GetValueAllRanges<uint16_t>(pageIndexWithRowCount);
							rectColor = LitColor::RGB565ToRGB888(rgb565);

							if (!_pokePrevious)
								vecCol = PackedColorToImVec4(reinterpret_cast<uint8_t*>(&rectColor));
						}
						else if (col == 2)
						{
							rgb565 = iterationCount < 2 ? 0 : MemoryCompare::MemCompare::GetResults().GetPreviousValueAllRanges<uint16_t>(pageIndexWithRowCount);
							rectColor = LitColor::RGB565ToRGB888(rgb565);

							if (_pokePrevious)
								vecCol = PackedColorToImVec4(reinterpret_cast<uint8_t*>(&rectColor));
						}
						else
							break;
					}
					break;
					case LitColor::RGB5A3:
					{
						static uint16_t rgb5A3;

						if (col == 1)
						{
							rgb5A3 = MemoryCompare::MemCompare::GetResults().GetValueAllRanges<uint16_t>(pageIndexWithRowCount);
							rectColor = _forceAlpha ? LitColor::RGB5A3ToRGBA8888(rgb5A3) : LitColor::RGB5A3ToRGB888(rgb5A3) | 0xFF;

							if (!_pokePrevious)
								vecCol = PackedColorToImVec4(reinterpret_cast<uint8_t*>(&rectColor));
						}
						else if (col == 2)
						{
							rgb5A3 = iterationCount < 2 ? 0 : MemoryCompare::MemCompare::GetResults().GetPreviousValueAllRanges<uint16_t>(pageIndexWithRowCount);
							rectColor = _forceAlpha ? LitColor::RGB5A3ToRGBA8888(rgb5A3) : LitColor::RGB5A3ToRGB888(rgb5A3) | 0xFF;

							if (_pokePrevious)
								vecCol = PackedColorToImVec4(reinterpret_cast<uint8_t*>(&rectColor));
						}
						else
							break;
					}
					break;
					case LitColor::RGBF: case LitColor::RGBAF:
					{
						static bool usesAlpha = _currentColorTypeSelect == LitColor::RGBAF;
						static int colorValueCount = _currentColorTypeSelect == LitColor::RGBAF ? 4 : 3;
						static float* colorPtr;

						if (col == 1)
						{
							colorPtr = MemoryCompare::MemCompare::GetResults().GetSpecificValuePtrAllRanges<float>(pageIndexWithRowCount * colorValueCount);
							rectColor = usesAlpha ? LitColor::RGBAFToRGBA8888(colorPtr) : LitColor::RGBFToRGB888(colorPtr);

							if (!_pokePrevious)
								vecCol = PackedColorToImVec4(reinterpret_cast<uint8_t*>(&rectColor));
						}
						else if (col == 2)
						{
							if (iterationCount < 2)
							{
								rectColor = 0x000000FF;
							}
							else
							{
								colorPtr = MemoryCompare::MemCompare::GetResults().GetSpecificPreviousValuePtrAllRanges<float>(pageIndexWithRowCount * colorValueCount);
								rectColor = usesAlpha ? LitColor::RGBAFToRGBA8888(colorPtr) : LitColor::RGBFToRGB888(colorPtr);
							}

							if (_pokePrevious)
								vecCol = PackedColorToImVec4(reinterpret_cast<uint8_t*>(&rectColor));
						}
						else
							break;
					}
					break;
					default: //RGB888, RGBA8888
					{
						if (col == 1)
						{
							rectColor = MemoryCompare::MemCompare::GetResults().GetValueAllRanges<uint32_t>(pageIndexWithRowCount);
							if (_currentColorTypeSelect == LitColor::RGB888)
								rectColor |= 0xFF;

							if (!_pokePrevious)
								vecCol = PackedColorToImVec4(reinterpret_cast<uint8_t*>(&rectColor));
						}
						else if (col == 2)
						{
							rectColor = iterationCount < 2 ? 0 : MemoryCompare::MemCompare::GetResults().GetValueAllRanges<uint32_t>(pageIndexWithRowCount);
							
							if (_currentColorTypeSelect == LitColor::RGB888)
								rectColor |= 0xFF;

							if (_pokePrevious)
								vecCol = PackedColorToImVec4(reinterpret_cast<uint8_t*>(&rectColor));
						}
						else
							break;
					}
					}

					ColorValuesToCString(vecCol, _currentColorTypeSelect, buf.Data(), _forceAlpha);
					tempValue = buf;
					buf = "";
					ImDrawList* drawList = ImGui::GetWindowDrawList();
					ImVec2 rectMin = ImGui::GetCursorScreenPos();
					ImVec2 rectMax = ImVec2(rectMin.x + 124, rectMin.y + 30);
					drawList->AddRectFilled(rectMin, rectMax, Xertz::SwapBytes<uint32_t>(rectColor));
				}
				break;
				case TEXT: {
					static int strLength = 0;
					if (!strLength)
						strLength = MemoryCompare::MemCompare::GetResults().GetValueWidth();

					pageIndexWithRowCount *= strLength;

					if (col == 1)
					{
						static std::string temputf8;

						switch (_currentTextTypeSelect)
						{
						case MorphText::ASCII: {
							temputf8 = MorphText::ASCII_To_Utf8(MemoryCompare::MemCompare::GetResults().GetSpecificValuePtrAllRanges<char>(pageIndexWithRowCount));
						} break;
						case MorphText::SHIFTJIS: {
							temputf8 = MorphText::ShiftJis_To_Utf8(MemoryCompare::MemCompare::GetResults().GetSpecificValuePtrAllRanges<char>(pageIndexWithRowCount));
						} break;
						case MorphText::JIS_X_0201_FULLWIDTH: {
							temputf8 = MorphText::JIS_X_0201_FullWidth_To_Utf8(MemoryCompare::MemCompare::GetResults().GetSpecificValuePtrAllRanges<char>(pageIndexWithRowCount));
						} break;
						case MorphText::JIS_X_0201_HALFWIDTH: {
							temputf8 = MorphText::JIS_X_0201_HalfWidth_To_Utf8(MemoryCompare::MemCompare::GetResults().GetSpecificValuePtrAllRanges<char>(pageIndexWithRowCount));
						} break;
						case MorphText::UTF8: {
							temputf8 = MemoryCompare::MemCompare::GetResults().GetSpecificValuePtrAllRanges<char>(pageIndexWithRowCount);
						} break;
						case MorphText::UTF16LE: case MorphText::UTF16BE: {
							temputf8 = _currentTextTypeSelect == MorphText::UTF16BE
								? MorphText::Utf16BE_To_Utf8(MemoryCompare::MemCompare::GetResults().GetSpecificValuePtrAllRanges<wchar_t>(pageIndexWithRowCount))
								: MorphText::Utf16LE_To_Utf8(MemoryCompare::MemCompare::GetResults().GetSpecificValuePtrAllRanges<wchar_t>(pageIndexWithRowCount));
						} break;
						case MorphText::UTF32LE: case MorphText::UTF32BE: {
							temputf8 = _currentTextTypeSelect == MorphText::UTF32BE
								? MorphText::Utf32BE_To_Utf8(MemoryCompare::MemCompare::GetResults().GetSpecificValuePtrAllRanges<char32_t>(pageIndexWithRowCount))
								: MorphText::Utf32LE_To_Utf8(MemoryCompare::MemCompare::GetResults().GetSpecificValuePtrAllRanges<char32_t>(pageIndexWithRowCount));
						} break;
						default: { //ISO-8859-X
							temputf8 = MorphText::ISO8859X_To_Utf8(MemoryCompare::MemCompare::GetResults().GetSpecificValuePtrAllRanges<char>(pageIndexWithRowCount), _currentTextTypeSelect);
						} break;
						}

						sprintf(buf.Data(), "%s", temputf8.c_str());
						tempValue = buf;
					}
					else
						break;
				}
				break;
				default:// PRIMITIVE
				{
					if (_currentPrimitiveTypeSelect < FLOAT)
					{
						static int64_t currentValue;
						static int64_t previousValue;

						switch (_currentPrimitiveTypeSelect)
						{
						case INT8:
						{
							if (_signed)
							{
								currentValue = MemoryCompare::MemCompare::GetResults().GetValueAllRanges<int8_t>(pageIndexWithRowCount);
								previousValue = iterationCount < 2 ? 0 : MemoryCompare::MemCompare::GetResults().GetPreviousValueAllRanges<int8_t>(pageIndexWithRowCount);
							}
							else
							{
								currentValue = MemoryCompare::MemCompare::GetResults().GetValueAllRanges<uint8_t>(pageIndexWithRowCount);
								previousValue = iterationCount < 2 ? 0 : MemoryCompare::MemCompare::GetResults().GetPreviousValueAllRanges<uint8_t>(pageIndexWithRowCount);
							}
						}break;
						case INT16:
						{
							if (_signed)
							{
								currentValue = MemoryCompare::MemCompare::GetResults().GetValueAllRanges<int16_t>(pageIndexWithRowCount);
								previousValue = iterationCount < 2 ? 0 : MemoryCompare::MemCompare::GetResults().GetPreviousValueAllRanges<int16_t>(pageIndexWithRowCount);
							}
							else
							{
								currentValue = MemoryCompare::MemCompare::GetResults().GetValueAllRanges<uint16_t>(pageIndexWithRowCount);
								previousValue = iterationCount < 2 ? 0 : MemoryCompare::MemCompare::GetResults().GetPreviousValueAllRanges<uint16_t>(pageIndexWithRowCount);
							}
						}break;
						case INT64:
						{
							if (_signed)
							{
								currentValue = MemoryCompare::MemCompare::GetResults().GetValueAllRanges<int64_t>(pageIndexWithRowCount);
								previousValue = iterationCount < 2 ? 0 : MemoryCompare::MemCompare::GetResults().GetPreviousValueAllRanges<int64_t>(pageIndexWithRowCount);
							}
							else
							{
								currentValue = MemoryCompare::MemCompare::GetResults().GetValueAllRanges<uint64_t>(pageIndexWithRowCount);
								previousValue = iterationCount < 2 ? 0 : MemoryCompare::MemCompare::GetResults().GetPreviousValueAllRanges<uint64_t>(pageIndexWithRowCount);
							}
						}break;
						default: //INT32
						{
							if (_signed)
							{
								currentValue = MemoryCompare::MemCompare::GetResults().GetValueAllRanges<int32_t>(pageIndexWithRowCount);
								previousValue = iterationCount < 2 ? 0 : MemoryCompare::MemCompare::GetResults().GetPreviousValueAllRanges<int32_t>(pageIndexWithRowCount);
							}
							else
							{
								currentValue = MemoryCompare::MemCompare::GetResults().GetValueAllRanges<uint32_t>(pageIndexWithRowCount);
								previousValue = iterationCount < 2 ? 0 : MemoryCompare::MemCompare::GetResults().GetPreviousValueAllRanges<uint32_t>(pageIndexWithRowCount);
							}
						}break;
						}

						switch (col)
						{
						case 1:
							sprintf(buf.Data(), literal, currentValue);
							if (!_pokePrevious)
								tempValue = buf.Data();
							break;
						case 2:
							sprintf(buf.Data(), literal, previousValue);
							if (_pokePrevious)
								tempValue = buf;
							break;
						case 3:
							sprintf(buf.Data(), literal, currentValue - previousValue);
							break;
						}
					}
					else
					{
						static double currentValue;
						static double previousValue;

						switch (_currentPrimitiveTypeSelect)
						{
						case DOUBLE:
						{
							currentValue = MemoryCompare::MemCompare::GetResults().GetValueAllRanges<double>(pageIndexWithRowCount);
							previousValue = iterationCount < 2 ? 0 : MemoryCompare::MemCompare::GetResults().GetPreviousValueAllRanges<double>(pageIndexWithRowCount);
						}break;
						default: //DOUBLE
						{
							currentValue = MemoryCompare::MemCompare::GetResults().GetValueAllRanges<float>(pageIndexWithRowCount);
							previousValue = iterationCount < 2 ? 0 : MemoryCompare::MemCompare::GetResults().GetPreviousValueAllRanges<float>(pageIndexWithRowCount);
						}
						}

						switch (col)
						{
						case 1:
							sprintf(buf.Data(), literal, currentValue);
							if (!_pokePrevious)
								tempValue = buf;
							break;
						case 2:
							sprintf(buf.Data(), literal, previousValue);
							if (_pokePrevious)
								tempValue = buf;
							break;
						case 3:
							sprintf(buf.Data(), literal, currentValue - previousValue);
							break;
						}
					}
				}
				}
			}
			else
			{
				sprintf(buf.Data(), addressTextWidth == 16 ? "%016llX" : "%08X", address);
				tempAddress = buf;
			}

			if (_selectedIndices[row])
			{
				ImGui::Selectable(buf.Data(), true, selectableFlags);
				for (int i = 0; i < 4; ++i)
				{
					ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImVec4ToPackedColor(ImGui::GetStyle().Colors[ImGuiCol_Header]));
					ImGui::TableSetColumnIndex(i);
				}
			}
			else
			{
				ImGui::Selectable(buf.Data(), false, selectableFlags);
			}

			if (_multiPoke //multi-select
				&& ImGui::IsItemClicked()
				&& (GetAsyncKeyState(VK_LCONTROL) || GetAsyncKeyState(VK_RCONTROL)))
			{
				tempAddress = "Multi-Poke";

				if (_selectedIndices[row] == true)
					_selectedIndices[row] = false;
				else
					_selectedIndices[row] = true;

				rowClicked = true;
			}
			else if (_multiPoke
				&& ImGui::IsItemClicked() //range-select
				&& (GetAsyncKeyState(VK_LSHIFT) || GetAsyncKeyState(VK_RSHIFT)))
			{
				tempAddress = "Multi-Poke";
				int rangeBegin = -1;
				int rangeEnd;

				for (int selIndex = 0; selIndex < _selectedIndices.size(); ++selIndex)
				{
					if (_selectedIndices[selIndex] == true)
					{
						if (selIndex == row)
							break;

						rangeBegin = selIndex;
						break;
					}
				}

				if (rangeBegin == -1)
					break;

				if (rangeBegin > row)
				{
					rangeEnd = rangeBegin;
					rangeBegin = row;
				}
				else
				{
					rangeEnd = row;
				}

				for (int markupIndex = rangeBegin; markupIndex <= rangeEnd; ++markupIndex)
				{
					_selectedIndices[markupIndex] = true;
				}

				rowClicked = true;
			}
			else if (ImGui::IsItemClicked() //single selection and multi-select reset
				&& !(GetAsyncKeyState(VK_LCONTROL) || GetAsyncKeyState(VK_RCONTROL))
				&& !(GetAsyncKeyState(VK_LSHIFT) || GetAsyncKeyState(VK_RSHIFT)))
			{
				for (int selIndex = 0; selIndex < _selectedIndices.size(); ++selIndex)
				{
					_selectedIndices[selIndex] = row == selIndex ? true : false;
				}

				rowClicked = true;
			}
		}

		if (rowClicked)
		{
			_pokeAddressText = tempAddress;
			_pokeValueText = tempValue;
		}
	}

	ImGui::EndTable();
}

void MungPlex::Search::generateDumpRegionMap()
{
	_dumpRegions.clear();

	if (_crossRegion)
	{
		for (int i = 0; i < _regions.size(); ++i)
			emplaceDumpRegion(i);
	}
	else
	{
		emplaceDumpRegion(_currentRegionSelect);
	}
}

void MungPlex::Search::emplaceDumpRegion(const uint16_t index)
{
	uint8_t* currentBaseLocation = reinterpret_cast<uint8_t*>(_regions[index].BaseLocationProcess);
	uint64_t currentBaseAddress = _regions[index].Base;
	uint64_t currentRegionSize = _regions[index].Size;

	if (_crossRegion)
	{
		if (_rangeStartValue >= _regions[index].Base + _regions[index].Size - 1)
			return;

		if (_rangeEndValue <= _regions[index].Base)
			return;

		if (_rangeStartValue > _regions[index].Base)
		{
			currentRegionSize -= _rangeStartValue - currentBaseAddress;
			currentBaseAddress = _rangeStartValue;
			currentBaseLocation += currentBaseAddress - _regions[index].Base;
		}

		if (_rangeEndValue < _regions[index].Base + _regions[index].Size - 1)
			currentRegionSize = _rangeEndValue - currentBaseAddress;
	}
	else // single region
	{
		if (_rangeStartValue >= _regions[index].Base + _regions[index].Size - 1 || _rangeEndValue <= _regions[index].Base)
		{
			_dumpRegions.emplace_back(_regions[index]);
			return;
		}

		if (_rangeStartValue > _regions[index].Base && _rangeStartValue < _regions[index].Base + _regions[index].Size - 1)
		{
			currentBaseAddress = _rangeStartValue;
			currentBaseLocation += currentBaseAddress - _regions[index].Base;
		}

		if (_rangeEndValue > currentBaseAddress && _rangeEndValue < _regions[index].Base + _regions[index].Size - 1)
			currentRegionSize = _rangeEndValue - currentBaseAddress;

		if (_rangeStartValue > _regions[index].Base || _rangeEndValue < _regions[index].Base + _regions[index].Size - 1)
			++currentRegionSize;
	}

	_dumpRegions.emplace_back(_regions[index].Label, currentBaseAddress, currentRegionSize, reinterpret_cast<void*>(currentBaseLocation));
}

void MungPlex::Search::SetUpAndIterate()
{
	uint16_t subsidiaryDatatype;

	switch (_currentValueTypeSelect)
	{
	case ARRAY:
		subsidiaryDatatype = _currentArrayTypeSelect;
		break;
	case COLOR:
		subsidiaryDatatype = _currentColorTypeSelect;
		break;
	case TEXT:
		subsidiaryDatatype = _currentTextTypeSelect;
		break;
	default: //PRIMITIVE
		subsidiaryDatatype = _currentPrimitiveTypeSelect;
	}

	std::string tempprimary(_knownValueText.CStr());
	std::string tempsecondary(_secondaryKnownValueText.CStr());

	if (MemoryCompare::MemCompare::GetIterationCount() < 1)
	{
		uint32_t setupFlags = 0;

		if (_signed)
			setupFlags |= MemoryCompare::SIGNED;

		if (_caseSensitive)
			setupFlags |= MemoryCompare::CASE_SENSITIVE;

		if (_underlyingBigEndian)
			setupFlags |= MemoryCompare::BIG_ENDIAN;

		if (_cached)
			setupFlags |= MemoryCompare::CACHED;

		MemoryCompare::MemCompare::SetUp(_resultsPath, _currentValueTypeSelect, subsidiaryDatatype, ProcessInformation::GetAddressWidth(), _alignmentValue, setupFlags);
	}

	uint32_t iterationFlags = 0;

	if (_currentComparisionTypeSelect == MemoryCompare::KNOWN)
		iterationFlags |= MemoryCompare::KNOWN;

	if (_hex)
		iterationFlags |= MemoryCompare::HEX;

	MemoryCompare::MemCompare::NewIteration(_currentConditionTypeSelect, _iterationIndex + 1, tempprimary, tempsecondary, _precision / 100.0f, iterationFlags);

	generateDumpRegionMap();

	for (SystemRegion& dumpRegion : _dumpRegions)
	{
		char* buf = new char[dumpRegion.Size];
		ProcessInformation::GetProcess().ReadExRAM(buf, dumpRegion.BaseLocationProcess, dumpRegion.Size, 0x1000);
		
		if (_rereorderRegion)
			Rereorder4BytesReorderedMemory(buf, dumpRegion.Size);

		MemoryCompare::MemDump dump(buf, dumpRegion.Base, dumpRegion.Size);
		delete[] buf;
		MemoryCompare::MemCompare::ProcessNextRange(&dump);
	}
}

void MungPlex::Search::SetUnderlyingBigEndianFlag(const bool isBigEndian)
{
	GetInstance()._underlyingBigEndian = isBigEndian;
}

void MungPlex::Search::SetRereorderRegion(const bool rereorder)
{
	GetInstance()._rereorderRegion = rereorder;
}

void MungPlex::Search::SetAlignment(const int alignment)
{
	GetInstance()._alignmentValue = alignment;
}

void MungPlex::Search::setRecommendedValueSettings(const int valueType)
{
	switch (valueType)
	{
		case ARRAY:
			_currentColorTypeSelect = _currentPrimitiveTypeSelect = _currentTextTypeSelect = 0;
		break;	
		case COLOR:
			_currentArrayTypeSelect = _currentPrimitiveTypeSelect = _currentTextTypeSelect = 0;
			break;
		case TEXT:
			_currentColorTypeSelect = _currentPrimitiveTypeSelect = _currentArrayTypeSelect = 0;
		break;
		default: //PRIMITIVE
			_currentColorTypeSelect = _currentArrayTypeSelect = _currentTextTypeSelect = 0;
	}

	switch (_currentValueTypeSelect)
	{
	case COLOR:
		switch (_currentColorTypeSelect)
		{
			case LitColor::RGB565: case LitColor::RGB5A3:
				_alignmentValue = 2;
			break;
			case LitColor::RGB888:
				_alignmentValue = 1;
			break;
			default:
				_alignmentValue = 4;
		}
	break;
	case TEXT:
		_alignmentValue = 1;
	break;
	default:// PRIMITIVE, ARRAY
		switch (_currentArrayTypeSelect | _currentPrimitiveTypeSelect)
		{
			case INT8:
				_alignmentValue = 1;
			break;
			case INT16:
				_alignmentValue = 2;
			break;
			default: //INT32, INT64
			_alignmentValue = 4;
		}
	}
}