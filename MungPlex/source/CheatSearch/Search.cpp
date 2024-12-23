﻿#pragma once
#include <algorithm>
#include "Search.hpp"

inline const MungPlex::StringIdPairs MungPlex::Search::_searchValueTypes =
{
	{ "Primitive", "Array", "Text", "Color"}, 
	{ PRIMITIVE,   ARRAY,   TEXT,   COLOR },
	"Value Types:"
};

inline const MungPlex::StringIdPairs MungPlex::Search::_endiannesses =
{
	{ "Little", "Big" },
	{  LITTLE,  BIG },
	"Endianness:"
};

inline const MungPlex::StringIdPairs MungPlex::Search::_searchPrimitiveTypes =
{
	{ "Int 8 (1 Byte)", "Int 16 (2 Bytes)", "Int 32 (4 Bytes)", "Int 64 (8 Bytes)", "Float Single", "Float Double" },
	{ INT8,             INT16,              INT32,              INT64,              FLOAT,           DOUBLE },
	"Primitive Type:"
};

inline const MungPlex::StringIdPairs MungPlex::Search::_searchArrayTypes =
{
	{ "Int 8 (1 Byte)", "Int 16 (2 Bytes)", "Int 32 (4 Bytes)", "Int 64 (8 Bytes)" },
	{ INT8,             INT16,              INT32,              INT64 },
	"Array Type:"
}; //remove once Arrays support floats

 inline const MungPlex::StringIdPairs MungPlex::Search::_searchColorTypes =
{
	{ "RGB 888 (3 Bytes)", "RGBA 8888 (4 Bytes)", "RGBF (3 Floats)", "RGBAF (4 Floats)", "RGB 565 (2 Bytes)", "RGB 5A3 (2 Bytes)" },
	{ LitColor::RGB888,    LitColor::RGBA8888,    LitColor::RGBF,    LitColor::RGBAF,    LitColor::RGB565,    LitColor::RGB5A3 },
	"Color Type:"
};

inline const MungPlex::StringIdPairs MungPlex::Search::_searchConditionTypes =
{
	{ "Equal (==)",         "Unequal (!=)",         "Greater (>)",          "Greater or Equal (>=)",      "Lower (<))",         "Lower or Equal (<=)",      "Increased by",              "Decreased by",              "Value Between",        "Value Not Between",        "AND (has all true bits)", "OR (has at least 1 true bit)" },
	{ MemoryCompare::EQUAL, MemoryCompare::UNEQUAL, MemoryCompare::GREATER, MemoryCompare::GREATER_EQUAL, MemoryCompare::LOWER, MemoryCompare::LOWER_EQUAL, MemoryCompare::INCREASED_BY, MemoryCompare::DECREASED_BY, MemoryCompare::BETWEEN, MemoryCompare::NOT_BETWEEN, MemoryCompare::AND,        MemoryCompare::OR  },
	"Condition:"
};

inline const MungPlex::StringIdPairs MungPlex::Search::_searchConditionTypesArray =
{
	{ "Equal (==)",         "Unequal (!=)" },
	{ MemoryCompare::EQUAL, MemoryCompare::UNEQUAL },
	"Condition:"
};

inline const MungPlex::StringIdPairs MungPlex::Search::_searchConditionTypesFloat =
{
	{ "Equal (==)",         "Unequal (!=)",         "Greater (>)",          "Greater or Equal (>=)",      "Lower (<))",         "Lower or Equal (<=)",      "Increased by",              "Decreased by",              "Value Between",        "Value Not Between" },
	{ MemoryCompare::EQUAL, MemoryCompare::UNEQUAL, MemoryCompare::GREATER, MemoryCompare::GREATER_EQUAL, MemoryCompare::LOWER, MemoryCompare::LOWER_EQUAL, MemoryCompare::INCREASED_BY, MemoryCompare::DECREASED_BY, MemoryCompare::BETWEEN, MemoryCompare::NOT_BETWEEN },
	"Condition:"
};

inline const MungPlex::StringIdPairs MungPlex::Search::_searchConditionTypesColor =
{
	{ "Equal (==)",         "Unequal (!=)",         "Greater (>)",          "Greater or Equal (>=)",      "Lower (<))",         "Lower or Equal (<=)" },
	{ MemoryCompare::EQUAL, MemoryCompare::UNEQUAL, MemoryCompare::GREATER, MemoryCompare::GREATER_EQUAL, MemoryCompare::LOWER, MemoryCompare::LOWER_EQUAL },
	"Condition:"
};

inline const MungPlex::StringIdPairs MungPlex::Search::_searchConditionTypesText =
{
	{ "Equal (==)" },
	{ MemoryCompare::EQUAL },
	"Condition:"
};

 inline const MungPlex::StringIdPairs MungPlex::Search::_searchComparasionType =
{
	{ "Unknwon/Initial", "Known Value" },
	{ 0,                 MemoryCompare::KNOWN },
	"Comparison:"
};

MungPlex::Search::Search()
{
	_RegionSelectSignalCombo.ConnectOnIndexChanged(Slot_IndexChanged);
	_RegionSelectSignalCombo.ConnectOnItemCountChanged(Slot_ItemCountChanged);
	_RegionSelectSignalCombo.ConnectOnTextChanged(Slot_TextChanged);
	//_rangeStartInput.ConnectOnTextChanged(std::bind(Slot_RangeTextChanged, _rangeStartInput.GetData(), std::ref(_rangeStartValue)));
	//_rangeEndInput.ConnectOnTextChanged(std::bind(Slot_RangeTextChanged, _rangeEndInput.GetData(), std::ref(_rangeEndValue)));

	_selectedIndices.resize(_maxResultsPerPage);
	_alignmentValue = Settings::GetSearchSettings().DefaultAlignment;
	_cached = Settings::GetSearchSettings().DefaultCached;
	_disableUndo = Settings::GetSearchSettings().DefaultDisableUndo;
	_caseSensitive = Settings::GetSearchSettings().DefaultCaseSensitive;
	_useColorWheel = Settings::GetSearchSettings().DefaultColorWheel;
	_hex = Settings::GetSearchSettings().DefaultValuesHex;
	_resultsPath = MT::Convert<std::string, std::wstring>(Settings::GetGeneralSettings().DocumentsPath.StdStrNoLeadinZeros(), MT::UTF8, MT::UTF16LE) + L"\\MungPlex\\Search\\";
	setFormatting();
}

void MungPlex::Search::DrawWindow()
{
	static bool stateSet = false;
	static bool busySearching = false;

	if (ImGui::Begin("Search"))
	{
		busySearching = GetInstance()._busySearching;

		if (!Connection::IsConnected() || busySearching)
			ImGui::BeginDisabled();
		else
		{
			if (!stateSet && Settings::GetGeneralSettings().EnableRichPresence && ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows))
			{
				Connection::GetDiscordRichPresence().SetRichPresenceState("Memory Search");
				stateSet = true;
			}
		}

		ImVec2 childXY = { ImGui::GetContentRegionAvail().x * 1.0f, ImGui::GetContentRegionAvail().y * 0.35f };

		ImGui::BeginChild("child_searchTop", childXY, ImGuiChildFlags_ResizeY);
		{
			childXY = { ImGui::GetContentRegionAvail().x * 0.5f, ImGui::GetContentRegionAvail().y };

			ImGui::BeginChild("child_searchTopLeft", childXY, ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeX);
			{
				GetInstance().drawValueTypeOptions();
				GetInstance().drawRangeOptions();
			}
			ImGui::EndChild();

			ImGui::SameLine();

			GetInstance().drawSearchOptions();
		}
		ImGui::EndChild();

		GetInstance().drawResultsArea();

		if (!Connection::IsConnected() || busySearching)
			ImGui::EndDisabled();
	}
	else
		stateSet = false;

	if(!ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows))
		stateSet = false;

	ImGui::End();
}

