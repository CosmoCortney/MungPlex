#include "MousePianoView_Impl.hpp"
#include "Settings.hpp"
#include "WatchControl.hpp"

MungPlex::MousePianoView::MousePianoView(const int id)
{
	_id = id;
	_idText = std::to_string(id);
	_switches.resize(32);

	for (auto& elem : _switches)
		elem.first.resize(32);

	assignCheckBoxIDs();
}

MungPlex::MousePianoView::MousePianoView(const int id, const nlohmann::json elem)
{
	_id = id;
	_idText = std::to_string(id);
	SetBasicMembers(elem);

	for (auto& entry : elem["Switches"])
		_switches.emplace_back(entry[0], entry[1]);

	_switches.resize(32);

	for (auto& elem : _switches)
		elem.first.resize(32);

	assignCheckBoxIDs();
}

void MungPlex::MousePianoView::assignCheckBoxIDs()
{
	for (int i = 0; i < 32; ++i)
	{
		_checkBoxIDs[i] = "##cb" + std::to_string(i);
		_checkBoxTextIDs[i] = "##cbt" + std::to_string(i);
	}
}

void MungPlex::MousePianoView::drawPlotArea(const float itemWidth, const float itemHeight, const int type)
{
	assert(false, "STOP, DON'T USE IT! 🧴💦");
}

void MungPlex::MousePianoView::Draw()
{
	float itemWidth = ImGui::GetContentRegionAvail().x;
	float itemHeight = 80.0f * Settings::GetGeneralSettings().Scale;

	ImGui::BeginChild(std::string("child_mousePianoView" + _idText).c_str(), ImVec2(itemWidth, itemHeight * 2.0f));
	{
		DrawSetup(itemWidth, itemHeight, IView::MOUSEPIANO);
		ImGui::SameLine();
		drawValueSetup(itemWidth, itemHeight, -1);
	}
	ImGui::EndChild();

	if (_delete)
		WatchControl::DeleteItem(_id);
}

nlohmann::json MungPlex::MousePianoView::GetJSON()
{
	nlohmann::json elemJson = GetBasicJSON();

	elemJson["Type"] = IView::MOUSEPIANO;

	for (int i = 0; i < 32; ++i)
		elemJson["Switches"].emplace_back(std::pair<std::string, bool>(_switches[i].first.c_str(), _switches[i].second));

	return elemJson;
}

void MungPlex::MousePianoView::drawValueSetup(const float itemWidth, const float itemHeight, const int type)
{
	ImGui::BeginChild("child_view", ImVec2(itemWidth * 0.5f, itemHeight * 1.5f), true);
	{
		if (_active)
		{
			uint64_t valptr = reinterpret_cast<uint64_t>(ProcessInformation::GetPointerFromPointerPathExpression(_pointerPath, _useModulePath, _moduleAddress));

			if ((valptr >= _rangeMin && valptr < _rangeMax) || _pointerPath.size() == 1)
			{
				uint32_t val = 0;

				if (_freeze)
				{
					for (int i = 0; i < 32; ++i)
						val |= _switches[i].second << i;

					ProcessInformation::WriteValue<uint32_t>(valptr, val);
				}
				else
				{
					val = ProcessInformation::ReadValue<uint32_t>(valptr);

					for (int i = 0; i < 32; ++i)
						_switches[i].second = static_cast<bool>((val >> i) & 1);
				}
			}
		}

		for (int i = 0; i < 32; ++i)
		{
			if (i & 0x3)
				ImGui::SameLine();

			ImGui::Checkbox(_checkBoxIDs[i].c_str(), &_switches[i].second);
			ImGui::SameLine();
			ImGui::SetNextItemWidth(itemWidth * 0.1f);
			ImGui::InputText(_checkBoxTextIDs[i].c_str(), _switches[i].first.data(), 32);
			ImGui::SameLine();
			ImGui::Dummy(ImVec2(5.0f, 0.0f));
		}
	}
	ImGui::EndChild();
}