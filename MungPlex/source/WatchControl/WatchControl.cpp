#include "WatchControl.hpp"
#include <algorithm>
#include "../../Helperfunctions.hpp"
#include "../../ProcessInformation.hpp"
#include "../../Settings.hpp"



void MungPlex::WatchControl::DrawWindow()
{
	static bool stateSet = false;

	if (ImGui::Begin("Watch & Control"))
	{
		if (!Connection::IsConnected())
			ImGui::BeginDisabled();
		else
		{
			if (!stateSet && Settings::GetGeneralSettings().EnableRichPresence && ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows))
			{
				Connection::GetDiscordRichPresence().SetRichPresenceState("Value Watch & Control");
				stateSet = true;
			}
		}

		GetInstance().drawList();

		if (!Connection::IsConnected())
			ImGui::EndDisabled();
	}
	else
		stateSet = false;

	if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows))
		stateSet = false;

	ImGui::End();
}

bool MungPlex::WatchControl::saveList()
{
	std::ofstream file(_currentFile, std::ios::binary);
	const bool isOpen = file.is_open();

	if (isOpen)
	{
		nlohmann::json jsonData;

		if (!_views.empty())
		{
			for (int i = 0; i < _views.size(); ++i)
			{
				nlohmann::json watchJson;

				switch (_views[i].first)
				{
				case IView::FLOAT:
					jsonData["Watchlist"].emplace_back(std::get<FloatView>(_views[i].second).GetJSON());
				break;
				case IView::BOOL:
					jsonData["Watchlist"].emplace_back(std::get<BoolView>(_views[i].second).GetJSON());
				break;
				case IView::MOUSEPIANO:
					jsonData["Watchlist"].emplace_back(std::get<MousePiano>(_views[i].second).GetJSON());
					break;
				default: //IView::INTEGRAL
					jsonData["Watchlist"].emplace_back(std::get<IntegralView>(_views[i].second).GetJSON());
				}
			}
		}
		else
		{
			jsonData["Watchlist"].emplace_back("");
		}

		file << "\xEF\xBB\xBF"; //write BOM
		file << jsonData.dump(2);
	}

	return isOpen;
}

void MungPlex::WatchControl::InitWatchFile()
{
	GetInstance()._currentFile = MT::Convert<char*, std::wstring>(Settings::GetGeneralSettings().DocumentsPath, MT::UTF8, MT::UTF16LE) + L"\\MungPlex\\WatchControl\\"
		+ MT::Convert<std::string, std::wstring>(ProcessInformation::GetPlatform(), MT::UTF8, MT::UTF16LE) + L"\\"
		+ MT::Convert<std::string, std::wstring>(ProcessInformation::GetGameID(), MT::UTF8, MT::UTF16LE) + L".json";

	if (!std::filesystem::exists(GetInstance()._currentFile))
	{
		std::ofstream file(GetInstance()._currentFile, std::ios::binary);

		if (file.is_open())
		{
			file << "\xEF\xBB\xBF"; //write BOM
			file << GetInstance()._placeholderFile;
		}
	}

	GetInstance()._views.clear();
	GetInstance()._ids.clear();

	std::string jsonstr;
	{
		std::ifstream inFile;
		inFile.open(GetInstance()._currentFile);

		if (inFile)
		{
			std::string buffer;
			while (std::getline(inFile, buffer))
			{
				jsonstr.append(buffer).append("\n");
			}
		}
	}

	try
	{
		auto doc = nlohmann::json::parse(jsonstr);
		auto& watchList = doc["Watchlist"];

		for (int i = 0; i < watchList.size(); ++i)
		{
			int type = watchList[i]["Type"];
			GetInstance()._ids.push_back(i);

			switch (type)
			{
			case IView::FLOAT:
				GetInstance()._views.emplace_back(IView::FLOAT, FloatView(i, watchList[i]));
				break;
			case IView::BOOL:
				GetInstance()._views.emplace_back(IView::BOOL, BoolView(i, watchList[i]));
				break;
			case IView::MOUSEPIANO:
				GetInstance()._views.emplace_back(IView::MOUSEPIANO, MousePiano(i, watchList[i]));
				break;
			default: //IView::INTEGRAL
				GetInstance()._views.emplace_back(IView::INTEGRAL, IntegralView(i, watchList[i]));
			}
		}
	}
	catch (const nlohmann::json::parse_error& exception)
	{
		std::cerr << "Failed parsing Watch Element: " << exception.what() << std::endl;
	}
}