void MungPlex::Search::drawValueTypeOptions()
{
	const ImVec2 childXY = { ImGui::GetContentRegionAvail().x * 1.0f, ImGui::GetContentRegionAvail().y * 0.4f };

	ImGui::BeginChild("child_valueOptions", childXY, ImGuiWindowFlags_NoScrollbar | ImGuiChildFlags_AutoResizeY);
	{
		ImGui::SeparatorText("Value Type Options");

		if (_searchActive) ImGui::BeginDisabled();
		{
			ImGui::BeginGroup();
			{
				if (SetUpPairCombo(_searchValueTypes, &_currentValueTypeSelect, 0.5f, 0.4f))
				{
					_updateLabels = true;
					_knownValueInput.SetText("");

					switch (_searchValueTypes.GetId(_currentValueTypeSelect))
					{
					case ARRAY:
						_knownValueInput.SetLabel("Array Expression:");
						break;
					case COLOR:
						_knownValueInput.SetLabel("Color Expression:");
						break;
					case TEXT:
						_knownValueInput.SetLabel("Text:");
						break;
					default:
						_knownValueInput.SetLabel("Value:");
					}

					_secondaryKnownValueInput.SetText("");
					setFormatting();
				}

				if (_currentValueTypeSelect == TEXT || _currentValueTypeSelect == COLOR)
					_currentcomparisonTypeSelect = MemoryCompare::KNOWN;
			} if (_searchActive) ImGui::EndDisabled();

			if (_currentValueTypeSelect == PRIMITIVE || _currentPrimitiveTypeSelect >= FLOAT)
			{
				ImGui::SameLine();
				if (ImGui::Checkbox("Hex", &GetInstance()._hex))
					setFormatting();
			}

			if (_searchActive) ImGui::BeginDisabled();
			{
				switch (_currentValueTypeSelect)
				{
				case ARRAY:
					if (SetUpPairCombo(_searchArrayTypes, &_currentArrayTypeSelect, 0.5f, 0.4f)) //use primitive types here once Arrays support floats
						setRecommendedValueSettings(ARRAY);
					break;
				case COLOR:
				{
					if (SetUpPairCombo(_searchColorTypes, &_currentColorTypeSelect, 0.5f, 0.4f))
						setRecommendedValueSettings(COLOR);

					if (_currentColorTypeSelect != LitColor::RGB5A3)
						_forceAlpha = false;

					ImGui::SameLine();

					if (_currentColorTypeSelect != LitColor::RGB5A3) ImGui::BeginDisabled();
					ImGui::Checkbox("Force Alpha", &_forceAlpha);
					if (_currentColorTypeSelect != LitColor::RGB5A3) ImGui::EndDisabled();
				}break;
				case TEXT:
					if (SetUpPairCombo(TextTypes, &_currentTextTypeIndex, 0.5f, 0.4f))
					{
						_currentTextTypeSelect = TextTypes.GetId(_currentTextTypeIndex);
						setRecommendedValueSettings(TEXT);
					} 
					break;
				default: //PRIMITIVE
					if (SetUpPairCombo(_searchPrimitiveTypes, &_currentPrimitiveTypeSelect, 0.5f, 0.4f))
						setRecommendedValueSettings(PRIMITIVE);
				}

				ImGui::SameLine();

				if (_currentPrimitiveTypeSelect < FLOAT && _currentValueTypeSelect == PRIMITIVE)
					ImGui::Checkbox("Signed", &_signed);
			}
			ImGui::EndGroup();
		}
		if (_searchActive) ImGui::EndDisabled();
	}
	ImGui::EndChild();
}

void MungPlex::Search::setFormatting()
{
	if (_currentValueTypeSelect == PRIMITIVE)
		_formatting = GetStringFormatting(_currentPrimitiveTypeSelect, _signed, _hex);
	else if (_currentValueTypeSelect == ARRAY)
		_formatting = GetStringFormatting(_currentArrayTypeSelect, _signed, _hex);
}

