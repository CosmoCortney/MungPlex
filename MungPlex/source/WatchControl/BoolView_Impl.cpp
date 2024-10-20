#include "BoolView_Impl.hpp"
#include "Settings.hpp"
#include "WatchControl.hpp"

MungPlex::BoolView::BoolView(const int id)
{
	_id = id;
	_idText = std::to_string(id);
}

MungPlex::BoolView::BoolView(const int id, const nlohmann::json elem)
{
	_id = id;
	_idText = std::to_string(id);
	SetBasicMembers(elem);
	_val = elem["Value"];
}

void MungPlex::BoolView::Draw()
{
	float itemWidth = ImGui::GetContentRegionAvail().x;
	float itemHeight = 80.0f * Settings::GetGeneralSettings().Scale;

	ImGui::BeginChild(std::string("child_boolView" + _idText).c_str(), ImVec2(itemWidth, itemHeight * 2.0f));
	{
		DrawSetup(itemWidth, itemHeight, IView::BOOL);

		ImGui::SameLine();

		ImGui::BeginChild("child_view", ImVec2(itemWidth * 0.5f, itemHeight * 1.5f), true);
		{
			drawValueSetup(itemWidth, itemHeight, -1);
		}
		ImGui::EndChild();

	}
	ImGui::EndChild();

	if (_delete)
		WatchControl::DeleteItem(_id);
}

nlohmann::json MungPlex::BoolView::GetJSON()
{
	nlohmann::json elemJson = GetBasicJSON();

	elemJson["Type"] = IView::BOOL;
	elemJson["Value"] = _val;

	return elemJson;
}

void MungPlex::BoolView::drawValueSetup(const float itemWidth, const float itemHeight, const int type)
{
	if (_active)
	{
		uint64_t valptr = reinterpret_cast<uint64_t>(ProcessInformation::GetPointerFromPointerPathExpression(_pointerPath, _useModulePath, _moduleAddress));

		if ((valptr >= _rangeMin && valptr < _rangeMax) || _pointerPath.size() == 1)
		{
			if (_freeze)
				ProcessInformation::WriteValue<bool>(valptr, _val);
			else
				_val = ProcessInformation::ReadValue<bool>(valptr);
		}
	}

	ImGui::SameLine();

	ImGui::SetNextItemWidth(itemWidth * 0.15f);
	ImGui::Checkbox("Is Set", &_val);
}

void MungPlex::BoolView::drawPlotArea(const float itemWidth, const float itemHeight, const int type)
{
	assert(false, "STOP, DON'T USE IT! 🧴💦");
}
