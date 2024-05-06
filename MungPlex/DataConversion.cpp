#include"DataConversion.h"
#include"MorphText.h"
#include"HelperFunctions.h"
#include"LitColor.h"
#include"Settings.h"
#include"Xertz.h"
#include"imgui_memory_editor.h"

MungPlex::DataConversion::DataConversion()
{
	_verticalSpacing = ImVec2(0.0f, 25.0f * Settings::GetGeneralSettings().Scale);

	_specializedColorTypes.emplace_back("RGBF (3 Floats)", LitColor::RGBF);
	_specializedColorTypes.emplace_back("RGBAF (4 Floats)", LitColor::RGBAF);
	_specializedColorTypes.emplace_back("RGB 565 (2 Bytes)", LitColor::RGB565);
	_specializedColorTypes.emplace_back("RGB 5A3 (2 Bytes)", LitColor::RGB5A3);

	_floatTypes.emplace_back("Float Single", FloatTypes::FLOAT);
	_floatTypes.emplace_back("Float Double", FloatTypes::DOUBLE);

	_intTypes.emplace_back("Int 16", IntTypes::INT16);
	_intTypes.emplace_back("Int 32", IntTypes::INT32);
	_intTypes.emplace_back("Int 64", IntTypes::INT64);

	_textTypes.emplace_back("UTF-8", MorphText::UTF8);
	_textTypes.emplace_back("UTF-16 Little Endian", MorphText::UTF16LE);
	_textTypes.emplace_back("UTF-16 Big Endian", MorphText::UTF16BE);
	_textTypes.emplace_back("UTF-32 Little Endian", MorphText::UTF32LE);
	_textTypes.emplace_back("UTF-32 Big Endian", MorphText::UTF32BE);
	_textTypes.emplace_back("ASCII", MorphText::ASCII);
	_textTypes.emplace_back("ISO-8859-1 (Latin 1)", MorphText::ISO_8859_1);
	_textTypes.emplace_back("ISO-8859-2 (Latin 2)", MorphText::ISO_8859_2);
	_textTypes.emplace_back("ISO-8859-3 (Latin 3)", MorphText::ISO_8859_3);
	_textTypes.emplace_back("ISO-8859-4 (Latin 4)", MorphText::ISO_8859_4);
	_textTypes.emplace_back("ISO-8859-5 (Cyrillic)", MorphText::ISO_8859_5);
	_textTypes.emplace_back("ISO-8859-6 (Arabic)", MorphText::ISO_8859_6);
	_textTypes.emplace_back("ISO-8859-7 (Greek)", MorphText::ISO_8859_7);
	_textTypes.emplace_back("ISO-8859-8 (Hebrew)", MorphText::ISO_8859_8);
	_textTypes.emplace_back("ISO-8859-9 (Turkish)", MorphText::ISO_8859_9);
	_textTypes.emplace_back("ISO-8859-10 (Nordic)", MorphText::ISO_8859_10);
	_textTypes.emplace_back("ISO-8859-11 (Thai)", MorphText::ISO_8859_11);
	_textTypes.emplace_back("ISO-8859-13 (Baltic)", MorphText::ISO_8859_13);
	_textTypes.emplace_back("ISO-8859-14 (Celtic", MorphText::ISO_8859_14);
	_textTypes.emplace_back("ISO-8859-15 (West European)", MorphText::ISO_8859_15);
	_textTypes.emplace_back("ISO-8859-16 (South-East European)", MorphText::ISO_8859_16);
	_textTypes.emplace_back("Shift-Jis", MorphText::SHIFTJIS);
	_textTypes.emplace_back("Jis X 0201 Full Width", MorphText::JIS_X_0201_FULLWIDTH);
	_textTypes.emplace_back("Jis X 0201 Half Width", MorphText::JIS_X_0201_HALFWIDTH);
}

