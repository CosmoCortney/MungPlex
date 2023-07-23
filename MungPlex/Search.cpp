#include"Search.h"
#include <Windows.h>

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
		_regions = ProcessInformation::GetRegions();
		ImGui::SeparatorText("Range Options");
		_RegionSelectSignalCombo.Draw("Region:", _regions, _currentRegionSelect, 0.75f, 0.3f);
		_SignalInputTextRangeStart.Draw("Start at (hex):", _rangeStartText, IM_ARRAYSIZE(_rangeStartText), 0.75f, 0.3f);
		_SignalInputTextRangeEnd.Draw("End at (hex):", _rangeEndText, IM_ARRAYSIZE(_rangeEndText), 0.75f, 0.3f);
	}
	ImGui::EndChild();
}

void MungPlex::Search::DrawSearchOptions()
{
	const ImVec2 childXY = { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y * 0.334f };
	//ImGui::PushStyleColor(ImGuiCol_ChildBg, Settings::GetGeneralSettings().BackgroundColor);

	ImGui::BeginChild("child_searchOptions", childXY, true);
	{
		//ImGui::PushItemWidth(groupWidth);
		ImGui::SeparatorText("Search Options");
		static char knownPrimaryValueLabel[64];
		static char knownSecondaryValueLabel[64];
		static bool disablePrimaryValueText = false;
		static bool disableSecondaryValueText = true;
		const int iterationCount = std::get<1>(_searchStats);

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
			if (_currentConditionTypeSelect == Xertz::BETWEEN)
			{
				strcpy_s(knownPrimaryValueLabel, "Lowest");
				strcpy_s(knownSecondaryValueLabel, "Highest");
				disableSecondaryValueText = false;
			}
			else if (_currentConditionTypeSelect == Xertz::NOT_BETWEEN)
			{
				strcpy_s(knownPrimaryValueLabel, "Below");
				strcpy_s(knownSecondaryValueLabel, "Above");
				disableSecondaryValueText = false;
			}
			else if (_currentConditionTypeSelect == Xertz::INCREASED_BY)
			{
				strcpy_s(knownPrimaryValueLabel, "Increased by");
				strcpy_s(knownSecondaryValueLabel, "Not applicable");
			}
			else if (_currentConditionTypeSelect == Xertz::DECREASED_BY)
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

		if (_disableBecauseNoPrimitive || (!_disableBecauseNoPrimitive && (_currentConditionTypeSelect < Xertz::BETWEEN || _currentConditionTypeSelect > Xertz::NOT_BETWEEN))) ImGui::BeginDisabled();
		if (SetUpInputText(knownSecondaryValueLabel, _secondaryKnownValueText, IM_ARRAYSIZE(_secondaryKnownValueText), 0.5f, 0.4f))
		{
		}
		if (_disableBecauseNoPrimitive || (!_disableBecauseNoPrimitive && (_currentConditionTypeSelect < Xertz::BETWEEN || _currentConditionTypeSelect > Xertz::NOT_BETWEEN))) ImGui::EndDisabled();

		if (!_disableBecauseNoInt || !_disableBecauseNoText) ImGui::BeginDisabled();
		SetUpSliderFloat("Precision (%%):", &_precision, 1.0f, 100.0f, "%0.2f", 0.5f, 0.4f);
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
			static int addressWidth = ProcessInformation::GetAddressWidth();
			switch (_currentValueTypeSelect)
			{
			case ARRAY:
			{
				switch (_currentArrayTypeSelect)
				{
				case INT8: {
					if (addressWidth > 4)
						_searchStats = _signed ? Xertz::MemCompare<OperativeArray<int8_t>, uint64_t>::Reset() : Xertz::MemCompare<OperativeArray<uint8_t>, uint64_t>::Reset();
					else
						_searchStats = _signed ? Xertz::MemCompare<OperativeArray<int8_t>, uint32_t>::Reset() : Xertz::MemCompare<OperativeArray<uint8_t>, uint32_t>::Reset();
				} break;
				case INT16: {
					if (addressWidth > 4)
						_searchStats = _signed ? Xertz::MemCompare<OperativeArray<int16_t>, uint64_t>::Reset() : Xertz::MemCompare<OperativeArray<uint16_t>, uint64_t>::Reset();
					else
						_searchStats = _signed ? Xertz::MemCompare<OperativeArray<int16_t>, uint32_t>::Reset() : Xertz::MemCompare<OperativeArray<uint16_t>, uint32_t>::Reset();
				} break;
				case INT64: {
					if (addressWidth > 4)
						_searchStats = _signed ? Xertz::MemCompare<OperativeArray<int64_t>, uint64_t>::Reset() : Xertz::MemCompare<OperativeArray<uint64_t>, uint64_t>::Reset();
					else
						_searchStats = _signed ? Xertz::MemCompare<OperativeArray<int64_t>, uint32_t>::Reset() : Xertz::MemCompare<OperativeArray<uint64_t>, uint32_t>::Reset();
				} break;
				case FLOAT: {
					if (addressWidth > 4)
						_searchStats = Xertz::MemCompare<OperativeArray<float>, uint64_t>::Reset();
					else
						_searchStats = Xertz::MemCompare<OperativeArray<float>, uint32_t>::Reset();
				} break;
				case DOUBLE: {
					if (addressWidth > 4)
						_searchStats = Xertz::MemCompare<OperativeArray<double>, uint64_t>::Reset();
					else
						_searchStats = Xertz::MemCompare<OperativeArray<double>, uint32_t>::Reset();
				} break;
				default: { //OperativeArray<INT32>
					if (addressWidth > 4)
						_searchStats = _signed ? Xertz::MemCompare<OperativeArray<int32_t>, uint64_t>::Reset() : Xertz::MemCompare<OperativeArray<uint32_t>, uint64_t>::Reset();
					else
						_searchStats = _signed ? Xertz::MemCompare<OperativeArray<int32_t>, uint32_t>::Reset() : Xertz::MemCompare<OperativeArray<uint32_t>, uint32_t>::Reset();
				}
				}
			} break;
			case COLOR: {
				if (addressWidth > 4)
					_searchStats = Xertz::MemCompare<LitColor, uint64_t>::Reset();
				else
					_searchStats = Xertz::MemCompare<LitColor, uint32_t>::Reset();
			} break;
			case TEXT: {
				if (addressWidth > 4)
					_searchStats = Xertz::MemCompare<MorphText, uint64_t>::Reset();
				else
					_searchStats = Xertz::MemCompare<MorphText, uint32_t>::Reset();
			} break;
			default: {//PRIMITIVE
				switch (_currentPrimitiveTypeSelect)
				{
				case INT8: {
					if (addressWidth > 4)
						_searchStats = _signed ? Xertz::MemCompare<int8_t, uint64_t>::Reset() : Xertz::MemCompare<uint8_t, uint64_t>::Reset();
					else
						_searchStats = _signed ? Xertz::MemCompare<int8_t, uint32_t>::Reset() : Xertz::MemCompare<uint8_t, uint32_t>::Reset();
				} break;
				case INT16: {
					if (addressWidth > 4)
						_searchStats = _signed ? Xertz::MemCompare<int16_t, uint64_t>::Reset() : Xertz::MemCompare<uint16_t, uint64_t>::Reset();
					else
						_searchStats = _signed ? Xertz::MemCompare<int16_t, uint32_t>::Reset() : Xertz::MemCompare<uint16_t, uint32_t>::Reset();
				} break;
				case INT64: {
					if (addressWidth > 4)
						_searchStats = _signed ? Xertz::MemCompare<int64_t, uint64_t>::Reset() : Xertz::MemCompare<uint64_t, uint64_t>::Reset();
					else
						_searchStats = _signed ? Xertz::MemCompare<int64_t, uint32_t>::Reset() : Xertz::MemCompare<uint64_t, uint32_t>::Reset();
				} break;
				case FLOAT: {
					if (addressWidth > 4)
						_searchStats = Xertz::MemCompare<float, uint64_t>::Reset();
					else
						_searchStats = Xertz::MemCompare<float, uint32_t>::Reset();
				} break;
				case DOUBLE: {
					if (addressWidth > 4)
						_searchStats = Xertz::MemCompare<double, uint64_t>::Reset();
					else
						_searchStats = Xertz::MemCompare<double, uint32_t>::Reset();
				} break;
				default: { //INT32
					if (addressWidth > 4)
						_searchStats = _signed ? Xertz::MemCompare<int32_t, uint64_t>::Reset() : Xertz::MemCompare<uint32_t, uint64_t>::Reset();
					else
						_searchStats = _signed ? Xertz::MemCompare<int32_t, uint32_t>::Reset() : Xertz::MemCompare<uint32_t, uint32_t>::Reset();
				} break;
				}
			}
			}

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
	//ImGui::PopStyleColor();
}

void MungPlex::Search::DrawResultsArea()
{
	const ImVec2 childXY = ImGui::GetContentRegionAvail();
	ImGui::BeginChild("child_searchResults", childXY, true);
	ImGui::SeparatorText("Results Panel");

	switch (_currentValueTypeSelect)
	{
		case ARRAY:
		{
			switch (_currentArrayTypeSelect)
			{
				case INT8: {
					if (ProcessInformation::GetAddressWidth() > 4)
						_signed ? DrawResultsTable<OperativeArray<int8_t>, uint64_t>() : DrawResultsTable<OperativeArray<uint8_t>, uint64_t>();
					else
						_signed ? DrawResultsTable<OperativeArray<int8_t>, uint32_t>() : DrawResultsTable<OperativeArray<uint8_t>, uint32_t>();
				} break;
				case INT16: {
					if (ProcessInformation::GetAddressWidth() > 4)
						_signed ? DrawResultsTable<OperativeArray<int16_t>, uint64_t>() : DrawResultsTable<OperativeArray<uint16_t>, uint64_t>();
					else
						_signed ? DrawResultsTable<OperativeArray<int16_t>, uint32_t>() : DrawResultsTable<OperativeArray<uint16_t>, uint32_t>();
				} break;
				case INT64: {
					if (ProcessInformation::GetAddressWidth() > 4)
						_signed ? DrawResultsTable<OperativeArray<int64_t>, uint64_t>() : DrawResultsTable<OperativeArray<uint64_t>, uint64_t>();
					else
						_signed ? DrawResultsTable<OperativeArray<int64_t>, uint32_t>() : DrawResultsTable<OperativeArray<uint64_t>, uint32_t>();
				} break;
				case FLOAT: {
					if (ProcessInformation::GetAddressWidth() > 4)
						DrawResultsTable<OperativeArray<float>, uint64_t>();
					else
						DrawResultsTable<OperativeArray<float>, uint32_t>();
				} break;
				case DOUBLE: {
					if (ProcessInformation::GetAddressWidth() > 4)
						DrawResultsTable<OperativeArray<double>, uint64_t>();
					else
						DrawResultsTable<OperativeArray<double>, uint32_t>();
				} break;
				default: { //OperativeArray<INT32>
					if (ProcessInformation::GetAddressWidth() > 4)
						_signed ? DrawResultsTable<OperativeArray<int32_t>, uint64_t>() : DrawResultsTable<OperativeArray<uint32_t>, uint64_t>();
					else
						_signed ? DrawResultsTable<OperativeArray<int32_t>, uint32_t>() : DrawResultsTable<OperativeArray<uint32_t>, uint32_t>();
				} break;
			}break;
		}
		case COLOR: {
			if (ProcessInformation::GetAddressWidth() > 4)
				DrawResultsTable<LitColor, uint64_t>();
			else
				DrawResultsTable<LitColor, uint32_t>();
		} break;
		case TEXT: {
			if (ProcessInformation::GetAddressWidth() > 4)
				DrawResultsTable<MorphText, uint64_t>();
			else
				DrawResultsTable<MorphText, uint32_t>();
		} break;
		default:{//PRIMITIVE
			switch (_currentPrimitiveTypeSelect)
			{
				case INT8: {
					if (ProcessInformation::GetAddressWidth() > 4)
						_signed ? DrawResultsTable<int8_t, uint64_t>() : DrawResultsTable<uint8_t, uint64_t>();
					else
						_signed ? DrawResultsTable<int8_t, uint32_t>() : DrawResultsTable<uint8_t, uint32_t>();
				} break;
				case INT16: {
					if (ProcessInformation::GetAddressWidth() > 4)
						_signed ? DrawResultsTable<int16_t, uint64_t>() : DrawResultsTable<uint16_t, uint64_t>();
					else
						_signed ? DrawResultsTable<int16_t, uint32_t>() : DrawResultsTable<uint16_t, uint32_t>();
				} break;
				case INT64: {
					if (ProcessInformation::GetAddressWidth() > 4)
						_signed ? DrawResultsTable<int64_t, uint64_t>() : DrawResultsTable<uint64_t, uint64_t>();
					else
						_signed ? DrawResultsTable<int64_t, uint32_t>() : DrawResultsTable<uint64_t, uint32_t>();
				} break;
				case FLOAT: {
					if (ProcessInformation::GetAddressWidth() > 4)
						DrawResultsTable<float, uint64_t>();
					else
						DrawResultsTable<float, uint32_t>();
				} break;
				case DOUBLE: {
					if (ProcessInformation::GetAddressWidth() > 4)
						DrawResultsTable<double, uint64_t>();
					else
						DrawResultsTable<double, uint32_t>();
				} break;
				default: { //INT32
					if (ProcessInformation::GetAddressWidth() > 4)
						_signed ? DrawResultsTable<int32_t, uint64_t>() : DrawResultsTable<uint32_t, uint64_t>();
					else
						_signed ? DrawResultsTable<int32_t, uint32_t>() : DrawResultsTable<uint32_t, uint32_t>();
				} break;
			}
		}
	}
	
	SetUpLableText("Results", std::to_string(std::get<0>(_searchStats)).c_str(), 32, 0.25f, 0.25f);
	
	ImGui::BeginGroup();
	{
		SetUpInputText("Address:", _pokeAddressText, IM_ARRAYSIZE(_pokeAddressText), 0.25f, 0.25f);
		SetUpInputText("Poke Value:", _pokeValueText, IM_ARRAYSIZE(_pokeValueText), 0.25f, 0.25f);
	}
	ImGui::EndGroup();

	ImGui::SameLine();

	ImGui::BeginGroup();
	{
		if (SetUpInputText("Page:", _currentPageText, IM_ARRAYSIZE(_currentPageText), 1.0f / 6.0f, 0.3f))
		{
			std::stringstream(_currentPageText) >> _currentPageValue;

			if (_currentPageValue < 1 || _currentPageValue > _pagesAmountValue)
				ResetCurrentPage();
		}

		ImGui::SameLine();
		SetUpLableText("Of", _pagesAmountText, IM_ARRAYSIZE(_pagesAmountText), 8.0f);

		std::stringstream stream;
		if (ImGui::Button("Page Up"))
		{
			if (_currentPageValue < _pagesAmountValue)
			{
				stream << ++_currentPageValue;
				stream >> _currentPageText;
			}
		}

		ImGui::SameLine();

		if (ImGui::Button("Page Down"))
		{
			if (_currentPageValue > 1)
			{
				stream << --_currentPageValue;
				stream >> _currentPageText;
			}
		}
	}
	ImGui::EndGroup();
		if (!_disableBecauseNoText) ImGui::BeginDisabled();
		ImGui::Checkbox("Previous Value", &_pokePrevious);
		if (!_disableBecauseNoText) ImGui::EndDisabled();

		ImGui::SameLine();
		HelpMarker("If \"Multi-Poke\" is checked this will enable poking previous values - no matter what's in the \"Value\" text field. If this is unchecked the expression inside \"Value\" will be written to all selected result addresses.");
		ImGui::SameLine();
		ImGui::Checkbox("Multi-Poke", &_multiPoke);
		ImGui::SameLine();

		if (ImGui::Button("Poke"))
		{
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
				if (_hex && _currentPrimitiveTypeSelect < FLOAT)
					stream << std::hex << std::string(_pokeValueText);
				else
					stream << std::string(_pokeValueText);

				switch (_currentPrimitiveTypeSelect)
				{
				case INT8: {
					stream >> *(uint8_t*)_pokeValue;
					if (ProcessInformation::GetAddressWidth() > 4)
						PokeValue<uint8_t, uint64_t>();
					else
						PokeValue<uint8_t, uint32_t>();
				} break;
				case INT16: {
					stream >> *(uint16_t*)_pokeValue;
					if (ProcessInformation::GetAddressWidth() > 4)
						PokeValue<uint16_t, uint64_t>();
					else
						PokeValue<uint16_t, uint32_t>();
				} break;
				case INT64: {
					stream >> *(uint64_t*)_pokeValue;
					if (ProcessInformation::GetAddressWidth() > 4)
						PokeValue<uint64_t, uint64_t>();
					else
						PokeValue<uint64_t, uint32_t>();
				} break;
				case FLOAT: {
					stream >> *(float*)_pokeValue;
					if (ProcessInformation::GetAddressWidth() > 4)
						PokeValue<float, uint64_t>();
					else
						PokeValue<float, uint32_t>();
				} break;
				case DOUBLE: {
					stream >> *(double*)_pokeValue;
					if (ProcessInformation::GetAddressWidth() > 4)
						PokeValue<double, uint64_t>();
					else
						PokeValue<double, uint32_t>();
				} break;
				default: {
					stream >> *(uint32_t*)_pokeValue;
					if (ProcessInformation::GetAddressWidth() > 4)
						PokeValue<uint32_t, uint64_t>();
					else
						PokeValue<uint32_t, uint32_t>();
				} break;
				}
			}
			}
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

	static char x[4];
	int iter = std::get<1>(_searchStats);
	if (iter < _iterations.size())
		_iterations.erase(_iterations.begin() + iter-1, _iterations.end());

	strcpy_s(x, sizeof(x), std::to_string(iter).c_str());
	_iterations.emplace_back(x);
	_iterationIndex = --iter;
	_selectedIndices.resize(_maxResultsPerPage);
	uint64_t resultCount = std::get<0>(_searchStats);
	_pagesAmountValue = resultCount / _maxResultsPerPage;

	if (resultCount % _maxResultsPerPage > 0)
		++_pagesAmountValue;

	std::stringstream stream;
	stream << std::dec << _pagesAmountValue;
	stream >> _pagesAmountText;
}

