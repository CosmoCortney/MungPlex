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

	_searchColorTypes.emplace_back("RGB 888 (3 Bytes)", LitColor::RGB888);
	_searchColorTypes.emplace_back("RGBA 8888 (4 Bytes)", LitColor::RGBA8888);
	_searchColorTypes.emplace_back("RGBF (3 Floats)", LitColor::RGBF);
	_searchColorTypes.emplace_back("RGBAF (4 Floats)", LitColor::RGBAF);
	_searchColorTypes.emplace_back("RGB 565 (2 Bytes)", LitColor::RGB565);

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

	_searchComparasionType.emplace_back("Unknown/Initial", MemoryCompare::UNKNOWN);
	_searchComparasionType.emplace_back("Known Value", MemoryCompare::KNOWN);

	_RegionSelectSignalCombo.ConnectOnIndexChanged(Slot_IndexChanged);
	_RegionSelectSignalCombo.ConnectOnItemCountChanged(Slot_ItemCountChanged);
	_RegionSelectSignalCombo.ConnectOnTextChanged(Slot_TextChanged);
	_SignalInputTextRangeStart.ConnectOnTextChanged(std::bind(Slot_RangeTextChanged, _rangeStartText, std::ref(_rangeStartValue)));
	_SignalInputTextRangeEnd.ConnectOnTextChanged(std::bind(Slot_RangeTextChanged, _rangeEndText, std::ref(_rangeEndValue)));

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
	char buf[256] = { "" };
	ImGui::BeginChild("child_valueOptions", childXY, true);
	{
		ImGui::SeparatorText("Value Type Options");

		ImGui::BeginGroup();
		{
			MungPlex::SetUpCombo("Value Type:", _searchValueTypes, _currentValueTypeSelect, 0.5f, 0.4f);

			_disableBecauseNoPrimitive = _currentValueTypeSelect != PRIMITIVE;
			_disableBecauseNoArray = _currentValueTypeSelect != ARRAY;
			_disableBecauseNoColor = _currentValueTypeSelect != COLOR;
			_disableBecauseNoText = _currentValueTypeSelect != TEXT;
			_disableBecauseNoInt = (!_disableBecauseNoPrimitive && _currentPrimitiveTypeSelect > INT64)
				|| (!_disableBecauseNoArray && _currentArrayTypeSelect > INT64)
				|| !_disableBecauseNoColor
				|| !_disableBecauseNoText;

			if (_disableBecauseNoPrimitive) ImGui::BeginDisabled();
			MungPlex::SetUpCombo("Primitive Type:", _searchPrimitiveTypes, _currentPrimitiveTypeSelect, 0.5f, 0.4f);
			if (_disableBecauseNoPrimitive) ImGui::EndDisabled();

			if (_disableBecauseNoArray) ImGui::BeginDisabled();
			MungPlex::SetUpCombo("Array Type:", _searchArrayTypes, _currentArrayTypeSelect, 0.5f, 0.4f); //use primitived types here once Arrays support floats
			if (_disableBecauseNoArray) ImGui::EndDisabled();

			if (_disableBecauseNoText) ImGui::BeginDisabled();
			MungPlex::SetUpCombo("Text Type:", _searchTextTypes, _currentTextTypeSelect, 0.5f, 0.4f);
			if (_disableBecauseNoText) ImGui::EndDisabled();

			if (_disableBecauseNoColor) ImGui::BeginDisabled();
			MungPlex::SetUpCombo("Color Type:", _searchColorTypes, _currentColorTypeSelect, 0.5f, 0.4f);
			if (_disableBecauseNoColor) ImGui::EndDisabled();
		}
		ImGui::EndGroup();

		ImGui::SameLine();

		ImGui::BeginGroup();
		{
			ImGui::Checkbox("Big Endian", &_underlyingBigEndian);

			if (_disableBecauseNoInt) ImGui::BeginDisabled();
			ImGui::Checkbox("Signed", &_signed);
			if (_disableBecauseNoInt) ImGui::EndDisabled();
		}
		ImGui::EndGroup();
	}
	ImGui::EndChild();
}

