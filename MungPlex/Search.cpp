#include"Search.h"

static float scale = 2.0f;
void MungPlex::Search::DrawWindow()
{
	ImGui::Begin("Search");

	if (!MungPlex::Connection::IsConnected()) ImGui::BeginDisabled();

		GetInstance().DrawValueTypeOptions();

		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_ChildBg, ImVec4(0.0f, 5.0f, 0.0f, 1.0f));
		ImGui::BeginGroup();
		{
			GetInstance().DrawRangeOptions();
			GetInstance().DrawSearchOptions();
		}
		ImGui::EndGroup();
		ImGui::PopStyleColor();
	
		GetInstance().DrawResultsArea();

	if (!MungPlex::Connection::IsConnected()) ImGui::EndDisabled();

	ImGui::End();
}

void MungPlex::Search::DrawValueTypeOptions()
{
	float groupWidth = ImGui::GetContentRegionAvail().x / scale;
	char buf[256] = { "" };
	ImGui::BeginGroup();
	{
		ImGui::PushItemWidth(groupWidth/2);
		ImGui::SeparatorText("Value Type Options");

		ImGui::BeginGroup();
		MungPlex::SetUpCombo("Value Type", _searchValueTypes, _currentValueTypeSelect);

		_disableBecauseNoPrimitive = _currentValueTypeSelect != PRIMITIVE; 
		_disableBecauseNoArray = _currentValueTypeSelect != ARRAY;
		_disableBecauseNoColor = _currentValueTypeSelect != COLOR;
		_disableBecauseNoText = _currentValueTypeSelect != TEXT;
		_disableBecauseNoInt = (!_disableBecauseNoPrimitive && _currentPrimitiveTypeSelect > INT64) || (!_disableBecauseNoArray && _currentArrayTypeSelect > INT64) || !_disableBecauseNoColor;

		if (_disableBecauseNoPrimitive) ImGui::BeginDisabled();
			MungPlex::SetUpCombo("Primitive Type", GetInstance()._searchPrimitiveTypes, _currentPrimitiveTypeSelect);
		if (_disableBecauseNoPrimitive) ImGui::EndDisabled();

		if (_disableBecauseNoArray) ImGui::BeginDisabled();
			MungPlex::SetUpCombo("Array Type", GetInstance()._searchArrayTypes, _currentArrayTypeSelect); //use primitived types here once Arrays support floats
		if (_disableBecauseNoArray) ImGui::EndDisabled();

		if (_disableBecauseNoText) ImGui::BeginDisabled();
			MungPlex::SetUpCombo("Text Type", _searchTextTypes, _currentTextTypeSelect);
		if (_disableBecauseNoText) ImGui::EndDisabled();

		if (_disableBecauseNoColor) ImGui::BeginDisabled();
			MungPlex::SetUpCombo("Color Type", _searchColorTypes, _currentColorTypeSelect);
		if (_disableBecauseNoColor) ImGui::EndDisabled();
		
		if (!_disableBecauseNoInt) ImGui::BeginDisabled();
			ImGui::SliderFloat("% Precision", &_precision, 1.0f, 100.0f, "%0.2f", NULL);
		if (!_disableBecauseNoInt) ImGui::EndDisabled();

		ImGui::Checkbox("Big Endian", Connection::IsBE());
		ImGui::SameLine();

		if (_disableBecauseNoInt) ImGui::BeginDisabled();
			ImGui::Checkbox("Signed", &_signed);
		if (_disableBecauseNoInt) ImGui::EndDisabled();

		ImGui::SameLine();

		_disableBecauseNoText = _currentValueTypeSelect != TEXT;

		if (_disableBecauseNoText) ImGui::BeginDisabled();
			ImGui::Checkbox("Case Sensitive", &_caseSensitive);
		if (_disableBecauseNoText) ImGui::EndDisabled();


		//ImGui::SameLine();

		if (_disableBecauseNoColor)
			ImGui::BeginDisabled();

			ImGui::Checkbox("Show Color Wheel", &_useColorWheel);
			ImGui::SameLine();

			if (ImGui::Button("Pick color from screen"))
			{
				//HWND windowHandle = GetForegroundWindow(); todo: make this work ): 
				PickColorFromScreen();
				//MungPlex::SetWindowToForeground(windowHandle);
			}

			ImGui::EndGroup();

			ImGui::SameLine();

			float w = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.y) * 0.18f;
			ImGui::SetNextItemWidth(w);
			ImGui::ColorPicker3("##MyColor##6", (float*)&_colorVec, (_useColorWheel ? ImGuiColorEditFlags_PickerHueWheel : ImGuiColorEditFlags_PickerHueBar) );
				
				

		if (_disableBecauseNoColor)
			ImGui::EndDisabled();

		

		ImGui::PopItemWidth();
	}
	ImGui::EndGroup();
}