void MungPlex::Search::PrimitiveTypeSearch()
{
	std::stringstream stream1, stream2;
	if (_currentPrimitiveTypeSelect < FLOAT)
	{
		if (_signed)
		{
			int64_t knownVal, knownValSecondary;
			if (GetInstance()._hex)
			{
				stream1 << std::hex << std::string(_knownValueText);
				stream2 << std::hex << std::string(_secondaryKnownValueText);
			}
			else
			{
				stream1 << std::string(_knownValueText);
				stream2 << std::string(_secondaryKnownValueText);
			}
			stream1 >> knownVal;
			stream2 >> knownValSecondary;

			switch (_currentPrimitiveTypeSelect)
			{
			case INT8:
				_searchStats = SetUpAndIterate<int8_t>(knownVal, knownValSecondary);
				break;
			case INT16:
				_searchStats = SetUpAndIterate<int16_t>(knownVal, knownValSecondary);
				break;
			case INT64:
				_searchStats = SetUpAndIterate<int64_t>(knownVal, knownValSecondary);
				break;
			default: //INT32
				_searchStats = SetUpAndIterate<int32_t>(knownVal, knownValSecondary);
			}
		}
		else
		{
			uint64_t knownVal, knownValSecondary;
			if (GetInstance()._hex)
			{
				stream1 << std::hex << std::string(_knownValueText);
				stream2 << std::hex << std::string(_secondaryKnownValueText);
			}
			else
			{
				stream1 << std::string(_knownValueText);
				stream2 << std::string(_secondaryKnownValueText);
			}
			stream1 >> knownVal;
			stream2 >> knownValSecondary;

			switch (_currentPrimitiveTypeSelect)
			{
			case INT8:
				_searchStats = SetUpAndIterate<uint8_t>(knownVal, knownValSecondary);
				break;
			case INT16:
				_searchStats = SetUpAndIterate<uint16_t>(knownVal, knownValSecondary);
				break;
			case INT64:
				_searchStats = SetUpAndIterate<uint64_t>(knownVal, knownValSecondary);
				break;
			default: //INT32
				_searchStats = SetUpAndIterate<uint32_t>(knownVal, knownValSecondary);
			}
		}
	}
	else if (_currentPrimitiveTypeSelect == FLOAT || _currentPrimitiveTypeSelect == DOUBLE)
	{
		double knownVal, knownValSecondary;
		stream1 << std::string(_knownValueText);
		stream2 << std::string(_secondaryKnownValueText);
		stream1 >> knownVal;
		stream2 >> knownValSecondary;

		if (_currentPrimitiveTypeSelect == FLOAT)
			_searchStats = SetUpAndIterate<float>(knownVal, knownValSecondary);
		else
			_searchStats = SetUpAndIterate<double>(knownVal, knownValSecondary);
	}
}