void MungPlex::Search::DrawRangeOptions()
{
	const ImVec2 childXY = { ImGui::GetContentRegionAvail().x * 0.5f, ImGui::GetContentRegionAvail().y * 0.175f };

	ImGui::BeginChild("child_rangeOptions", childXY, true);
	{
		ImGui::SeparatorText("Range Options");

		ImGui::BeginGroup();
		{
			_regions = ProcessInformation::GetRegions();
			_RegionSelectSignalCombo.Draw("Region:", _regions, _currentRegionSelect, 0.75f, 0.3f);
			_SignalInputTextRangeStart.Draw("Start at (hex):", _rangeStartText, IM_ARRAYSIZE(_rangeStartText), 0.75f, 0.3f);
			_SignalInputTextRangeEnd.Draw("End at (hex):", _rangeEndText, IM_ARRAYSIZE(_rangeEndText), 0.75f, 0.3f);
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

				strcpy_s(_rangeStartText, hexStartStr.c_str());
				strcpy_s(_rangeEndText, hexEndStr.c_str());
			}

			ImGui::Checkbox("Re-reorder Region", &_rereorderRegion);
			ImGui::SameLine();
			HelpMarker("Some emulators like Project64 reorder the emulatoed RAM in 4 byte chunks of the opposite endianness which requires re-reordering before scanning. The best option is auto-selected for you, but it might be helpful to set it manually if you encounter a reordered region or structure on another platform.");
		}
		ImGui::EndGroup();
	}
	ImGui::EndChild();
}

