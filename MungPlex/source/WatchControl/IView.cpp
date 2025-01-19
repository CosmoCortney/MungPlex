#include "IView.hpp"
#include "imgui.h"
#include "ProcessInformation.hpp"

inline const std::vector<MungPlex::StringIdCombo::VecType> MungPlex::IView::s_IntTypes =
{
	{
		{ "Int 8", ImGuiDataType_S8 },
		{ "UInt 8", ImGuiDataType_U8 },
		{ "Int 16", ImGuiDataType_S16 },
		{ "UInt 16", ImGuiDataType_U16 },
		{ "Int 32", ImGuiDataType_S32 },
		{ "UInt 32", ImGuiDataType_U32 },
		{ "Int 64", ImGuiDataType_S64 },
		{ "UInt 64", ImGuiDataType_U64 }
	}
};

inline const std::vector<MungPlex::StringIdCombo::VecType>  MungPlex::IView::s_FloatTypes =
{
	{
		{ "Single", ImGuiDataType_Float },
		{ "Double", ImGuiDataType_Double }
	}
};

inline const std::vector<MungPlex::StringIdCombo::VecType>  MungPlex::IView::s_SuperiorTypes =
{
	{
		{ "Integral", INTEGRAL },
		{ "Float", FLOAT },
		{ "Bool", BOOL },
		{ "DIP Switch", MOUSEPIANO },
		{ "3D Map", MAP3D }
	}
};

int MungPlex::IView::GetID()
{
	return _id;
}

bool MungPlex::IView::DrawSetup(const float itemWidth, const float itemHeight, const int type)
{
	static bool res = false;

	if(type != ViewTypes::MAP3D)
	{
		ImGui::Dummy(ImVec2(0.0f, 5.0f));
		res = drawActiveCheckBox();
		ImGui::SameLine();
	}

	switch (type)
	{
	case ViewTypes::FLOAT:
		ImGui::SeparatorText("Float View:");
		break;
	case ViewTypes::BOOL:
		ImGui::SeparatorText("Bool View:");
		break;
	case ViewTypes::MOUSEPIANO:
		ImGui::SeparatorText("DIP Switch View:");
		break;
	case ViewTypes::MAP3D:
		ImGui::SeparatorText("3D Map:");
		break;
	default: //ViewTypes::Integral
		ImGui::SeparatorText("Integral View:");
	}

	ImGui::SameLine();
	_labelInput.Draw(0.25f, 0.2f);
	ImGui::SameLine();

	if (ImGui::Button("Delete"))
		_delete = true;

	if (type == ViewTypes::MAP3D)
		return true;

	ImGui::BeginChild("child_Setup", ImVec2(itemWidth * 0.5f, itemHeight * 1.5f));
	{
		res = drawGeneralSetup(itemWidth, itemHeight, type);

		ImGui::SameLine();

		drawPointerPathSetup(itemWidth, itemHeight, type);
	}
	ImGui::EndChild();

	return res;
}

bool MungPlex::IView::drawGeneralSetup(const float itemWidth, const float itemHeight, const int type)
{
	static bool res = false;

	ImGui::BeginChild("child_setup", ImVec2(itemWidth * 0.15f, itemHeight * 1.5f), true);
	{
		//_enableSignal = _disableSignal = false;

		switch (type)
		{
		case ViewTypes::FLOAT:
			if (_floatTypeSelectCombo.Draw(1.0f, 0.5f))
			{
				res = true;
				_typeSelect = _floatTypeSelectCombo.GetSelectedId();
			}
			break;
		case ViewTypes::INTEGRAL:
			if (_intTypeSelectCombo.Draw(1.0f, 0.5f))
			{
				res = true;
				_typeSelect = _intTypeSelectCombo.GetSelectedId();
			}
			break;
		}

		ImGui::Checkbox("Write", &_freeze);
	}
	ImGui::EndChild();

	return res;
}