void MungPlex::DataConversion::DrawWindow()
{
	static bool stateSet = false;

	if (ImGui::Begin("Data Conversion"))
	{
		if (!stateSet && Connection::IsConnected() && Settings::GetGeneralSettings().EnableRichPresence && ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows))
		{
			Connection::SetRichPresenceState("Data Conversion");
			stateSet = true;
		}

		GetInstance().drawPrimitiveConversion();
		ImGui::SameLine();
		GetInstance().drawColorConversion();
		GetInstance().drawTextConversion();
	}
	else
		stateSet = false;

	if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows))
		stateSet = false;

	ImGui::End();
}

void MungPlex::DataConversion::drawPrimitiveConversion()
{
	ImVec2 childXY = ImGui::GetContentRegionAvail();
	childXY.x *= 0.3333f;
	childXY.y *= 0.5f;

	ImGui::BeginChild("Primitive Value Conversion", childXY);
	{
		ImGui::SeparatorText("Primitive Value Conversion");
		drawHexFloatConversion();
		drawEndiannessConversion();
	}
	ImGui::EndChild();
}

void MungPlex::DataConversion::drawTextConversion()
{
	ImGui::BeginChild("Text Conversion");
	{
		ImGui::SeparatorText("Text Conversion");

		static int charCount = 512;
		static std::string plainText(charCount, 0);
		static std::string convertedText(charCount, 0);
		static MemoryEditor memEdit;
		static int memEditFlags = MemoryEditor::HIDE_OPTIONS | MemoryEditor::HIDE_ASCII;
		static int textTypeSelect = MorphText::UTF8;
		static bool update = true;

		ImGui::BeginGroup();
		{
			if (SetUpCombo("Text Type:", _textTypes, textTypeSelect, 0.5f, 0.35f))
				update = true;

			float height = 1.0f - ImGui::GetCursorPosY() / ImGui::GetContentRegionAvail().y;
			if (SetUpInputTextMultiline("Plain Text:", plainText.data(), plainText.size() + 1, 0.5f, height))
				update = true;
		}
		ImGui::EndGroup();

		ImGui::SameLine();
		ImGui::Dummy(ImVec2(10.0f, 0.0f));
		ImGui::SameLine();

		ImGui::BeginGroup();
		{
			if (memEdit.DrawContents(convertedText.data(), convertedText.size(), 0, NULL, nullptr, false, memEditFlags))
				convertHexText(convertedText, plainText, textTypeSelect);
			ImGui::Separator();

			if (update)
			{
				convertText(plainText, convertedText, textTypeSelect);
				update = false;
			}	
		}
		ImGui::EndGroup();
	}
	ImGui::EndChild();
}

