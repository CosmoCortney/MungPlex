#include "Helperfunctions.hpp"
#include "Map3dView_Impl.hpp"
#include "Settings.hpp"
#include "WatchControl.hpp"
#include <fstream>
#include <sstream>
#include <vector>
#include <array>

inline const std::vector<MungPlex::StringIdCombo::VecType> MungPlex::Map3dView::_plotTypes =
{
	{
		{ "Scatter", SCATTER },
		{ "Mesh", MESH }
	}
};

inline const std::vector < MungPlex::StringIdCombo::VecType> MungPlex::Map3dView::_markerTypes =
{
	{
		{ "Circle", ImPlot3DMarker_Circle },
		{ "Square", ImPlot3DMarker_Square },
		{ "Diamond", ImPlot3DMarker_Diamond },
		{ "Up", ImPlot3DMarker_Up },
		{ "Down", ImPlot3DMarker_Down },
		{ "Left", ImPlot3DMarker_Left },
		{ "Right", ImPlot3DMarker_Right },
		{ "Cross", ImPlot3DMarker_Cross },
		{ "Plus", ImPlot3DMarker_Plus },
		{ "Asterisk", ImPlot3DMarker_Asterisk },
		//{ "Count", ImPlot3DMarker_COUNT }
	}
};

MungPlex::Map3dView::Map3dView(const int id)
{
	_id = id;
	_idText = std::to_string(id);
	_markerSizeSlider.SetLabelDecimal("Marker Size: %.1F");
}

MungPlex::Map3dView::Map3dView(const int id, const nlohmann::json& elem)
{
	_items.clear();
	_itemSelectCombo.Clear();
	_rangeBeginnings.Clear();
	_rangeEnds.Clear();
	_moduleInputs.Clear();
	_pointerPathInputs.Clear();
	_useModulePathVec.clear();
	_objPaths.Clear();
	_fillColorVec.clear();
	_lineColorVec.clear();
	_markerColorVec.clear();
	_colorPickerEnablerVec.clear();
	_scatterCounts.Clear();
	_scatterOffsets.Clear();
	_plotNames.Clear();
	_markerOffset.clear();
	_coordinateDisplacements.Clear();
	_trackLines.clear();
	_markerTypeSelects.clear();
	_id = id;
	_idText = std::to_string(id);
	_labelInput.SetText(elem["Title"]);
	_clippingOn = elem["ClippingOn"];
	_markerSizeSlider.SetValue(elem["MarkerSize"]);
	_axesFlipFlags = elem["FlipAxes"];
	uint64_t count = elem["ItemCount"];
	const nlohmann::json items = elem["Items"];

	for (int i = 0; i < count; ++i)
	{
		_plotTypeSelectCombo.SetSelectedById(items["ItemTypes"][i]);
		initNewitem();
		_rangeBeginnings.SetValueAt(items["RangeBeginnings"][i], i);
		_rangeEnds.SetValueAt(items["RangeEnds"][i], i);
		_moduleInputs.SetValueAt(items["Modules"][i], i);
		_pointerPathInputs.SetValueAt(items["PointerPaths"][i], i);
		_useModulePathVec[i] = items["UseModule"][i];
		_objPaths.SetValueAt(items["ObjPaths"][i], i);
		_fillColorVec[i] = GetColorVecFromJson(items["FillColors"][i]);
		_lineColorVec[i] = GetColorVecFromJson(items["LineColors"][i]);
		_markerColorVec[i] = GetColorVecFromJson(items["MarkerColors"][i]);
		_colorPickerEnablerVec[i] = items["EnableColorPickers"][i];
		_scatterCounts.SetValueAt(items["ScatterCounts"][i], i);
		_scatterOffsets.SetValueAt(items["ScatterOffset"][i], i);
		_plotNames.SetValueAt(items["PlotNames"][i], i);
		_markerOffset[i] = items["MarkerOffsets"][i];
		_coordinateDisplacements.SetValueAt(items["Displacements"][i], i);
		_trackLines[i] = items["TrackLines"][i];
		_markerTypeSelects[i] = items["MarkerTypes"][i];
		_markerSizeSlider.SetLabelDecimal("Marker Size: %.1F");
		_markerTypeSelectCombo.SetSelectedById(_markerTypeSelects[i]);

		if (_plotTypeSelectCombo.GetSelectedId() == MESH)
			_setAxisLimit = loadOBJ(_objPaths.GetStdStringNoZerosAt(i), _meshes[i], _mesheVertCounts[i], _meshesIndecies[i]);
		else if (_plotTypeSelectCombo.GetSelectedId() == SCATTER)
		{
			if (_coordinatesVecVecVec[i].empty())
				throw "Coordinates matrix should've been set by initNewitem(), but it's not!";

			for (int coo = 0; coo < 3; ++coo)
			{
				_coordinatesVecVecVec[i][coo].resize(_scatterCounts.GetValueAt(i));

				if(_coordinatesVecVecVec[i][coo].empty())
					throw "Empty coordinates vector!";
			}
		}
	}

	_active = elem["Active"];
}

