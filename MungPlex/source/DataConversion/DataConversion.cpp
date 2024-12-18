#include "DataConversion.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "MorphText.hpp"
#include "Settings.hpp"
#include <utility>
#include "Xertz.hpp"

inline const MungPlex::StringIdPairs MungPlex::DataConversion::_specializedColorTypes =
{
	{ "RGBF (3 Floats)", "RGBAF (4 Floats)", "RGB 565 (2 Bytes)", "RGB 5A3 (2 Bytes)" },
	{ LitColor::RGBF,    LitColor::RGBAF,    LitColor::RGB565,    LitColor::RGB5A3 },
	"Color Type:"
};

inline const MungPlex::StringIdPairs MungPlex::DataConversion::_floatTypes =
{
	{ "Float Single",    "Float Double" },
	{ FloatTypes::FLOAT, FloatTypes::DOUBLE },
	"Float Type:"
};

inline const MungPlex::StringIdPairs MungPlex::DataConversion::_intTypes =
{
	{ "Int 16",        "Int 32",        "Int 64" },
	{ IntTypes::INT16, IntTypes::INT32, IntTypes::INT64},
	"Integer Type:"
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
		static int textTypeSelect = MorphText::UTF8;
		static int textTypeIndex = 0;
		static bool update = true;

		ImGui::BeginGroup();
		{
			if (SetUpPairCombo(TextTypes, &textTypeIndex, 0.5f, 0.35f))
			{
				textTypeSelect = TextTypes.GetId(textTypeIndex);
				update = true;
			}

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
	static std::string specializedColorValueStr = std::string(48, 0);
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
			ColorValuesToCString(specializedColorVal, selectedColorID, specializedColorValueStr.data(), alpha);
		}

		ImGui::SameLine();

		ImGui::BeginGroup();
		{
			if (SetUpPairCombo(_specializedColorTypes, &selectedSpecializedColorTypeIndex, 1.0f, 0.5f))
			{
				selectedColorID = _specializedColorTypes.GetId(selectedSpecializedColorTypeIndex);
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

			if (SetUpInputText("##Specialized Val:", specializedColorValueStr.data(), specializedColorValueStr.size(), 1.0f, 0.333f, false))
				LitColorExpressionToImVec4(specializedColorValueStr.c_str(), &specializedColorVal);
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

		if (SetUpPairCombo(_floatTypes, &selectedFloatType, 1.0f, 0.35f))
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

		if (SetUpPairCombo(_intTypes, &intSelect, 1.0f, 0.35f) || update)
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