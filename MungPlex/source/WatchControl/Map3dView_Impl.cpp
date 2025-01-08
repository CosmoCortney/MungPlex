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
}

MungPlex::Map3dView::Map3dView(const int id, const nlohmann::json elem)
{
	_id = id;
	_idText = std::to_string(id);
	SetBasicMembers(elem);
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

nlohmann::json MungPlex::Map3dView::GetJSON()
{
	nlohmann::json elemJson = GetBasicJSON();

	elemJson["Type"] = IView::MAP3D;

	return elemJson;
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

	_moduleWVec = other._moduleWVec;
	_moduleInputs = other._moduleInputs;
	_pointerPathInputs = other._pointerPathInputs;
	_pointerPathVecVec = other._pointerPathVecVec;
	_useModulePathVec = other._useModulePathVec;
	_moduleAddressVec = other._moduleAddressVec;
	_rangeBeginnings = other._rangeBeginnings;
	_rangeEnds = other._rangeEnds;
	_coordinatesVecVecVec = other._coordinatesVecVecVec;
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

				_markerTypeSelectCombo[index].Draw(0.25f, 0.4f);
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
			ImPlot3D::SetupAxesLimits(-_axisLimit, _axisLimit, -_axisLimit, _axisLimit, -_axisLimit, _axisLimit, ImPlot3DCond_Always);
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

						ImPlot3D::SetNextMarkerStyle(_markerTypeSelectCombo[i].GetSelectedId(), _markerSize, _markerColorVec[i], IMPLOT3D_AUTO, _markerColorVec[i]);
						ImPlot3D::PlotScatter(_plotNames.GetCStringAt(i), _coordinatesVecVecVec[i][0].data(), _coordinatesVecVecVec[i][1].data(), _coordinatesVecVecVec[i][2].data(), _scatterCounts.GetValueAt(i));
					}
				}

				ImPlot3D::SetNextMarkerStyle(_markerTypeSelectCombo[i].GetSelectedId(), _markerSize, _markerColorVec[i], IMPLOT3D_AUTO, _markerColorVec[i]);
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
			
		if (ImGui::Button("Delete"))
		{

		}

		SetUpSliderFloat("Marker Size", &_markerSize, 1.0f, 15.0f, "Marker Size", 0.5f, 0.0f, false);

		if (ImGui::IsItemActive() || ImGui::IsItemHovered())
			ImGui::SetTooltip("%.1f", _markerSize);

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

	useModulePath = _useModulePathVec[index]; //mind elements of bool vecs are not addressable
	disable = _itemSelectCombo.GetSelectedId() == MESH;
	index = _itemSelectCombo.GetSelectedIndex();

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
	_items.emplace_back(std::to_string(_items.size() + 1), plotTypeId);
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
	_markerTypeSelectCombo.emplace_back("Marker Type:", false, _markerTypes);
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
