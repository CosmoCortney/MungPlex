#include "WatchControl.hpp"
#include <algorithm>
#include "Helperfunctions.hpp"
#include "ProcessInformation.hpp"
#include "Settings.hpp"

MungPlex::WatchControl::WatchControl()
{ 
	_views.reserve(100); 
}

void MungPlex::WatchControl::DrawWindow()
{
	static bool stateSet = false;

	if (ImGui::Begin("Watch & Control"))
	{/*
		if (!Connection::IsConnected())
			ImGui::BeginDisabled();
		else
		{
			if (!stateSet && Settings::GetGeneralSettings().EnableRichPresence && ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows))
			{
				Connection::GetDiscordRichPresence().SetRichPresenceState("Value Watch & Control");
				stateSet = true;
			}
		}*/

		GetInstance().drawList();

//		if (!Connection::IsConnected())
	//		ImGui::EndDisabled();
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
					jsonData["Watchlist"].emplace_back(std::get<MousePianoView>(_views[i].second).GetJSON());
					break;
				case IView::MAP3D:
					jsonData["Watchlist"].emplace_back(std::get<Map3dView>(_views[i].second).GetJSON());
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
	GetInstance()._currentFile = MT::Convert<std::string, std::wstring>(Settings::GetGeneralSettings().DocumentsPath.StdStrNoLeadinZeros(), MT::UTF8, MT::UTF16LE) + L"\\MungPlex\\WatchControl\\"
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
				GetInstance()._views.emplace_back(IView::MOUSEPIANO, MousePianoView(i, watchList[i]));
				break;
			case IView::MAP3D:
				GetInstance()._views.emplace_back(IView::MAP3D, Map3dView(i, watchList[i]));
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
		//static int typeSelect = 0;
		static uint64_t viewsCount = 0;
		viewsCount = _views.size();

		_typeSelectCombo.Draw();
		ImGui::SameLine();

		if (viewsCount >= 100) ImGui::BeginDisabled();

		if (ImGui::Button("Add Item"))
		{
			_ids.push_back(viewsCount);

			for (int i = 0; i < _ids.size()-1; ++i)
			{
				if (_ids.back() == _ids[i])
				{
					++_ids.back();
					i = 0;
				}
			}

			switch (_typeSelectCombo.GetSelectedId())
			{
			case IView::FLOAT:
				_views.emplace_back(IView::FLOAT, FloatView(_ids.back()));
				break;
			case IView::BOOL:
				_views.emplace_back(IView::BOOL, BoolView(_ids.back()));
				break;
			case IView::MOUSEPIANO:
				_views.emplace_back(IView::MOUSEPIANO, MousePianoView(_ids.back()));
				break;
			case IView::MAP3D:
				_views.emplace_back(IView::MAP3D, Map3dView(_ids.back()));
				break;
			default: //IView::INTEGRAL
				_views.emplace_back(IView::INTEGRAL, IntegralView(_ids.back()));
			}
		} if (viewsCount >= 100) ImGui::EndDisabled();

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
				std::get<MousePianoView>(_views[i].second).Draw();
				break;
			case IView::MAP3D:
				std::get<Map3dView>(_views[i].second).Draw();
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
			if (std::get<MousePianoView>(views[i].second).GetID() == id)
			{
				views.erase(views.begin() + i);
				return;
			}
			break;
		case IView::MAP3D:
			if (std::get<Map3dView>(views[i].second).GetID() == id)
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