void MungPlex::Search::DrawRangeOptions()
{
	float groupWidth = ImGui::GetContentRegionAvail().x / scale;

	ImGui::BeginGroup();
	{
		ImGui::PushItemWidth(groupWidth);
		_regions = MungPlex::Connection::GetRegions();
		ImGui::SeparatorText("Range Options");
		_RegionSelectSignalCombo.Draw("Region", _regions, _currentRegionSelect);
		int changed;

		_SignalInputTextRangeStart.Draw("Start at (hex)", _rangeStartText, IM_ARRAYSIZE(_rangeStartText));
		_SignalInputTextRangeEnd.Draw("End at (hex)", _rangeEndText, IM_ARRAYSIZE(_rangeEndText));

		ImGui::PopItemWidth();
	}
	ImGui::EndGroup();
}

void MungPlex::Search::DrawSearchOptions()
{
	float groupWidth = ImGui::GetContentRegionAvail().x / scale;

	ImGui::BeginGroup();
	{
		ImGui::PushItemWidth(groupWidth);
		ImGui::SeparatorText("Search Options");

		if(ImGui::Button("Search"))
			PerformSearch();

		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
		}

		static char knownPrimaryValueLabel[64];
		static char knownSecondaryValueLabel[64];
		static bool disablePrimaryValueText = false;
		static bool disableSecondaryValueText = true;

		switch (_currentValueTypeSelect)
		{
		case PRIMITIVE:
			if (_currentConditionTypeSelect == Xertz::BETWEEN)
			{
				strcpy(knownPrimaryValueLabel, "Lowest");
				strcpy(knownSecondaryValueLabel, "Highest");
				disableSecondaryValueText = false;
		}
			else if (_currentConditionTypeSelect == Xertz::NOT_BETWEEN)
		 {
				strcpy(knownPrimaryValueLabel, "Below");
				strcpy(knownSecondaryValueLabel, "Above");
				disableSecondaryValueText = false;
			}
			else
			{
				strcpy(knownPrimaryValueLabel, "Value");
				strcpy(knownSecondaryValueLabel, "Not applicable");
			}
			break;
		case ARRAY:
			strcpy(knownPrimaryValueLabel, "Array Expression");
			strcpy(knownSecondaryValueLabel, "Not applicable");
			break;
		case COLOR:
			switch(_currentColorTypeSelect)
		case RGB_BYTE:
		case RGBA_BYTE:
		case RGB_FLOAT:
		case RGBA_FLOAT:
			strcpy(knownPrimaryValueLabel, "Color Expression");
			strcpy(knownSecondaryValueLabel, "Not applicable");
			break;
		case TEXT:
			strcpy(knownPrimaryValueLabel, "Text Value");
			strcpy(knownSecondaryValueLabel, "Not applicable");
		}

		if (disablePrimaryValueText) ImGui::BeginDisabled();
			if(ImGui::InputText(knownPrimaryValueLabel, _knownValueText, IM_ARRAYSIZE(_knownValueText)))
		{
		}
		if (disablePrimaryValueText) ImGui::EndDisabled();

		if (knownSecondaryValueLabel) ImGui::BeginDisabled();
			if (ImGui::InputText(knownSecondaryValueLabel, _secondaryKnownValueText, IM_ARRAYSIZE(_secondaryKnownValueText)))
		{
		}
		if (knownSecondaryValueLabel) ImGui::EndDisabled();

		MungPlex::SetUpCombo("Comparision Type", _searchComparasionType, _currentComparisionTypeSelect);

		std::vector<std::pair<std::string, int>>* conditionTypeItems;

		switch (_currentValueTypeSelect)
		{
		case PRIMITIVE:
			if (_currentPrimitiveTypeSelect < FLOAT)
				conditionTypeItems = &_searchConditionTypes;
			else
				conditionTypeItems = &_searchConditionTypesFloat;
		break;
		case ARRAY:
			conditionTypeItems = &_searchConditionTypesArray;
		break;
		case COLOR:
			conditionTypeItems = &_searchConditionTypesColor;
		break;
		case TEXT:
			conditionTypeItems = &_searchConditionTypesText;
		break;
		}

		MungPlex::SetUpCombo("Condition Type", *conditionTypeItems, _currentConditionTypeSelect);

		if (ImGui::InputText("Alignment", _alignmentText, IM_ARRAYSIZE(_alignmentText)))
		{
			std::stringstream stream;
			stream << std::hex << std::string(_alignmentText);
			stream >> _alignmentValue;
			
			if (_alignmentValue < 1)
				_alignmentValue = 1;
		}

		ImGui::Checkbox("Values are hex", &GetInstance()._hex);
		ImGui::SameLine();
		ImGui::Checkbox("Cached", &GetInstance()._cached);


		std::vector<std::string> iterationItems;
		iterationItems.resize(0);
		std::vector<const char*> iterationItems_cstr(iterationItems.size());
		int tempselect = 0;
		ImGui::Combo("Compare against iteration", &tempselect, iterationItems_cstr.data(), iterationItems.size());
		ImGui::PopItemWidth();
	}
	ImGui::EndGroup();
}