void MungPlex::DataConversion::convertHexText(std::string& in, std::string& out, const int textTypeSelect)
{
	out = std::string(in.size(), 0);

	switch (textTypeSelect)
	{
	case MorphText::UTF16LE: {
		std::string temp = MorphText::Utf16LE_To_Utf8(std::wstring(reinterpret_cast<wchar_t*>(in.data())));
		out.resize(out.size() * 2);
		memcpy_s(out.data(), out.size(), temp.data(), temp.size() * 2);
	} break;
	case MorphText::UTF16BE: {
		std::string temp = MorphText::Utf16BE_To_Utf8(std::wstring(reinterpret_cast<wchar_t*>(in.data())));
		out.resize(out.size() * 2);
		memcpy_s(out.data(), out.size(), temp.data(), temp.size() * 2);
	} break;
	case MorphText::UTF32LE: {
		std::string temp = MorphText::Utf32LE_To_Utf8(std::u32string(reinterpret_cast<char32_t*>(in.data())));
		out.resize(out.size() * 4);
		memcpy_s(out.data(), out.size(), temp.data(), temp.size() * 4);
	} break;
	case MorphText::UTF32BE: {
		std::string temp = MorphText::Utf32BE_To_Utf8(std::u32string(reinterpret_cast<char32_t*>(in.data())));
		out.resize(out.size() * 4);
		memcpy_s(out.data(), out.size(), temp.data(), temp.size() * 4);
	} break;
	case MorphText::ASCII: {
		std::string temp = MorphText::ASCII_To_Utf8(in.data());
		memcpy_s(out.data(), out.size(), temp.data(), temp.size());
	} break;
	case MorphText::ISO_8859_1: case MorphText::ISO_8859_2: case MorphText::ISO_8859_3:
	case MorphText::ISO_8859_4: case MorphText::ISO_8859_5: case MorphText::ISO_8859_6:
	case MorphText::ISO_8859_7: case MorphText::ISO_8859_8: case MorphText::ISO_8859_9:
	case MorphText::ISO_8859_10: case MorphText::ISO_8859_11: case MorphText::ISO_8859_13:
	case MorphText::ISO_8859_14: case MorphText::ISO_8859_15: case MorphText::ISO_8859_16: {
		std::string temp = MorphText::ISO8859X_To_Utf8(in.data(), textTypeSelect);
		memcpy_s(out.data(), out.size(), temp.data(), temp.size());
	} break;
	case MorphText::SHIFTJIS: {
		std::string temp = MorphText::ShiftJis_To_Utf8(in.data());
		memcpy_s(out.data(), out.size(), temp.data(), temp.size());
	} break;
	case MorphText::JIS_X_0201_FULLWIDTH: {
		std::string temp = MorphText::JIS_X_0201_FullWidth_To_Utf8(in.data());
		memcpy_s(out.data(), out.size(), temp.data(), temp.size());
	} break;
	case MorphText::JIS_X_0201_HALFWIDTH: {
		std::string temp = MorphText::JIS_X_0201_HalfWidth_To_Utf8(in.data());
		memcpy_s(out.data(), out.size(), temp.data(), temp.size());
	} break;
	default: {// MorphText::UTF8
		out = in;
	} break;
	}
}

void MungPlex::DataConversion::convertText(std::string& in, std::string& out, const int textTypeSelect)
{
	out = std::string(in.size(), 0);

	switch (textTypeSelect)
	{
		case MorphText::UTF16LE: {
			std::wstring temp = MorphText::Utf8_To_Utf16LE(in);
			out.resize(out.size() * 2);
			memcpy_s(out.data(), out.size(), temp.data(), temp.size() * 2);
		} break;
		case MorphText::UTF16BE: {
			std::wstring temp = MorphText::Utf8_To_Utf16BE(in);
			out.resize(out.size() * 2);
			memcpy_s(out.data(), out.size(), temp.data(), temp.size() * 2);
		} break;
		case MorphText::UTF32LE: {
			std::u32string temp = MorphText::Utf8_To_Utf32LE(in);
			out.resize(out.size() * 4);
			memcpy_s(out.data(), out.size(), temp.data(), temp.size() * 4);
		} break;
		case MorphText::UTF32BE: {
			std::u32string temp = MorphText::Utf8_To_Utf32BE(in);
			out.resize(out.size() * 4);
			memcpy_s(out.data(), out.size(), temp.data(), temp.size() * 4);
		} break;
		case MorphText::ASCII: {
			std::string temp = MorphText::Utf8_To_ASCII(in);
			memcpy_s(out.data(), out.size(), temp.data(), temp.size());
		} break;
		case MorphText::ISO_8859_1: case MorphText::ISO_8859_2: case MorphText::ISO_8859_3: 
		case MorphText::ISO_8859_4: case MorphText::ISO_8859_5: case MorphText::ISO_8859_6:
		case MorphText::ISO_8859_7: case MorphText::ISO_8859_8: case MorphText::ISO_8859_9:
		case MorphText::ISO_8859_10: case MorphText::ISO_8859_11: case MorphText::ISO_8859_13: 
		case MorphText::ISO_8859_14: case MorphText::ISO_8859_15: case MorphText::ISO_8859_16: {
			std::string temp = MorphText::Utf8_To_ISO8859X(in, textTypeSelect);
			memcpy_s(out.data(), out.size(), temp.data(), temp.size());
		} break;
		case MorphText::SHIFTJIS: {
			out = MorphText::Utf8_To_ShiftJis(in);
			out.resize(512);
		} break;
		case MorphText::JIS_X_0201_FULLWIDTH: {
			std::string temp = MorphText::Utf8_To_JIS_X_0201_FullWidth(in);
			memcpy_s(out.data(), out.size(), temp.data(), temp.size());
		} break;
		case MorphText::JIS_X_0201_HALFWIDTH: {
			std::string temp = MorphText::Utf8_To_JIS_X_0201_HalfWidth(in);
			memcpy_s(out.data(), out.size(), temp.data(), temp.size());
		} break;
		default:{// MorphText::UTF8
			out = in;
		} break;
	}
}