void MungPlex::WatchControl::drawList()
{
	ImGui::BeginChild("Watch List");
	{
		static int typeSelect = 0;

		SetUpCombo("New Item's Type:", IView::s_SuperiorTypes, typeSelect);
		ImGui::SameLine();

		if (ImGui::Button("Add Item"))
		{
			_ids.push_back(_views.size());

			for (int i = 0; i < _ids.size()-1; ++i)
			{
				if (_ids.back() == _ids[i])
				{
					++_ids.back();
					i = 0;
				}
			}

			switch (IView::s_SuperiorTypes[typeSelect].second)
			{
			case IView::FLOAT:
				_views.emplace_back(IView::FLOAT, FloatView(_ids.back()));
				break;
			case IView::BOOL:
				_views.emplace_back(IView::BOOL, BoolView(_ids.back()));
				break;
			case IView::MOUSEPIANO:
				_views.emplace_back(IView::MOUSEPIANO, MousePiano(_ids.back()));
				break;
			default: //IView::INTEGRAL
				_views.emplace_back(IView::INTEGRAL, IntegralView(_ids.back()));
			}
		}

		ImGui::SameLine();

		if (ImGui::Button("Save List"))
			GetInstance().saveList();

		for (int i = 0; i < _views.size(); ++i)
		{
			switch (_views[i].first)
			{
			case IView::FLOAT:
				std::get<FloatView>(_views[i].second).Draw();
				break;
			case IView::BOOL:
				std::get<BoolView>(_views[i].second).Draw();
				break;
			case IView::MOUSEPIANO:
				std::get<MousePiano>(_views[i].second).Draw();
				break;
			default: //IView::INTEGRAL
				std::get<IntegralView>(_views[i].second).Draw();
			}
		}
	}
	ImGui::EndChild();
}

void MungPlex::WatchControl::DeleteItem(const int id)
{
	auto& views = GetInstance()._views;

	for (int i = 0; i < views.size(); ++i)
	{
		switch (views[i].first)
		{
		case IView::FLOAT:
			if(std::get<FloatView>(views[i].second).GetID() == id)
			{
				views.erase(views.begin() + i);
				return;
			}
			break;
		case IView::BOOL:
			if (std::get<BoolView>(views[i].second).GetID() == id)
			{
				views.erase(views.begin() + i);
				return;
			}
			break;
		case IView::MOUSEPIANO:
			if (std::get<MousePiano>(views[i].second).GetID() == id)
			{
				views.erase(views.begin() + i);
				return;
			}
			break;
		default: //IView::INTEGRAL
			if (std::get<IntegralView>(views[i].second).GetID() == id)
			{
				views.erase(views.begin() + i);
				return;
			}
		}
	}
}

MungPlex::WatchControl::IntegralView::IntegralView(const int id)
{
	_id = id;
	_idText = std::to_string(id);
	_formatPlot = "%lld/%lld";
	_plotVals.resize(_plotCount);
}

MungPlex::WatchControl::FloatView::FloatView(const int id)
{
	_id = id;
	_idText = std::to_string(id);
	_plotVals.resize(_plotCount);
}

MungPlex::WatchControl::BoolView::BoolView(const int id)
{
	_id = id;
	_idText = std::to_string(id);
}

void MungPlex::WatchControl::MousePiano::assignCheckBoxIDs()
{
	for (int i = 0; i < 32; ++i)
	{
		_checkBoxIDs[i] = "##cb" + std::to_string(i);
		_checkBoxTextIDs[i] = "##cbt" + std::to_string(i);
	}
}