void MungPlex::Search::DrawResultsArea()
{
	float groupWidth = ImGui::GetContentRegionAvail().x / scale;
	ImGui::BeginGroup();

	switch (_currentValueTypeSelect)
	{
		case ARRAY:
		{
			switch (_currentArrayTypeSelect)
			{
				case INT8: {
					if (*Connection::GetAddressWidth() > 4)
						_signed ? DrawResultsTable<OperativeArray<int8_t>, uint64_t>() : DrawResultsTable<OperativeArray<uint8_t>, uint64_t>();
					else
						_signed ? DrawResultsTable<OperativeArray<int8_t>, uint32_t>() : DrawResultsTable<OperativeArray<uint8_t>, uint32_t>();
				} break;
				case INT16: {
					if (*Connection::GetAddressWidth() > 4)
						_signed ? DrawResultsTable<OperativeArray<int16_t>, uint64_t>() : DrawResultsTable<OperativeArray<uint16_t>, uint64_t>();
					else
						_signed ? DrawResultsTable<OperativeArray<int16_t>, uint32_t>() : DrawResultsTable<OperativeArray<uint16_t>, uint32_t>();
				} break;
				case INT64: {
					if (*Connection::GetAddressWidth() > 4)
						_signed ? DrawResultsTable<OperativeArray<int64_t>, uint64_t>() : DrawResultsTable<OperativeArray<uint64_t>, uint64_t>();
					else
						_signed ? DrawResultsTable<OperativeArray<int64_t>, uint32_t>() : DrawResultsTable<OperativeArray<uint64_t>, uint32_t>();
				} break;
				case FLOAT: {
					if (*Connection::GetAddressWidth() > 4)
						DrawResultsTable<OperativeArray<float>, uint64_t>();
					else
						DrawResultsTable<OperativeArray<float>, uint32_t>();
				} break;
				case DOUBLE: {
					if (*Connection::GetAddressWidth() > 4)
						DrawResultsTable<OperativeArray<double>, uint64_t>();
					else
						DrawResultsTable<OperativeArray<double>, uint32_t>();
				} break;
				default: { //OperativeArray<INT32>
					if (*Connection::GetAddressWidth() > 4)
						_signed ? DrawResultsTable<OperativeArray<int32_t>, uint64_t>() : DrawResultsTable<OperativeArray<uint32_t>, uint64_t>();
					else
						_signed ? DrawResultsTable<OperativeArray<int32_t>, uint32_t>() : DrawResultsTable<OperativeArray<uint32_t>, uint32_t>();
				} break;
			}break;
		}
		case PRIMITIVE:{//PRIMITIVE
	switch (_currentPrimitiveTypeSelect)
	{
	case INT8: {
		if (*Connection::GetAddressWidth() > 4)
			_signed ? DrawResultsTable<int8_t, uint64_t>() : DrawResultsTable<uint8_t, uint64_t>();
		else
			_signed ? DrawResultsTable<int8_t, uint32_t>() : DrawResultsTable<uint8_t, uint32_t>();
	} break;
	case INT16: {
		if (*Connection::GetAddressWidth() > 4)
			_signed ? DrawResultsTable<int16_t, uint64_t>() : DrawResultsTable<uint16_t, uint64_t>();
		else
			_signed ? DrawResultsTable<int16_t, uint32_t>() : DrawResultsTable<uint16_t, uint32_t>();
	} break;
	case INT64: {
		if (*Connection::GetAddressWidth() > 4)
			_signed ? DrawResultsTable<int64_t, uint64_t>() : DrawResultsTable<uint64_t, uint64_t>();
		else
			_signed ? DrawResultsTable<int64_t, uint32_t>() : DrawResultsTable<uint64_t, uint32_t>();
	} break;
	case FLOAT: {
		if (*Connection::GetAddressWidth() > 4)
			DrawResultsTable<float, uint64_t>();
		else
			DrawResultsTable<float, uint32_t>();
	} break;
	case DOUBLE: {
		if (*Connection::GetAddressWidth() > 4)
			DrawResultsTable<double, uint64_t>();
		else
			DrawResultsTable<double, uint32_t>();
	} break;
	default: { //INT32
		if (*Connection::GetAddressWidth() > 4)
			_signed ? DrawResultsTable<int32_t, uint64_t>() : DrawResultsTable<uint32_t, uint64_t>();
		else
			_signed ? DrawResultsTable<int32_t, uint32_t>() : DrawResultsTable<uint32_t, uint32_t>();
	} break;
			}
		}
	}


	ImGui::PushItemWidth(groupWidth/5);
	ImGui::LabelText("Results", std::to_string(_resultCount).c_str());
	ImGui::SameLine();

	ImGui::BeginGroup();
	ImGui::PushItemWidth(groupWidth);
	ImGui::InputText("Address", _pokeAddressText, IM_ARRAYSIZE(_pokeAddressText));
	ImGui::PushItemWidth(groupWidth);
	ImGui::InputText("Value", _pokeValueText, IM_ARRAYSIZE(_pokeValueText));
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
				if (*Connection::GetAddressWidth() > 4)
					PokeArray<uint8_t, uint64_t>();
				else
					PokeArray<uint8_t, uint32_t>();
			} break;
			case INT16: {
				if (*Connection::GetAddressWidth() > 4)
					PokeArray<uint16_t, uint64_t>();
				else
					PokeArray<uint16_t, uint32_t>();
			} break;
			case INT64: {
				if (*Connection::GetAddressWidth() > 4)
					PokeArray<uint64_t, uint64_t>();
				else
					PokeArray<uint64_t, uint32_t>();
			} break;
			case FLOAT: {
				if (*Connection::GetAddressWidth() > 4)
					PokeArray<float, uint64_t>();
				else
					PokeArray<float, uint32_t>();
			} break;
			case DOUBLE: {
				if (*Connection::GetAddressWidth() > 4)
					PokeArray<double, uint64_t>();
				else
					PokeArray<double, uint32_t>();
			} break;
			default: { //OperativeArray<INT32>
				if (*Connection::GetAddressWidth() > 4)
					PokeArray<uint32_t, uint64_t>();
				else
					PokeArray<uint32_t, uint32_t>();
			} break;
			}break;
		} break;
		case TEXT: {

		} break;
		case COLOR: {

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
				if (*Connection::GetAddressWidth() > 4)
					PokeValue<uint8_t, uint64_t>();
				else
					PokeValue<uint8_t, uint32_t>();
			} break;
			case INT16: {
				stream >> *(uint16_t*)_pokeValue;
				if (*Connection::GetAddressWidth() > 4)
					PokeValue<uint16_t, uint64_t>();
				else
					PokeValue<uint16_t, uint32_t>();
			} break;
			case INT64: {
				stream >> *(uint64_t*)_pokeValue;
				if (*Connection::GetAddressWidth() > 4)
					PokeValue<uint64_t, uint64_t>();
				else
					PokeValue<uint64_t, uint32_t>();
			} break;
			case FLOAT: {
				stream >> *(float*)_pokeValue;
				if (*Connection::GetAddressWidth() > 4)
					PokeValue<float, uint64_t>();
				else
					PokeValue<float, uint32_t>();
			} break;
			case DOUBLE: {
				stream >> *(double*)_pokeValue;
				if (*Connection::GetAddressWidth() > 4)
					PokeValue<double, uint64_t>();
				else
					PokeValue<double, uint32_t>();
			} break;
			default: {
				stream >> *(uint32_t*)_pokeValue;
				if (*Connection::GetAddressWidth() > 4)
					PokeValue<uint32_t, uint64_t>();
				else
					PokeValue<uint32_t, uint32_t>();
			} break;
			}
		}
		}
	}

	ImGui::SameLine();
	ImGui::Checkbox("Previous Value", &_pokePrevious);
	ImGui::SameLine();
	HelpMarker("If \"Multi-Poke\" is checked this will enable poking previous value. No matter what's in the \"Value\" text field. If this one is unchecked the expression inside \"Value\" will be written to all selected result addresses.");
	ImGui::SameLine();
	ImGui::Checkbox("Multi-Poke", &_multiPoke);

	ImGui::EndGroup();

	ImGui::BeginGroup();//page control
	{	
		ImGui::BeginGroup();
		{
			std::stringstream stream;
			if (ImGui::Button("^"))
			{
				if (_currentPageValue < _pagesAmountValue)
				{
					stream << ++_currentPageValue;
					stream >> _currentPageText;
				}
			}

			if(ImGui::Button("v"))
			{
				if (_currentPageValue > 1)
				{
					stream << --_currentPageValue;
					stream >> _currentPageText;
				}
			}
		}
		ImGui::EndGroup();
	
		ImGui::SameLine();

		ImGui::BeginGroup();
		ImGui::PushItemWidth(groupWidth/2);
			if (ImGui::InputText("Page", _currentPageText, IM_ARRAYSIZE(_currentPageText)))
			{
				std::stringstream(_currentPageText) >> _currentPageValue;
				
				if (_currentPageValue < 1 || _currentPageValue > _pagesAmountValue)
					ResetCurrentPage();
			}

			ImGui::SameLine();
			ImGui::PushItemWidth(groupWidth/2);
			ImGui::InputText("Of", _pagesAmountText, IM_ARRAYSIZE(_pagesAmountText));
	}	ImGui::EndGroup();
	ImGui::EndGroup();

	ImGui::EndGroup();
}

