#include "IView.hpp"
#include "imgui.h"
#include "ProcessInformation.hpp"

inline const MungPlex::StringIdPairs MungPlex::IView::s_IntTypes =
{
	{ "Int 8",          "UInt 8",         "Int 16",          "UInt 16",         "Int 32",          "UInt 32",         "Int 64",          "UInt 64" },
	{ ImGuiDataType_S8, ImGuiDataType_U8, ImGuiDataType_S16, ImGuiDataType_U16, ImGuiDataType_S32, ImGuiDataType_U32, ImGuiDataType_S64, ImGuiDataType_U64 },
	"Int Type:"
};

inline const MungPlex::StringIdPairs  MungPlex::IView::s_FloatTypes =
{
	{ "Single", "Double" },
	{ ImGuiDataType_Float, ImGuiDataType_Double },
	"Float Type:"
};

inline const MungPlex::StringIdPairs  MungPlex::IView::s_SuperiorTypes =
{
	{ "Integral", "Float", "Bool", "DIP Switch" },
	{ INTEGRAL, FLOAT, BOOL, MOUSEPIANO },
	"Item Type:"
};

int MungPlex::IView::GetID()
{
	return _id;
}

bool MungPlex::IView::DrawSetup(const float itemWidth, const float itemHeight, const int type)
{
	static bool res = false;

	switch (type)
	{
	case ViewTypes::FLOAT:
		ImGui::SeparatorText("Float View");
		break;
	case ViewTypes::BOOL:
		ImGui::SeparatorText("Bool View");
		break;
	case ViewTypes::MOUSEPIANO:
		ImGui::SeparatorText("DIP Switch View");
		break;
	default: //ViewTypes::Integral
		ImGui::SeparatorText("Integral View");
	}

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
		_labelInput.Draw(1.0f, 0.3f);
		_enableSignal = _disableSignal = false;

		if (ImGui::Checkbox("Active", &_active))
		{
			res = true;

			if (_active)
			{
				ParsePointerPath(_pointerPath, _pointerPathInput.GetStdStringNoZeros());

				if(_useModulePath)
					_moduleAddress = ProcessInformation::GetModuleAddress<uint64_t>(_moduleW);
			}

			_enableSignal = _active;
			_disableSignal = !_enableSignal;
		}


		switch (type)
		{
		case ViewTypes::FLOAT:
			res |= SetUpPairCombo(s_FloatTypes, &_typeSelect, 1.0f, 0.5f);
			break;
		case ViewTypes::INTEGRAL:
			res |= SetUpPairCombo(s_IntTypes, &_typeSelect, 1.0f, 0.5f);
			break;
		}

		if (ImGui::Button("Delete"))
		{
			_delete = true;
		}

		ImGui::SameLine();
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
}