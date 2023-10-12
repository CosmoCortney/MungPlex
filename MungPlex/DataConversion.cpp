#include"DataConversion.h"
#include"MorphText.h"
#include"HelperFunctions.h"
#include"LitColor.h"
#include"Settings.h"

MungPlex::DataConversion::DataConversion()
{
	_verticalSpacing = ImVec2(0.0f, 25.0f * Settings::GetGeneralSettings().Scale);

	_specializedColorTypes.emplace_back("RGBF (3 Floats)", LitColor::RGBF);
	_specializedColorTypes.emplace_back("RGBAF (4 Floats)", LitColor::RGBAF);
	_specializedColorTypes.emplace_back("RGB 565 (2 Bytes)", LitColor::RGB565);
	_specializedColorTypes.emplace_back("RGB 5A3 (2 Bytes)", LitColor::RGB5A3);

	_floatTypes.emplace_back("Float Single", FloatTypes::FLOAT);
	_floatTypes.emplace_back("Float Double", FloatTypes::DOUBLE);
}

void MungPlex::DataConversion::DrawWindow()
{
	ImGui::Begin("Data Conversion");
	
	GetInstance().drawPrimitiveConversion();
	ImGui::SameLine();
	GetInstance().drawColorConversion();

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
		ImGui::Dummy(ImVec2(0.0f, _verticalSpacing.y * 0.75f));

		CenteredText("Float <-> Hex Conversion");
		ImGui::Dummy(ImVec2(0.0f, _verticalSpacing.y * 0.4f));
		{
			static int selectedFloatType = FloatTypes::FLOAT;
			static std::string hexFloat(17, 0);
			static float floatVal = 1.0f;
			static double doubleVal = 1.0;
			static bool isDouble = false;
			static bool update = true;

			if (SetUpCombo("FloatType:", _floatTypes, selectedFloatType, 1.0f, 0.35f))
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
	}
	ImGui::EndChild();
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

			if (ImGui::Button("Specialized To RGB(A)"))
			{
				LitColor specialized(specializedColorValueStr);
				specializedColorVal.x = specialized.GetColorValue<float>(0);
				specializedColorVal.y = specialized.GetColorValue<float>(1);
				specializedColorVal.z = specialized.GetColorValue<float>(2);
				specializedColorVal.w = specialized.GetColorValue<float>(3);
			}

			ImGui::Dummy(ImVec2(0.0f, _verticalSpacing.y * 2.0f));
			ImGui::Text("Specialized Value:");
			SetUpInputText("##Specialized Val:", specializedColorValueStr.data(), specializedColorValueStr.size(), 1.0f, 0.333f, false);
		}
		ImGui::EndGroup();
	}
	ImGui::EndChild();
}