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

MungPlex::BoolView::BoolView(const BoolView& other)
{
	assign(other);
}

MungPlex::BoolView& MungPlex::BoolView::operator=(const BoolView& other)
{
	return *this;
}

MungPlex::BoolView::BoolView(BoolView&& other) noexcept
{
	assign(other);
}

MungPlex::BoolView& MungPlex::BoolView::operator=(BoolView&& other) noexcept
{
	return *this;
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

void MungPlex::BoolView::assign(const BoolView& other)
{
	_moduleW = other._moduleW;
	_module = other._module;
	_pointerPathText = other._pointerPathText;
	_label = other._label;
	_pointerPath = other._pointerPath;
	_useModulePath = other._useModulePath;
	_moduleAddress = other._moduleAddress;
	_freeze = other._freeze;
	_active = other._active;
	_enableSignal = other._enableSignal;
	_disableSignal = other._disableSignal;
	_id = other._id;
	_delete = other._delete;
	_idText = other._idText;
	_rangeMin = other._rangeMin;
	_rangeMax = other._rangeMax;
	_typeSelect = other._typeSelect;

	_val = other._val;
}

void MungPlex::BoolView::drawValueSetup(const float itemWidth, const float itemHeight, const int type)
{
	manageProcessValueThread();

	ImGui::SameLine();
	ImGui::SetNextItemWidth(itemWidth * 0.15f);
	ImGui::Checkbox("Is Set", &_val);
}

void MungPlex::BoolView::drawPlotArea(const float itemWidth, const float itemHeight, const int type)
{
	assert(false, "STOP, DON'T USE IT! 🧴💦");
}

void MungPlex::BoolView::processValue()
{
	while (_processValueThreadFlag)
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
}

void MungPlex::BoolView::manageProcessValueThread()
{
	if (_enableSignal || _disableSignal)
	{
		_processValueThreadFlag = false;

		if (_processValueThread.joinable())
			_processValueThread.detach();
	}

	if (_enableSignal)
	{
		_processValueThread = boost::thread(&MungPlex::BoolView::processValue, this);
		_processValueThreadFlag = true;
	}
}