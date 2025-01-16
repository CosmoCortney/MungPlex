#pragma once
#include <algorithm>
#include "Search.hpp"

inline const std::vector<MungPlex::StringIdCombo::VecType> MungPlex::Search::_searchValueTypes =
{
	{
		{ "Primitive", PRIMITIVE },
		{ "Array", ARRAY },
		{ "Text", TEXT },
		{ "Color", COLOR }
	}
};

inline const std::vector<MungPlex::StringIdCombo::VecType> MungPlex::Search::_endiannesses =
{
	{
		{ "Little", LITTLE },
		{ "Big", BIG }
	}
};

inline const std::vector<MungPlex::StringIdCombo::VecType> MungPlex::Search::_searchPrimitiveTypes =
{
	{
		{ "Int 8 (1 Byte)", INT8 },
		{ "Int 16 (2 Bytes)", INT16 },
		{ "Int 32 (4 Bytes)", INT32 },
		{ "Int 64 (8 Bytes)", INT64 },
		{ "Float Single", FLOAT },
		{ "Float Double", DOUBLE }
	}
};

inline const std::vector<MungPlex::StringIdCombo::VecType> MungPlex::Search::_searchArrayTypes =
{
	{
		{ "Int 8 (1 Byte)", INT8 },
		{ "Int 16 (2 Bytes)", INT16 },
		{ "Int 32 (4 Bytes)", INT32 },
		{ "Int 64 (8 Bytes)", INT64 }
	}
}; //remove once Arrays support floats

inline const std::vector<MungPlex::StringIdCombo::VecType> MungPlex::Search::_searchColorTypes =
{
	{
		{ "RGB 888 (3 Bytes)", LitColor::RGB888 },
		{ "RGBA 8888 (4 Bytes)", LitColor::RGBA8888 },
		{ "RGBF (3 Floats)", LitColor::RGBF },
		{ "RGBAF (4 Floats)", LitColor::RGBAF },
		{ "RGB 565 (2 Bytes)", LitColor::RGB565 },
		{ "RGB 5A3 (2 Bytes)",  LitColor::RGB5A3 }
	}
};

inline const std::vector<MungPlex::StringIdCombo::VecType> MungPlex::Search::_intSearchConditionTypes =
{
    {
		{ "Equal (==)", MemoryCompare::EQUAL },
		{ "Unequal (!=)", MemoryCompare::UNEQUAL },
		{ "Greater (>)", MemoryCompare::GREATER },
		{ "Greater or Equal (>=)", MemoryCompare::GREATER_EQUAL },
		{ "Lower (<)", MemoryCompare::LOWER },
		{ "Lower or Equal (<=)", MemoryCompare::LOWER_EQUAL },
		{ "Increased by", MemoryCompare::INCREASED_BY },
		{ "Decreased by", MemoryCompare::DECREASED_BY },
		{ "Value Between", MemoryCompare::BETWEEN },
		{ "Value Not Between", MemoryCompare::NOT_BETWEEN },
		{ "AND (has all true bits)", MemoryCompare::AND },
		{ "OR(has at least 1 true bit)", MemoryCompare::OR }
	}
};

inline const std::vector<MungPlex::StringIdCombo::VecType> MungPlex::Search::_arraySearchConditionTypes =
{
	{
		{ "Equal (==)", MemoryCompare::EQUAL },
		{ "Unequal (!=)", MemoryCompare::UNEQUAL }
	}
};

inline const std::vector<MungPlex::StringIdCombo::VecType> MungPlex::Search::_floatSearchConditionTypes =
{
	{
		{ "Equal (==)", MemoryCompare::EQUAL },
		{ "Unequal (!=)",MemoryCompare::UNEQUAL },
		{ "Greater (>)", MemoryCompare::GREATER },
		{ "Greater or Equal (>=)", MemoryCompare::GREATER_EQUAL },
		{ "Lower (<)", MemoryCompare::LOWER },
		{ "Lower or Equal (<=)", MemoryCompare::LOWER_EQUAL },
		{ "Increased by", MemoryCompare::INCREASED_BY },
		{ "Decreased by", MemoryCompare::DECREASED_BY },
		{ "Value Between", MemoryCompare::BETWEEN }
	}
};