void MungPlex::IView::drawPointerPathSetup(const float itemWidth, const float itemHeight, const int type)
{
	ImGui::BeginChild("child_pointer", ImVec2(itemWidth * 0.345f, itemHeight * 1.5f), true);
	{
		if (ImGui::Checkbox("Use Module", &_useModulePath))
			if (_useModulePath)
				_moduleAddress = ProcessInformation::GetModuleAddress<uint64_t>(_moduleW);

		ImGui::SameLine();
		if (!_useModulePath) ImGui::BeginDisabled();
		_moduleInput.Draw(1.0f, 0.0f);
		if (!_useModulePath) ImGui::EndDisabled();

		if (_pointerPathInput.Draw(1.0f, 0.3f))
			ParsePointerPath(_pointerPath, _pointerPathInput.GetStdStringNoZeros());

		int addrType = ProcessInformation::GetAddressWidth() == 8 ? ImGuiDataType_U64 : ImGuiDataType_U32;
		std::string format = ProcessInformation::GetAddressWidth() == 8 ? "%016X" : "%08X";
		ImGui::Text("Target Addr Range:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(itemWidth * 0.1f);
		ImGui::InputScalar("##Target addr range:", addrType, &_rangeMin, NULL, NULL, format.c_str());
		ImGui::SameLine();
		ImGui::Text(" - ");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 2.0f);
		ImGui::InputScalar("## - ", addrType, &_rangeMax, NULL, NULL, format.c_str());
	}
	ImGui::EndChild();
}

bool MungPlex::IView::drawActiveCheckBox()
{
	static bool res = false;

	if (ImGui::Checkbox("##Active", &_active))
	{
		res = true;

		if (_active)
		{
			ParsePointerPath(_pointerPath, _pointerPathInput.GetStdStringNoZeros());

			if (_useModulePath)
				_moduleAddress = ProcessInformation::GetModuleAddress<uint64_t>(_moduleW);
		}

		_enableSignal = _active;
		_disableSignal = !_enableSignal;
	}

	return res;
}

nlohmann::json MungPlex::IView::GetBasicJSON()
{
	nlohmann::json elemJson;

	elemJson["Title"] = _labelInput.GetStdStringNoZeros();
	elemJson["Module"] = _moduleInput.GetStdStringNoZeros();
	elemJson["UseModule"] = _useModulePath;
	elemJson["WriteOn"] = _freeze;
	elemJson["Active"] = _active;
	elemJson["PointerMin"] = _rangeMin;
	elemJson["PointerMax"] = _rangeMax;

	for (int64_t ptr : _pointerPath)
		elemJson["PointerPath"].push_back(ptr);

	return elemJson;
}

void MungPlex::IView::SetBasicMembers(const nlohmann::json elem)
{
	_labelInput.SetText(elem["Title"]);
	_moduleInput.SetText(elem["Module"]);
	_moduleW.resize(32);
	_moduleW = MT::Convert<std::string, std::wstring>(_moduleInput.GetStdStringNoZeros(), MT::UTF8, MT::UTF16LE);
	_moduleW.resize(32);
	_useModulePath = elem["UseModule"];
	_freeze = elem["WriteOn"];
	_active = elem["Active"];
	_rangeMin = elem["PointerMin"];
	_rangeMax = elem["PointerMax"];
	_pointerPathInput.SetText("");

	for (int i = 0; i < elem["PointerPath"].size(); ++i)
	{
		int64_t ptr = elem["PointerPath"][i];
		_pointerPath.push_back(ptr);
		std::stringstream stream;
		stream << std::uppercase << std::hex << ptr;
		_pointerPathInput.AppendText(stream.str());

		if (i < elem["PointerPath"].size() - 1)
			_pointerPathInput.AppendText(", ");
	}

	if (_active)
	{
		_enableSignal = _active;
		_disableSignal = !_enableSignal;
	}

}