MungPlex::Map3dView::Map3dView(const Map3dView& other)
{
	assign(other);
}

MungPlex::Map3dView& MungPlex::Map3dView::operator=(const Map3dView& other)
{
	assign(other);
	return *this;
}

MungPlex::Map3dView::Map3dView(Map3dView&& other) noexcept
{
	assign(other);
}

MungPlex::Map3dView& MungPlex::Map3dView::operator=(Map3dView&& other) noexcept
{
	assign(other);
	return *this;
}

void MungPlex::Map3dView::Draw()
{
	float itemWidth = ImGui::GetContentRegionAvail().x;
	float itemHeight = 40.0f * Settings::GetGeneralSettings().Scale;

	ImGui::BeginChild(std::string("child_Map3dView" + _idText).c_str(), ImVec2(itemWidth, itemHeight * 28.0f));
	{
		drawActiveCheckBox();
		ImGui::SameLine();

		DrawSetup(0.0f, 0.0f, IView::MAP3D); //only draws separator line
			
		drawGeneralSetup(itemWidth * 0.175f, itemHeight * 3.0f);
		ImGui::SameLine();

		drawPointerPathSetup(itemWidth * 0.425f, itemHeight * 3.0f);
			ImGui::SameLine();
		drawValueSetup(itemWidth * 0.4f, itemHeight * 3.0f);
	
		drawPlotArea(itemWidth, itemHeight * 25.0f);
	}
	ImGui::EndChild();

	if (_delete)
	{
		WatchControl::DeleteItem(_id);
		_delete = false;
	}
}

nlohmann::json MungPlex::Map3dView::GetJSON()
{
	nlohmann::json elemJson;
	elemJson["Type"] = IView::MAP3D;
	elemJson["Title"] = _labelInput.GetStdStringNoZeros();
	elemJson["Active"] = _active;
	elemJson["ClippingOn"] = _clippingOn;
	elemJson["MarkerSize"] = _markerSizeSlider.GetValue();
	elemJson["FlipAxes"] = _axesFlipFlags;
	elemJson["ItemCount"] = _itemSelectCombo.GetCount();
	nlohmann::json arrs;
	arrs["ItemTypes"] =  _itemSelectCombo.GetAllIds();
	arrs["RangeBeginnings"] = _rangeBeginnings.GetAll();
	arrs["RangeEnds"] = _rangeEnds.GetAll();
	arrs["Modules"] = _moduleInputs.GetAllStdStringNoZeros();
	arrs["PointerPaths"] = _pointerPathInputs.GetAllStdStringNoZeros();
	arrs["UseModule"] = _useModulePathVec;
	arrs["ObjPaths"] = _objPaths.GetAllStdStringNoZeros();
	arrs["FillColors"] = imVec4VecToStdVec(_fillColorVec);
	arrs["LineColors"] = imVec4VecToStdVec(_lineColorVec);
	arrs["MarkerColors"] = imVec4VecToStdVec(_markerColorVec);
	arrs["EnableColorPickers"] = _colorPickerEnablerVec;
	arrs["ScatterCounts"] = _scatterCounts.GetAll();
	arrs["ScatterOffset"] = _scatterOffsets.GetAll();
	arrs["PlotNames"] = _plotNames.GetAllStdStringNoZeros();
	arrs["MarkerOffsets"] = _markerOffset;
	arrs["Displacements"] = _coordinateDisplacements.GetAll();
	arrs["TrackLines"] = _trackLines;
	arrs["MarkerTypes"] = _markerTypeSelects;
	elemJson["Items"] = arrs;
	return elemJson;
}