void MungPlex::DataConversion::drawColorConversion()
{
	static bool alpha = false;
	static int specializedColorPickerFlags = ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_Uint8;
	static bool update = true;
	static int selectedColorID = LitColor::RGBF;
	static int selectedSpecializedColorTypeIndex = 0;
	static ImVec4 specializedColorVal = { 0.0f, 0.0f, 0.0f, 1.0f };
	static std::string specializedColorValueStr = std::string(48, 0);

	ImVec2 childXY = ImGui::GetContentRegionAvail();
	//childXY.x *= 0.666f;
	childXY.y *= 0.5f;

	ImGui::BeginChild("Color Conversion", childXY);
	{
		ImGui::SeparatorText("Color Conversion");

		ImVec2 spaceAvail = ImGui::GetContentRegionAvail();
		ImGui::PushItemWidth(spaceAvail.x * 0.4f);
		if (ImGui::ColorPicker4("##ColorPickerSpecialized", (float*)&specializedColorVal, specializedColorPickerFlags) || update)
		{
			update = false;
			ColorValuesToCString(specializedColorVal, selectedColorID, specializedColorValueStr.data(), alpha);
		}

		ImGui::SameLine();

		ImGui::BeginGroup();
		{
			if (SetUpCombo("Specialized Color Type:", _specializedColorTypes, selectedSpecializedColorTypeIndex, 1.0f, 0.5f))
			{
				selectedColorID = _specializedColorTypes[selectedSpecializedColorTypeIndex].second;
				update = true;
			}

			if (ImGui::Checkbox("Enable Alpha", &alpha))
			{
				if (alpha)
				{
					specializedColorPickerFlags &= ~ImGuiColorEditFlags_NoAlpha;
					specializedColorPickerFlags |= ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf;
				}
				else
				{
					specializedColorPickerFlags |= ImGuiColorEditFlags_NoAlpha;
					specializedColorPickerFlags &= ~(ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf);
				}

				update = true;
			}

			if(ImGui::Button("Pick Color From Screen"))
			{
				//HWND windowHandle = GetForegroundWindow(); todo: make this work ):
				specializedColorVal = PickColorFromScreen();
				update = true;
				//MungPlex::SetWindowToForeground(windowHandle);
			}

			ImGui::Dummy(ImVec2(0.0f, _verticalSpacing.y * 2.0f));
			ImGui::Text("Specialized Value:");

			if (SetUpInputText("##Specialized Val:", specializedColorValueStr.data(), specializedColorValueStr.size(), 1.0f, 0.333f, false))
			{
				LitColor specialized(specializedColorValueStr);
				specializedColorVal.x = specialized.GetColorValue<float>(0);
				specializedColorVal.y = specialized.GetColorValue<float>(1);
				specializedColorVal.z = specialized.GetColorValue<float>(2);
				specializedColorVal.w = specialized.GetColorValue<float>(3);
			}
		}
		ImGui::EndGroup();
	}
	ImGui::EndChild();
}

