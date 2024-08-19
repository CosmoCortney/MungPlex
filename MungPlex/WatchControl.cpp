#include "WatchControl.hpp"
#include <algorithm>
#include "Helperfunctions.hpp"
#include "ProcessInformation.hpp"
#include "Settings.hpp"

const std::vector<std::pair<std::string, int>> MungPlex::WatchControl::View::s_IntTypes = 
{
	{ "Int 8", ImGuiDataType_S8 },
	{ "UInt 8", ImGuiDataType_U8 },
	{ "Int 16", ImGuiDataType_S16 },
	{ "UInt 16", ImGuiDataType_U16 },
	{ "Int 32", ImGuiDataType_S32 },
	{ "UInt 32", ImGuiDataType_U32 },
	{ "Int 64", ImGuiDataType_S64 },
	{ "UInt 64", ImGuiDataType_U64 }
};

const std::vector<std::pair<std::string, int>> MungPlex::WatchControl::View::s_FloatTypes =
{
	{ "Float", ImGuiDataType_Float },
	{ "Double", ImGuiDataType_Double }
};

const std::vector<std::pair<std::string, int>> MungPlex::WatchControl::View::s_SuperiorTypes =
{
	{ "Integral", INTEGRAL },
	{ "Floats", FLOAT },
	{ "Bool", BOOL },
	{ "DIP Switch", MOUSEPIANO }
};

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
				Connection::SetRichPresenceState("Value Watch & Control");
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
				case FLOAT:
					jsonData["Watchlist"].emplace_back(std::get<FloatView>(_views[i].second).GetJSON());
				break;
				case BOOL:
					jsonData["Watchlist"].emplace_back(std::get<BoolView>(_views[i].second).GetJSON());
				break;
				case MOUSEPIANO:
					jsonData["Watchlist"].emplace_back(std::get<MousePiano>(_views[i].second).GetJSON());
					break;
				default: //INTEGRAL
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
			case FLOAT:
				GetInstance()._views.emplace_back(FLOAT, FloatView(i, watchList[i]));
				break;
			case BOOL:
				GetInstance()._views.emplace_back(BOOL, BoolView(i, watchList[i]));
				break;
			case MOUSEPIANO:
				GetInstance()._views.emplace_back(MOUSEPIANO, MousePiano(i, watchList[i]));
				break;
			default: //INTEGRAL
				GetInstance()._views.emplace_back(INTEGRAL, IntegralView(i, watchList[i]));
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

		SetUpCombo("New Item's Type:", View::s_SuperiorTypes, typeSelect);
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

			switch (View::s_SuperiorTypes[typeSelect].second)
			{
			case FLOAT:
				_views.emplace_back(FLOAT, FloatView(_ids.back()));
				break;
			case BOOL:
				_views.emplace_back(BOOL, BoolView(_ids.back()));
				break;
			case MOUSEPIANO:
				_views.emplace_back(MOUSEPIANO, MousePiano(_ids.back()));
				break;
			default: //INTEGRAL
				_views.emplace_back(INTEGRAL, IntegralView(_ids.back()));
			}
		}

		ImGui::SameLine();

		if (ImGui::Button("Save List"))
			GetInstance().saveList();

		for (int i = 0; i < _views.size(); ++i)
		{
			switch (_views[i].first)
			{
			case FLOAT:
				std::get<FloatView>(_views[i].second).Draw();
				break;
			case BOOL:
				std::get<BoolView>(_views[i].second).Draw();
				break;
			case MOUSEPIANO:
				std::get<MousePiano>(_views[i].second).Draw();
				break;
			default: //INTEGRAL
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
		case FLOAT:
			if(std::get<FloatView>(views[i].second).GetID() == id)
			{
				views.erase(views.begin() + i);
				return;
			}
			break;
		case BOOL:
			if (std::get<BoolView>(views[i].second).GetID() == id)
			{
				views.erase(views.begin() + i);
				return;
			}
			break;
		case MOUSEPIANO:
			if (std::get<MousePiano>(views[i].second).GetID() == id)
			{
				views.erase(views.begin() + i);
				return;
			}
			break;
		default: //INTEGRAL
			if (std::get<IntegralView>(views[i].second).GetID() == id)
			{
				views.erase(views.begin() + i);
				return;
			}
		}
	}
}

int MungPlex::WatchControl::View::GetID()
{
	return _id;
}