void MungPlex::Map3dView::assign(const Map3dView& other)
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

	_items = other._items;
	_plotTypeSelectCombo = other._plotTypeSelectCombo;
	_itemSelectCombo = other._itemSelectCombo;
	_clippingOn = other._clippingOn;
	_markerSizeSlider = other._markerSizeSlider;
	_axesFlipFlags = other._axesFlipFlags;
	_rangeBeginnings = other._rangeBeginnings;
	_rangeEnds = other._rangeEnds;
	_moduleWVec = other._moduleWVec;
	_moduleInputs = other._moduleInputs;
	_pointerPathInputs = other._pointerPathInputs;
	_pointerPathVecVec = other._pointerPathVecVec;
	_useModulePathVec = other._useModulePathVec;
	_moduleAddressVec = other._moduleAddressVec;
	_objPaths = other._objPaths;
	_fillColorVec = other._fillColorVec;
	_lineColorVec = other._lineColorVec;
	_markerColorVec = other._markerColorVec;
	_meshes = other._meshes;
	_mesheVertCounts = other._mesheVertCounts;
	_meshesIndecies = other._meshesIndecies;
	_colorPickerEnablerVec = other._colorPickerEnablerVec;
	_coordinatesVecVecVec = other._coordinatesVecVecVec;
	_scatterCounts = other._scatterCounts;
	_scatterOffsets = other._scatterOffsets;
	_scatterColorVec = other._scatterColorVec;
	_plotNames = other._plotNames;
	_axisLimit = other._axisLimit;
	_setAxisLimit = other._setAxisLimit;
	_markerTypeSelectCombo = other._markerTypeSelectCombo;
	_markerOffset = other._markerOffset;
	_coordinateDisplacements = other._coordinateDisplacements;
	_trackLines = other._trackLines;
	_linesVecVecVecVec = other._linesVecVecVecVec;
	_frameCount = other._frameCount;
	_linePlotRotate = other._linePlotRotate;
	_linePlotNames = other._linePlotNames;
	_markerTypeSelects = other._markerTypeSelects;
}

void MungPlex::Map3dView::drawValueSetup(const float itemWidth, const float itemHeight, const int type)
{
	static uint64_t itemCount = 0;
	static uint64_t index = 0;
	static int32_t id = 0;
	itemCount = _itemSelectCombo.GetCount();

	manageProcessValueThread();

	ImGui::BeginChild("child_view", ImVec2(itemWidth, itemHeight), true);
	{
		
		if (itemCount == 0)
		{
			ImGui::EndChild();
			return;
		}

		index = _itemSelectCombo.GetSelectedIndex();
		id = _itemSelectCombo.GetSelectedId();

		switch (id)
		{
			case MESH:
			{
				_objPaths.Draw(0.45f, 0.4f);
				ImGui::SameLine();
				
				if(ImGui::Button("Load"))
				{
					_setAxisLimit = loadOBJ(_objPaths.GetStdStringNoZerosAt(index), _meshes[index], _mesheVertCounts[index], _meshesIndecies[index]);
				}

				ImGui::SameLine();

				_plotNames.Draw(1.0f, 0.3f);

				ImGui::Checkbox("Mesh Color", &_colorPickerEnablerVec[index][0]);

				if (_colorPickerEnablerVec[index][0])
				{
					ImGui::SameLine();
					ImGui::ColorEdit4("##MeshFillColor", (float*)&_fillColorVec[index]);
					ImPlot3D::SetNextFillStyle(_fillColorVec[index]);
				}

				ImGui::Checkbox("Line Color", &_colorPickerEnablerVec[index][1]);

				if (_colorPickerEnablerVec[index][1])
				{
					ImGui::SameLine();
					ImGui::ColorEdit4("##LineColor", (float*)&_lineColorVec[index]);
				}

				ImGui::Checkbox("Marker Color", &_colorPickerEnablerVec[index][2]);

				if (_colorPickerEnablerVec[index][2])
				{
					ImGui::SameLine();
					ImGui::ColorEdit4("##MarkerColor", (float*)&_markerColorVec[index]);
				}

			} break;
			case SCATTER:
			{
				if(_scatterCounts.Draw(0.33f, 0.3f))
				{
					if (_scatterCounts.GetSelectedValue() < 1)
						_scatterCounts.SetSelectedValue(1);

					resizeCoordinatesVec(index, _scatterCounts.GetSelectedValue());
					_trackLines[index] = false;
				}

				ImGui::SameLine();

				_scatterOffsets.Draw(0.5f, 0.3f, true);

				ImGui::SameLine();

				if(_plotNames.Draw(1.0f, 0.3f))
					_linePlotNames[index] = _plotNames.GetStdStringNoZerosAt(index) + " lines";

				if(_markerTypeSelectCombo.Draw(0.25f, 0.4f))
					_markerTypeSelects[index] = _markerTypeSelectCombo.GetSelectedId();

				ImGui::SameLine();
				ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
				ImGui::ColorEdit4("##MarkerColorScatter", (float*)&_markerColorVec[index]);
				ImGui::Text("Marker Offset:");
				ImGui::SameLine();
				ImGui::SliderFloat3("##Offset", (float*)_markerOffset[index].data(), -50.0f, 50.0f, "%.1f");
				_coordinateDisplacements.Draw(0.5f, 0.5f);

				static bool trackHistory = false;
				trackHistory = _trackLines[index];
				ImGui::SameLine();

				if (ImGui::Checkbox("Track History", &trackHistory))
				{
					_trackLines[index] = trackHistory;
					resizeLinesVec(index);
					_frameCount[index] = 0;
				}
			} break;
		}
	}
	ImGui::EndChild();
}