void MungPlex::Search::DrawSearchOptions()
{
	const ImVec2 childXY = { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y * 0.334f };

	ImGui::BeginChild("child_searchOptions", childXY, true);
	{
		ImGui::SeparatorText("Search Options");
		static char knownPrimaryValueLabel[64];
		static char knownSecondaryValueLabel[64];
		static bool disablePrimaryValueText = false;
		static bool disableSecondaryValueText = true;
		int iterationCount = MemoryCompare::MemCompare::GetSearchStats().second;

		switch (_currentValueTypeSelect)
		{
		case ARRAY:
			strcpy_s(knownPrimaryValueLabel, "Array Expression");
			strcpy_s(knownSecondaryValueLabel, "Not applicable");
			break;
		case COLOR:
			strcpy_s(knownPrimaryValueLabel, "Color Expression");
			strcpy_s(knownSecondaryValueLabel, "Not applicable");
			break;
		case TEXT:
			strcpy_s(knownPrimaryValueLabel, "Text Value");
			strcpy_s(knownSecondaryValueLabel, "Not applicable");
			break;
		default: //PRIMITIVE
			if (_currentConditionTypeSelect == MemoryCompare::BETWEEN)
			{
				strcpy_s(knownPrimaryValueLabel, "Lowest");
				strcpy_s(knownSecondaryValueLabel, "Highest");
				disableSecondaryValueText = false;
			}
			else if (_currentConditionTypeSelect == MemoryCompare::NOT_BETWEEN)
			{
				strcpy_s(knownPrimaryValueLabel, "Below");
				strcpy_s(knownSecondaryValueLabel, "Above");
				disableSecondaryValueText = false;
			}
			else if (_currentConditionTypeSelect == MemoryCompare::INCREASED_BY)
			{
				strcpy_s(knownPrimaryValueLabel, "Increased by");
				strcpy_s(knownSecondaryValueLabel, "Not applicable");
			}
			else if (_currentConditionTypeSelect == MemoryCompare::DECREASED_BY)
			{
				strcpy_s(knownPrimaryValueLabel, "Decreased by");
				strcpy_s(knownSecondaryValueLabel, "Not applicable");
			}
			else
			{
				strcpy_s(knownPrimaryValueLabel, "Value");
				strcpy_s(knownSecondaryValueLabel, "Not applicable");
			}
		}

		ImGui::BeginGroup();

		if (!iterationCount) ImGui::BeginDisabled();
			SetUpCombo("Counter Iteration:", _iterations, _iterationIndex, 0.5f, 0.4f);
		if (!iterationCount) ImGui::EndDisabled();
		
		if (SetUpInputText("Alignment:", _alignmentText, IM_ARRAYSIZE(_alignmentText),0.5f, 0.4f))
		{
			std::stringstream stream;
			stream << std::hex << std::string(_alignmentText);
			stream >> _alignmentValue;

			if (_alignmentValue < 1)
				_alignmentValue = 1;
		}

		if(!_disableBecauseNoText || !_disableBecauseNoColor) ImGui::BeginDisabled();
			MungPlex::SetUpCombo("Comparision Type:", _searchComparasionType, _currentComparisionTypeSelect, 0.5f, 0.4f);
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
			MungPlex::SetUpCombo("Condition Type:", *conditionTypeItems, _currentConditionTypeSelect, 0.5f, 0.4f);
		if (!_disableBecauseNoText) ImGui::EndDisabled();

		if (disablePrimaryValueText) ImGui::BeginDisabled();
		if (SetUpInputText(knownPrimaryValueLabel, _knownValueText, IM_ARRAYSIZE(_knownValueText), 0.5f, 0.4f))
		{
		}
		if (disablePrimaryValueText) ImGui::EndDisabled();

		if (_disableBecauseNoPrimitive || (!_disableBecauseNoPrimitive && (_currentConditionTypeSelect < MemoryCompare::BETWEEN || _currentConditionTypeSelect > MemoryCompare::NOT_BETWEEN))) ImGui::BeginDisabled();
		if (SetUpInputText(knownSecondaryValueLabel, _secondaryKnownValueText, IM_ARRAYSIZE(_secondaryKnownValueText), 0.5f, 0.4f))
		{
		}
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
			PerformSearch();

		ImGui::SameLine();

		if (iterationCount < 1) ImGui::BeginDisabled();
		if (ImGui::Button("Reset"))
		{
			MemoryCompare::MemCompare::Reset();
			_iterations.clear();
			_iterationIndex = 0;
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
			default: //RGB888, RGB565
				colorPickerFlags |= ImGuiColorEditFlags_Uint8 | ImGuiColorEditFlags_NoAlpha;
			}

			ImGui::PushItemWidth(childXY.x * 0.275f);
			ImGui::ColorPicker4("##ColorPicker", (float*)&_colorVec, colorPickerFlags);
			ImGui::PopItemWidth();

			if (!_disableBecauseNoColor)
				ColorValuesToCString(_colorVec, _currentColorTypeSelect, _knownValueText);


			ImGui::Checkbox("Color Wheel", &_useColorWheel);
			ImGui::SameLine();

			if (ImGui::Button("Pick color from screen"))
			{
				//HWND windowHandle = GetForegroundWindow(); todo: make this work ): 
				PickColorFromScreen();
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

			SetUpLableText("Results:", std::to_string(MemoryCompare::MemCompare::GetSearchStats().first).c_str(), 32, 1.0f, 0.2f);

			ImGui::BeginGroup();
			{
				SetUpInputText("Address:", _pokeAddressText, IM_ARRAYSIZE(_pokeAddressText), 1.0f, 0.2f);
				SetUpInputText("Value:", _pokeValueText, IM_ARRAYSIZE(_pokeValueText), 1.0f, 0.2f);
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
				std::stringstream stream;

				if (!_multiPoke)
				{
					stream << std::hex << std::string(_pokeAddressText);
					stream >> _pokeAddress;
					stream.str(std::string());
					stream.clear();
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
						stream << std::hex << std::string(_pokeValueText);
						stream >> tempVal;
					}
					else
						tempVal = std::stoll(_pokeValueText);

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
						float temp = std::stof(stream.str());
						_pokeValue.insert(_pokeValue.end(), reinterpret_cast<char*>(&temp), reinterpret_cast<char*>(&temp) + 4);
						ProcessInformation::GetAddressWidth() > 4 ? PokeValue<float, uint64_t>() : PokeValue<float, uint32_t>();
					}   break;
					case DOUBLE:
					{
						double temp = std::stof(stream.str());
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
		ImGui::EndGroup();
	}
	ImGui::EndChild();
}

void MungPlex::Search::PickColorFromScreen()
{
	POINT point;
	std::atomic_bool buttonPressed(false);

	std::thread mouseThread([&buttonPressed]()
	{
		while (!buttonPressed)
		{
			if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
			{
				buttonPressed = true;
				break;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	});

	mouseThread.join();

	HDC hdc = GetDC(NULL);
	if (hdc == NULL)
		return;

	if (!GetCursorPos(&point))
		return;

	COLORREF color = GetPixel(hdc, point.x, point.y);
	if (color == CLR_INVALID)
		return;

	ReleaseDC(GetDesktopWindow(), hdc);

	_colorVec.x = static_cast<float>(GetRValue(color)) / 255.0f;
	_colorVec.y = static_cast<float>(GetGValue(color)) / 255.0f;
	_colorVec.z = static_cast<float>(GetBValue(color)) / 255.0f;
	_colorVec.w = 1.0f;
}

void MungPlex::Search::PerformSearch()
{
	switch (_currentValueTypeSelect)
	{
	case ARRAY:
		ArrayTypeSearch();
		break;
	case TEXT:
		TextTypeSearch();
		break;
	case COLOR:
		ColorTypeSearch();
		break;
	default:
		PrimitiveTypeSearch();
		break;
	}

	SetUpAndIterate();

	int iter = MemoryCompare::MemCompare::GetSearchStats().second;

	if (iter < _iterations.size())
		_iterations.erase(_iterations.begin() + iter-1, _iterations.end());

	_iterations.emplace_back(std::to_string(iter).c_str());
	_iterationIndex = --iter;
	_selectedIndices.resize(_maxResultsPerPage);
	uint64_t resultCount = MemoryCompare::MemCompare::GetSearchStats().first;
	_pagesAmountValue = resultCount / _maxResultsPerPage;

	if (resultCount % _maxResultsPerPage > 0)
		++_pagesAmountValue;

	std::stringstream stream;
	stream << std::dec << _pagesAmountValue;
	stream >> _pagesAmountText;
}

void MungPlex::Search::PrimitiveTypeSearch()
{
	std::string logMsg("Perform Search for ");
	std::stringstream stream1;

	if (_currentPrimitiveTypeSelect < FLOAT)
	{
		if (_signed)
		{
			logMsg.append("signed ");
			int64_t knownVal, knownValSecondary;

			if (_currentComparisionTypeSelect == MemoryCompare::KNOWN)
				logMsg.append("known value of ");
			else
				logMsg.append("unknown value ");

			if (GetInstance()._hex)
			{
				stream1 << std::hex << std::string(_knownValueText);

				if (_currentComparisionTypeSelect == MemoryCompare::KNOWN)
					logMsg.append(stream1.str());
			}
			else
			{
				stream1 << std::string(_knownValueText);

				if (_currentComparisionTypeSelect == MemoryCompare::KNOWN)
					logMsg.append(stream1.str());
			}

			switch (_currentPrimitiveTypeSelect)
			{
			case INT8:
				logMsg.append("(int 8)");
				break;
			case INT16:
				logMsg.append("(int 16)");
				break;
			case INT64:
				logMsg.append("(int 64)");
				break;
			default: //INT32
				logMsg.append("(int 32)");
			}
		}
		else
		{
			logMsg.append("unsigned ");

			if (_currentComparisionTypeSelect == MemoryCompare::KNOWN)
				logMsg.append("known value of ");
			else
				logMsg.append("unknown value ");

			if (GetInstance()._hex)
			{
				stream1 << std::hex << std::string(_knownValueText);

				if (_currentComparisionTypeSelect == MemoryCompare::KNOWN)
					logMsg.append(stream1.str());
			}
			else
			{
				stream1 << std::string(_knownValueText);

				if (_currentComparisionTypeSelect == MemoryCompare::KNOWN)
					logMsg.append(stream1.str());
			}

			switch (_currentPrimitiveTypeSelect)
			{
			case INT8:
				logMsg.append("(int 8)");
				break;
			case INT16:
				logMsg.append("(int 16)");
				break;
			case INT64:
				logMsg.append("(int 64)");
				break;
			default: //INT32
				logMsg.append("(int 32)");
			}
		}
	}
	else if (_currentPrimitiveTypeSelect == FLOAT || _currentPrimitiveTypeSelect == DOUBLE)
	{
		if (_currentComparisionTypeSelect == MemoryCompare::KNOWN)
		{
			logMsg.append("known value of ");
		}
		else
			logMsg.append("unknown value ");

		if (_currentPrimitiveTypeSelect == FLOAT)
		{
			logMsg.append("(float)");
		}
		else
		{
			logMsg.append("(double)");
		}
	}

	Log::LogInformation(logMsg.c_str());
}

void MungPlex::Search::ArrayTypeSearch()
{
	Log::LogInformation((std::string("Perform Search for array of type ") + _searchArrayTypes[_currentArrayTypeSelect].first).c_str());
	_currentComparisionTypeSelect = MemoryCompare::KNOWN;
}

void MungPlex::Search::TextTypeSearch()
{
	Log::LogInformation((std::string("Perform Search for text of type ") + _searchTextTypes[_currentTextTypeSelect].first).c_str());
	_currentComparisionTypeSelect = MemoryCompare::KNOWN;
}

void MungPlex::Search::ColorTypeSearch()
{
	Log::LogInformation((std::string("Perform Search for color of type ") + _searchColorTypes[_currentColorTypeSelect].first).c_str());
	_currentComparisionTypeSelect = MemoryCompare::KNOWN;
}

void MungPlex::Search::drawResultsTableNew()
{
	static ImGuiSelectableFlags selectableFlags = ImGuiSelectableFlags_SpanAllColumns;
	static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

	if (!ImGui::BeginTable("Results", 4, flags, ImVec2(ImGui::GetContentRegionAvail().x * 0.666f, ImGui::GetContentRegionAvail().y)))
		return;

	/*auto results = MemoryCompare::MemCompare<dataType, addressType>::GetResults();
	const int iterationCount = std::get<1>(_searchStats);
	if (!_cached && iterationCount > 0)
	{
		if (iterationCount > 1 && results->at(iterationCount - 2)->HasResults())
			results->at(iterationCount - 2)->FreeData(false);

		if (!results->at(iterationCount - 1)->HasResults())
			if (!results->at(iterationCount - 1)->LoadResults(false))
				return;
	}*/

	const uint64_t resultCount = MemoryCompare::MemCompare::GetSearchStats().first;
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
			if (resultCount % _maxResultsPerPage == 0)
				;
			else if (row >= resultCount % _maxResultsPerPage)
				break;

		static char tempAddress[1024];
		static char tempValue[1024];
		static char buf[1024];
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
		uint16_t iterationCount = MemoryCompare::MemCompare::GetSearchStats().second;

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
						DrawArrayValues<uint8_t>(col, itemCount, pageIndexWithRowCount, buf, tempValue, literal);
						break;
					case INT16:
						DrawArrayValues<uint16_t>(col, itemCount, pageIndexWithRowCount, buf, tempValue, literal);
						break;
					case INT32:
						DrawArrayValues<uint32_t>(col, itemCount, pageIndexWithRowCount, buf, tempValue, literal);
						break;
					case INT64:
						DrawArrayValues<uint64_t>(col, itemCount, pageIndexWithRowCount, buf, tempValue, literal);
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

					ColorValuesToCString(vecCol, _currentColorTypeSelect, buf);
					std::memcpy(tempValue, buf, 1024);
					strcpy_s(buf, "");
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

						sprintf_s(buf, "%s", temputf8.c_str());
						std::memcpy(tempValue, buf, 1024);
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
							sprintf(buf, literal, currentValue);
							if (!_pokePrevious)
								std::memcpy(tempValue, buf, 1024);
							break;
						case 2:
							sprintf(buf, literal, previousValue);
							if (_pokePrevious)
								std::memcpy(tempValue, buf, 1024);
							break;
						case 3:
							sprintf(buf, literal, currentValue - previousValue);
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
							sprintf(buf, literal, currentValue);
							if (!_pokePrevious)
								std::memcpy(tempValue, buf, 1024);
							break;
						case 2:
							sprintf(buf, literal, previousValue);
							if (_pokePrevious)
								std::memcpy(tempValue, buf, 1024);
							break;
						case 3:
							sprintf(buf, literal, currentValue - previousValue);
							break;
						}
					}
				}
				}
			}
			else
			{
				sprintf_s(buf, addressTextWidth == 16 ? "%016X" : "%08X", address);
				std::memcpy(tempAddress, buf, 17);
			}

			if (_selectedIndices[row])
			{
				ImGui::Selectable(buf, true, selectableFlags);
				for (int i = 0; i < 4; ++i)
				{
					ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImVec4ToPackedColor(ImGui::GetStyle().Colors[ImGuiCol_Header]));
					ImGui::TableSetColumnIndex(i);
				}
			}
			else
			{
				ImGui::Selectable(buf, false, selectableFlags);
			}

			if (_multiPoke //multi-select
				&& ImGui::IsItemClicked()
				&& (GetAsyncKeyState(VK_LCONTROL) || GetAsyncKeyState(VK_RCONTROL)))
			{
				strncpy(tempAddress, "Multi-Poke", 17);

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
				strncpy(tempAddress, "Multi-Poke", 17);
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
			std::memcpy(_pokeAddressText, tempAddress, 17);
			std::memcpy(_pokeValueText, tempValue, 1024);
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
	}

	_dumpRegions.emplace_back(_regions[index].Label, currentBaseAddress, currentRegionSize, reinterpret_cast<void*>(currentBaseLocation));
}