void MungPlex::Search::drawRangeOptions()
{
	const ImVec2 childXY = { ImGui::GetContentRegionAvail().x * 1.0f, ImGui::GetContentRegionAvail().y * 1.0f };

	ImGui::BeginChild("child_rangeOptions", childXY);
	{
		ImGui::SeparatorText("Range Options");

		if (_searchActive) ImGui::BeginDisabled();
		{
			static std::stringstream stream;
			_RegionSelectSignalCombo.Draw(ProcessInformation::GetSystemRegionList_(), _currentRegionSelect, 0.5f, 0.4f);

			ImGui::SameLine();

			SetUpPairCombo(_endiannesses, &_endiannessSelect, 1.0f, 0.4f);

			_rangeStartInput.Draw(0.5f, 0.4f, true);

			ImGui::SameLine();

			_rangeEndInput.Draw(1.0f, 0.4f, true);

			if (ImGui::Checkbox("Cross-Region", &_crossRegion))
			{
				if (_crossRegion)
				{
					_rangeStartInput.SetValue(_regions.front().Base);
					_rangeEndInput.SetValue(_regions.back().Base + _regions.back().Size - 1);
				}
				else
				{
					_rangeStartInput.SetValue(_regions[_currentRegionSelect].Base);
					_rangeEndInput.SetValue(_regions[_currentRegionSelect].Base + _regions[_currentRegionSelect].Size - 1);
				}
			}

			ImGui::SameLine();
			ImGui::Checkbox("Re-reorder Region", &_rereorderRegion);
			ImGui::SameLine();
			HelpMarker("Some emulators like Project64 reorder the emulatoed RAM in 4 byte chunks of the opposite endianness which requires re-reordering before scanning. The best option is auto-selected for you, but it might be helpful to set it manually if you encounter a reordered region or structure on another platform.");
		
			if (ProcessInformation::GetProcessType() == ProcessInformation::NATIVE)
			{
				static bool refresh = false;
				ImGui::SameLine();

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
		}
		if (_searchActive) ImGui::EndDisabled();
	}
	ImGui::EndChild();
}

void MungPlex::Search::drawPrimitiveSearchOptions()
{
	static bool disablePrimaryValueText = false;
	static bool disableSecondaryValueText = true;

	if (_updateLabels)
	{
		switch (_currentConditionTypeSelect)
		{
			case MemoryCompare::BETWEEN:
			{
				_knownValueInput.SetLabel("Lowest:");
				_knownValueInput.SetLabel("Highest:");
				disableSecondaryValueText = false;
			} break;
			case MemoryCompare::NOT_BETWEEN:
			{
				_knownValueInput.SetLabel("Below:");
				_knownValueInput.SetLabel("Above:");
				disableSecondaryValueText = false;
			} break;
			case MemoryCompare::INCREASED_BY:
			{
				_knownValueInput.SetLabel("Increased by:");
				_knownValueInput.SetLabel("Not applicable");
				disableSecondaryValueText = true;
			} break;
			case MemoryCompare::DECREASED_BY:
			{
				_knownValueInput.SetLabel("Decreased by:");
				_knownValueInput.SetLabel("Not applicable");
				disableSecondaryValueText = true;
			} break;
			default:
			{
				_knownValueInput.SetLabel("Value:");
				_knownValueInput.SetLabel("Not applicable");
				disableSecondaryValueText = true;
			}
		}
		_updateLabels = false;
	}

	if (MungPlex::SetUpPairCombo(_searchComparasionType, &_currentcomparisonTypeSelect, 1.0f, 0.4f))
		_updateLabels = true;

	static const StringIdPairs* conditionTypeItems;

	if (_currentPrimitiveTypeSelect < FLOAT)
		conditionTypeItems = &_searchConditionTypes;
	else
		conditionTypeItems = &_searchConditionTypesFloat;

	if (SetUpPairCombo(*conditionTypeItems, &_currentConditionTypeSelect, 1.0f, 0.4f))
		_updateLabels = true;

	if (!_diableBecauseUnknownAndNotRangebased)
		_knownValueInput.Draw(1.0f, 0.4f);

	if (_currentConditionTypeSelect >= MemoryCompare::BETWEEN && _currentConditionTypeSelect <= MemoryCompare::NOT_BETWEEN)
		_secondaryKnownValueInput.Draw(1.0f, 0.4f);

	if (_currentPrimitiveTypeSelect >= FLOAT)
		SetUpSliderFloat("Precision (%%):", &_precision, 75.0f, 100.0f, "%0.2f", 1.0f, 0.4f);
}

void MungPlex::Search::drawArraySearchOptions()
{
	static bool disablePrimaryValueText = false;

	ImGui::BeginGroup();
	
	if (SetUpPairCombo(_searchConditionTypesArray, &_currentConditionTypeSelect, 1.0f, 0.4f))
		_updateLabels = true;

	_knownValueInput.Draw(1.0f, 0.4f);

	//keep just in case float arrays will ever happen
	//if (_disableBecauseNoInt)
	//SetUpSliderFloat("Precision (%%):", &_precision, 75.0f, 100.0f, "%0.2f", 0.5f, 0.4f);
	//if (_disableBecauseNoInt) ImGui::EndDisabled();
	ImGui::EndGroup();
}

void MungPlex::Search::drawColorSearchOptions()
{
	//static std::string knownPrimaryValueLabel = "Color Expression:";

	ImGui::BeginGroup();
	{
		MungPlex::SetUpPairCombo(_searchComparasionType, &_currentcomparisonTypeSelect, 1.0f, 0.4f);

		if (SetUpPairCombo(_searchConditionTypesText, &_currentConditionTypeSelect, 1.0f, 0.4f))
			_updateLabels = true;

		if (!_diableBecauseUnknownAndNotRangebased)
			if(_knownValueInput.Draw(1.0f, 0.4f))
				LitColorExpressionToImVec4(_knownValueInput.GetCString(), &_searchColorVec);

		SetUpSliderFloat("Precision (%%):", &_precision, 75.0f, 100.0f, "%0.2f", 1.0f, 0.4f);
	}
	ImGui::EndGroup();
}

void MungPlex::Search::drawColorSelectOptions()
{
	ImGui::SameLine();

	ImGui::BeginChild("child_colorSelect");
	ImGui::BeginGroup();
	DrawExtraColorPickerOptions(&_useColorWheel, &_searchColorVec);
	ImGui::EndGroup();

	ImGui::BeginGroup();
	{
		if (DrawColorPicker(_currentColorTypeSelect, _forceAlpha, &_searchColorVec, _useColorWheel, 0.75f))
			ColorValuesToCString(_searchColorVec, _currentColorTypeSelect, _knownValueInput.GetData(), _forceAlpha);
	}
	ImGui::EndGroup();
	ImGui::EndChild();
}

void MungPlex::Search::drawTextSearchOptions()
{
	static bool disablePrimaryValueText = false;

	_diableBecauseUnknownAndNotRangebased = _currentcomparisonTypeSelect == 0 && _currentConditionTypeSelect != MemoryCompare::INCREASED_BY && _currentConditionTypeSelect != MemoryCompare::DECREASED_BY;

	ImGui::BeginGroup();
	{
		if (_diableBecauseUnknownAndNotRangebased) ImGui::BeginDisabled();
			_knownValueInput.Draw(1.0f, 0.4f);
		if (_diableBecauseUnknownAndNotRangebased) ImGui::EndDisabled();

		ImGui::Checkbox("Case Sensitive", &_caseSensitive);
	}
	ImGui::EndGroup();
}

void MungPlex::Search::drawSearchOptions()
{
	ImVec2 childXY = { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y };

	ImGui::BeginChild("child_searchOptions", childXY, ImGuiChildFlags_Border);
	{
		ImGui::SeparatorText("Search Options");
		static std::string knownPrimaryValueLabel;
		static std::string knownSecondaryValueLabel;
		static bool disablePrimaryValueText = false;
		static bool disableSecondaryValueText = true;

		_diableBecauseUnknownAndNotRangebased = _currentcomparisonTypeSelect == 0 && _currentConditionTypeSelect != MemoryCompare::INCREASED_BY && _currentConditionTypeSelect != MemoryCompare::DECREASED_BY;
		
		if(_currentValueTypeSelect == COLOR)
			childXY = { ImGui::GetContentRegionAvail().x * 0.5f, ImGui::GetContentRegionAvail().y };
		else
			childXY = { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y };

		ImGui::BeginChild("child_searchLeft", childXY);

		if (_iterationCount) ImGui::BeginDisabled();
		if (SetUpInputInt("Alignment:", &_alignmentValue, 1, 1, 1.0f, 0.4f, true, "This value specifies the increment of the next address to be scanned. 1 means that the following value to be scanned is the at the current address + 1. Here are some recommendations for each value type: Int8/Text/Color/Array<Int8> - 1, Int16/Color/Array<Int16> - 2, Int32/Int64/Float/Double/Color/Array<Int32>/Array<Int64> - 4. Systems that use less than 4MBs of RAM (PS1, SNES, MegaDrive, ...) should always consider an alignment of 1, despite the value recommendations."))
		{
			if (_alignmentValue < 1)
				_alignmentValue = 1;
		}
		if (_iterationCount) ImGui::EndDisabled();

		ImGui::SameLine();

		if (_iterationCount) ImGui::BeginDisabled();
		{
			ImGui::Checkbox("Cached", &GetInstance()._cached);
			ImGui::SameLine();
			HelpMarker("If checked, search results will be kept in RAM. Recommended for PC games!");
			
			ImGui::Checkbox("Disable Undo", &GetInstance()._disableUndo); ImGui::SameLine();
			ImGui::SameLine();
			HelpMarker("If checked, comparing against iterations older than the previous one is disabled to safe memory usage. Recommended for PC games!");
		}
		if (_iterationCount) ImGui::EndDisabled();

		if (!_iterationCount || _disableUndo) ImGui::BeginDisabled();
			SetUpPairCombo(_iterations, &_iterationIndex, 1.0f, 0.4f);
		if (!_iterationCount || _disableUndo) ImGui::EndDisabled();

		switch (_currentValueTypeSelect)
		{
		case ARRAY:
			drawArraySearchOptions();
			break;
		case COLOR:
			drawColorSearchOptions();
			break;
		case TEXT:
			drawTextSearchOptions();
			break;
		default: //PRIMITIVE
			drawPrimitiveSearchOptions();
		}
		
		if (ImGui::Button("Search"))
		{
			if(_iterationCount < 1)
				_regions = ProcessInformation::GetSystemRegionList();

			_searchActive = true;

			if (_searchThread.joinable())
				_searchThread.join();

			_searchThread = boost::thread(&MungPlex::Search::performSearch, this);
		}

		ImGui::SameLine();
		static bool disableResetButton = true;
		disableResetButton = _iterationCount < 1;

		if (disableResetButton) ImGui::BeginDisabled();
		if (ImGui::Button("Reset"))
		{
			MemoryCompare::MemCompare::Reset();
			_iterationCount = 0;
			_iterations.Clear();
			_iterationIndex = 0;
			_searchActive = false;
			_currentPageValue = 0;
		}
		if (disableResetButton) ImGui::EndDisabled();

		ImGui::EndChild();

		if (_currentValueTypeSelect == COLOR)
			drawColorSelectOptions();
	}
	ImGui::EndChild();
}

void MungPlex::Search::drawResultsArea()
{
	const ImVec2 childXY = ImGui::GetContentRegionAvail();
	ImGui::BeginChild("child_searchResults", childXY, true);
	{
		ImGui::SeparatorText("Results Panel");

		drawResultsTable();

		ImGui::SameLine();

		ImGui::BeginGroup();
		{
			ImGui::SeparatorText("Value Poke");

			if (MemoryCompare::MemCompare::GetResultCount() == 0) ImGui::BeginDisabled();
			{
				SetUpLableText("Results:", std::to_string(MemoryCompare::MemCompare::GetResultCount()).c_str(), 32, 1.0f, 0.2f);

				ImGui::BeginGroup();
				{
					_pokeAddressInput.Draw(1.0f, 0.2f);

					if (_pokeValueInput.Draw(1.0f, 0.2f))
					{
						if(_currentValueTypeSelect == COLOR)
							LitColorExpressionToImVec4(_pokeValueInput.GetCString(), &_pokeColorVec);
					}
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

				if (_currentValueTypeSelect != TEXT)
				{

					ImGui::Checkbox("Previous Value", &_pokePrevious);
					ImGui::SameLine();
					HelpMarker("If \"Multi-Poke\" is checked this will enable poking previous values - no matter what's in the \"Value\" text field. If this is unchecked the expression inside \"Value\" will be written to all selected result addresses.");
					ImGui::SameLine();
				}

				ImGui::Checkbox("Multi-Poke", &_multiPoke);

				if (ImGui::Button("Poke"))
					performValuePoke();

				if (ProcessInformation::GetProcessType() == ProcessInformation::CONSOLE)
				{
					ImGui::SameLine();

					if (ImGui::Button("Update Real-Time column"))
						updateLivePreviewOnce();
				}

				if (_currentValueTypeSelect == COLOR)
				{
					DrawExtraColorPickerOptions(&_useColorWheel, &_pokeColorVec);
					if(DrawColorPicker(_currentColorTypeSelect, _forceAlpha, &_pokeColorVec, _useColorWheel, 0.8f))
						ColorValuesToCString(_pokeColorVec, _currentColorTypeSelect, _pokeValueInput.GetData(), _forceAlpha);
				}
			}
			if (MemoryCompare::MemCompare::GetResultCount() == 0) ImGui::EndDisabled();

			ImGui::Dummy(ImVec2(0.0f, ImGui::GetContentRegionAvail().y - 40.f));

			if (_iterationCount > 0) ImGui::BeginDisabled();
			{
				SetUpInputInt("Max. results per page:", &_maxResultsPerPage, 32, 128, 1.0f, 0.5f);
			}
			if (_iterationCount > 0) ImGui::EndDisabled();
		}
		ImGui::EndGroup();
	}
	ImGui::EndChild();
}

void MungPlex::Search::performSearch()
{
	_busySearching = true;
	Log::LogInformation("Search: Iteration " + std::to_string(_iterationCount + 1)); //+1 because it has not been increased yet

	if (ProcessInformation::GetProcessType() != ProcessInformation::CONSOLE)
	{
		_updateThreadFlag = false;

		if (_updateThread.joinable())
			_updateThread.join();
	}

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
		_currentcomparisonTypeSelect = MemoryCompare::KNOWN;

	setUpAndIterate();
	setUpIterationSelect();
	setUpResultPaging();
	_iterationCount = MemoryCompare::MemCompare::GetIterationCount();

	if(_iterationCount < 2)
		prepareLiveUpdateValueList();

	if (ProcessInformation::GetProcessType() != ProcessInformation::CONSOLE)
	{
		_updateThreadFlag = true;
		_updateThread = boost::thread(&MungPlex::Search::updateLivePreviewConditional, this);
	}

	_busySearching = false;
}

void MungPlex::Search::updateLivePreview()
{
	static int rows = 0;

	if (Connection::IsConnected())
	{
		boost::this_thread::sleep_for(boost::chrono::milliseconds(_liveUpdateMilliseconds));

		if (_currentPageValue < _pagesAmountValue)
			rows = _maxResultsPerPage;
		else
			rows = MemoryCompare::MemCompare::GetResultCount() % _maxResultsPerPage;

		for (int row = 0; row < rows; ++row)
		{
			static uint64_t address = 0;
			uint64_t addressIndex = (_currentPageValue - 1) * _maxResultsPerPage + row;
			uint8_t* updateArrayPtr = _updateValues.data();

			switch (ProcessInformation::GetAddressWidth())
			{
			case 1:
				address = MemoryCompare::MemCompare::GetResults().GetAddressAllRanges<uint8_t>(addressIndex);
				break;
			case 2:
				address = MemoryCompare::MemCompare::GetResults().GetAddressAllRanges<uint16_t>(addressIndex);
				break;
			case 8:
				address = MemoryCompare::MemCompare::GetResults().GetAddressAllRanges<uint64_t>(addressIndex);
				break;
			default: //4
				address = MemoryCompare::MemCompare::GetResults().GetAddressAllRanges<uint32_t>(addressIndex);
			}

			switch (_currentValueTypeSelect)
			{
			case ARRAY:
			{
				for (int i = 0; i < _arrayItemCount; ++i)
					switch (_currentArrayTypeSelect)
					{
					case INT8:
						*(updateArrayPtr + row * _arrayItemCount + i) = ProcessInformation::ReadValue<uint8_t>(address + i);
						break;
					case INT16:
						*reinterpret_cast<uint16_t*>(updateArrayPtr + row * _arrayItemCount * sizeof(uint16_t) + i * sizeof(uint16_t))
							= ProcessInformation::ReadValue<uint16_t>(address + i * sizeof(uint16_t));
						break;
					case INT64:
						*reinterpret_cast<uint64_t*>(updateArrayPtr + row * _arrayItemCount * sizeof(uint64_t) + i * sizeof(uint64_t))
							= ProcessInformation::ReadValue<uint64_t>(address + i * sizeof(uint64_t));
						break;
						/*case FLOAT:
							break;
						case DOUBLE:
							break;*/
					default: //INT32
						*reinterpret_cast<uint32_t*>(updateArrayPtr + row * _arrayItemCount * sizeof(uint32_t) + i * sizeof(uint32_t))
							= ProcessInformation::ReadValue<uint32_t>(address + i * sizeof(uint32_t));
					}
			}break;
			case COLOR:
			{
				static int updateValuesSize = 0;

				switch (_currentColorTypeSelect)
				{
				case LitColor::RGBF:
				{
					for (int i = 0; i < 3; ++i)
					{
						*reinterpret_cast<float*>(updateArrayPtr + row * 3 * sizeof(float) + i * sizeof(float))
							= ProcessInformation::ReadValue<float>(address + i * sizeof(float));
					}
				} break;
				case LitColor::RGBAF:
				{
					for (int i = 0; i < 4; ++i)
					{
						*reinterpret_cast<float*>(updateArrayPtr + row * 4 * sizeof(float) + i * sizeof(float))
							= ProcessInformation::ReadValue<float>(address + i * sizeof(float));
					}
				} break;
				case LitColor::RGB565: case LitColor::RGB5A3:
					*reinterpret_cast<uint16_t*>(updateArrayPtr + row * sizeof(uint16_t))
						= ProcessInformation::ReadValue<uint16_t>(address);
					break;
				default: //RGB888, RGBA8888
					*reinterpret_cast<uint32_t*>(updateArrayPtr + row * sizeof(uint32_t))
						= ProcessInformation::ReadValue<uint32_t>(address);
				}
			}break;
			case TEXT:
			{
				static int strLength = 0;
				if (!strLength)
					strLength = MemoryCompare::MemCompare::GetResults().GetValueWidth();

				for (int i = 0; i < strLength; ++i)
					*(updateArrayPtr + row * strLength + i) = ProcessInformation::ReadValue<uint8_t>(address + i);
			}break;
			default: //PRIMITIVE
			{
				switch (_currentPrimitiveTypeSelect)
				{
				case INT8:
					*(updateArrayPtr + row) = ProcessInformation::ReadValue<uint8_t>(address);
					break;
				case INT16:
					*reinterpret_cast<uint16_t*>(updateArrayPtr + row * sizeof(uint16_t))
						= ProcessInformation::ReadValue<uint16_t>(address);
					break;
				case INT64:
					*reinterpret_cast<uint64_t*>(updateArrayPtr + row * sizeof(uint64_t))
						= ProcessInformation::ReadValue<uint64_t>(address);
					break;
				case FLOAT:
					*reinterpret_cast<float*>(updateArrayPtr + row * sizeof(float))
						= ProcessInformation::ReadValue<float>(address);
					break;
				case DOUBLE:
					*reinterpret_cast<double*>(updateArrayPtr + row * sizeof(double))
						= ProcessInformation::ReadValue<double>(address);
					break;
				default: //INT32
					*reinterpret_cast<uint32_t*>(updateArrayPtr + row * sizeof(uint32_t))
						= ProcessInformation::ReadValue<uint32_t>(address);
				}
			}
			}
		}
	}
}

void MungPlex::Search::updateLivePreviewOnce()
{
	updateLivePreview();
}

void MungPlex::Search::updateLivePreviewConditional()
{
	while (_updateThreadFlag)
		updateLivePreview();
}

void MungPlex::Search::setUpIterationSelect()
{
	_iterationCount = MemoryCompare::MemCompare::GetIterationCount();

	if (_iterationCount < _iterations.GetCount())
		_iterations.PopBack(1 + _iterations.GetCount() - _iterationCount);

	_iterations.PushBack(std::to_string(_iterationCount) + ": " + _searchComparasionType.GetStdString(_currentcomparisonTypeSelect)
		+ (_iterationCount < 2 && _currentcomparisonTypeSelect == 0 ? "" : ", " + _searchConditionTypes.GetStdString(_currentConditionTypeSelect)), _currentConditionTypeSelect);
	_iterationIndex = _iterationCount-1;
	_selectedIndices.resize(_maxResultsPerPage);
}

void MungPlex::Search::setLiveUpdateRefreshRate()
{
	if (_maxResultsPerPage <= 256)
		_liveUpdateMilliseconds = 16;
	else
		_liveUpdateMilliseconds = _maxResultsPerPage >> 4;
}

void MungPlex::Search::prepareLiveUpdateValueList()
{
	_updateValues.clear();
	int updateValuesSize = 0;

	switch (_currentValueTypeSelect)
	{
		case ARRAY:
		{
			_arrayItemCount = std::ranges::count(_knownValueInput.GetStdStringNoZeros(), ',') + 1;

			switch (_currentArrayTypeSelect)
			{
			case INT8:
				updateValuesSize = _arrayItemCount;
				break;
			case INT16:
				updateValuesSize = _arrayItemCount * 2;
				break;
			case INT64: case DOUBLE:
				updateValuesSize = _arrayItemCount * 8;
				break;
			default:
				updateValuesSize = _arrayItemCount * 4;
			}
		} break;
		case COLOR:
		{
			switch (_currentColorTypeSelect)
			{
			case LitColor::RGBF:
				updateValuesSize = 12;
				break;
			case LitColor::RGBAF:
				updateValuesSize = 16;
				break;
			case LitColor::RGB565: case LitColor::RGB5A3:
				updateValuesSize = 2;
				break;
			default: //RGB888, RGBA8888
				updateValuesSize = 4;
			}
		} break;
		case TEXT:
		{
			updateValuesSize = _knownValueInput.GetStdStringNoZeros().size();

			switch (_currentTextTypeSelect)
			{
			case MT::UTF32BE: case MT::UTF32LE:
				updateValuesSize *= 4;
				break;
			default:
				updateValuesSize *= 2; //consider double-byte encodings and encoding with single AND double bytes
			}
		} break;
		default: //PRIMITIVE
		{
			switch (_currentPrimitiveTypeSelect)
			{
			case INT8:
				updateValuesSize = 1;
				break;
			case INT16:
				updateValuesSize = 2;
				break;
			case INT64: case DOUBLE:
				updateValuesSize = 8;
				break;
			default:
				updateValuesSize = 4;
			}
		}
	}

	_updateValues.resize(updateValuesSize * _maxResultsPerPage);
	setLiveUpdateRefreshRate();
}

void MungPlex::Search::setUpResultPaging()
{
	_pagesAmountValue = MemoryCompare::MemCompare::GetResultCount() / _maxResultsPerPage;

	if (MemoryCompare::MemCompare::GetResultCount() % _maxResultsPerPage > 0)
		++_pagesAmountValue;

	_pagesAmountText = std::to_string(_pagesAmountValue);
}

void MungPlex::Search::performValuePoke()
{
	_pokeValue.clear();
	std::stringstream stream;

	if (!_multiPoke)
	{
		std::string test = _pokeAddressInput.GetStdStringNoZeros();
		stream << std::hex << test;
		stream >> _pokeAddress;
	}

	switch (_currentValueTypeSelect)
	{
	case ARRAY: {
		switch (_currentArrayTypeSelect)
		{
		case INT8: {
			if (ProcessInformation::GetAddressWidth() > 4)
				pokeArray<uint8_t, uint64_t>();
			else
				pokeArray<uint8_t, uint32_t>();
		} break;
		case INT16: {
			if (ProcessInformation::GetAddressWidth() > 4)
				pokeArray<uint16_t, uint64_t>();
			else
				pokeArray<uint16_t, uint32_t>();
		} break;
		case INT64: {
			if (ProcessInformation::GetAddressWidth() > 4)
				pokeArray<uint64_t, uint64_t>();
			else
				pokeArray<uint64_t, uint32_t>();
		} break;
		case FLOAT: {
			if (ProcessInformation::GetAddressWidth() > 4)
				pokeArray<float, uint64_t>();
			else
				pokeArray<float, uint32_t>();
		} break;
		case DOUBLE: {
			if (ProcessInformation::GetAddressWidth() > 4)
				pokeArray<double, uint64_t>();
			else
				pokeArray<double, uint32_t>();
		} break;
		default: { //OperativeArray<INT32>
			if (ProcessInformation::GetAddressWidth() > 4)
				pokeArray<uint32_t, uint64_t>();
			else
				pokeArray<uint32_t, uint32_t>();
		} break;
		}break;
	} break;
	case TEXT: {
		if (ProcessInformation::GetAddressWidth() > 4)
			pokeText<uint64_t>();
		else
			pokeText<uint32_t>();
	} break;
	case COLOR: {
		if (ProcessInformation::GetAddressWidth() > 4)
			pokeColor<uint64_t>();
		else
			pokeColor<uint32_t>();
	} break;
	default: { //PRIMITIVE
		int64_t tempVal;
		std::stringstream streamV;

		if (_hex && _currentPrimitiveTypeSelect < FLOAT)
		{
			streamV << std::hex << _pokeValueInput.GetStdStringNoZeros();
			streamV >> tempVal;
		}
		else
			tempVal = std::stoll(_pokeValueInput.GetStdStringNoZeros());

		switch (_currentPrimitiveTypeSelect)
		{
		case INT8:
			_pokeValue.insert(_pokeValue.end(), reinterpret_cast<char*>(&tempVal), reinterpret_cast<char*>(&tempVal) + 1);
			ProcessInformation::GetAddressWidth() > 4 ? pokeValue<uint8_t, uint64_t>() : pokeValue<uint8_t, uint32_t>();
			break;
		case INT16:
			_pokeValue.insert(_pokeValue.end(), reinterpret_cast<char*>(&tempVal), reinterpret_cast<char*>(&tempVal) + 2);
			ProcessInformation::GetAddressWidth() > 4 ? pokeValue<uint16_t, uint64_t>() : pokeValue<uint16_t, uint32_t>();
			break;
		case INT64:
			_pokeValue.insert(_pokeValue.end(), reinterpret_cast<char*>(&tempVal), reinterpret_cast<char*>(&tempVal) + 8);
			ProcessInformation::GetAddressWidth() > 4 ? pokeValue<uint64_t, uint64_t>() : pokeValue<uint64_t, uint32_t>();
			break;
		case FLOAT:
		{
			float temp = std::stof(_pokeValueInput.GetCString());
			_pokeValue.insert(_pokeValue.end(), reinterpret_cast<char*>(&temp), reinterpret_cast<char*>(&temp) + 4);
			ProcessInformation::GetAddressWidth() > 4 ? pokeValue<float, uint64_t>() : pokeValue<float, uint32_t>();
		}   break;
		case DOUBLE:
		{
			double temp = std::stod(_pokeValueInput.GetCString());
			_pokeValue.insert(_pokeValue.end(), reinterpret_cast<char*>(&temp), reinterpret_cast<char*>(&temp) + 8);
			ProcessInformation::GetAddressWidth() > 4 ? pokeValue<double, uint64_t>() : pokeValue<double, uint32_t>();
		}
		break;
		default:
			_pokeValue.insert(_pokeValue.end(), reinterpret_cast<char*>(&tempVal), reinterpret_cast<char*>(&tempVal) + 4);
			ProcessInformation::GetAddressWidth() > 4 ? pokeValue<uint32_t, uint64_t>() : pokeValue<uint32_t, uint32_t>();
			break;
		}
	}
	}
}

bool MungPlex::Search::isSelectionOrIndexOurOfBounds(const uint64_t row, const uint64_t resultCount)
{
	if (row >= _maxResultsPerPage)
		return true;

	if (row >= resultCount)
		return true;

	if (_currentPageValue > _pagesAmountValue)
		return true;

	if (_currentPageValue == _pagesAmountValue)
	{
		const uint64_t lastPageResultCount = resultCount % _maxResultsPerPage;

		if (lastPageResultCount == 0)
			;
		else if (row >= lastPageResultCount)
			return true;

		if (_deselectedIllegalSelection && lastPageResultCount != 0)
			for (int i = lastPageResultCount; i < _maxResultsPerPage; ++i)
				_selectedIndices[i] = false;

		_deselectedIllegalSelection = false;
	}

	return false;
}

void MungPlex::Search::drawPrimitiveTableRow(const int col, const uint64_t row, const uint64_t pageIndexWithRowCount, FloorString& buf, FloorString& tempValue)
{
	if (_currentPrimitiveTypeSelect < FLOAT)
	{
		static int64_t currentValue;
		static int64_t previousValue;
		static int64_t liveValue;

		switch (_currentPrimitiveTypeSelect)
		{
		case INT8:
		{
			if (_signed)
			{
				currentValue = MemoryCompare::MemCompare::GetResults().GetValueAllRanges<int8_t>(pageIndexWithRowCount);
				previousValue = _iterationCount < 2 ? 0 : MemoryCompare::MemCompare::GetResults().GetPreviousValueAllRanges<int8_t>(pageIndexWithRowCount);
				liveValue = *reinterpret_cast<int8_t*>(_updateValues.data() + row);
			}
			else
			{
				currentValue = MemoryCompare::MemCompare::GetResults().GetValueAllRanges<uint8_t>(pageIndexWithRowCount);
				previousValue = _iterationCount < 2 ? 0 : MemoryCompare::MemCompare::GetResults().GetPreviousValueAllRanges<uint8_t>(pageIndexWithRowCount);
				liveValue = *(_updateValues.data() + row);
			}
		}break;
		case INT16:
		{
			if (_signed)
			{
				currentValue = MemoryCompare::MemCompare::GetResults().GetValueAllRanges<int16_t>(pageIndexWithRowCount);
				previousValue = _iterationCount < 2 ? 0 : MemoryCompare::MemCompare::GetResults().GetPreviousValueAllRanges<int16_t>(pageIndexWithRowCount);
				liveValue = *reinterpret_cast<int16_t*>(_updateValues.data() + row * sizeof(int16_t));
			}
			else
			{
				currentValue = MemoryCompare::MemCompare::GetResults().GetValueAllRanges<uint16_t>(pageIndexWithRowCount);
				previousValue = _iterationCount < 2 ? 0 : MemoryCompare::MemCompare::GetResults().GetPreviousValueAllRanges<uint16_t>(pageIndexWithRowCount);
				liveValue = *reinterpret_cast<uint16_t*>(_updateValues.data() + row * sizeof(uint16_t));
			}
		}break;
		case INT64:
		{
			if (_signed)
			{
				currentValue = MemoryCompare::MemCompare::GetResults().GetValueAllRanges<int64_t>(pageIndexWithRowCount);
				previousValue = _iterationCount < 2 ? 0 : MemoryCompare::MemCompare::GetResults().GetPreviousValueAllRanges<int64_t>(pageIndexWithRowCount);
				liveValue = *reinterpret_cast<int64_t*>(_updateValues.data() + row * sizeof(int64_t));
			}
			else
			{
				currentValue = MemoryCompare::MemCompare::GetResults().GetValueAllRanges<uint64_t>(pageIndexWithRowCount);
				previousValue = _iterationCount < 2 ? 0 : MemoryCompare::MemCompare::GetResults().GetPreviousValueAllRanges<uint64_t>(pageIndexWithRowCount);
				liveValue = *reinterpret_cast<uint64_t*>(_updateValues.data() + row * sizeof(uint64_t));
			}
		}break;
		default: //INT32
		{
			if (_signed)
			{
				currentValue = MemoryCompare::MemCompare::GetResults().GetValueAllRanges<int32_t>(pageIndexWithRowCount);
				previousValue = _iterationCount < 2 ? 0 : MemoryCompare::MemCompare::GetResults().GetPreviousValueAllRanges<int32_t>(pageIndexWithRowCount);
				liveValue = *reinterpret_cast<int32_t*>(_updateValues.data() + row * sizeof(int32_t));
			}
			else
			{
				currentValue = MemoryCompare::MemCompare::GetResults().GetValueAllRanges<uint32_t>(pageIndexWithRowCount);
				previousValue = _iterationCount < 2 ? 0 : MemoryCompare::MemCompare::GetResults().GetPreviousValueAllRanges<uint32_t>(pageIndexWithRowCount);
				liveValue = *reinterpret_cast<uint32_t*>(_updateValues.data() + row * sizeof(uint32_t));
			}
		}break;
		}

		switch (col)
		{
		case 1:
			sprintf(buf.Data(), _formatting.c_str(), currentValue);
			if (!_pokePrevious)
				tempValue = buf.Data();
			break;
		case 2:
			sprintf(buf.Data(), _formatting.c_str(), previousValue);
			if (_pokePrevious)
				tempValue = buf;
			break;
		case 3:
			sprintf(buf.Data(), _formatting.c_str(), currentValue - previousValue);
			break;
		case 4:
			sprintf(buf.Data(), _formatting.c_str(), liveValue);
		}
	}
	else
	{
		static double currentValue;
		static double previousValue;
		static double liveValue;

		switch (_currentPrimitiveTypeSelect)
		{
		case DOUBLE:
		{
			currentValue = MemoryCompare::MemCompare::GetResults().GetValueAllRanges<double>(pageIndexWithRowCount);
			previousValue = _iterationCount < 2 ? 0 : MemoryCompare::MemCompare::GetResults().GetPreviousValueAllRanges<double>(pageIndexWithRowCount);
			liveValue = *reinterpret_cast<double*>(_updateValues.data() + row * sizeof(double));
		}break;
		default: //FLOAT
		{
			currentValue = MemoryCompare::MemCompare::GetResults().GetValueAllRanges<float>(pageIndexWithRowCount);
			previousValue = _iterationCount < 2 ? 0 : MemoryCompare::MemCompare::GetResults().GetPreviousValueAllRanges<float>(pageIndexWithRowCount);
			liveValue = *reinterpret_cast<float*>(_updateValues.data() + row * sizeof(float));
		}
		}

		switch (col)
		{
		case 1:
			sprintf(buf.Data(), _formatting.c_str(), currentValue);
			if (!_pokePrevious)
				tempValue = buf;
			break;
		case 2:
			sprintf(buf.Data(), _formatting.c_str(), previousValue);
			if (_pokePrevious)
				tempValue = buf;
			break;
		case 3:
			sprintf(buf.Data(), _formatting.c_str(), currentValue - previousValue);
			break;
		case 4:
			sprintf(buf.Data(), _formatting.c_str(), liveValue);
		}
	}
}

void MungPlex::Search::drawArrayTableRow(const int col, const uint64_t pageIndexWithRowCount, FloorString& buf, FloorString& tempValue)
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
			drawArrayValues<uint8_t>(col, itemCount, pageIndexWithRowCount, buf, tempValue, _formatting.c_str());
			break;
		case INT16:
			drawArrayValues<uint16_t>(col, itemCount, pageIndexWithRowCount, buf, tempValue, _formatting.c_str());
			break;
		case INT32:
			drawArrayValues<uint32_t>(col, itemCount, pageIndexWithRowCount, buf, tempValue, _formatting.c_str());
			break;
		case INT64:
			drawArrayValues<uint64_t>(col, itemCount, pageIndexWithRowCount, buf, tempValue, _formatting.c_str());
			break;
			/*case FLOAT:
				DrawArrayValues<float>(col, itemCount, resultsIndex, buf, tempValue, _formatting.c_str());
			break;
			case DOUBLE:
				DrawArrayValues<double>(col, itemCount, resultsIndex, buf, tempValue, _formatting.c_str());
			break;*/
	}
}