void MungPlex::DataConversion::drawHexFloatConversion()
{
	ImGui::Dummy(ImVec2(0.0f, _verticalSpacing.y * 0.75f));
	CenteredText("Float <-> Hex Conversion");
	ImGui::Dummy(ImVec2(0.0f, _verticalSpacing.y * 0.4f));

	ImGui::BeginGroup();
	{
		static int selectedFloatType = FloatTypes::FLOAT;
		static std::string hexFloat(17, 0);
		static float floatVal = 1.0f;
		static double doubleVal = 1.0;
		static bool isDouble = false;
		static bool update = true;

		if (SetUpCombo("Float Type:", _floatTypes, selectedFloatType, 1.0f, 0.35f))
		{
			isDouble = selectedFloatType == FloatTypes::DOUBLE;
			update = true;
		}

		if (isDouble)
		{
			if (SetUpInputDouble("Double:", &doubleVal, 0.1, 1.0, "%.16f", 1.0f, 0.35f) || update)
			{
				hexFloat = ToHexString(*(uint64_t*)&doubleVal, 16, false);
				update = false;
			}
		}
		else
		{
			if (SetUpInputFloat("Float:", &floatVal, 0.1f, 1.0f, "%.8f", 1.0f, 0.35f) || update)
			{
				hexFloat = ToHexString(*(uint32_t*)&floatVal, 8, false);
				update = false;
			}
		}

		if (SetUpInputText("Hex Float:", hexFloat.data(), hexFloat.size() + 1, 1.0f, 0.35f))
		{
			std::stringstream stream;
			stream << hexFloat;

			if (isDouble)
			{
				uint64_t temp;
				stream >> std::hex >> temp;
				doubleVal = *(double*)&temp;
			}
			else
			{
				uint32_t temp;
				stream >> std::hex >> temp;
				floatVal = *(float*)&temp;
			}
		}
	}
	ImGui::EndGroup();
}

void MungPlex::DataConversion::drawEndiannessConversion()
{
	ImGui::Dummy(ImVec2(0.0f, _verticalSpacing.y * 0.75f));
	CenteredText("Change Endianness");
	ImGui::Dummy(ImVec2(0.0f, _verticalSpacing.y * 0.4f));

	ImGui::BeginGroup();
	{
		static uint64_t le = 0;
		static uint64_t be = 0;
		static std::string leStr;
		static std::string beStr;
		static int intSelect = IntTypes::INT16;
		static bool update = true;

		if (SetUpCombo("Int Type:", _intTypes, intSelect, 1.0f, 0.35f) || update)
		{
			leStr.resize(4 << intSelect);
			beStr.resize(4 << intSelect);
			update = false;
		}

		if (SetUpInputText("Little Endian:", leStr.data(), leStr.size() + 1, 1.0f, 0.35f))
		{
			beStr = swapBytes(leStr, intSelect);
			update = true;
		}

		if (SetUpInputText("Big Endian:", beStr.data(), beStr.size() + 1, 1.0f, 0.35f))
		{
			leStr = swapBytes(beStr, intSelect);
			update = true;
		}
	}
	ImGui::EndGroup();
}

std::string MungPlex::DataConversion::swapBytes(std::string& in, const int select)
{
	uint64_t temp = 0;
	std::stringstream stream;
	stream << in;
	stream >> std::hex >> temp;

	switch (select)
	{
	case IntTypes::INT16:
		temp = Xertz::SwapBytes(static_cast<uint16_t>(temp));
		break;
	case IntTypes::INT64:
		temp = Xertz::SwapBytes(static_cast<uint64_t>(temp));
		break;
	default: //INT32
		temp = Xertz::SwapBytes(static_cast<uint32_t>(temp));
	}

	return ToHexString(temp, 4 << select, false);
}