inline const std::vector<MungPlex::StringIdCombo::VecType> MungPlex::Search::_colorSearchConditionTypes =
{
	{
		{ "Equal (==)", MemoryCompare::EQUAL },
		{ "Unequal (!=)",MemoryCompare::UNEQUAL },
		{ "Greater (>)", MemoryCompare::GREATER },
		{ "Greater or Equal (>=)", MemoryCompare::GREATER_EQUAL },
		{ "Lower (<)", MemoryCompare::LOWER },
		{ "Lower or Equal (<=)", MemoryCompare::LOWER_EQUAL }
	}
};

inline const std::vector<MungPlex::StringIdCombo::VecType> MungPlex::Search::_textSearchConditionTypes =
{
	{
		{ "Equal (==)", MemoryCompare::EQUAL },
		{ "Unequal (!=)", MemoryCompare::UNEQUAL }
	}
};

 inline const std::vector<MungPlex::StringIdCombo::VecType> MungPlex::Search::_searchComparasionTypes =
{
	{
		{ "Unknwon/Initial", 0 },
		{ "Known Value", MemoryCompare::KNOWN }
	}
};

MungPlex::Search::Search()
{
	_regionSelectCombo.ConnectOnIndexChangedSlot(Slot_IndexChanged);
	_selectedIndices.resize(_maxResultsPerPageInput.GetValue());
	_alignmentValueInput.SetHelpText("This value specifies the increment of the next address to be scanned. 1 means that the following value to be scanned is the at the current address + 1. Here are some recommendations for each value type: Int8/Text/Color/Array<Int8> - 1, Int16/Color/Array<Int16> - 2, Int32/Int64/Float/Double/Color/Array<Int32>/Array<Int64> - 4. Systems that use less than 4MBs of RAM (PS1, SNES, MegaDrive, ...) should always consider an alignment of 1, despite the value recommendations.", true);
	_alignmentValueInput.SetValue(Settings::GetSearchSettings().DefaultAlignment);
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
				if (_searchValueTypesCombo.Draw(0.5f, 0.4f))
				{
					_updateLabels = true;
					_knownValueInput.SetText("");

					switch (_searchValueTypesCombo.GetSelectedId())
					{
					case ARRAY:
						_knownValueInput.SetLabel("Array Expression:");
						_subsidiaryTypeSearchConditionsCombo.SetItems(_arraySearchConditionTypes);
						break;
					case COLOR:
						_knownValueInput.SetLabel("Color Expression:");
						_subsidiaryTypeSearchConditionsCombo.SetItems(_colorSearchConditionTypes);
						break;
					case TEXT:
						_knownValueTextInput.SetText("");
						_subsidiaryTypeSearchConditionsCombo.SetItems(_textSearchConditionTypes);
						break;
					default:
						_knownValueInput.SetLabel("Value:");
						_subsidiaryTypeSearchConditionsCombo.SetItems(_intSearchConditionTypes);
					}

					_secondaryKnownValueInput.SetText("");
					setFormatting();
				}

				if (_searchValueTypesCombo.GetSelectedId() == TEXT || _searchValueTypesCombo.GetSelectedId() == COLOR)
					_searchComparasionTypeCombo.SetSelectedById(MemoryCompare::KNOWN);
			} 
			if (_searchActive) ImGui::EndDisabled();

			if (_searchValueTypesCombo.GetSelectedId() == PRIMITIVE || _searchValueTypesCombo.GetSelectedId() >= FLOAT)
			{
				ImGui::SameLine();
				if (ImGui::Checkbox("Hex", &GetInstance()._hex))
					setFormatting();
			}

			if (_searchActive) ImGui::BeginDisabled();
			{
				switch (_searchValueTypesCombo.GetSelectedId())
				{
				case ARRAY:
					if (_arrayTypesCombo.Draw(0.5f, 0.4f)) //use primitive types here once Arrays support floats
						setRecommendedValueSettings(ARRAY);
					break;
				case COLOR:
				{
					if (_colorTypesCombo.Draw(0.5f, 0.4f))
						setRecommendedValueSettings(COLOR);

					if (_colorTypesCombo.GetSelectedId() != LitColor::RGB5A3)
						_forceAlpha = false;

					ImGui::SameLine();

					if (_colorTypesCombo.GetSelectedId() != LitColor::RGB5A3) ImGui::BeginDisabled();
					ImGui::Checkbox("Force Alpha", &_forceAlpha);
					if (_colorTypesCombo.GetSelectedId() != LitColor::RGB5A3) ImGui::EndDisabled();
				}break;
				case TEXT:
					if (_textTypesCombo.Draw(0.5f, 0.4f))
						setRecommendedValueSettings(TEXT);
					break;
				default: //PRIMITIVE
					if (_primitiveTypesCombo.Draw(0.5f, 0.4f))
						setRecommendedValueSettings(PRIMITIVE);
				}

				ImGui::SameLine();

				if (_primitiveTypesCombo.GetSelectedId() < FLOAT && _searchValueTypesCombo.GetSelectedId() == PRIMITIVE)
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
	if (_searchValueTypesCombo.GetSelectedId() == PRIMITIVE)
		_formatting = GetStringFormatting(_primitiveTypesCombo.GetSelectedId(), _signed, _hex);
	else if (_searchValueTypesCombo.GetSelectedId() == ARRAY)
		_formatting = GetStringFormatting(_arrayTypesCombo.GetSelectedId(), _signed, _hex);
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

			_regionSelectCombo.Draw(0.5f, 0.4f);

			ImGui::SameLine();

			_endiannessCombo.Draw(1.0f, 0.4f);

			_rangeStartInput.Draw(0.5f, 0.4f, true);

			ImGui::SameLine();

			_rangeEndInput.Draw(1.0f, 0.4f, true);

			if (ImGui::Checkbox("Cross-Region", &_crossRegion))
			{
				if (_regions.empty())
					_regions = ProcessInformation::GetSystemRegionList();

				if (_crossRegion)
				{
					_rangeStartInput.SetValue(_regions.front().Base);
					_rangeEndInput.SetValue(_regions.back().Base + _regions.back().Size - 1);
				}
				else
				{
					_rangeStartInput.SetValue(_regions[_regionSelectCombo.GetSelectedIndex()].Base);
					_rangeEndInput.SetValue(_regions[_regionSelectCombo.GetSelectedIndex()].Base + _regions[_regionSelectCombo.GetSelectedIndex()].Size - 1);
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
		switch (_subsidiaryTypeSearchConditionsCombo.GetSelectedId())
		{
			case MemoryCompare::BETWEEN:
			{
				_knownValueInput.SetLabel("Lowest:");
				_secondaryKnownValueInput.SetLabel("Highest:");
				disableSecondaryValueText = false;
			} break;
			case MemoryCompare::NOT_BETWEEN:
			{
				_knownValueInput.SetLabel("Below:");
				_secondaryKnownValueInput.SetLabel("Above:");
				disableSecondaryValueText = false;
			} break;
			case MemoryCompare::INCREASED_BY:
			{
				_knownValueInput.SetLabel("Increased by:");
				_secondaryKnownValueInput.SetLabel("Not applicable");
				disableSecondaryValueText = true;
			} break;
			case MemoryCompare::DECREASED_BY:
			{
				_knownValueInput.SetLabel("Decreased by:");
				_secondaryKnownValueInput.SetLabel("Not applicable");
				disableSecondaryValueText = true;
			} break;
			default:
			{
				_knownValueInput.SetLabel("Value:");
				_secondaryKnownValueInput.SetLabel("Not applicable");
				disableSecondaryValueText = true;
			}
		}
		_updateLabels = false;
	}

	if (_searchComparasionTypeCombo.Draw(1.0f, 0.4f))
		_updateLabels = true;

	if (_subsidiaryTypeSearchConditionsCombo.Draw(1.0f, 0.4f))
		_updateLabels = true;

	if (!_diableBecauseUnknownAndNotRangebased)
		_knownValueInput.Draw(1.0f, 0.4f);

	if (_subsidiaryTypeSearchConditionsCombo.GetSelectedId() >= MemoryCompare::BETWEEN && _subsidiaryTypeSearchConditionsCombo.GetSelectedId() <= MemoryCompare::NOT_BETWEEN)
		_secondaryKnownValueInput.Draw(1.0f, 0.4f);

	if (_primitiveTypesCombo.GetSelectedId() >= FLOAT)
		SetUpSliderFloat("Precision (%%):", &_precision, 75.0f, 100.0f, "%0.2f", 1.0f, 0.4f);
}

void MungPlex::Search::drawArraySearchOptions()
{
	static bool disablePrimaryValueText = false;

	ImGui::BeginGroup();
	
	if (_subsidiaryTypeSearchConditionsCombo.Draw(1.0f, 0.4f))
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
		_searchComparasionTypeCombo.Draw(1.0f, 0.4f);

		if (_subsidiaryTypeSearchConditionsCombo.Draw(1.0f, 0.4f))
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
	static bool colorPicked = false;

	ImGui::SameLine();

	ImGui::BeginChild("child_colorSelect");
	ImGui::BeginGroup();
	colorPicked = DrawExtraColorPickerOptions(&_useColorWheel, &_searchColorVec);
	ImGui::EndGroup();

	ImGui::BeginGroup();
	{
		if (DrawColorPicker(_colorTypesCombo.GetSelectedId(), _forceAlpha, &_searchColorVec, _useColorWheel, 0.75f) || colorPicked)
			ColorValuesToCString(_searchColorVec, _colorTypesCombo.GetSelectedId(), _knownValueInput.GetData(), _forceAlpha);
	}
	ImGui::EndGroup();
	ImGui::EndChild();
}

void MungPlex::Search::drawTextSearchOptions()
{
	static bool disablePrimaryValueText = false;

	_diableBecauseUnknownAndNotRangebased = _searchComparasionTypeCombo.GetSelectedId() == 0 && _subsidiaryTypeSearchConditionsCombo.GetSelectedId() != MemoryCompare::INCREASED_BY && _subsidiaryTypeSearchConditionsCombo.GetSelectedId() != MemoryCompare::DECREASED_BY;

	ImGui::BeginGroup();
	{
		if (_diableBecauseUnknownAndNotRangebased) ImGui::BeginDisabled();
			_knownValueTextInput.Draw(1.0f, 0.4f);
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

		_diableBecauseUnknownAndNotRangebased = _searchComparasionTypeCombo.GetSelectedId() == 0 && _subsidiaryTypeSearchConditionsCombo.GetSelectedId() != MemoryCompare::INCREASED_BY && _subsidiaryTypeSearchConditionsCombo.GetSelectedId() != MemoryCompare::DECREASED_BY;
		
		if(_searchValueTypesCombo.GetSelectedId() == COLOR)
			childXY = { ImGui::GetContentRegionAvail().x * 0.5f, ImGui::GetContentRegionAvail().y };
		else
			childXY = { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y };

		ImGui::BeginChild("child_searchLeft", childXY);

		if (_iterationCount) ImGui::BeginDisabled();

		if (_alignmentValueInput.Draw(1.0f, 0.4f))
		{
			if (_alignmentValueInput.GetValue() < 1)
				_alignmentValueInput.SetValue(1);
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
			_iterationsCombo.Draw(1.0f, 0.4f);
		if (!_iterationCount || _disableUndo) ImGui::EndDisabled();

		switch (_searchValueTypesCombo.GetSelectedId())
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
			_iterationsCombo.Clear();
			_searchActive = false;
			_currentPageInput.SetValue(0);
		}
		if (disableResetButton) ImGui::EndDisabled();

		ImGui::EndChild();

		if (_searchValueTypesCombo.GetSelectedId() == COLOR)
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

					if (_searchValueTypesCombo.GetSelectedId() == TEXT)
					{
						_pokeValueTextInput.Draw(1.0f, 0.2f);
					}
					else
					{
						if (_pokeValueInput.Draw(1.0f, 0.2f))
						{
							if (_searchValueTypesCombo.GetSelectedId() == COLOR)
								LitColorExpressionToImVec4(_pokeValueInput.GetCString(), &_pokeColorVec);
						}
					}
				}
				ImGui::EndGroup();

				ImGui::BeginGroup();
				{
					_currentPageInput.Draw(0.5f, 0.4f); //don't make this an if-statement. Things will break! The below code is just in braces for clarification. Believe me, I fooled myself and went through a painful debgging adventure
					{
						if (_currentPageInput.GetValue() < 1)
							_currentPageInput.SetValue(_pagesAmountValue);
						else if (_currentPageInput.GetValue() > _pagesAmountValue && _pagesAmountValue > 0)
							_currentPageInput.SetValue(1);

						if (_currentPageInput.GetValue() == _pagesAmountValue)
							_deselectedIllegalSelection = true;
					}

					ImGui::SameLine();
					SetUpLableText("Of", _pagesAmountText.c_str(), _pagesAmountText.size(), 1.0f, 0.125f);
				}
				ImGui::EndGroup();

				if (_searchValueTypesCombo.GetSelectedId() != TEXT)
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

				if (_searchValueTypesCombo.GetSelectedId() == COLOR)
				{
					DrawExtraColorPickerOptions(&_useColorWheel, &_pokeColorVec);
					if(DrawColorPicker(_colorTypesCombo.GetSelectedId(), _forceAlpha, &_pokeColorVec, _useColorWheel, 0.8f))
						ColorValuesToCString(_pokeColorVec, _colorTypesCombo.GetSelectedId(), _pokeValueInput.GetData(), _forceAlpha);
				}
			}
			if (MemoryCompare::MemCompare::GetResultCount() == 0) ImGui::EndDisabled();

			ImGui::Dummy(ImVec2(0.0f, ImGui::GetContentRegionAvail().y - 40.f));

			if (_iterationCount > 0) ImGui::BeginDisabled();
			{
				_maxResultsPerPageInput.Draw(1.0f, 0.5f);
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

	switch (_searchValueTypesCombo.GetSelectedId())
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

	if(_searchValueTypesCombo.GetSelectedId() != PRIMITIVE)
		_searchComparasionTypeCombo.SetSelectedById(MemoryCompare::KNOWN);

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

		if (_currentPageInput.GetValue() < _pagesAmountValue)
			rows = _maxResultsPerPageInput.GetValue();
		else
			rows = MemoryCompare::MemCompare::GetResultCount() % _maxResultsPerPageInput.GetValue();

		for (int row = 0; row < rows; ++row)
		{
			static uint64_t address = 0;
			static uint64_t currentPageTemp = 0;
			currentPageTemp = _currentPageInput.GetValue();
			uint64_t addressIndex = (currentPageTemp == 0 ? 0 : currentPageTemp - 1) * _maxResultsPerPageInput.GetValue() + row;
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

			switch (_searchValueTypesCombo.GetSelectedId())
			{
			case ARRAY:
			{
				for (int i = 0; i < _arrayItemCount; ++i)
					switch (_arrayTypesCombo.GetSelectedId())
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

				switch (_colorTypesCombo.GetSelectedId())
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
				switch (_primitiveTypesCombo.GetSelectedId())
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

	if (_iterationCount < _iterationsCombo.GetCount())
		_iterationsCombo.PopBack(1 + _iterationsCombo.GetCount() - _iterationCount);

	_iterationsCombo.PushBack(std::pair<std::string, uint32_t>(std::to_string(_iterationCount) + ": " + _searchComparasionTypeCombo.GetSelectedStdString()
		+ (_iterationCount < 2 && _searchComparasionTypeCombo.GetSelectedId() == 0 ? "" : ", " + _subsidiaryTypeSearchConditionsCombo.GetSelectedStdString()), _subsidiaryTypeSearchConditionsCombo.GetSelectedId()));
	_iterationsCombo.SetSelectedByIndex(_iterationCount - 1);
	_selectedIndices.resize(_maxResultsPerPageInput.GetValue());
}

void MungPlex::Search::setLiveUpdateRefreshRate()
{
	if (_maxResultsPerPageInput.GetValue() <= 256)
		_liveUpdateMilliseconds = 16;
	else
		_liveUpdateMilliseconds = _maxResultsPerPageInput.GetValue() >> 4;
}

void MungPlex::Search::prepareLiveUpdateValueList()
{
	_updateValues.clear();
	int updateValuesSize = 0;

	switch (_searchValueTypesCombo.GetSelectedId())
	{
		case ARRAY:
		{
			_arrayItemCount = std::ranges::count(_knownValueInput.GetStdStringNoZeros(), ',') + 1;

			switch (_arrayTypesCombo.GetSelectedId())
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
			switch (_colorTypesCombo.GetSelectedId())
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
			updateValuesSize = _knownValueTextInput.GetStdStringNoZeros().size();

			switch (_textTypesCombo.GetSelectedId())
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
			switch (_primitiveTypesCombo.GetSelectedId())
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

	_updateValues.resize(updateValuesSize * _maxResultsPerPageInput.GetValue());
	setLiveUpdateRefreshRate();
}

void MungPlex::Search::setUpResultPaging()
{
	_pagesAmountValue = MemoryCompare::MemCompare::GetResultCount() / _maxResultsPerPageInput.GetValue();

	if (MemoryCompare::MemCompare::GetResultCount() % _maxResultsPerPageInput.GetValue() > 0)
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

	switch (_searchValueTypesCombo.GetSelectedId())
	{
	case ARRAY: {
		switch (_arrayTypesCombo.GetSelectedId())
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

		if (_hex && _primitiveTypesCombo.GetSelectedId() < FLOAT)
		{
			streamV << std::hex << _pokeValueInput.GetStdStringNoZeros();
			streamV >> tempVal;
		}
		else
			tempVal = std::stoll(_pokeValueInput.GetStdStringNoZeros());

		switch (_primitiveTypesCombo.GetSelectedId())
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
	if (row >= _maxResultsPerPageInput.GetValue())
		return true;

	if (row >= resultCount)
		return true;

	if (_currentPageInput.GetValue() > _pagesAmountValue)
		return true;

	if (_currentPageInput.GetValue() == _pagesAmountValue)
	{
		const uint64_t lastPageResultCount = resultCount % _maxResultsPerPageInput.GetValue();

		if (lastPageResultCount == 0)
			;
		else if (row >= lastPageResultCount)
			return true;

		if (_deselectedIllegalSelection && lastPageResultCount != 0)
			for (int i = lastPageResultCount; i < _maxResultsPerPageInput.GetValue(); ++i)
				_selectedIndices[i] = false;

		_deselectedIllegalSelection = false;
	}

	return false;
}

void MungPlex::Search::drawPrimitiveTableRow(const int col, const uint64_t row, const uint64_t pageIndexWithRowCount, FloorString& buf, FloorString& tempValue)
{
	if (_primitiveTypesCombo.GetSelectedId() < FLOAT)
	{
		static int64_t currentValue;
		static int64_t previousValue;
		static int64_t liveValue;

		switch (_primitiveTypesCombo.GetSelectedId())
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

		switch (_primitiveTypesCombo.GetSelectedId())
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

	switch (_arrayTypesCombo.GetSelectedId())
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

	switch (_arrayTypesCombo.GetSelectedId())
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

	switch (_colorTypesCombo.GetSelectedId())
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
		static bool usesAlpha = _colorTypesCombo.GetSelectedId() == LitColor::RGBAF;
		static int colorValueCount = _colorTypesCombo.GetSelectedId() == LitColor::RGBAF ? 4 : 3;
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
			if (_colorTypesCombo.GetSelectedId() == LitColor::RGB888)
				rectColor |= 0xFF;

			if (!_pokePrevious)
				vecCol = PackedColorToImVec4(reinterpret_cast<uint8_t*>(&rectColor));
		}
		else if (col == 2)
		{
			rectColor = _iterationCount < 2 ? 0 : MemoryCompare::MemCompare::GetResults().GetValueAllRanges<uint32_t>(pageIndexWithRowCount);

			if (_colorTypesCombo.GetSelectedId() == LitColor::RGB888)
				rectColor |= 0xFF;

			if (_pokePrevious)
				vecCol = PackedColorToImVec4(reinterpret_cast<uint8_t*>(&rectColor));
		}
		else if (col == 4)
		{
			rectColor = *reinterpret_cast<uint32_t*>(_updateValues.data() + row * sizeof(uint32_t));

			if (_colorTypesCombo.GetSelectedId() == LitColor::RGB888)
				rectColor |= 0xFF;
		}
		else
			break;
	}
	}

	ColorValuesToCString(vecCol, _colorTypesCombo.GetSelectedId(), buf.Data(), _forceAlpha);
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
		switch (_textTypesCombo.GetSelectedId())
		{
		case MT::UTF16LE: case MT::UTF16BE:
			temputf8 = MT::Convert<wchar_t*, std::string>(MC::GetResults().GetSpecificValuePtrAllRanges<wchar_t>(pageIndexWithRowCountAndStrLength), _textTypesCombo.GetSelectedId(), MT::UTF8);
			break;
		case MT::UTF32LE: case MT::UTF32BE:
			temputf8 = MT::Convert<char32_t*, std::string>(MC::GetResults().GetSpecificValuePtrAllRanges<char32_t>(pageIndexWithRowCountAndStrLength), _textTypesCombo.GetSelectedId(), MT::UTF8);
			break;
		default:
			temputf8 = MT::Convert<char*, std::string>(MC::GetResults().GetSpecificValuePtrAllRanges<char>(pageIndexWithRowCountAndStrLength), _textTypesCombo.GetSelectedId(), MT::UTF8);
		}
	}
	else if (col == 4)
	{
		switch (_textTypesCombo.GetSelectedId())
		{
		case MT::UTF16LE: case MT::UTF16BE:
			temputf8 = MT::Convert<wchar_t*, std::string>(reinterpret_cast<wchar_t*>(_updateValues.data() + row * MC::GetResults().GetValueWidth()), _textTypesCombo.GetSelectedId(), MT::UTF8);
			break;
		case MT::UTF32LE: case MT::UTF32BE:
			temputf8 = MT::Convert<char32_t*, std::string>(reinterpret_cast<char32_t*>(_updateValues.data() + row * MC::GetResults().GetValueWidth()), _textTypesCombo.GetSelectedId(), MT::UTF8);
			break;
		default:
			temputf8 = MT::Convert<char*, std::string>(reinterpret_cast<char*>(_updateValues.data() + row * MC::GetResults().GetValueWidth()), _textTypesCombo.GetSelectedId(), MT::UTF8);
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

	if (_primitiveTypesCombo.GetSelectedId() < FLOAT)
	{
		if (_signed)
			Log::LogInformation("signed", true);
		else
			Log::LogInformation("unsigned", true);
	}

	switch (_primitiveTypesCombo.GetSelectedId())
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

	if (_searchComparasionTypeCombo.GetSelectedId() == MemoryCompare::KNOWN)
	{
		Log::LogInformation("Known: " + _knownValueInput.GetStdStringNoZeros() + ", " + _secondaryKnownValueInput.GetStdStringNoZeros(), true);
	}
	else
		Log::LogInformation("Unknown", true);
}

void MungPlex::Search::arrayTypeSearchLog()
{
	Log::LogInformation("Array<" + _arrayTypesCombo.GetSelectedStdString() + ">: " + _knownValueInput.GetStdStringNoZeros(), true, 4);
}

void MungPlex::Search::textTypeSearchLog()
{
	Log::LogInformation("Text<" + _textTypesCombo.GetSelectedStdString() + ">: " + _knownValueInput.GetStdStringNoZeros(), true, 4);
}

void MungPlex::Search::colorTypeSearchLog()
{
	Log::LogInformation("Text<" + _colorTypesCombo.GetSelectedStdString() + ">: " + _knownValueInput.GetStdStringNoZeros(), true, 4);
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
		static uint64_t currentPageTemp = 0;
		currentPageTemp = _currentPageInput.GetValue();
		pageIndex = (currentPageTemp == 0 ? 0 : currentPageTemp - 1) * _maxResultsPerPageInput.GetValue();

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
				switch (_searchValueTypesCombo.GetSelectedId())
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

			if (_searchValueTypesCombo.GetSelectedId() == TEXT)
				_pokeValueTextInput.SetText(tempValue.StdStrNoLeadinZeros());
			else
				_pokeValueInput.SetText(tempValue.StdStrNoLeadinZeros());

			if (_searchValueTypesCombo.GetSelectedId() == COLOR)
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
		emplaceDumpRegion(_regionSelectCombo.GetSelectedIndex());
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

	switch (_searchValueTypesCombo.GetSelectedId())
	{
	case ARRAY:
		subsidiaryDatatype = _arrayTypesCombo.GetSelectedId();
		break;
	case COLOR:
		subsidiaryDatatype = _colorTypesCombo.GetSelectedId();
		break;
	case TEXT:
		subsidiaryDatatype = _textTypesCombo.GetSelectedId();
		break;
	default: //PRIMITIVE
		subsidiaryDatatype = _primitiveTypesCombo.GetSelectedId();
	}

	std::string tempprimary;
	
	if(_searchValueTypesCombo.GetSelectedId() == TEXT)
		tempprimary = _knownValueTextInput.GetStdStringNoZeros();
	else
		tempprimary = _knownValueInput.GetStdStringNoZeros();

	std::string tempsecondary = _secondaryKnownValueInput.GetStdStringNoZeros();

	if (_iterationCount < 1)
	{
		uint32_t setupFlags = 0;

		if (_signed)
			setupFlags |= MemoryCompare::SIGNED;

		if (_caseSensitive)
			setupFlags |= MemoryCompare::CASE_SENSITIVE;

		if (_endiannessCombo.GetSelectedId())
			setupFlags |= MemoryCompare::BIG_ENDIAN;

		if (_cached)
			setupFlags |= MemoryCompare::CACHED;

		if (_disableUndo)
			setupFlags |= MemoryCompare::DISABLE_UNDO;

		MemoryCompare::MemCompare::SetUp(_resultsPath, _searchValueTypesCombo.GetSelectedId(), subsidiaryDatatype, ProcessInformation::GetAddressWidth(), _alignmentValueInput.GetValue(), setupFlags);
	}

	uint32_t iterationFlags = 0;

	if (_searchComparasionTypeCombo.GetSelectedId() == MemoryCompare::KNOWN)
		iterationFlags |= MemoryCompare::KNOWN;

	if (_hex)
		iterationFlags |= MemoryCompare::HEX;

	MemoryCompare::MemCompare::NewIteration(_subsidiaryTypeSearchConditionsCombo.GetSelectedId(), _iterationsCombo.GetSelectedIndex() + 1, tempprimary, tempsecondary, _precision / 100.0f, iterationFlags);

	generateDumpRegionMap();

	for (auto& dumpRegion : _dumpRegions)
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
				ProcessInformation::Rereorder4BytesReorderedMemory(buf.data(), dumpRegion.Size);
		}

		MemoryCompare::MemDump dump(buf.data(), dumpRegion.Base, dumpRegion.Size);
		MemoryCompare::MemCompare::ProcessNextRange(&dump);
	}
}

void MungPlex::Search::SetUnderlyingBigEndianFlag(const bool isBigEndian)
{
	GetInstance()._endiannessCombo.SetSelectedById(isBigEndian ? BIG : LITTLE);
}

void MungPlex::Search::SetRereorderRegion(const bool rereorder)
{
	GetInstance()._rereorderRegion = rereorder;
}

void MungPlex::Search::SetAlignment(const int alignment)
{
	GetInstance()._alignmentValueInput.SetValue(alignment);
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

void MungPlex::Search::SetMemoryRegions(const std::vector<SystemRegion>& regions)
{
	GetInstance()._regionSelectCombo.SetItems(regions);
}

void MungPlex::Search::setRecommendedValueSettings(const int valueType)
{
	setFormatting();

	switch (valueType)
	{
		case ARRAY:
			_colorTypesCombo.SetSelectedByIndex(0);
			_textTypesCombo.SetSelectedById(0);
			_primitiveTypesCombo.SetSelectedByIndex(0);
		break;	
		case COLOR:
			_textTypesCombo.SetSelectedById(0);
			_primitiveTypesCombo.SetSelectedByIndex(0);
			_arrayTypesCombo.SetSelectedByIndex(0);
			break;
		case TEXT:
			_colorTypesCombo.SetSelectedByIndex(0);
			_primitiveTypesCombo.SetSelectedByIndex(0);
			_arrayTypesCombo.SetSelectedByIndex(0);
		break;
		default: //PRIMITIVE
			_colorTypesCombo.SetSelectedByIndex(0);
			_textTypesCombo.SetSelectedById(0);
			_arrayTypesCombo.SetSelectedByIndex(0);

			if (_primitiveTypesCombo.GetSelectedId() < FLOAT)
				_subsidiaryTypeSearchConditionsCombo.SetItems(_intSearchConditionTypes);
			else
				_subsidiaryTypeSearchConditionsCombo.SetItems(_floatSearchConditionTypes);
	}

	switch (_searchValueTypesCombo.GetSelectedId())
	{
	case COLOR:
		switch (_colorTypesCombo.GetSelectedId())
		{
			case LitColor::RGB565: case LitColor::RGB5A3:
				_alignmentValueInput.SetValue(2);
			break;
			case LitColor::RGB888:
				_alignmentValueInput.SetValue(1);
			break;
			default:
				_alignmentValueInput.SetValue(4);
		}
	break;
	case TEXT:
		_alignmentValueInput.SetValue(1);
	break;
	default:// PRIMITIVE, ARRAY
		switch (_arrayTypesCombo.GetSelectedId() | _primitiveTypesCombo.GetSelectedId())
		{
			case INT8:
				_alignmentValueInput.SetValue(1);
			break;
			case INT16:
				_alignmentValueInput.SetValue(2);
			break;
			default: //INT32, INT64
			_alignmentValueInput.SetValue(4);
		}
	}
}