void MungPlex::Search::drawColorTableRow(const int col, const uint64_t row, const uint64_t pageIndexWithRowCount, FloorString& buf, FloorString& tempValue)
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
			rgb565 = _iterationCount < 2 ? 0 : MemoryCompare::MemCompare::GetResults().GetPreviousValueAllRanges<uint16_t>(pageIndexWithRowCount);
			rectColor = LitColor::RGB565ToRGB888(rgb565);

			if (_pokePrevious)
				vecCol = PackedColorToImVec4(reinterpret_cast<uint8_t*>(&rectColor));
		}
		else if (col == 4)
		{
			rgb565 = *reinterpret_cast<uint16_t*>(_updateValues.data() + row * 2);
			rectColor = LitColor::RGB565ToRGB888(rgb565);
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
			rgb5A3 = _iterationCount < 2 ? 0 : MemoryCompare::MemCompare::GetResults().GetPreviousValueAllRanges<uint16_t>(pageIndexWithRowCount);
			rectColor = _forceAlpha ? LitColor::RGB5A3ToRGBA8888(rgb5A3) : LitColor::RGB5A3ToRGB888(rgb5A3) | 0xFF;

			if (_pokePrevious)
				vecCol = PackedColorToImVec4(reinterpret_cast<uint8_t*>(&rectColor));
		}
		else if (col == 4)
		{
			rgb5A3 = *reinterpret_cast<uint16_t*>(_updateValues.data() + row * sizeof(uint16_t));
			rectColor = LitColor::RGB5A3ToRGBA8888(rgb5A3);
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
			if (_iterationCount < 2)
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
		else if (col == 4)
		{
			colorPtr = reinterpret_cast<float*>(_updateValues.data() + row * colorValueCount * sizeof(float));
			rectColor = usesAlpha ? LitColor::RGBAFToRGBA8888(colorPtr) : LitColor::RGBFToRGB888(colorPtr);
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
			rectColor = _iterationCount < 2 ? 0 : MemoryCompare::MemCompare::GetResults().GetValueAllRanges<uint32_t>(pageIndexWithRowCount);

			if (_currentColorTypeSelect == LitColor::RGB888)
				rectColor |= 0xFF;

			if (_pokePrevious)
				vecCol = PackedColorToImVec4(reinterpret_cast<uint8_t*>(&rectColor));
		}
		else if (col == 4)
		{
			rectColor = *reinterpret_cast<uint32_t*>(_updateValues.data() + row * sizeof(uint32_t));

			if (_currentColorTypeSelect == LitColor::RGB888)
				rectColor |= 0xFF;
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

void MungPlex::Search::drawTextTableRow(const int col, const uint64_t row, const uint64_t pageIndexWithRowCount, FloorString& buf, FloorString& tempValue)
{
	static uint64_t strLength = 0;
	static uint64_t pageIndexWithRowCountAndStrLength = 0;

	if (!strLength)
		strLength = MemoryCompare::MemCompare::GetResults().GetValueWidth();

	pageIndexWithRowCountAndStrLength = pageIndexWithRowCount * strLength;

	static std::string temputf8;

	if (col == 1)
	{
		switch (_currentTextTypeSelect)
		{
		case MT::UTF16LE: case MT::UTF16BE:
			temputf8 = MT::Convert<wchar_t*, std::string>(MC::GetResults().GetSpecificValuePtrAllRanges<wchar_t>(pageIndexWithRowCountAndStrLength), _currentTextTypeSelect, MT::UTF8);
			break;
		case MT::UTF32LE: case MT::UTF32BE:
			temputf8 = MT::Convert<char32_t*, std::string>(MC::GetResults().GetSpecificValuePtrAllRanges<char32_t>(pageIndexWithRowCountAndStrLength), _currentTextTypeSelect, MT::UTF8);
			break;
		default:
			temputf8 = MT::Convert<char*, std::string>(MC::GetResults().GetSpecificValuePtrAllRanges<char>(pageIndexWithRowCountAndStrLength), _currentTextTypeSelect, MT::UTF8);
		}
	}
	else if (col == 4)
	{
		switch (_currentTextTypeSelect)
		{
		case MT::UTF16LE: case MT::UTF16BE:
			temputf8 = MT::Convert<wchar_t*, std::string>(reinterpret_cast<wchar_t*>(_updateValues.data() + row * MC::GetResults().GetValueWidth()), _currentTextTypeSelect, MT::UTF8);
			break;
		case MT::UTF32LE: case MT::UTF32BE:
			temputf8 = MT::Convert<char32_t*, std::string>(reinterpret_cast<char32_t*>(_updateValues.data() + row * MC::GetResults().GetValueWidth()), _currentTextTypeSelect, MT::UTF8);
			break;
		default:
			temputf8 = MT::Convert<char*, std::string>(reinterpret_cast<char*>(_updateValues.data() + row * MC::GetResults().GetValueWidth()), _currentTextTypeSelect, MT::UTF8);
		}
	}
	else
		return;

	sprintf(buf.Data(), "%s", temputf8.c_str());
	tempValue = buf;
}

void MungPlex::Search::drawSelectedTableRowColor(const uint64_t row, FloorString& buf)
{
	if (_selectedIndices[row])
	{
		ImGui::Selectable(buf.Data(), true, ImGuiSelectableFlags_SpanAllColumns);
		for (int i = 0; i < 4; ++i)
		{
			ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImVec4ToPackedColor(ImGui::GetStyle().Colors[ImGuiCol_Header]));
			ImGui::TableSetColumnIndex(i);
		}
	}
	else
		ImGui::Selectable(buf.Data(), false, ImGuiSelectableFlags_SpanAllColumns);
}

void MungPlex::Search::selectRows(const uint64_t row, bool& rowClicked, FloorString& tempAddress)
{
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
			return;

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

	Log::LogInformation("comparison Type:", true, 4);

	if (_currentcomparisonTypeSelect == MemoryCompare::KNOWN)
	{
		Log::LogInformation("Known: " + _knownValueInput.GetStdStringNoZeros() + ", " + _secondaryKnownValueInput.GetStdStringNoZeros(), true);
	}
	else
		Log::LogInformation("Unknown", true);
}

void MungPlex::Search::arrayTypeSearchLog()
{
	Log::LogInformation("Array<" + _searchArrayTypes.GetStdString(_currentArrayTypeSelect) + ">: " + _knownValueInput.GetStdStringNoZeros(), true, 4);
}

void MungPlex::Search::textTypeSearchLog()
{
	Log::LogInformation("Text<" + TextTypes.GetStdString(_currentTextTypeIndex) + ">: " + _knownValueInput.GetStdStringNoZeros(), true, 4);
}

void MungPlex::Search::colorTypeSearchLog()
{
	Log::LogInformation("Text<" + _searchColorTypes.GetStdString(_currentColorTypeSelect) + ">: " + _knownValueInput.GetStdStringNoZeros(), true, 4);
}

void MungPlex::Search::drawResultsTable()
{
	static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

	if (!ImGui::BeginTable("Results", 5, flags, ImVec2(ImGui::GetContentRegionAvail().x * 0.666f, ImGui::GetContentRegionAvail().y)))
		return;

	const uint64_t resultCount = MemoryCompare::MemCompare::GetResultCount();
	ImGui::TableSetupColumn("Address");
	ImGui::TableSetupColumn("Value");
	ImGui::TableSetupColumn("Previous");
	ImGui::TableSetupColumn("Difference (a - b)");
	ImGui::TableSetupColumn("Real-Time");
	ImGui::TableHeadersRow();

	for (uint64_t row = 0; row < resultCount && !_busySearching; ++row)
	{
		if (isSelectionOrIndexOurOfBounds(row, resultCount))
			break;

		static FloorString tempAddress("", 17);
		static FloorString tempValue("", 1024);
		static FloorString buf("", 1024);
		static int addressTextWidth = ProcessInformation::GetAddressWidth() > 4 ? 16 : 8;
		static int64_t pageIndex;
		pageIndex = (_currentPageValue - 1) * _maxResultsPerPage;

		if (pageIndex < 0)
			pageIndex = 0;

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
		ImGui::TableNextRow(ImGuiSelectableFlags_SpanAllColumns);

		for (int col = 0; col < 5; ++col)
		{
			ImGui::TableSetColumnIndex(col);

			if (col > 0)
			{
				switch (_currentValueTypeSelect)
				{
					case ARRAY:
						drawArrayTableRow(col, pageIndexWithRowCount, buf, tempValue);
					break;
					case COLOR:
						drawColorTableRow(col, row, pageIndexWithRowCount, buf, tempValue);
					break;
					case TEXT: 
						drawTextTableRow(col, row, pageIndexWithRowCount, buf, tempValue);
					break;
					default:// PRIMITIVE
						drawPrimitiveTableRow(col, row, pageIndexWithRowCount, buf, tempValue);
				}
			}
			else //draw address cell
			{
				sprintf(buf.Data(), addressTextWidth == 16 ? "%016llX" : "%08X", address);
				tempAddress = buf.CStr();
			}

			drawSelectedTableRowColor(row, buf);
			selectRows(row, rowClicked, tempAddress);
		}

		//set poke address and value if row is clicked
		if (rowClicked)
		{
			_pokeAddressInput.SetText(tempAddress.StdStrNoLeadinZeros());
			_pokeValueInput.SetText(tempValue.StdStrNoLeadinZeros());

			if (_currentValueTypeSelect == COLOR)
				LitColorExpressionToImVec4(_pokeValueInput.GetCString(), &_pokeColorVec);
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
		if (_rangeStartInput.GetValue() >= _regions[index].Base + _regions[index].Size - 1)
			return;

		if (_rangeEndInput.GetValue() <= _regions[index].Base)
			return;

		if (_rangeStartInput.GetValue() > _regions[index].Base)
		{
			currentRegionSize -= _rangeStartInput.GetValue() - currentBaseAddress;
			currentBaseAddress = _rangeStartInput.GetValue();
			currentBaseLocation += currentBaseAddress - _regions[index].Base;
		}

		if (_rangeEndInput.GetValue() < _regions[index].Base + _regions[index].Size - 1)
			currentRegionSize = _rangeEndInput.GetValue() - currentBaseAddress;
	}
	else // single region
	{
		if (_rangeStartInput.GetValue() >= _regions[index].Base + _regions[index].Size - 1 || _rangeEndInput.GetValue() <= _regions[index].Base)
		{
			_dumpRegions.emplace_back(_regions[index]);
			return;
		}

		if (_rangeStartInput.GetValue() > _regions[index].Base && _rangeStartInput.GetValue() < _regions[index].Base + _regions[index].Size - 1)
		{
			currentBaseAddress = _rangeStartInput.GetValue();
			currentBaseLocation += currentBaseAddress - _regions[index].Base;
		}

		if (_rangeEndInput.GetValue() > currentBaseAddress && _rangeEndInput.GetValue() < _regions[index].Base + _regions[index].Size - 1)
			currentRegionSize = _rangeEndInput.GetValue() - currentBaseAddress;

		if (_rangeStartInput.GetValue() > _regions[index].Base || _rangeEndInput.GetValue() < _regions[index].Base + _regions[index].Size - 1)
			++currentRegionSize;
	}

	_dumpRegions.emplace_back(_regions[index].Label, currentBaseAddress, currentRegionSize, reinterpret_cast<void*>(currentBaseLocation));
}

void MungPlex::Search::setUpAndIterate()
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

	std::string tempprimary = _knownValueInput.GetStdStringNoZeros();
	std::string tempsecondary = _secondaryKnownValueInput.GetStdStringNoZeros();

	if (_iterationCount < 1)
	{
		uint32_t setupFlags = 0;

		if (_signed)
			setupFlags |= MemoryCompare::SIGNED;

		if (_caseSensitive)
			setupFlags |= MemoryCompare::CASE_SENSITIVE;

		if (_endiannessSelect)
			setupFlags |= MemoryCompare::BIG_ENDIAN;

		if (_cached)
			setupFlags |= MemoryCompare::CACHED;

		if (_disableUndo)
			setupFlags |= MemoryCompare::DISABLE_UNDO;

		MemoryCompare::MemCompare::SetUp(_resultsPath, _currentValueTypeSelect, subsidiaryDatatype, ProcessInformation::GetAddressWidth(), _alignmentValue, setupFlags);
	}

	uint32_t iterationFlags = 0;

	if (_currentcomparisonTypeSelect == MemoryCompare::KNOWN)
		iterationFlags |= MemoryCompare::KNOWN;

	if (_hex)
		iterationFlags |= MemoryCompare::HEX;

	MemoryCompare::MemCompare::NewIteration(_currentConditionTypeSelect, _iterationIndex + 1, tempprimary, tempsecondary, _precision / 100.0f, iterationFlags);

	generateDumpRegionMap();

	for (SystemRegion& dumpRegion : _dumpRegions)
	{
		std::vector<char> buf(dumpRegion.Size);

		if (ProcessInformation::GetProcessType() == ProcessInformation::CONSOLE)
		{
			switch (ProcessInformation::GetConsoleConnectionType())
			{
			case ProcessInformation::CON_USBGecko:
			{
				ProcessInformation::GetUsbGecko()->Read(buf.data(), dumpRegion.Base, dumpRegion.Size);
			} break;
			}
		}
		else
		{
			ProcessInformation::GetProcess().ReadMemoryFast(buf.data(), dumpRegion.BaseLocationProcess, dumpRegion.Size, 0x1000);
		
			if (_rereorderRegion)
				Rereorder4BytesReorderedMemory(buf.data(), dumpRegion.Size);
		}

		MemoryCompare::MemDump dump(buf.data(), dumpRegion.Base, dumpRegion.Size);
		MemoryCompare::MemCompare::ProcessNextRange(&dump);
	}
}

void MungPlex::Search::SetUnderlyingBigEndianFlag(const bool isBigEndian)
{
	GetInstance()._endiannessSelect = isBigEndian;
}

void MungPlex::Search::SetRereorderRegion(const bool rereorder)
{
	GetInstance()._rereorderRegion = rereorder;
}

void MungPlex::Search::SetAlignment(const int alignment)
{
	GetInstance()._alignmentValue = alignment;
}

void MungPlex::Search::SetNativeAppSearchSettings()
{
	GetInstance()._cached = true;
	GetInstance()._disableUndo = true;
	GetInstance()._crossRegion = true;
}

void MungPlex::Search::SetDefaultSearchSettings()
{
	GetInstance()._cached = Settings::GetSearchSettings().DefaultCached;
	GetInstance()._disableUndo = Settings::GetSearchSettings().DefaultDisableUndo;
}

bool MungPlex::Search::IsBusySearching()
{
	return GetInstance()._busySearching;
}

void MungPlex::Search::setRecommendedValueSettings(const int valueType)
{
	setFormatting();

	switch (valueType)
	{
		case ARRAY:
			_currentColorTypeSelect = _currentPrimitiveTypeSelect = _currentTextTypeIndex = 0;
		break;	
		case COLOR:
			_currentArrayTypeSelect = _currentPrimitiveTypeSelect = _currentTextTypeIndex = 0;
			break;
		case TEXT:
			_currentColorTypeSelect = _currentPrimitiveTypeSelect = _currentArrayTypeSelect = 0;
		break;
		default: //PRIMITIVE
			_currentColorTypeSelect = _currentArrayTypeSelect = _currentTextTypeIndex = 0;
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