void MungPlex::WatchControl::View::DrawSetup(const float itemWidth, const float itemHeight, const int type)
{
	SetUpInputText("Title:", _label.data(), _label.size(), 0.25f, 0.2f);

	ImGui::BeginChild("child_Setup", ImVec2(itemWidth * 0.5f, itemHeight * 1.5f));
	{
		ImGui::BeginChild("child_setup", ImVec2(itemWidth * 0.15f, itemHeight * 1.5f), true);
		{
			if (ImGui::Checkbox("Active", &_active))
				if (_active && _useModulePath)
					_moduleAddress = ProcessInformation::GetModuleAddress<uint64_t>(_moduleW);

			switch (type)
			{
			case FLOAT:
				SetUpCombo("Float Type:", s_FloatTypes, _typeSelect, 1.0f, 0.5f);
			break;
			case INTEGRAL:
				SetUpCombo("Int Type:", s_IntTypes, _typeSelect, 1.0f, 0.5f);
			break;
			}

			if (ImGui::Button("Delete"))
			{
				_delete = true;
			}

			ImGui::SameLine();
			ImGui::Checkbox("Write", &_freeze);
		}
		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginChild("child_pointer", ImVec2(itemWidth * 0.345f, itemHeight * 1.5f), true);
		{
			if (ImGui::Checkbox("Use Module", &_useModulePath))
				if(_useModulePath)
					_moduleAddress = ProcessInformation::GetModuleAddress<uint64_t>(_moduleW);

			ImGui::SameLine();
			if (!_useModulePath) ImGui::BeginDisabled();
			SetUpInputText("Module", _module.data(), _module.size(), 1.0f, 0.0f, false);
			if (!_useModulePath) ImGui::EndDisabled();

			if (SetUpInputText("Pointer Path:", _pointerPathText.data(), _pointerPathText.size(), 1.0f, 0.3f))
			{
				_pointerPath.clear();
				std::string line;

				if (_pointerPathText.find(',') == std::string::npos)
				{
					line = RemoveSpacePadding(_pointerPathText, true);

					if (!line.empty())
						if (line.front() != '\0')
							if (IsValidHexString(line))
								_pointerPath.push_back(stoll(line, 0, 16));
				}
				else
				{
					std::stringstream stream;
					stream << _pointerPathText;

					while (std::getline(stream, line, ','))
					{
						line = RemoveSpacePadding(line, true);

						if (line.empty())
							break;

						if (line.front() == '\0')
							break;

						if (!IsValidHexString(line))
							break;

						_pointerPath.push_back(stoll(line, 0, 16));
					}
				}
			}

			int addrType = ProcessInformation::GetAddressWidth() == 8 ? ImGuiDataType_U64 : ImGuiDataType_U32;
			std::string format = ProcessInformation::GetAddressWidth() == 8 ? "%016X" : "%08X";
			ImGui::Text("Target Addr Range:");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(itemWidth * 0.1f);
			ImGui::InputScalar("##Target addr range:", addrType, &_rangeMin, NULL, NULL, format.c_str());
			ImGui::SameLine();
			ImGui::Text(" - ");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 2.0f);
			ImGui::InputScalar("## - ", addrType, &_rangeMax, NULL, NULL, format.c_str());
		}
		ImGui::EndChild();
	}
	ImGui::EndChild();
}

nlohmann::json MungPlex::WatchControl::View::GetBasicJSON()
{
	nlohmann::json elemJson;

	elemJson["Title"] = _label.c_str();
	elemJson["Module"] = _module.c_str();
	elemJson["UseModule"] = _useModulePath;
	elemJson["WriteOn"] = _freeze;
	elemJson["Active"] = _active;
	elemJson["PointerMin"] = _rangeMin;
	elemJson["PointerMax"] = _rangeMax;

	for (int64_t ptr : _pointerPath)
		elemJson["PointerPath"].push_back(ptr);

	return elemJson;
}

void MungPlex::WatchControl::View::SetBasicMembers(const nlohmann::json elem)
{
	_label = elem["Title"];
	_module = elem["Module"];
	_moduleW.resize(32);
	_moduleW = MT::Convert<std::string, std::wstring>(_module, MT::UTF8, MT::UTF16LE);
	_moduleW.resize(32);
	_useModulePath = elem["UseModule"];
	_freeze = elem["WriteOn"];
	_active = elem["Active"];
	_rangeMin = elem["PointerMin"];
	_rangeMax = elem["PointerMax"];
	_pointerPathText = "";

	for (int i = 0; i < elem["PointerPath"].size(); ++i)
	{
		int64_t ptr = elem["PointerPath"][i];
		_pointerPath.push_back(ptr);
		std::stringstream stream;
		stream << std::uppercase << std::hex << ptr;
		_pointerPathText.append(stream.str());

		if (i < elem["PointerPath"].size() - 1)
			_pointerPathText.append(", ");
	}

	_pointerPathText.resize(128);
}