void MungPlex::Map3dView::drawPlotArea(const float itemWidth, const float itemHeight, const int type)
{
	
	ImGui::BeginChild("esdrftgf", ImGui::GetContentRegionAvail());

	if (ImPlot3D::BeginPlot("##plot twist", ImGui::GetContentRegionAvail(), _clippingOn ? 0 : ImPlot3DFlags_NoClip))
	{
		ImPlot3D::SetupAxes("--- X ---", "--- Y ---", "--- Z ---",  
			_axesFlipFlags[0] ? ImPlot3DAxisFlags_Invert : 0, 
			_axesFlipFlags[1] ? ImPlot3DAxisFlags_Invert : 0,
			_axesFlipFlags[2] ? ImPlot3DAxisFlags_Invert : 0);

		if (_setAxisLimit)
		{
			ImPlot3D::SetupAxesLimits(-_axisLimit, _axisLimit, -_axisLimit, _axisLimit, -_axisLimit, _axisLimit, ImPlot3DCond_Once);
			_setAxisLimit = false;
		}

		for(uint64_t i = 0; i < _itemSelectCombo.GetCount(); ++i)
		{
			switch (_itemSelectCombo.GetIdAt(i))
			{
			case MESH:
				if (_colorPickerEnablerVec[i][0])
					ImPlot3D::SetNextFillStyle(_fillColorVec[i]);
				else
					ImPlot3D::SetNextFillStyle(_colorDisable);

				if (_colorPickerEnablerVec[i][1])
					ImPlot3D::SetNextLineStyle(_lineColorVec[i]);
				else
					ImPlot3D::SetNextLineStyle(_colorDisable);

				if (_colorPickerEnablerVec[i][2])
					ImPlot3D::SetNextMarkerStyle(ImPlot3DMarker_Circle, 3, _markerColorVec[i], IMPLOT3D_AUTO, _markerColorVec[i]);
				else
					ImPlot3D::SetNextMarkerStyle(ImPlot3DMarker_Circle, 3, _colorDisable, IMPLOT3D_AUTO, _colorDisable);

				ImPlot3D::PlotMesh(_plotNames.GetCStringAt(i), _meshes[i].get(), reinterpret_cast<const uint32_t*>(_meshesIndecies[i].data()), _mesheVertCounts[i], _meshesIndecies[i].size());
				break;
			default:
				if (_coordinatesVecVecVec[i].size() != 3)
					break;

				if (_trackLines[i])
				{
					if (_frameCount[i] >= _maxFrames)
						_linePlotRotate[i] = true;

					for (uint64_t sCounts = 0; sCounts < _scatterCounts.GetValueAt(i); ++sCounts)
					{
						ImPlot3D::SetNextLineStyle(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), 2);
						ImPlot3D::PlotLine(_linePlotNames[i].c_str(), _linesVecVecVecVec[i][0][sCounts].data(), _linesVecVecVecVec[i][1][sCounts].data(), _linesVecVecVecVec[i][2][sCounts].data(), _frameCount[i], ImPlot3DLineFlags_None);
						
						if (_linePlotRotate[i])
						{
							ImPlot3D::SetNextLineStyle(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), 2);
							ImPlot3D::PlotLine(_linePlotNames[i].c_str(), _linesVecVecVecVec[i][0][sCounts].data() + _frameCount[i], _linesVecVecVecVec[i][1][sCounts].data() + _frameCount[i], _linesVecVecVecVec[i][2][sCounts].data() + _frameCount[i], _maxFrames - _frameCount[i], ImPlot3DLineFlags_None);
						}

						ImPlot3D::SetNextMarkerStyle(_markerTypeSelects[i], _markerSizeSlider.GetValue(), _markerColorVec[i], IMPLOT3D_AUTO, _markerColorVec[i]);
						ImPlot3D::PlotScatter(_plotNames.GetCStringAt(i), _coordinatesVecVecVec[i][0].data(), _coordinatesVecVecVec[i][1].data(), _coordinatesVecVecVec[i][2].data(), _scatterCounts.GetValueAt(i));
					}
				}

				ImPlot3D::SetNextMarkerStyle(_markerTypeSelects[i], _markerSizeSlider.GetValue(), _markerColorVec[i], IMPLOT3D_AUTO, _markerColorVec[i]);
				ImPlot3D::PlotScatter(_plotNames.GetCStringAt(i), _coordinatesVecVecVec[i][0].data(), _coordinatesVecVecVec[i][1].data(), _coordinatesVecVecVec[i][2].data(), _scatterCounts.GetValueAt(i));
			}
		} 
		ImPlot3D::EndPlot();
	}
	ImGui::EndChild();
}