MungPlex::WatchControl::MousePiano::MousePiano(const int id)
{
	_id = id;
	_idText = std::to_string(id);
	_switches.resize(32);

	for (auto& elem : _switches)
		elem.first.resize(32);

	assignCheckBoxIDs();
}

MungPlex::WatchControl::IntegralView::IntegralView(const int id, const nlohmann::json elem)
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

MungPlex::WatchControl::FloatView::FloatView(const int id, const nlohmann::json elem)
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

MungPlex::WatchControl::BoolView::BoolView(const int id, const nlohmann::json elem)
{
	_id = id;
	_idText = std::to_string(id);
	SetBasicMembers(elem);
	_val = elem["Value"];
}

MungPlex::WatchControl::MousePiano::MousePiano(const int id, const nlohmann::json elem)
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

void MungPlex::WatchControl::IntegralView::Draw()
{
	float itemWidth = ImGui::GetContentRegionAvail().x;
	float itemHeight = 80.0f * Settings::GetGeneralSettings().Scale;

	ImGui::BeginChild(std::string("child_integralView" + _idText).c_str(), ImVec2(itemWidth, itemHeight * 2.0f));
	{
		DrawSetup(itemWidth, itemHeight, IView::INTEGRAL);

		ImGui::SameLine();

		ImGui::BeginChild("child_view", ImVec2(itemWidth * 0.5f, itemHeight * 1.5f), true);
		{
			static std::string format(8, '\0');
			
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

				std::rotate(_plotVals.begin(), _plotVals.begin() + 1, _plotVals.end());
				_plotVals.back() = static_cast<float>(_val);
			}

			if (_hex)
			{
				switch (_typeSelect)
				{
				case ImGuiDataType_S8: case ImGuiDataType_U8:
					format = "%02X";
					break;
				case ImGuiDataType_S16: case ImGuiDataType_U16:
					format = "%04X";
					break;
				case ImGuiDataType_S32: case ImGuiDataType_U32:
					format = "%08X";
					break;
				default:
					format = "%016X";
				}
			}
			else
			{
				switch (_typeSelect)
				{
				case ImGuiDataType_S8: case ImGuiDataType_S16: case ImGuiDataType_S32: case ImGuiDataType_S64:
					format = "%i";
					break;
				default:
					format = "%u";
				}
			}

			if (ImGui::Checkbox("Hex", &_hex))
				_formatPlot = _hex ? "%llX/%llX" : "%lld/%lld";

			ImGui::SameLine();

			ImGui::SetNextItemWidth(itemWidth * 0.15f);
			ImGui::InputScalar("##value", _typeSelect, &_val, NULL, NULL, format.c_str());

			ImGui::SameLine();

			ImGui::Text("Plot Range:");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(itemWidth * 0.1f);


			if (ImGui::InputScalar("##plot range start", _typeSelect, &_plotMin, NULL, NULL, format.c_str()))
				_plotMinF = static_cast<float>(_plotMin);
			
			ImGui::SameLine();
			ImGui::Text(" - ");
			ImGui::SameLine();

			if(ImGui::InputScalar("##plot range end", _typeSelect, &_plotMax, NULL, NULL, format.c_str()))
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
				ImGui::SliderScalar("##writeval", _typeSelect, &_val, &_plotMin, &_plotMax, format.c_str());
			else
			ImGui::ProgressBar(progVal, ImVec2(0.0f, 0.0f), _plotBuf.data());
		}
		ImGui::EndChild();

	}
	ImGui::EndChild();

	if(_delete)
		WatchControl::DeleteItem(_id);
}

