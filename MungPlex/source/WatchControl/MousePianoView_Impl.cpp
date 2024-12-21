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

MungPlex::MousePianoView::MousePianoView(const MousePianoView& other)
{
	assign(other);
}

MungPlex::MousePianoView& MungPlex::MousePianoView::operator=(const MousePianoView& other)
{
	return *this;
}

MungPlex::MousePianoView::MousePianoView(MousePianoView&& other) noexcept
{
	assign(other);
}

MungPlex::MousePianoView& MungPlex::MousePianoView::operator=(MousePianoView&& other) noexcept
{
	return *this;
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
	{
		WatchControl::DeleteItem(_id);
		_delete = false;
	}
}

nlohmann::json MungPlex::MousePianoView::GetJSON()
{
	nlohmann::json elemJson = GetBasicJSON();

	elemJson["Type"] = IView::MOUSEPIANO;

	for (int i = 0; i < 32; ++i)
		elemJson["Switches"].emplace_back(std::pair<std::string, bool>(_switches[i].first.c_str(), _switches[i].second));

	return elemJson;
}

void MungPlex::MousePianoView::assign(const MousePianoView& other)
{
	_moduleW = other._moduleW;
	_moduleInput = other._moduleInput;
	_pointerPathInput = other._pointerPathInput;
	_labelInput = other._labelInput;
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

	_switches = other._switches;
	_checkBoxIDs = other._checkBoxIDs;
	_checkBoxTextIDs = other._checkBoxTextIDs;
	_idText = other._idText;
}

void MungPlex::MousePianoView::drawValueSetup(const float itemWidth, const float itemHeight, const int type)
{
	ImGui::BeginChild("child_view", ImVec2(itemWidth * 0.5f, itemHeight * 1.5f), true);
	{
		manageProcessValueThread();
		

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

void MungPlex::MousePianoView::processValue()
{
	while (_processValueThreadFlag)
	{
		boost::this_thread::sleep_for(boost::chrono::milliseconds(13));
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
}

void MungPlex::MousePianoView::manageProcessValueThread()
{
	if (_enableSignal || _disableSignal)
	{
		_processValueThreadFlag = false;

		if (_processValueThread.joinable())
			_processValueThread.detach();
	}

	if (_enableSignal)
	{
		_processValueThread = boost::thread(&MungPlex::MousePianoView::processValue, this);
		_processValueThreadFlag = true;
	}
}