void MungPlex::Search::PickColorFromScreen()
{
	POINT point;
	COLORREF color;
	HDC hdc;

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

	hdc = GetDC(NULL);
	if (hdc == NULL)
		return;

	if (!GetCursorPos(&point))
		return;

	color = GetPixel(hdc, point.x, point.y);
	if (color == CLR_INVALID)
		return;

	ReleaseDC(GetDesktopWindow(), hdc);

	_colorVec.x = (float)GetRValue(color) / (float)255;
	_colorVec.y = (float)GetGValue(color) / (float)255;
	_colorVec.z = (float)GetBValue(color) / (float)255;
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

	

	++_iterationCount;
	_selectedIndices.resize(_maxResultsPerPage);
	_pagesAmountValue = _resultCount / _maxResultsPerPage;

	if (_resultCount % _maxResultsPerPage > 0)
		++_pagesAmountValue;

	std::stringstream stream;
	stream << std::dec << _pagesAmountValue;
	stream >> _pagesAmountText;
}

void MungPlex::Search::PrimitiveTypeSearch()
{
	std::stringstream stream1, stream2; //i know this is bloated but better than keeping track if the stream is still good
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
				_resultCount = SetUpAndIterate<int8_t>(knownVal, knownValSecondary);
				break;
			case INT16:
				_resultCount = SetUpAndIterate<int16_t>(knownVal, knownValSecondary);
				break;
			case INT32:
				_resultCount = SetUpAndIterate<int32_t>(knownVal, knownValSecondary);
				break;
			case INT64:
				_resultCount = SetUpAndIterate<int64_t>(knownVal, knownValSecondary);
				break;
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
				_resultCount = SetUpAndIterate<uint8_t>(knownVal, knownValSecondary);
				break;
			case INT16:
				_resultCount = SetUpAndIterate<uint16_t>(knownVal, knownValSecondary);
				break;
			case INT32:
				_resultCount = SetUpAndIterate<uint32_t>(knownVal, knownValSecondary);
				break;
			case INT64:
				_resultCount = SetUpAndIterate<uint64_t>(knownVal, knownValSecondary);
				break;
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

		switch (_currentPrimitiveTypeSelect)
		{
		case FLOAT:
			_resultCount = SetUpAndIterate<float>(knownVal, knownValSecondary);
			break;
		case DOUBLE:
			_resultCount = SetUpAndIterate<double>(knownVal, knownValSecondary);
			break;
		}
	}
}

