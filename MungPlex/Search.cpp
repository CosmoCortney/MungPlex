#include"Search.h"

static float scale = 2.0f;
void MungPlex::Search::DrawWindow()
{
	ImGui::Begin("Search");

	GetInstance().MungPlex::Search::DrawValueTypeOptions();

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
		MungPlex::SetUpCombo("Value Type", GetInstance()._searchValueTypes, GetInstance()._currentValueTypeSelect);

		_disableBecauseNoArray = GetInstance()._currentValueTypeSelect != ARRAY;
		_disableBecauseNoColor = _currentValueTypeSelect < RGB_BYTE;
		_disableBecauseNoInt = !(_currentValueTypeSelect < FLOAT || (_currentValueTypeSelect == ARRAY && (_currentArrayTypeSelect < FLOAT)));

		//ImGui::SameLine();

		if (_disableBecauseNoPrimitive) ImGui::BeginDisabled();
			MungPlex::SetUpCombo("Primitive Type", GetInstance()._searchPrimitiveTypes, GetInstance()._currentPrimitiveTypeSelect);
		if (_disableBecauseNoPrimitive) ImGui::EndDisabled();

		if (_disableBecauseNoArray) ImGui::BeginDisabled();
			MungPlex::SetUpCombo("Array Type", GetInstance()._searchPrimitiveTypes, GetInstance()._currentArrayTypeSelect);
		if (_disableBecauseNoArray) ImGui::EndDisabled();

		//ImGui::SameLine();

		if (_disableBecauseNoText) ImGui::BeginDisabled();
			MungPlex::SetUpCombo("Text Type", GetInstance()._searchTextTypes, GetInstance()._currentTextTypeSelect);
		if (_disableBecauseNoText) ImGui::EndDisabled();
		
		if (_disableBecauseNoColor) ImGui::BeginDisabled();
			MungPlex::SetUpCombo("Color Type", GetInstance()._searchColorTypes, GetInstance()._currentColorTypeSelect);
		if (_disableBecauseNoColor) ImGui::EndDisabled();

		if (!_disableBecauseNoInt) ImGui::BeginDisabled();
		ImGui::SliderFloat("% Precision", &GetInstance()._precision, 1.0f, 100.0f, "%0.2f", NULL);
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
			ImGui::ColorPicker3("##MyColor##6", (float*)&GetInstance()._colorVec, (_useColorWheel ? ImGuiColorEditFlags_PickerHueWheel : ImGuiColorEditFlags_PickerHueBar) );
				
				

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
		GetInstance()._regions = MungPlex::Connection::GetRegions();
		ImGui::SeparatorText("Range Options");
		_RegionSelectSignalCombo.Draw("Region", _regions, _currentRegionSelect);
		int changed;

		ImGui::InputText("Start at (hex)", _rangeStartText, IM_ARRAYSIZE(_rangeStartText)/*, ImGuiInputTextFlags_CallbackEdit, static_cast<ImGuiInputTextFlags_CallbackAlways>(MyInputTextCallback)*/);
		ImGui::InputText("End at (hex)", _rangeEndText, IM_ARRAYSIZE(_rangeEndText));
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
		ImGui::Button("Cancel");
		ImGui::InputText("Known Value", _knownValueText, IM_ARRAYSIZE(_knownValueText));

		MungPlex::SetUpCombo("Comparision Type", GetInstance()._searchComparasionType, GetInstance()._currentComparisionTypeSelect);

		std::vector<std::string> conditionTypeItems;
		conditionTypeItems.resize(GetInstance()._searchConditionTypes.size());
		int typeIterator = conditionTypeItems.size();

		if ((GetInstance()._currentValueTypeSelect > INT64 && GetInstance()._currentValueTypeSelect != ARRAY)
			|| (GetInstance()._currentValueTypeSelect == ARRAY && GetInstance()._currentArrayTypeSelect > INT64))
		{
			if (GetInstance()._currentConditionTypeSelect > LOWER_EQUAL)
				GetInstance()._currentConditionTypeSelect = EQUAL;

			typeIterator = LOWER_EQUAL + 1;
			conditionTypeItems.resize(LOWER_EQUAL + 1);
		}

		for (int i = 0; i < typeIterator; ++i)
		{
			conditionTypeItems[i] = GetInstance()._searchConditionTypes[i].first;
		}

		MungPlex::SetUpCombo("Condition Type", conditionTypeItems, GetInstance()._currentConditionTypeSelect);

		ImGui::InputText("Alignment", _alignmentText, IM_ARRAYSIZE(_alignmentText));
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
	ImGui::BeginGroup();
	enum ContentsType { CT_Text, CT_FillButton };
	static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY;
	static bool display_headers = true;
	static int contents_type = CT_Text;
	std::wstring currentModule;

	if (!ImGui::BeginTable("Results", 4, flags))
		return;

	ImGui::TableSetupColumn("Address");
	ImGui::TableSetupColumn("Value");
	ImGui::TableSetupColumn("Previous");
	ImGui::TableSetupColumn("Difference");
	ImGui::TableHeadersRow();

	currentModule.reserve(1024);

	for (int row = 0; row < _resultCount; ++row)
	{
		ImGui::TableNextRow();
		for (int column = 0; column < 4; ++column)
		{
			std::stringstream stream;
			ImGui::TableSetColumnIndex(column);
			char buf[256];

			switch (column)
			{
			case 0:
				sprintf(buf, "%llX", _searchResults[row]._address);
				break;
			case 1:
				sprintf(buf, "%llX", _searchResults[row]._currentValue);
				break;
			case 2:
				sprintf(buf, "%u", _searchResults[row]._previousValue);
				break;
			case 3:
				sprintf(buf, "%u", _searchResults[row]._difference);
				break;
			}

			if (contents_type == CT_Text)
				ImGui::TextUnformatted(buf);
			else if (contents_type == CT_FillButton)
				ImGui::Button(buf, ImVec2(-FLT_MIN, 0.0f));
		}
	}

	ImGui::EndTable();
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
	std::stringstream stream;
	stream << std::hex << std::string(_rangeStartText);
	stream >> _rangeStartValue;
	uint64_t offset = _rangeStartValue - MungPlex::Connection::GetRegions()[GetInstance()._currentRegionSelect].Base;
	HANDLE handle = MungPlex::Connection::GetCurrentHandle();
	void* baseAddressEx = MungPlex::Connection::GetRegions()[GetInstance()._currentRegionSelect].BaseLocationProcess;
	uint64_t size;
	stream.str(std::string());
	stream.clear();
	stream << std::hex << std::string(_rangeEndText);
	stream >> _rangeEndValue;
	size = _rangeEndValue - _rangeStartValue + 1;
	std::cout << std::hex << size << std::endl;
	std::cout << std::hex << handle << std::endl;
	std::cout << std::hex << (uint64_t)((char*)baseAddressEx + offset) << std::endl;


	if (_currentComparisionTypeSelect == ComparasionType::UNKNOWN)
	{
		if (_iterationCount == 0)
		{
			std::wstring path = L"F:\\test\\file.bin";
			//Xertz::MemDump dump;
			//dump = Xertz::MemDump(handle, baseAddressEx, size);
			//dump.SaveDump(path);
			//_memDumps[0].SaveDump(path);
			//std::cout << "3\n";

			//uint32_t offs[5] = { 1,2,3,4,5 };
			//double vals[5] = { 1,2,3,4,5 };

			//Xertz::MemCompareResult<double, uint32_t> test(false, path);
			
			//test.SetResultOffsets(offs);
			//test.SetResultValues(vals);
			//test.SetResultCount(5);
			//std::cout << "saved: " << test.SaveResults();

			//test.LoadResults(false);

			Xertz::MemCompare<uint32_t, uint32_t>::Iterate(Connection::GetCurrentPID(), baseAddressEx, size, 0, false);
		}
	}
}