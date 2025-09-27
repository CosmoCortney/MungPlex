#include "WaveView_Impl.hpp"
#include "Search.hpp"
#include "Settings.hpp"
#include "StringHelpers.hpp"
#include "WatchControl.hpp"

MungPlex::WaveView::WaveView(const int id)
{
	_id = id;
	_idText = std::to_string(id);
}

MungPlex::WaveView::WaveView(const int id, const nlohmann::json elem)
{
	_id = id;
	_idText = std::to_string(id);
	SetBasicMembers(elem);
	_plotMinWithDelta = _plotMin = elem["PlotMin"];
	_plotMaxWithDelta = _plotMax = elem["PlotMax"];
	_inputDelta.SetValue(elem["Delta"]);
	_inputExtremes.SetText(elem["Extremes"]);
	parseExtremes();
	_modifierType = elem["ModifierType"];
	_waveTableOffset = elem["WaveTableOffset"];
	_regionIndex = elem["RegionIndex"];
	_ogWaveTableCopy.resize(elem["Elements"]);
	void* ptr = ProcessInformation::GetPointerFromPointerPathExpression(_pointerPath, _useModulePath, _moduleAddress);
	ProcessInformation::DumpMemory(_ogWaveTableCopy.data(), ptr, _ogWaveTableCopy.size() * sizeof(float));

	if (ProcessInformation::UnderlyingIsBigEndian() && !ProcessInformation::GetRereorderFlag())
		_ogWaveTableCopy = SwapBytesVector<float>(_ogWaveTableCopy);

	_modifiedValues = _ogWaveTableCopy;
	_countStr = std::to_string(_ogWaveTableCopy.size());
}

MungPlex::WaveView::WaveView(const WaveView& other)
{
	assign(other);
}

MungPlex::WaveView& MungPlex::WaveView::operator=(const WaveView& other)
{
	return *this;
}

MungPlex::WaveView::WaveView(WaveView&& other) noexcept
{
	assign(other);
}

MungPlex::WaveView& MungPlex::WaveView::operator=(WaveView&& other) noexcept
{
	return *this;
}

void MungPlex::WaveView::Draw()
{
	float itemWidth = ImGui::GetContentRegionAvail().x;
	float itemHeight = 80.0f * Settings::GetGeneralSettings().Scale;

	ImGui::BeginChild(std::string("child_WaveView" + _idText).c_str(), ImVec2(itemWidth, itemHeight * 2.0f));
	{
		DrawSetup(itemWidth, itemHeight, IView::WAVE);
		drawSearchSetup(itemWidth, itemHeight);
		ImGui::SameLine();
		drawPointerPathSetup(itemWidth, itemHeight, ViewTypes::WAVE);
		ImGui::SameLine();
		drawPlotArea(itemWidth, itemHeight, IView::WAVE);
	}
	ImGui::EndChild();

	if (_delete)
	{
		WatchControl::DeleteItem(_id);
		_delete = false;
	}
}

nlohmann::json MungPlex::WaveView::GetJSON()
{
	nlohmann::json elemJson = GetBasicJSON();
	elemJson["Type"] = IView::WAVE;
	elemJson["Delta"] = _inputDelta.GetValue();
	elemJson["PlotMin"] = _plotMin;
	elemJson["PlotMax"] = _plotMax;
	elemJson["Extremes"] = _inputExtremes.GetStdStringNoZeros();
	elemJson["WaveTableOffset"] = _waveTableOffset;
	elemJson["ModifierType"] = _modifierType;
	elemJson["RegionIndex"] = _regionIndex;
	elemJson["Elements"] = _ogWaveTableCopy.size();
	return elemJson;
}

void MungPlex::WaveView::assign(const WaveView& other)
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

	_inputDelta = other._inputDelta;
	_plotVals = other._plotVals;
	_plotMin = other._plotMin;
	_plotMax = other._plotMax;
	_plotMinWithDelta = other._plotMinWithDelta;
	_plotMaxWithDelta = other._plotMaxWithDelta;
	_plotBuf = other._plotBuf;
	_inputExtremes = other._inputExtremes;
	_extremes = other._extremes;
	_ogWaveTableCopy = other._ogWaveTableCopy;
	_modifiedValues = other._modifiedValues;
	_waveTableOffset = other._waveTableOffset;
	_modifierType = other._modifierType;
	_regionIndex = other._regionIndex;
	_countStr = other._countStr;
}

void MungPlex::WaveView::drawSearchSetup(const float itemWidth, const float itemHeight)
{
	ImGui::BeginChild("child_Setup", ImVec2(itemWidth * 0.175f, itemHeight * 1.5f), true);
	{
		_inputExtremes.Draw(1.0f, 0.5f);

		if (ImGui::Button("Search"))
		{
			parseExtremes();
			processWavetableThread(std::bind(&WaveView::searchWave, this));
		}

		SetUpLableText("Elements: ", _countStr.c_str(), _countStr.size(), 1.0f, 0.5f);
	}
	ImGui::EndChild();
}

void MungPlex::WaveView::drawPlotArea(const float itemWidth, const float itemHeight, const int type)
{
	ImGui::BeginChild("child_view", ImVec2(itemWidth * 0.47f, itemHeight * 1.5f), true);
	{
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		ImGui::PlotLines("##Lines", _modifiedValues.data(), _modifiedValues.size(), 0, NULL, _plotMinWithDelta, _plotMaxWithDelta, ImVec2(0.0f, 40.0f));
		
		_inputDelta.Draw(0.33f, 0.33f);
		ImGui::SameLine();
		ImGui::RadioButton("Add", &_modifierType, ADD);
		ImGui::SameLine();
		ImGui::RadioButton("Multiply", &_modifierType, MUL);
		ImGui::SameLine();
		ImGui::RadioButton("Superposition", &_modifierType, SUPPO);

		if(ImGui::Button("Apply"))
			processWavetableThread(std::bind(&WaveView::modify, this));

		ImGui::SameLine();

		if (ImGui::Button("Reset"))
			processWavetableThread(std::bind(&WaveView::reset, this));
	}
	ImGui::EndChild();
}