void MungPlex::Map3dView::processValue()
{
	while (_processValueThreadFlag)
	{
		boost::this_thread::sleep_for(boost::chrono::milliseconds(13));

		for (uint64_t i = 0; i < _itemSelectCombo.GetCount(); ++i)
		{
			if (_itemSelectCombo.GetIdAt(i) == MESH)
				continue;

			if (_coordinatesVecVecVec[i].size() != 3)
				continue;

			if (_coordinatesVecVecVec[i][0].size() != _scatterCounts.GetValueAt(i))
				throw "Scatters improperly set!";

			static uint64_t addr = 0;
			addr = reinterpret_cast<uint64_t>(ProcessInformation::GetPointerFromPointerPathExpression(_pointerPathVecVec[i], _useModulePathVec[i], _moduleAddressVec[i]));

			if ((addr < _rangeMin || addr > _rangeMax) && _pointerPathVecVec[i].size() != 1)
				continue;

			for (uint64_t coo = 0; coo < 3; ++coo)
			{
				for (uint64_t sCounts = 0; sCounts < _scatterCounts.GetValueAt(i); ++sCounts)
				{
					_coordinatesVecVecVec[i][coo][sCounts] = ProcessInformation::ReadValue<float>(addr
						+ _scatterOffsets.GetValueAt(i) * sCounts								    //add next object's coordinate location
						+ coo * _coordinateDisplacements.GetValueAt(i))								//add displacement between coordinates
						+ _markerOffset[i][coo];													//add optional marker offset
				}
			}

			if (_trackLines[i])
			{
				if (_frameCount[i] >= _maxFrames)
					_frameCount[i] = 0;

				for (uint64_t coo = 0; coo < 3; ++coo)
				{
					for (uint64_t sCounts = 0; sCounts < _scatterCounts.GetValueAt(i); ++sCounts)
					{
						_linesVecVecVecVec[i][coo][sCounts][_frameCount[i]] = _coordinatesVecVecVec[i][coo][sCounts];
					}
				}

					++_frameCount[i];
			}
		}
	}
}

void MungPlex::Map3dView::manageProcessValueThread()
{
	if (_enableSignal || _disableSignal)
	{
		_processValueThreadFlag = false;

		if (_processValueThread.joinable())
			_processValueThread.detach();
	}

	if (_enableSignal)
	{
		_processValueThread = boost::thread(&MungPlex::Map3dView::processValue, this);
		_processValueThreadFlag = true;
	}

	_enableSignal = _disableSignal = false;
}