void MungPlex::Search::ArrayTypeSearch()
{
	_currentComparisionTypeSelect = Xertz::KNOWN;

	const std::string strArray = std::string(_knownValueText);
	const std::string strArraySecondary = std::string(_secondaryKnownValueText);

	if (_currentArrayTypeSelect < FLOAT)
	{
		switch (_currentArrayTypeSelect)
		{
		case INT8: {
			const OperativeArray<uint8_t> arrayP(strArray);
			const OperativeArray<uint8_t> arrayS(strArraySecondary);
			_searchStats = SetUpAndIterate<OperativeArray<uint8_t>>(arrayP, arrayS);
		}break;
		case INT16: {
			const OperativeArray<uint16_t> arrayP(strArray);
			const OperativeArray<uint16_t> arrayS(strArraySecondary);
			_searchStats = SetUpAndIterate<OperativeArray<uint16_t>>(arrayP, arrayS);
		}break;
		case INT64: {
			const OperativeArray<uint64_t> arrayP(strArray);
			const OperativeArray<uint64_t> arrayS(strArraySecondary);
			_searchStats = SetUpAndIterate<OperativeArray<uint64_t>>(arrayP, arrayS);
		}break;
		default: //INT32
		{
			const OperativeArray<uint32_t> arrayP(strArray);
			const OperativeArray<uint32_t> arrayS(strArraySecondary);
			_searchStats = SetUpAndIterate<OperativeArray<uint32_t>>(arrayP, arrayS);
		}
		}
	}
	else if (_currentValueTypeSelect == FLOAT || _currentValueTypeSelect == DOUBLE)
	{
		if(_currentArrayTypeSelect == DOUBLE)
		{
			const OperativeArray<float> arrayP(strArray);
			const OperativeArray<float> arrayS(strArraySecondary);
			_searchStats = SetUpAndIterate<OperativeArray<float>>(arrayP, arrayS);
		}
		else
		{
			const OperativeArray<double> arrayP(strArray);
			const OperativeArray<double> arrayS(strArraySecondary);
			_searchStats = SetUpAndIterate<OperativeArray<double>>(arrayP, arrayS);
		}
	}
}

void MungPlex::Search::TextTypeSearch()
{
	_currentComparisionTypeSelect = Xertz::KNOWN;
	MorphText searchText = MorphText(std::string(_knownValueText));
	searchText.SetMaxLength(256);
	searchText.SetPrimaryFormat(_currentTextTypeSelect);
	_searchStats = SetUpAndIterate<MorphText>(searchText, searchText);
}

void MungPlex::Search::ColorTypeSearch()
{
	_currentComparisionTypeSelect = Xertz::KNOWN;
	std::string arg(_knownValueText);
	const LitColor colorP(arg);
	arg = std::string(_secondaryKnownValueText);
	const LitColor colorS(arg);

	_searchStats = SetUpAndIterate<LitColor>(colorP, colorS);
}