void MungPlex::WaveView::processWavetableThread(const std::function<void()>& func)
{
	_processValueThread = boost::thread(func);
	_processValueThread.detach();
}

void MungPlex::WaveView::parseExtremes()
{
	std::string inputExtremes = _inputExtremes.GetStdStringNoZeros();

	if (inputExtremes.empty() || inputExtremes.size() == 0)
		return;

	_extremes.clear();
	inputExtremes = RemoveSpacePadding(inputExtremes, true);
	std::stringstream ss(inputExtremes);
	std::vector<std::string> strVec = SplitString(inputExtremes, ',');

	for (std::string& s : strVec)
		_extremes.push_back(std::stof(s));
}

void MungPlex::WaveView::setOriginalPlotRange()
{
	_plotMin = _plotMax = _extremes.front();

	for (float& f : _extremes)
	{
		if (f < _plotMin)
			_plotMin = f;

		if (f > _plotMax)
			_plotMax = f;
	}

	_plotMinWithDelta = _plotMin;
	_plotMaxWithDelta = _plotMax;
}

void MungPlex::WaveView::setModifiedPlotRange()
{
	if (_modifierType == SUPPO)
		return;

	float tempPlotMin = _modifierType ? _plotMin * _inputDelta.GetValue() : _plotMin + _inputDelta.GetValue();
	float tempPlotMax = _modifierType ? _plotMax * _inputDelta.GetValue() : _plotMax + _inputDelta.GetValue();
	
	if (_modifierType == MUL)
	{
		_plotMinWithDelta = tempPlotMin < -1.0f ? tempPlotMin : -1.0f;
		_plotMaxWithDelta = tempPlotMax > 1.0f ? tempPlotMax : 1.0f;
		return;
	}

	_plotMinWithDelta = tempPlotMin > 0.0f ? 0.0f : tempPlotMin;
	_plotMaxWithDelta = tempPlotMax < 0.0f ? 0.0f : tempPlotMax;
}

void MungPlex::WaveView::searchWave()
{
	Search::FindWaveTable(_extremes, _ogWaveTableCopy, _waveTableOffset, _regionIndex);
	_modifiedValues = _ogWaveTableCopy;
	_countStr = std::to_string(_ogWaveTableCopy.size());
	int64_t offset = _waveTableOffset + ProcessInformation::GetSystemRegionList()[_regionIndex].Base;
	_pointerPath.clear();
	_pointerPath.push_back(offset);
	_pointerPathInput.SetText(ToHexString(offset, ProcessInformation::GetAddressWidth() * 2, false));
}

void MungPlex::WaveView::modify()
{
	float delta = _inputDelta.GetValue();

	if (_modifierType == SUPPO)
	{
		if (true)
		{
			uint32_t elemCount = _ogWaveTableCopy.size();
			float elemCountF = elemCount;

			int i = 0;

			if(delta >= 0.0f)
				while(i < elemCount)
					for (float f = 0.0f; f < elemCountF && i < elemCount; f += delta)
					{
						_modifiedValues[i] = _ogWaveTableCopy[f];
						++i;
					}
			else
			{
				while (i < elemCount)
					for (float f = elemCountF-1.0f; f >= 0.0f && i < elemCount; f += delta)
					{
						_modifiedValues[i] = _ogWaveTableCopy[f];
						++i;
					}
			}
		}
	}
	else
	{
		for (uint64_t i = 0; i < _ogWaveTableCopy.size(); ++i)
		{
			if (_modifierType == MUL)
				_modifiedValues[i] = _ogWaveTableCopy[i] * delta;
			else if (_modifierType == ADD)
				_modifiedValues[i] = _ogWaveTableCopy[i] + delta;
		}
	}
	
	std::vector<float>* ref = &_modifiedValues;
	std::vector<float> modifiedBigEndian;

	if (ProcessInformation::UnderlyingIsBigEndian() && !ProcessInformation::GetRereorderFlag())
	{
		modifiedBigEndian = SwapBytesVector<float>(_modifiedValues);
		ref = &modifiedBigEndian;
	}

	void* ptr = ProcessInformation::GetPointerFromPointerPathExpression(_pointerPath, _useModulePath, _moduleAddress);
	ProcessInformation::PatchMemory<float*>(ref->data(), ptr, ref->size() * sizeof(float));
	setOriginalPlotRange();
	setModifiedPlotRange();
}

void MungPlex::WaveView::reset()
{
	_modifiedValues = _ogWaveTableCopy;

	if (ProcessInformation::UnderlyingIsBigEndian() && !ProcessInformation::GetRereorderFlag())
		_modifiedValues = SwapBytesVector<float>(_modifiedValues);

	void* ptr = ProcessInformation::GetPointerFromPointerPathExpression(_pointerPath, _useModulePath, _moduleAddress);
	ProcessInformation::PatchMemory<float*>(_modifiedValues.data(), ptr, _modifiedValues.size() * sizeof(float));
	_modifiedValues = _ogWaveTableCopy;
	setOriginalPlotRange();
}