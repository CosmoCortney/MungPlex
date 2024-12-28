#include "DataConversion.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "MorphText.hpp"
#include "Settings.hpp"
#include <utility>
#include "Xertz.hpp"

inline const MungPlex::StringIdCombo::Type MungPlex::DataConversion::_specializedColorTypes =
{
	{
		{ "RGBF (3 Floats)", LitColor::RGBF },
		{ "RGBAF (4 Floats)", LitColor::RGBAF },
		{ "RGB 565 (2 Bytes)", LitColor::RGB565 },
		{ "RGB 5A3 (2 Bytes)", LitColor::RGB5A3 }
	}
};

inline const MungPlex::StringIdCombo::Type MungPlex::DataConversion::_floatTypes =
{
	{
		{ "Single", FLOAT },
		{ "Double", DOUBLE }
	}
};

inline const MungPlex::StringIdCombo::Type MungPlex::DataConversion::_intTypes =
{
	{
		{ "Int 16", INT16 },
		{ "Int 32", INT32 },
		{ "Int 64", INT64 }
	}
};

MungPlex::DataConversion::DataConversion()
{
	_verticalSpacing = ImVec2(0.0f, 25.0f * Settings::GetGeneralSettings().Scale);
}

void MungPlex::DataConversion::DrawWindow()
{
	static bool stateSet = false;

	if (ImGui::Begin("Data Conversion"))
	{
		if (!stateSet && Connection::IsConnected() && Settings::GetGeneralSettings().EnableRichPresence && ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows))
		{
			Connection::GetDiscordRichPresence().SetRichPresenceState("Data Conversion");
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
		static bool update = true;

		ImGui::BeginGroup();
		{
			if (_textTypesCombo.Draw(0.5f, 0.35f))
				update = true;

			float height = 1.0f - ImGui::GetCursorPosY() / ImGui::GetContentRegionAvail().y;
			if (_plainTextInput.Draw(0.5f, height))
				update = true;
		}
		ImGui::EndGroup();

		ImGui::SameLine();
		ImGui::Dummy(ImVec2(10.0f, 0.0f));
		ImGui::SameLine();

		ImGui::BeginGroup();
		{
			if (memEdit.DrawContents(convertedText.data(), convertedText.size(), 0, NULL, nullptr, false, memEditFlags))
			{
				convertHexText(convertedText, plainText, _textTypesCombo.GetSelectedId());
				_plainTextInput.SetText(plainText);
			}

			ImGui::Separator();

			if (update)
			{
				plainText = _plainTextInput.GetStdString();
				convertText(plainText, convertedText, _textTypesCombo.GetSelectedId());
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
		case MorphText::UTF16LE: case MorphText::UTF16BE:
		{
			std::string temp = MorphText::Convert<std::wstring, std::string>(reinterpret_cast<wchar_t*>(in.data()), textTypeSelect, MorphText::UTF8);
			out.resize(out.size() * 2);
			memcpy_s(out.data(), out.size(), temp.data(), temp.size() * 2);
		} break;
		case MorphText::UTF32LE: case MorphText::UTF32BE:
		{
			std::string temp = MorphText::Convert<std::u32string, std::string>(reinterpret_cast<char32_t*>(in.data()), textTypeSelect, MorphText::UTF8);
			out.resize(out.size() * 4);
			memcpy_s(out.data(), out.size(), temp.data(), temp.size() * 4);
		} break;
		case MorphText::ISO_8859_1: case MorphText::ISO_8859_2: case MorphText::ISO_8859_3:
		case MorphText::ISO_8859_4: case MorphText::ISO_8859_5: case MorphText::ISO_8859_6:
		case MorphText::ISO_8859_7: case MorphText::ISO_8859_8: case MorphText::ISO_8859_9:
		case MorphText::ISO_8859_10: case MorphText::ISO_8859_11: case MorphText::ISO_8859_13:
		case MorphText::ISO_8859_14: case MorphText::ISO_8859_15: case MorphText::ISO_8859_16:
		case MorphText::ASCII: case MorphText::JIS_X_0201_FULLWIDTH: case MorphText::JIS_X_0201_HALFWIDTH: 
		case MorphText::SHIFTJIS_CP932: case MorphText::POKEMON_GEN1_ENGLISH: case MorphText::POKEMON_GEN1_FRENCH_GERMAN:
		case MorphText::POKEMON_GEN1_ITALIAN_SPANISH:  case MorphText::POKEMON_GEN1_JAPANESE: case MorphText::KS_X_1001:
		{
			std::string temp = MorphText::Convert<std::string, std::string>(in.data(), textTypeSelect, MorphText::UTF8);
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
		case MorphText::UTF16LE: case MorphText::UTF16BE:
		{
			std::wstring temp = MorphText::Convert<std::string, std::wstring>(in, MorphText::UTF8, textTypeSelect);
			out.resize(out.size() * 2);
			memcpy_s(out.data(), out.size(), temp.data(), temp.size() * 2);
		} break;
		case MorphText::UTF32LE: case MorphText::UTF32BE:
		{
			std::u32string temp = MorphText::Convert<std::string, std::u32string>(in, MorphText::UTF8, textTypeSelect);
			out.resize(out.size() * 4);
			memcpy_s(out.data(), out.size(), temp.data(), temp.size() * 4);
		} break;
		case MorphText::ISO_8859_1: case MorphText::ISO_8859_2: case MorphText::ISO_8859_3: 
		case MorphText::ISO_8859_4: case MorphText::ISO_8859_5: case MorphText::ISO_8859_6:
		case MorphText::ISO_8859_7: case MorphText::ISO_8859_8: case MorphText::ISO_8859_9:
		case MorphText::ISO_8859_10: case MorphText::ISO_8859_11: case MorphText::ISO_8859_13: 
		case MorphText::ISO_8859_14: case MorphText::ISO_8859_15: case MorphText::ISO_8859_16: 
		case MorphText::JIS_X_0201_FULLWIDTH: case MorphText::JIS_X_0201_HALFWIDTH: case MorphText::ASCII: 
		case MorphText::SHIFTJIS_CP932: case MorphText::POKEMON_GEN1_ENGLISH: case MorphText::POKEMON_GEN1_FRENCH_GERMAN:
		case MorphText::POKEMON_GEN1_ITALIAN_SPANISH:  case MorphText::POKEMON_GEN1_JAPANESE: case MorphText::KS_X_1001:
		{
			std::string temp = MorphText::Convert<std::string, std::string>(in, MorphText::UTF8, textTypeSelect);
			memcpy_s(out.data(), out.size(), temp.data(), temp.size());

			if(textTypeSelect == MorphText::SHIFTJIS_CP932) //consider single- and double-byte widths
				out.resize(512);
		} break;
		default: // MorphText::UTF8
			out = in;
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
	static bool useColorWheel = false;
	ImVec2 childXY = ImGui::GetContentRegionAvail();
	//childXY.x *= 0.666f;
	childXY.y *= 0.5f;

	ImGui::BeginChild("Color Conversion", childXY);
	{
		ImGui::SeparatorText("Color Conversion");
		ImVec2 spaceAvail = ImGui::GetContentRegionAvail();

		if (DrawColorPicker(selectedColorID, alpha, &specializedColorVal, useColorWheel, 0.4f))
		{
			update = false;
			ColorValuesToCString(specializedColorVal, selectedColorID, _specializedColorValueInput.GetData(), alpha);
		}

		ImGui::SameLine();

		ImGui::BeginGroup();
		{
			if (_colorTypesCombo.Draw(1.0f, 0.5f))
			{
				selectedColorID = _colorTypesCombo.GetSelectedId();
				update = true;
			}

			DrawExtraColorPickerOptions(&useColorWheel, &specializedColorVal);

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

			ImGui::Dummy(ImVec2(0.0f, _verticalSpacing.y * 2.0f));
			ImGui::Text("Specialized Value:");

			if (_specializedColorValueInput.Draw(1.0f, 0.333f))
				LitColorExpressionToImVec4(_specializedColorValueInput.GetCString(), &specializedColorVal);
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
		static float floatVal = 1.0f;
		static double doubleVal = 1.0;
		static bool isDouble = false;
		static bool update = true;
		static bool scientific = false;

		if(_floatTypesCombo.Draw(0.7f, 0.5f))
		{
			isDouble = _floatTypesCombo.GetSelectedId() == DOUBLE;

			if (isDouble)
				_floatDoubleConvertInput.SetValue(_floatSingleConvertInput.GetValue());
			else
				_floatSingleConvertInput.SetValue(_floatDoubleConvertInput.GetValue());

			update = true;
		}

		ImGui::SameLine();

		ImGui::Checkbox("Scientific", &scientific);

		if (isDouble)
		{
			if (_floatDoubleConvertInput.Draw(1.0f, 0.35f, scientific) || update)
			{
				doubleVal = _floatDoubleConvertInput.GetValue();
				_hexDoubleInput.SetValue(*reinterpret_cast<uint64_t*>(&doubleVal));
				update = false;
			}
		}
		else
		{
			if (_floatSingleConvertInput.Draw(1.0f, 0.35f, scientific) || update)
			{
				floatVal = _floatSingleConvertInput.GetValue();
				_hexFloatInput.SetValue(*reinterpret_cast<uint32_t*>(&floatVal));
				update = false;
			}
		}

		if (isDouble)
		{
			if (_hexDoubleInput.Draw(1.0f, 0.35f, true))
			{
				static uint64_t int64Val = 0;
				int64Val = _hexDoubleInput.GetValue();
				_floatDoubleConvertInput.SetValue(*reinterpret_cast<double*>(&int64Val));
			}
		}
		else
		{
			if (_hexFloatInput.Draw(1.0f, 0.35f, true))
			{
				static uint32_t int32Val = 0;
				int32Val = _hexFloatInput.GetValue();
				_floatSingleConvertInput.SetValue(*reinterpret_cast<float*>(&int32Val));
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
		_intTypesCombo.Draw(1.0f, 0.35f);

		switch (_intTypesCombo.GetSelectedId())
		{
		case INT16:
			if (_littleEndianInput16.Draw(1.0f, 0.35f, true))
				_bigEndianInput16.SetValue(std::byteswap(_littleEndianInput16.GetValue()));
		break;
		case INT64:
			if (_littleEndianInput64.Draw(1.0f, 0.35f, true))
				_bigEndianInput64.SetValue(std::byteswap(_littleEndianInput64.GetValue()));
		break;
		default:
			if (_littleEndianInput32.Draw(1.0f, 0.35f, true))
				_bigEndianInput32.SetValue(std::byteswap(_littleEndianInput32.GetValue()));
		}

		switch (_intTypesCombo.GetSelectedId())
		{
		case INT16:
			if (_bigEndianInput16.Draw(1.0f, 0.35f, true))
				_littleEndianInput16.SetValue(std::byteswap(_bigEndianInput16.GetValue()));
		break;
		case INT64:
			if (_bigEndianInput64.Draw(1.0f, 0.35f, true))
				_littleEndianInput64.SetValue(std::byteswap(_bigEndianInput64.GetValue()));
		break;
		default:
			if (_bigEndianInput32.Draw(1.0f, 0.35f, true))
				_littleEndianInput32.SetValue(std::byteswap(_bigEndianInput32.GetValue()));
		}
	}
	ImGui::EndGroup();
}