#include "FloatView_Impl.hpp"
#include "Settings.hpp"
#include "WatchControl.hpp"

MungPlex::FloatView::FloatView(const int id)
{
	_id = id;
	_idText = std::to_string(id);
	_plotVals.resize(_plotCount);
}

MungPlex::FloatView::FloatView(const int id, const nlohmann::json elem)
{
	_id = id;
	_idText = std::to_string(id);
	SetBasicMembers(elem);
	_val = elem["Value"];
	_typeSelect = elem["FloatType"];
	_plotMin = elem["PlotMin"];
	_plotMax = elem["PlotMax"];
	_plotVals.resize(_plotCount);
}

MungPlex::FloatView::FloatView(const FloatView& other)
{
	assign(other);
}

MungPlex::FloatView& MungPlex::FloatView::operator=(const FloatView& other)
{
	return *this;
}

MungPlex::FloatView::FloatView(FloatView&& other) noexcept
{
	assign(other);
}

MungPlex::FloatView& MungPlex::FloatView::operator=(FloatView&& other) noexcept
{
	return *this;
}

void MungPlex::FloatView::Draw()
{
	float itemWidth = ImGui::GetContentRegionAvail().x;
	float itemHeight = 80.0f * Settings::GetGeneralSettings().Scale;

	ImGui::BeginChild(std::string("child_floatView" + _idText).c_str(), ImVec2(itemWidth, itemHeight * 2.0f));
	{
		DrawSetup(itemWidth, itemHeight, IView::FLOAT);
		drawPlotArea(itemWidth, itemHeight, IView::FLOAT);
	}
	ImGui::EndChild();

	if (_delete)
		WatchControl::DeleteItem(_id);
}

nlohmann::json MungPlex::FloatView::GetJSON()
{
	nlohmann::json elemJson = GetBasicJSON();

	elemJson["Type"] = IView::FLOAT;
	elemJson["FloatType"] = _typeSelect;
	elemJson["Value"] = _val;
	elemJson["PlotMin"] = _plotMin;
	elemJson["PlotMax"] = _plotMax;

	return elemJson;
}

void MungPlex::FloatView::assign(const FloatView& other)
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
	DoublePrecision = other.DoublePrecision;
	_useSlider = other._useSlider;
	_min = other._min;
	_max = other._max;
	_plotCount = other._plotCount;
	_plotVals = other._plotVals;
	_plotMin = other._plotMin;
	_plotMax = other._plotMax;
	_plotBuf = other._plotBuf;
}

void MungPlex::FloatView::drawValueSetup(const float itemWidth, const float itemHeight, const int type)
{
	manageProcessValueThread();

	ImGui::SameLine();
	ImGui::SetNextItemWidth(itemWidth * 0.15f);
	ImGui::InputDouble("##value", &_val, NULL, NULL, "%.6f");
}

void MungPlex::FloatView::drawPlotArea(const float itemWidth, const float itemHeight, const int type)
{
	ImGui::SameLine();

	ImGui::BeginChild("child_view", ImVec2(itemWidth * 0.5f, itemHeight * 1.5f), true);
	{
		drawValueSetup(itemWidth, itemHeight, -1);

		ImGui::SameLine();

		if(_active)
		{
			std::rotate(_plotVals.begin(), _plotVals.begin() + 1, _plotVals.end());
			_plotVals.back() = static_cast<float>(_val);
		}

		ImGui::Text("Plot Range:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(itemWidth * 0.1f);

		if (ImGui::InputFloat("##plot range start", &_plotMin, NULL, NULL, "%.6f"))
			_plotMin = _plotMin;

		ImGui::SameLine();
		ImGui::Text(" - ");
		ImGui::SameLine();

		if (ImGui::InputFloat("##plot range end", &_plotMax, NULL, NULL, "%.6f"))
			_plotMax = _plotMax;

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		ImGui::PlotLines("##Lines", _plotVals.data(), _plotCount, 0, NULL, _plotMin, _plotMax, ImVec2(0.0f, 40.0f));

		float progVal = 0;

		if (_active)
		{
			progVal = _val / _plotMax;
			sprintf(_plotBuf.data(), "%.6f/%.6f", _val, _plotMax);
		}

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

		if (_freeze)
		{
			float val = static_cast<float>(_val);
			SetUpSliderFloat("writeval", &val, _plotMin, _plotMax, "%.6f", 1.0f, 0.0f, false);
			_val = static_cast<double>(val);
		}
		else
			ImGui::ProgressBar(progVal, ImVec2(0.0f, 0.0f), _plotBuf.data());
	}
	ImGui::EndChild();
}

void MungPlex::FloatView::processValue()
{
	while (_processValueThreadFlag)
	{
		boost::this_thread::sleep_for(boost::chrono::milliseconds(13));
		uint64_t valptr = reinterpret_cast<uint64_t>(ProcessInformation::GetPointerFromPointerPathExpression(_pointerPath, _useModulePath, _moduleAddress));

		if ((valptr >= _rangeMin && valptr < _rangeMax) || _pointerPath.size() == 1)
		{
			if (_freeze)
			{
				switch (s_FloatTypes.GetId(_typeSelect))
				{
				case ImGuiDataType_Double:
					ProcessInformation::WriteValue<double>(valptr, _val);
					break;
				default: // float
					ProcessInformation::WriteValue<float>(valptr, static_cast<float>(_val));
				}
			}
			else
			{
				switch (s_FloatTypes.GetId(_typeSelect))
				{
				case ImGuiDataType_Double:
					_val = ProcessInformation::ReadValue<double>(valptr);
					break;
				default: // float
					_val = static_cast<double>(ProcessInformation::ReadValue<float>(valptr));
				}
			}
		}
	}
}

void MungPlex::FloatView::manageProcessValueThread()
{
	if (_enableSignal || _disableSignal)
	{
		_processValueThreadFlag = false;

		if (_processValueThread.joinable())
			_processValueThread.detach();
	}

	if (_enableSignal)
	{
		_processValueThread = boost::thread(&MungPlex::FloatView::processValue, this);
		_processValueThreadFlag = true;
	}
}