void MungPlex::WatchControl::FloatView::Draw()
{
	float itemWidth = ImGui::GetContentRegionAvail().x;
	float itemHeight = 80.0f * Settings::GetGeneralSettings().Scale;

	ImGui::BeginChild(std::string("child_floatView" + _idText).c_str(), ImVec2(itemWidth, itemHeight * 2.0f));
	{
		DrawSetup(itemWidth, itemHeight, IView::FLOAT);

		ImGui::SameLine();

		ImGui::BeginChild("child_view", ImVec2(itemWidth * 0.5f, itemHeight * 1.5f), true);
		{
			std::string format(8, '\0');
			if (_active)
			{
				uint64_t valptr = reinterpret_cast<uint64_t>(ProcessInformation::GetPointerFromPointerPathExpression(_pointerPath, _useModulePath, _moduleAddress));

				if ((valptr >= _rangeMin && valptr < _rangeMax) || _pointerPath.size() == 1)
				{
					if (_freeze)
					{
						switch (s_FloatTypes[_typeSelect].second)
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
						switch (s_FloatTypes[_typeSelect].second)
						{
						case ImGuiDataType_Double:
							_val = ProcessInformation::ReadValue<double>(valptr);
							break;
						default: // float
							_val = static_cast<double>(ProcessInformation::ReadValue<float>(valptr));
						}
					}
				}

				std::rotate(_plotVals.begin(), _plotVals.begin() + 1, _plotVals.end());
				_plotVals.back() = static_cast<float>(_val);
			}

			ImGui::SameLine();

			ImGui::SetNextItemWidth(itemWidth * 0.15f);
			ImGui::InputDouble("##value", &_val, NULL, NULL, "%.6f");

			ImGui::SameLine();

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
	ImGui::EndChild();

	if (_delete)
		WatchControl::DeleteItem(_id);
}

void MungPlex::WatchControl::BoolView::Draw()
{
	float itemWidth = ImGui::GetContentRegionAvail().x;
	float itemHeight = 80.0f * Settings::GetGeneralSettings().Scale;

	ImGui::BeginChild(std::string("child_boolView" + _idText).c_str(), ImVec2(itemWidth, itemHeight * 2.0f));
	{
		DrawSetup(itemWidth, itemHeight, IView::BOOL);

		ImGui::SameLine();

		ImGui::BeginChild("child_view", ImVec2(itemWidth * 0.5f, itemHeight * 1.5f), true);
		{
			std::string format(8, '\0');
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
		ImGui::EndChild();

	}
	ImGui::EndChild();

	if (_delete)
		WatchControl::DeleteItem(_id);
}

void MungPlex::WatchControl::MousePiano::Draw()
{
	float itemWidth = ImGui::GetContentRegionAvail().x;
	float itemHeight = 80.0f * Settings::GetGeneralSettings().Scale;

	ImGui::BeginChild(std::string("child_mousePianoView" + _idText).c_str(), ImVec2(itemWidth, itemHeight * 2.0f));
	{
		DrawSetup(itemWidth, itemHeight, IView::MOUSEPIANO);

		ImGui::SameLine();

		ImGui::BeginChild("child_view", ImVec2(itemWidth * 0.5f, itemHeight * 1.5f), true);
		{
			std::string format(8, '\0');
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
	ImGui::EndChild();

	if (_delete)
		WatchControl::DeleteItem(_id);
}

nlohmann::json MungPlex::WatchControl::IntegralView::GetJSON()
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

nlohmann::json MungPlex::WatchControl::FloatView::GetJSON()
{
	nlohmann::json elemJson = GetBasicJSON();

	elemJson["Type"] = IView::FLOAT;
	elemJson["FloatType"] = _typeSelect;
	elemJson["Value"] = _val;
	elemJson["PlotMin"] = _plotMin;
	elemJson["PlotMax"] = _plotMax;

	return elemJson;
}

nlohmann::json MungPlex::WatchControl::BoolView::GetJSON()
{
	nlohmann::json elemJson = GetBasicJSON();

	elemJson["Type"] = IView::BOOL;
	elemJson["Value"] = _val;

	return elemJson;
}

nlohmann::json MungPlex::WatchControl::MousePiano::GetJSON()
{
	nlohmann::json elemJson = GetBasicJSON();

	elemJson["Type"] = IView::MOUSEPIANO;

	for(int i = 0; i < 32; ++i)
		elemJson["Switches"].emplace_back(std::pair<std::string, bool>(_switches[i].first.c_str(), _switches[i].second));

	return elemJson;
}