bool MungPlex::Map3dView::drawGeneralSetup(const float itemWidth, const float itemHeight)
{
	static bool res = false;

	ImGui::BeginChild("child_setup", ImVec2(itemWidth, itemHeight), true);
	{
		if (ImGui::Button("Add Layer"))
			initNewitem();

		ImGui::SameLine();

		_plotTypeSelectCombo.Draw(1.0f, 0.0f);

		if (_itemSelectCombo.Draw(0.7f, 0.4f))
			setItemIndices(_itemSelectCombo.GetSelectedIndex());

		ImGui::SameLine();
			
		if (_itemSelectCombo.GetCount() == 0) ImGui::BeginDisabled();
		{
			if (ImGui::Button("Delete"))
				deleteItem(_itemSelectCombo.GetSelectedIndex());
		}
		if (_itemSelectCombo.GetCount() == 0) ImGui::EndDisabled();

		_markerSizeSlider.Draw(0.5f, 0.0f);

		if (ImGui::IsItemActive() || ImGui::IsItemHovered())
			ImGui::SetTooltip("%.1f", _markerSizeSlider.GetValue());

		ImGui::SameLine();
		ImGui::Checkbox("Clipping", &_clippingOn);

		ImGui::Text("Flip:");
		static bool test = false;
		ImGui::SameLine();
		ImGui::Checkbox("X", &_axesFlipFlags[0]);
		ImGui::SameLine();
		ImGui::Checkbox("Y", &_axesFlipFlags[1]);
		ImGui::SameLine();
		ImGui::Checkbox("Z", &_axesFlipFlags[2]);
	}
	ImGui::EndChild();

	return res;
}

void MungPlex::Map3dView::drawPointerPathSetup(const float itemWidth, const float itemHeight)
{
	static bool disable = true;
	static uint64_t index = 0;
	static uint64_t itemCount = 0;
	static bool useModulePath = false;
	itemCount = _itemSelectCombo.GetCount();

	ImGui::BeginChild("child_pointer", ImVec2(itemWidth, itemHeight), true);

	if (itemCount == 0)
	{
		ImGui::EndChild();
		return;
	}

	disable = _itemSelectCombo.GetSelectedId() == MESH;
	index = _itemSelectCombo.GetSelectedIndex();
	useModulePath = _useModulePathVec[index]; //mind elements of bool vecs are not addressable

	if (disable) ImGui::BeginDisabled();
	{
		if (ImGui::Checkbox("Use Module", &useModulePath))
		{
			_useModulePathVec[index] = useModulePath;

			if (useModulePath)
				_moduleAddressVec[index] = ProcessInformation::GetModuleAddress<uint64_t>(_moduleWVec[index]);
		}

		ImGui::SameLine();
		if (!useModulePath) ImGui::BeginDisabled();
		_moduleInputs.Draw(1.0f, 0.0f);
		if (!useModulePath) ImGui::EndDisabled();

		if (_pointerPathInputs.Draw(1.0f, 0.3f))
			ParsePointerPath(_pointerPathVecVec[index], _pointerPathInputs.GetStdStringNoZerosAt(index));

		_rangeBeginnings.Draw(0.65f, 0.4f, true);
		ImGui::SameLine();
		_rangeEnds.Draw(1.f, 0.4f, true);
	}
	if (disable) ImGui::EndDisabled();
	ImGui::EndChild();
}

bool MungPlex::Map3dView::drawActiveCheckBox()
{
	static bool res = false;

	if (ImGui::Checkbox("##Active", &_active))
	{
		res = true;

		if (_active)
		{
			for (int i = 0; i < _pointerPathInputs.GetCount(); ++i)
			{
				ParsePointerPath(_pointerPathVecVec[i], _pointerPathInputs.GetStdStringNoZerosAt(i));

				if (_useModulePathVec[i])
					_moduleAddressVec[i] = ProcessInformation::GetModuleAddress<uint64_t>(_moduleWVec[i]);

				_linePlotRotate[i] = false;
			}
		}

		_enableSignal = _active;
		_disableSignal = !_enableSignal;
	}

	return res;
}

