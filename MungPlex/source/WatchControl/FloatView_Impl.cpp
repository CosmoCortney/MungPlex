#include "FloatView_Impl.hpp"
#include "Settings.hpp"
#include "WatchControl.hpp"

MungPlex::FloatView::FloatView(const int id)
{
	_id = id;
	_idText = std::to_string(id);
	_plotVals.resize(_plotCount);
	_sliderD.SetLabelDecimal("Adjust: %.6F");
	_sliderF.SetLabelDecimal("Adjust: %.6F");
}

MungPlex::FloatView::FloatView(const int id, const nlohmann::json elem)
{
	_id = id;
	_idText = std::to_string(id);
	SetBasicMembers(elem);
	_sliderD.SetValue(elem["Value"]);
	_sliderF.SetValue(elem["Value"]);
	_typeSelect = elem["FloatType"];
	_plotMinF.SetValue(elem["PlotMin"]);
	_plotMinD.SetValue(elem["PlotMin"]);
	_plotMaxF.SetValue(elem["PlotMax"]);
	_plotMaxD.SetValue(elem["PlotMax"]);
	_plotVals.resize(_plotCount);
	_sliderD.SetLabelDecimal("Adjust: %.6F");
	_sliderF.SetLabelDecimal("Adjust: %.6F");
	_sliderD.SetBoundaryLow(elem["PlotMin"]);
	_sliderD.SetBoundaryHigh(elem["PlotMax"]);
	_sliderF.SetBoundaryLow(elem["PlotMin"]);
	_sliderF.SetBoundaryHigh(elem["PlotMax"]);
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
	{
		WatchControl::DeleteItem(_id);
		_delete = false;
	}
}

nlohmann::json MungPlex::FloatView::GetJSON()
{
	nlohmann::json elemJson = GetBasicJSON();

	elemJson["Type"] = IView::FLOAT;
	elemJson["FloatType"] = _typeSelect;

	if (_typeSelect == ImGuiDataType_Double)
	{
		elemJson["Value"] = _inputD.GetValue();	
		elemJson["PlotMin"] = _plotMinD.GetValue();
		elemJson["PlotMax"] = _plotMaxD.GetValue();
	}
	else
	{
		elemJson["Value"] = _inputF.GetValue();
		elemJson["PlotMin"] = _plotMinF.GetValue();
		elemJson["PlotMax"] = _plotMaxF.GetValue();
	}

	return elemJson;
}

void MungPlex::FloatView::assign(const FloatView& other)
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

	_sliderD = other._sliderD;
	_sliderF = other._sliderF;
	_useSlider = other._useSlider;
	_min = other._min;
	_max = other._max;
	_plotCount = other._plotCount;
	_plotVals = other._plotVals;
	_plotMinD = other._plotMinD;
	_plotMaxD = other._plotMaxD;
	_plotMinF = other._plotMinF;
	_plotMaxF = other._plotMaxF;
	_plotBuf = other._plotBuf;
}

void MungPlex::FloatView::drawValueSetup(const float itemWidth, const float itemHeight, const int type)
{
	manageProcessValueThread();

	ImGui::SameLine();
	ImGui::SetNextItemWidth(itemWidth * 0.15f);

	if (_typeSelect == ImGuiDataType_Double)
	{
		if(_inputD.Draw())
			_sliderD.SetValue(_inputD.GetValue());
	}
	else
	{
		if (_inputF.Draw())
			_sliderF.SetValue(_inputF.GetValue());
	}
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

			if (_typeSelect == ImGuiDataType_Double)
				_plotVals.back() = static_cast<float>(_sliderD.GetValue());
			else
				_plotVals.back() = _sliderF.GetValue();
		}

		ImGui::Text("Plot Range:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(itemWidth * 0.1f);

		if (_typeSelect == ImGuiDataType_Double)
		{
			if(_plotMinD.Draw())
				_sliderD.SetBoundaryLow(_plotMinD.GetValue());
		}
		else
			if (_plotMinF.Draw())
				_sliderF.SetBoundaryLow(_plotMinF.GetValue());

		ImGui::SameLine();
		ImGui::Text(" - ");
		ImGui::SameLine();

		if (_typeSelect == ImGuiDataType_Double)
		{
			if (_plotMaxD.Draw())
				_sliderD.SetBoundaryHigh(_plotMaxD.GetValue());
		}
		else
			if (_plotMaxF.Draw())
				_sliderF.SetBoundaryHigh(_plotMaxF.GetValue());

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

		if (_typeSelect == ImGuiDataType_Double)
			ImGui::PlotLines("##Lines", _plotVals.data(), _plotCount, 0, NULL, _plotMinD.GetValue(), _plotMaxD.GetValue(), ImVec2(0.0f, 40.0f));
		else
			ImGui::PlotLines("##Lines", _plotVals.data(), _plotCount, 0, NULL, _plotMinF.GetValue(), _plotMaxF.GetValue(), ImVec2(0.0f, 40.0f));

		static float progVal = 0;

		if (_active)
		{
			if (_typeSelect == ImGuiDataType_Double)
			{
				progVal = static_cast<float>(_inputD.GetValue() / _plotMaxD.GetValue());
				sprintf(_plotBuf.data(), "%.6f/%.6f", _inputD.GetValue(), _plotMaxD.GetValue());
			}
			else
			{
				progVal = _inputF.GetValue() / _plotMaxF.GetValue();
				sprintf(_plotBuf.data(), "%.6f/%.6f", _inputF.GetValue(), _plotMaxF.GetValue());
			}
		}

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

		if (_freeze)
		{
			if (_typeSelect == ImGuiDataType_Double)
			{
				if (_sliderD.Draw(1.0f, 0.0f))
					_inputD.SetValue(_sliderD.GetValue());
			}
			else
			{
				if (_sliderF.Draw(1.0f, 0.0f))
					_inputF.SetValue(_sliderF.GetValue());
			}
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
				if (_typeSelect == ImGuiDataType_Double)
					ProcessInformation::WriteValue<double>(valptr, _inputD.GetValue());
				else
					ProcessInformation::WriteValue<float>(valptr, _inputF.GetValue());
			}
			else
			{
				if (_typeSelect == ImGuiDataType_Double)
				{
					static double temp = 0.0;
					temp = ProcessInformation::ReadValue<double>(valptr);
					_inputD.SetValue(temp);
					_sliderD.SetValue(temp);
				}
				else
				{
					static float temp = 0.0;
					temp = ProcessInformation::ReadValue<float>(valptr);
					_inputF.SetValue(temp);
					_sliderF.SetValue(temp);
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

	_enableSignal = _disableSignal = false;
}