void MungPlex::Search::ArrayTypeSearch()
{
	std::string strArray = std::string(_knownValueText);
	std::string strArraySecondary = std::string(_secondaryKnownValueText);

	if (_currentArrayTypeSelect < FLOAT)
	{
		switch (_currentArrayTypeSelect)
		{
		case INT8: {
			OperativeArray<uint8_t> arrayP(strArray);
			OperativeArray<uint8_t> arrayS(strArraySecondary);
			_resultCount = SetUpAndIterate<OperativeArray<uint8_t>>(arrayP, arrayS);
		}break;
		case INT16: {
			OperativeArray<uint16_t> arrayP(strArray);
			OperativeArray<uint16_t> arrayS(strArraySecondary);
			_resultCount = SetUpAndIterate<OperativeArray<uint16_t>>(arrayP, arrayS);
		}break;
		case INT32: {
			OperativeArray<uint32_t> arrayP(strArray);
			OperativeArray<uint32_t> arrayS(strArraySecondary);
			_resultCount = SetUpAndIterate<OperativeArray<uint32_t>>(arrayP, arrayS);
		}break;
		case INT64: {
			OperativeArray<uint64_t> arrayP(strArray);
			OperativeArray<uint64_t> arrayS(strArraySecondary);
			_resultCount = SetUpAndIterate<OperativeArray<uint64_t>>(arrayP, arrayS);
		}break;
		}
	}
	else if (_currentValueTypeSelect == FLOAT || _currentValueTypeSelect == DOUBLE)
	{
		switch (_currentArrayTypeSelect)
		{
		case FLOAT: {
			OperativeArray<float> arrayP(strArray);
			OperativeArray<float> arrayS(strArraySecondary);
			_resultCount = SetUpAndIterate<OperativeArray<float>>(arrayP, arrayS);
		}break;
		case DOUBLE: {
			OperativeArray<double> arrayP(strArray);
			OperativeArray<double> arrayS(strArraySecondary);
			_resultCount = SetUpAndIterate<OperativeArray<double>>(arrayP, arrayS);
		}break;
		}
	}
}

void MungPlex::Search::TextTypeSearch()
{

}

void MungPlex::Search::ColorTypeSearch()
{

}