bool MungPlex::Map3dView::loadOBJ(const std::string& path, std::shared_ptr<ImPlot3DPoint>& mesh, uint32_t& vertCount, std::vector<uint32_t>& indices)
{
	std::ifstream file(path.c_str());
	vertCount = 0;
	int parseCount = 0;
	std::string line;

	if (!file.is_open())
	{
		std::cerr << "Failed to open file: " << path << std::endl;
		return false;
	}

	while (std::getline(file, line))
	{
		if (line.rfind("v ", 0) == 0) //find point
			vertCount++;
	}

	mesh = std::shared_ptr<ImPlot3DPoint>(new ImPlot3DPoint[vertCount], [](ImPlot3DPoint* p)
	    { delete[] p; } );

	line.clear();
	file.clear();
	file.seekg(0);

	while (std::getline(file, line))
	{
		if (line.rfind("v ", 0) == 0) //find point
		{
			std::istringstream iss(line.substr(2));
			static float x, y, z;

			if (iss >> x >> y >> z)
			{
				mesh.get()[parseCount].x = x;
				mesh.get()[parseCount].y = y;
				mesh.get()[parseCount].z = z;

				if (_axisLimit < std::abs(x))
					_axisLimit = std::abs(x);
				else if(_axisLimit < std::abs(y))
					_axisLimit = std::abs(y);
				else if (_axisLimit < std::abs(z))
					_axisLimit = std::abs(z);
			}

			++parseCount;
		}
		else if (line.rfind("f ", 0) == 0) //find index
		{
			std::istringstream iss(line.substr(2));
			static std::string vertex;

			while (iss >> vertex)
			{
				static uint64_t slashPos = 0;
				static uint32_t vertexIndex = 0;
				slashPos = vertex.find('/');
				vertexIndex = std::stoi(vertex.substr(0, slashPos)) - 1; // convert 1-based to 0-based
				indices.push_back(vertexIndex);
			}
		}
	}
	
	file.close();
	return true;
}

void MungPlex::Map3dView::initNewitem()
{
	int plotTypeId = _plotTypeSelectCombo.GetSelectedId();
	_items.emplace_back("", plotTypeId);

	for (int i = 0; i < _items.size(); ++i)
		_items[i].first = std::to_string(i + 1);

	_itemSelectCombo.SetItems(_items, true);
	_moduleWVec.push_back(std::wstring(128, L'\0'));
	_moduleInputs.PushBack("");
	_rangeBeginnings.PushBack(0);
	_rangeEnds.PushBack(0);
	_pointerPathInputs.PushBack("");
	_pointerPathVecVec.push_back({});
	_useModulePathVec.push_back(false);
	_moduleAddressVec.push_back(0);
	_objPaths.PushBack("");
	_fillColorVec.push_back(_defaultMeshFaceColor);
	_lineColorVec.push_back(_defaultMeshLineColor);
	_markerColorVec.push_back(_defaultMeshMarkerColor);
	_meshes.push_back(std::shared_ptr<ImPlot3DPoint>(new ImPlot3DPoint[42], [](ImPlot3DPoint* p) {
		delete[] p;
		}));
	_mesheVertCounts.push_back(0);
	_meshesIndecies.push_back({});
	_colorPickerEnablerVec.push_back({ true, false, false });
	_coordinatesVecVecVec.push_back({ { 0.0f }, { 0.0f }, { 0.0f } });
	_scatterCounts.PushBack(1);
	_scatterOffsets.PushBack(0);
	_scatterColorVec.push_back({});
	_plotNames.PushBack(std::string("plot " + std::to_string(_plotNames.GetCount() + 1)));
	_markerTypeSelects.push_back(0);
	_markerOffset.push_back({ 0.0f, 0.0f, 0.0f });
	_coordinateDisplacements.PushBack(4);
	setItemIndices(_itemSelectCombo.GetSelectedIndex());
	_trackLines.push_back(false);
	_linesVecVecVecVec.push_back({ {{}}, {{}}, {{}} });
	_frameCount.push_back(0);
	_linePlotRotate.push_back(false);
	_linePlotNames.push_back("plot " + std::to_string(_plotNames.GetCount()) + " lines");
}