void* MungPlex::WatchControl::View::GetCurrentPointer()
{
	if (_pointerPath.empty())
		return 0;

	int64_t ptr = 0;

	if (_useModulePath)
		ptr = _moduleAddress;

	for (int i = 0; i < _pointerPath.size() - 1; ++i)
	{
		ptr += _pointerPath[i];

		if (ProcessInformation::GetProcessType() == ProcessInformation::EMULATOR)
		{
			int regionIndex = ProcessInformation::GetRegionIndex(ptr);

			if (regionIndex >= 0)
			{
				switch (ProcessInformation::GetAddressWidth())
				{
				case 2:
					ptr = ProcessInformation::ReadValue<uint8_t>(ptr);
					break;
				case 4:
					ptr = ProcessInformation::ReadValue<uint32_t>(ptr);
					break;
				default://8
					ptr = ProcessInformation::ReadValue<uint64_t>(ptr);
				}
			}
			else
			{
				ptr = 0;
				break;
			}
		}
		else
		{
			ptr = ProcessInformation::ReadValue<uint64_t>(ptr);
		}
	}

	return reinterpret_cast<void*>(ptr + _pointerPath.back());
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

MungPlex::WatchControl::MousePiano::MousePiano(const int id)
{
	_id = id;
	_idText = std::to_string(id);
	_switches.resize(32);

	for (auto& elem : _switches)
		elem.first.resize(32);
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
}

void MungPlex::WatchControl::IntegralView::Draw()
{
	float itemWidth = ImGui::GetContentRegionAvail().x;
	float itemHeight = 80.0f * Settings::GetGeneralSettings().Scale;

	ImGui::Separator();

	ImGui::BeginChild(std::string("child_integralView" + _idText).c_str(), ImVec2(itemWidth, itemHeight * 2.0f));
	{
		DrawSetup(itemWidth, itemHeight, INTEGRAL);

		ImGui::SameLine();

		ImGui::BeginChild("child_view", ImVec2(itemWidth * 0.5f, itemHeight * 1.5f), true);
		{
			std::string format(8, '\0');
			if (_active)
			{
				uint64_t valptr = reinterpret_cast<uint64_t>(GetCurrentPointer());

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

	ImGui::Separator();

	ImGui::BeginChild(std::string("child_floatView" + _idText).c_str(), ImVec2(itemWidth, itemHeight * 2.0f));
	{
		DrawSetup(itemWidth, itemHeight, FLOAT);

		ImGui::SameLine();

		ImGui::BeginChild("child_view", ImVec2(itemWidth * 0.5f, itemHeight * 1.5f), true);
		{
			std::string format(8, '\0');
			if (_active)
			{
				uint64_t valptr = reinterpret_cast<uint64_t>(GetCurrentPointer());

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

	ImGui::Separator();

	ImGui::BeginChild(std::string("child_boolView" + _idText).c_str(), ImVec2(itemWidth, itemHeight * 2.0f));
	{
		DrawSetup(itemWidth, itemHeight, BOOL);

		ImGui::SameLine();

		ImGui::BeginChild("child_view", ImVec2(itemWidth * 0.5f, itemHeight * 1.5f), true);
		{
			std::string format(8, '\0');
			if (_active)
			{
				uint64_t valptr = reinterpret_cast<uint64_t>(GetCurrentPointer());

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

	ImGui::Separator();

	ImGui::BeginChild(std::string("child_boolView" + _idText).c_str(), ImVec2(itemWidth, itemHeight * 2.0f));
	{
		DrawSetup(itemWidth, itemHeight, MOUSEPIANO);

		ImGui::SameLine();

		ImGui::BeginChild("child_view", ImVec2(itemWidth * 0.5f, itemHeight * 1.5f), true);
		{
			std::string format(8, '\0');
			if (_active)
			{
				uint64_t valptr = reinterpret_cast<uint64_t>(GetCurrentPointer());

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

				_idText = "##" + std::to_string(i);
				ImGui::Checkbox((_idText + "cb").c_str(), &_switches[i].second);
				ImGui::SameLine();
				ImGui::SetNextItemWidth(itemWidth * 0.1f);
				ImGui::InputText((_idText).c_str(), _switches[i].first.data(), 32);
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

	elemJson["Type"] = INTEGRAL;
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

	elemJson["Type"] = FLOAT;
	elemJson["FloatType"] = _typeSelect;
	elemJson["Value"] = _val;
	elemJson["PlotMin"] = _plotMin;
	elemJson["PlotMax"] = _plotMax;

	return elemJson;
}

nlohmann::json MungPlex::WatchControl::BoolView::GetJSON()
{
	nlohmann::json elemJson = GetBasicJSON();

	elemJson["Type"] = BOOL;
	elemJson["Value"] = _val;

	return elemJson;
}

nlohmann::json MungPlex::WatchControl::MousePiano::GetJSON()
{
	nlohmann::json elemJson = GetBasicJSON();

	elemJson["Type"] = MOUSEPIANO;

	for(int i = 0; i < 32; ++i)
		elemJson["Switches"].emplace_back(std::pair<std::string, bool>(_switches[i].first.c_str(), _switches[i].second));

	return elemJson;
}