#include "IntegralView_Impl.hpp"
#include "../../Settings.hpp"
#include "WatchControl.hpp"

MungPlex::IntegralView::IntegralView(const int id)
{
	_id = id;
	_idText = std::to_string(id);
	_formatPlot = "%lld/%lld";
	_plotVals.resize(_plotCount);
}

MungPlex::IntegralView::IntegralView(const int id, const nlohmann::json elem)
{
	_id = id;
	_idText = std::to_string(id);
	SetBasicMembers(elem);
	_val = elem["Value"];
	_typeSelect = elem["IntegralType"];
	_hex = elem["Hex"];
	_plotMin = elem["PlotMin"];
	_plotMax = elem["PlotMax"];
	_plotMinF = static_cast<float>(_plotMin);
	_plotMaxF = static_cast<float>(_plotMax);
	_plotVals.resize(_plotCount);
	_formatPlot = _hex ? "%llX/%llX" : "%lld/%lld";
}

nlohmann::json MungPlex::IntegralView::GetJSON()
{
	nlohmann::json elemJson = GetBasicJSON();

	elemJson["Type"] = IView::INTEGRAL;
	elemJson["IntegralType"] = _typeSelect;
	elemJson["Value"] = _val;
	elemJson["Hex"] = _hex;
	elemJson["PlotMin"] = _plotMin;
	elemJson["PlotMax"] = _plotMax;

	return elemJson;
}

void MungPlex::IntegralView::Draw()
{
	float itemWidth = ImGui::GetContentRegionAvail().x;
	float itemHeight = 80.0f * Settings::GetGeneralSettings().Scale;

	ImGui::BeginChild(std::string("child_integralView" + _idText).c_str(), ImVec2(itemWidth, itemHeight * 2.0f));
	{
		if (DrawSetup(itemWidth, itemHeight, IView::INTEGRAL))
			setFormatting();

		ImGui::SameLine();

		drawValueSetup(itemWidth, itemHeight, IView::INTEGRAL);
	}
	ImGui::EndChild();

	if (_delete)
		WatchControl::DeleteItem(_id);
}

void MungPlex::IntegralView::drawValueSetup(const float itemWidth, const float itemHeight, const int type)
{
	ImGui::BeginChild("child_view", ImVec2(itemWidth * 0.5f, itemHeight * 1.5f), true);
	{
		if (_active)
		{
			uint64_t valptr = reinterpret_cast<uint64_t>(ProcessInformation::GetPointerFromPointerPathExpression(_pointerPath, _useModulePath, _moduleAddress));

			if ((valptr >= _rangeMin && valptr < _rangeMax) || _pointerPath.size() == 1)
			{
				if (_freeze)
				{
					switch (_typeSelect)
					{
					case ImGuiDataType_S8: case ImGuiDataType_U8:
						ProcessInformation::WriteValue<uint8_t>(valptr, _val);
						break;
					case ImGuiDataType_S16: case ImGuiDataType_U16:
						ProcessInformation::WriteValue<uint16_t>(valptr, _val);
						break;
					case ImGuiDataType_S32: case ImGuiDataType_U32:
						ProcessInformation::WriteValue<uint32_t>(valptr, _val);
						break;
					default:
						ProcessInformation::WriteValue<uint64_t>(valptr, _val);
					}
				}
				else
				{
					switch (_typeSelect)
					{
					case ImGuiDataType_S8: case ImGuiDataType_U8:
						_val = ProcessInformation::ReadValue<uint8_t>(valptr);
						break;
					case ImGuiDataType_S16: case ImGuiDataType_U16:
						_val = ProcessInformation::ReadValue<uint16_t>(valptr);
						break;
					case ImGuiDataType_S32: case ImGuiDataType_U32:
						_val = ProcessInformation::ReadValue<uint32_t>(valptr);
						break;
					default:
						_val = ProcessInformation::ReadValue<uint64_t>(valptr);
					}
				}
			}
		}

		if (ImGui::Checkbox("Hex", &_hex))
			setFormatting();

		ImGui::SameLine();

		ImGui::SetNextItemWidth(itemWidth * 0.15f);
		ImGui::InputScalar("##value", _typeSelect, &_val, NULL, NULL, _format.c_str());

		ImGui::SameLine();

		drawPlotArea(itemWidth, itemHeight, type);
	}
	ImGui::EndChild();
}

void MungPlex::IntegralView::drawPlotArea(const float itemWidth, const float itemHeight, const int type)
{
	if(_active)
	{ 
		std::rotate(_plotVals.begin(), _plotVals.begin() + 1, _plotVals.end());
		_plotVals.back() = static_cast<float>(_val);
	}

	ImGui::Text("Plot Range:");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(itemWidth * 0.1f);

	if (ImGui::InputScalar("##plot range start", _typeSelect, &_plotMin, NULL, NULL, _format.c_str()))
		_plotMinF = static_cast<float>(_plotMin);

	ImGui::SameLine();
	ImGui::Text(" - ");
	ImGui::SameLine();

	if (ImGui::InputScalar("##plot range end", _typeSelect, &_plotMax, NULL, NULL, _format.c_str()))
		_plotMaxF = static_cast<float>(_plotMax);

	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
	ImGui::PlotHistogram("##values", _plotVals.data(), _plotVals.size(), 0, NULL, _plotMinF, _plotMaxF, ImVec2(0.0f, 40.0f));

	float progVal = 0;

	if (_active)
	{
		progVal = static_cast<float>(_val) / _plotMax;
		sprintf(_plotBuf.data(), _formatPlot.data(), _val, _plotMax);
	}

	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

	if (_freeze)
		ImGui::SliderScalar("##writeval", _typeSelect, &_val, &_plotMin, &_plotMax, _format.c_str());
	else
		ImGui::ProgressBar(progVal, ImVec2(0.0f, 0.0f), _plotBuf.data());
}

void MungPlex::IntegralView::setFormatting()
{
	if (_hex)
	{
		switch (_typeSelect)
		{
		case ImGuiDataType_S8: case ImGuiDataType_U8:
			_format = "%02X";
			break;
		case ImGuiDataType_S16: case ImGuiDataType_U16:
			_format = "%04X";
			break;
		case ImGuiDataType_S32: case ImGuiDataType_U32:
			_format = "%08X";
			break;
		default:
			_format = "%016X";
		}
	}
	else
	{
		switch (_typeSelect)
		{
		case ImGuiDataType_S8: case ImGuiDataType_S16: case ImGuiDataType_S32: case ImGuiDataType_S64:
			_format = "%i";
			break;
		default:
			_format = "%u";
		}
	}

	_formatPlot = _hex ? "%llX/%llX" : "%lld/%lld";
}