void MungPlex::Map3dView::resizeCoordinatesVec(const uint64_t index, const uint64_t count)
{
	if (count == 0)
		throw "Error: cannot resize to 0!";

	if (_coordinatesVecVecVec[index][0].size() < count)
	{
		_coordinatesVecVecVec[index][0].resize(count);
		_coordinatesVecVecVec[index][1].resize(count);
		_coordinatesVecVecVec[index][2].resize(count);
		return;
	}

	for (uint64_t i = _coordinatesVecVecVec[index][0].size(); i < count; ++i)
	{
		_coordinatesVecVecVec[index][0].push_back(0.0f);
		_coordinatesVecVecVec[index][1].push_back(0.0f);
		_coordinatesVecVecVec[index][2].push_back(0.0f);
	}
}

void MungPlex::Map3dView::setItemIndices(const uint64_t index)
{
	_coordinateDisplacements.SelectByIndex(index);
	_scatterCounts.SelectByIndex(index);
	_scatterOffsets.SelectByIndex(index);
	_rangeBeginnings.SelectByIndex(index);
	_rangeEnds.SelectByIndex(index);
	_moduleInputs.SelectByIndex(index);
	_pointerPathInputs.SelectByIndex(index);
	_objPaths.SelectByIndex(index);
	_plotNames.SelectByIndex(index);
	_markerTypeSelectCombo.SetSelectedByIndex(index);
}

void MungPlex::Map3dView::resizeLinesVec(const uint64_t index)
{
	_linesVecVecVecVec[index].clear();

	if (_trackLines[index])
	{
		for (uint64_t coords = 0; coords < 3; ++coords)
		{
			_linesVecVecVecVec[index].push_back({});
			_linesVecVecVecVec[index].back().resize(_scatterCounts.GetValueAt(index));

			for (uint64_t sCount = 0; sCount < _scatterCounts.GetValueAt(index); ++sCount)
			{
				_linesVecVecVecVec[index][coords][sCount].push_back({});
				_linesVecVecVecVec[index][coords][sCount].resize(_maxFrames);
			}
		}
	}
}

std::vector<std::vector<float>> MungPlex::Map3dView::imVec4VecToStdVec(const std::vector<ImVec4>& vec4)
{
	std::vector<std::vector<float>> ret;

	for (auto& temp : vec4)
		ret.push_back({ temp.x, temp.y, temp.z, temp.w });

	return ret;
}

void MungPlex::Map3dView::deleteItem(const uint64_t index)
{
	_items.erase(_items.begin() + index);
	_itemSelectCombo.DeleteItemAt(index);
	_rangeBeginnings.DeleteItemAt(index);
	_rangeEnds.DeleteItemAt(index);
	_moduleWVec.erase(_moduleWVec.begin() + index);
	_moduleInputs.DeleteItemAt(index);
	_pointerPathInputs.DeleteItemAt(index);
	_pointerPathVecVec.erase(_pointerPathVecVec.begin() + index);
	_useModulePathVec.erase(_useModulePathVec.begin() + index);
	_moduleAddressVec.erase(_moduleAddressVec.begin() + index);
	_objPaths.DeleteItemAt(index);
	_fillColorVec.erase(_fillColorVec.begin() + index);
	_lineColorVec.erase(_lineColorVec.begin() + index);
	_markerColorVec.erase(_markerColorVec.begin() + index);
	_meshes.erase(_meshes.begin() + index);
	_mesheVertCounts.erase(_mesheVertCounts.begin() + index);
	_meshesIndecies.erase(_meshesIndecies.begin() + index);
	_colorPickerEnablerVec.erase(_colorPickerEnablerVec.begin() + index);
	_coordinatesVecVecVec.erase(_coordinatesVecVecVec.begin() + index);
	_scatterCounts.DeleteItemAt(index);
	_scatterOffsets.DeleteItemAt(index);
	_scatterColorVec.erase(_scatterColorVec.begin() + index);
	_plotNames.DeleteItemAt(index);
	_markerTypeSelectCombo.DeleteItemAt(index);
	_markerTypeSelects.erase(_markerTypeSelects.begin() + index);
	_markerOffset.erase(_markerOffset.begin() + index);
	_coordinateDisplacements.DeleteItemAt(index);
	_trackLines.erase(_trackLines.begin() + index);
	_linesVecVecVecVec.erase(_linesVecVecVecVec.begin() + index);
	_frameCount.erase(_frameCount.begin() + index);
	_linePlotRotate.erase(_linePlotRotate.begin() + index);
	_linePlotNames.erase(_linePlotNames.begin() + index);

	for (int i = 0; i < _items.size(); ++i)
		_items[i].first = std::to_string(i + 1);
}