void MungPlex::Search::SetUpAndIterate()
{
	bool isKnown = _currentComparisionTypeSelect == MemoryCompare::KNOWN;
	bool signedOrCaseSensitive = _currentValueTypeSelect == TEXT ? _caseSensitive : _signed;
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

	std::string tempprimary(_knownValueText);
	std::string tempsecondary(_secondaryKnownValueText);

	if (MemoryCompare::MemCompare::GetSearchStats().second < 2)
		MemoryCompare::MemCompare::SetUp(_resultsPath, _currentValueTypeSelect, subsidiaryDatatype, ProcessInformation::GetAddressWidth(), signedOrCaseSensitive, _alignmentValue, _underlyingBigEndian, _cached, false);

	MemoryCompare::MemCompare::NewIteration(_currentConditionTypeSelect, _hex, isKnown, _iterationIndex + 1, tempprimary, tempsecondary, _precision / 100.0f);

	generateDumpRegionMap();

	for (SystemRegion& dumpRegion : _dumpRegions)
	{
		char* buf = new char[dumpRegion.Size];
		Xertz::SystemInfo::GetProcessInfo(ProcessInformation::GetPID()).ReadExRAM(buf, dumpRegion.BaseLocationProcess, dumpRegion.Size);
		{
			if (_rereorderRegion)
				Rereorder4BytesReorderedMemory(buf, dumpRegion.Size);

			MemoryCompare::MemDump dump(buf, dumpRegion.Base, dumpRegion.Size);
			delete[] buf;
			MemoryCompare::MemCompare::ProcessNextRange(&dump);
		}
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