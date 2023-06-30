#include"Cheats.h"
static float scale = 2.0f;

MungPlex::Cheats::Cheats()
{
	updateConnectionInfo();
	_lua.open_libraries(sol::lib::base,
		sol::lib::string,
		sol::lib::math,
		sol::lib::package,
		sol::lib::coroutine,
		sol::lib::table,
		sol::lib::io,
		sol::lib::os);

	_lua.set("INT8", INT8);
	_lua.set("INT16", INT16);
	_lua.set("INT32", INT32);
	_lua.set("INT64", INT64);
	_lua.set("FLOAT", FLOAT);
	_lua.set("DOUBLE", DOUBLE);
	_lua.set("BOOL", BOOL);

	_lua.set_function("ReadFromRAM", &readFromRAM);
	_lua.set_function("ReadBool", &readBool);
	_lua.set_function("ReadInt8", &readInt8);
	_lua.set_function("ReadInt16", &readInt16);
	_lua.set_function("ReadInt32", &readInt32);
	_lua.set_function("ReadInt64", &readInt64);
	_lua.set_function("ReadUInt8", &readUInt8);
	_lua.set_function("ReadUInt16", &readUInt16);
	_lua.set_function("ReadUInt32", &readUInt32);
	_lua.set_function("ReadUInt64", &readUInt64);
	_lua.set_function("ReadFloat", &readFloat);
	_lua.set_function("ReadDouble", &readDouble);

	_lua.set_function("WriteToRAM", &writeToRAM);
	_lua.set_function("WriteBool", &writeBool);
	_lua.set_function("WriteInt8", &writeInt8);
	_lua.set_function("WriteInt16", &writeInt16);
	_lua.set_function("WriteInt32", &writeInt32);
	_lua.set_function("WriteInt64", &writeInt64);
	_lua.set_function("WriteFloat", &writeFloat);
	_lua.set_function("WriteDouble", &writeDouble);

	lua_State* L = _lua.lua_state();
	for (int i = 0; i < 32; ++i)
	{
		std::string nr = ((i < 10) ? std::to_string(0).append(std::to_string(i)) : std::to_string(i));
		std::string name = std::string("INTREG").append(nr);
		
		lua_pushinteger(L, 0);
		lua_setglobal(L, name.c_str());

		name = std::string("NUMREG").append(nr);
		lua_pushnumber(L, 0);
		lua_setglobal(L, name.c_str());

		name = std::string("BOOLREG").append(nr);
		lua_pushboolean(L, false);
		lua_setglobal(L, name.c_str());
	}

	_lua.set_exception_handler(&luaExceptionHandler);

	_cheatList = Settings::GetCheatsSettings().DefaultCheatList;
	_perSecond = Settings::GetCheatsSettings().DefaultInterval;
	_documentsPath = MorphText::Utf8_To_Utf16LE(Settings::GetGeneralSettings().DocumentsPath);
	initCheatFile();
}

void MungPlex::Cheats::DrawWindow()
{
	ImGui::Begin("Cheats");
	GetInstance().DrawCheatList();
	ImGui::SameLine();
	GetInstance().DrawCheatInformation(); 
	GetInstance().DrawControl();
	ImGui::End();
}

void MungPlex::Cheats::DrawCheatList()
{
	float groupWidth = ImGui::GetContentRegionAvail().x / scale;
	ImGui::BeginGroup();
	{
		ImGui::PushItemWidth(groupWidth);
		ImGui::SeparatorText("Cheat List");

		ImGui::BeginChild("child", ImVec2(500, 500), true);
		{
			for (int i = 0; i < _luaCheats.size(); ++i)
			{
				bool marked = _markedCheats[i];
				if (ImGui::Checkbox(_checkBoxIDs[i].c_str(), &_luaCheats[i].Checked))
				{
					_unsavedChangesCheatList = true;
				}

				ImGui::SameLine();

				if (ImGui::Selectable(_luaCheats[i].Title.c_str(), &marked))
				{
					_markedCheats.assign(_markedCheats.size(), false);
					_markedCheats[i] = marked;
					_selectedID = i;
					copyCheatToInformationBox(i);
				}
			}
		}
		ImGui::EndChild();
	}
	ImGui::EndGroup();
}

void MungPlex::Cheats::DrawCheatInformation()
{
	float groupWidth = ImGui::GetContentRegionAvail().x / scale;

	ImGui::BeginGroup();
	{
		ImGui::PushItemWidth(groupWidth);
		ImGui::SeparatorText("Cheat Information");

		
		if (ImGui::InputText("Title", _textCheatTitle, IM_ARRAYSIZE(_textCheatTitle)))
		{
			_unsavedChangesTextCheat = true;
		}

		if(ImGui::InputText("Hacker(s)", _textCheatHacker, IM_ARRAYSIZE(_textCheatHacker)))
		{
			_unsavedChangesTextCheat = true;
		}
		
		static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
		
		if(ImGui::InputTextMultiline("Lua Cheat", _textCheatLua, IM_ARRAYSIZE(_textCheatLua), ImVec2(1000, ImGui::GetTextLineHeight() * 16), flags))
		{
			_unsavedChangesTextCheat = true;
		}
		
		if(ImGui::InputTextMultiline("Description", _textCheatDescription, IM_ARRAYSIZE(_textCheatDescription), ImVec2(1000, ImGui::GetTextLineHeight() * 8), flags))
		{
			_unsavedChangesTextCheat = true;
		}

		if (ImGui::Button("Add to list"))
		{
			copyCheatToList(-1);
			saveCheatList();
			_unsavedChangesTextCheat = false;
			_markedCheats.assign(_markedCheats.size(), false);
			_markedCheats.emplace_back(true);
			_checkBoxIDs.emplace_back("##cheat_" + std::to_string(_luaCheats.back().ID));
		}

		ImGui::SameLine();

		static bool disableFlag = false;
		if (_luaCheats.empty())
			_disableEditButtons = true;

		if (_disableEditButtons) ImGui::BeginDisabled();
			if (ImGui::Button("Update Entry"))
			{
				copyCheatToList(_selectedID);
				saveCheatList();
				_unsavedChangesTextCheat = false;
				disableFlag = false;
			}

			ImGui::SameLine();

			if (ImGui::Button("Delete Entry"))
			{
				deleteCheat(_selectedID);
				saveCheatList();
				_unsavedChangesTextCheat = false;
				
				if (_luaCheats.empty())
					disableFlag = true;
			}
		if (_disableEditButtons) ImGui::EndDisabled();

		if (disableFlag)
		{
			disableFlag = false;
			_disableEditButtons = true;
		}
		else
		{
			_disableEditButtons = false;
		}
	}
	ImGui::EndGroup();
}

void MungPlex::Cheats::DrawControl()
{
	float groupWidth = ImGui::GetContentRegionAvail().x / scale;

	ImGui::BeginGroup();
	{
		ImGui::PushItemWidth(groupWidth);
		ImGui::SeparatorText("Cheat Control");

		ImGui::BeginGroup();
		{
			if (ImGui::RadioButton("Cheat List", _cheatList))
				_cheatList = true;

			if (ImGui::RadioButton("Text Cheat", !_cheatList))
				_cheatList = false;
		}
		ImGui::EndGroup();

		ImGui::SameLine();
		
		ImGui::BeginGroup();
		{
			if (ImGui::Button(_executeCheats ? "Terminate Cheats" : "Apply Cheats"))
			{
				if (_cheatError)
				{
					_cheatThread.join();
					_cheatError = false;
				}

				if (_executeCheats)
				{
					_executeCheats = false;
					_cheatThread.join();
				}
				else
				{
					updateConnectionInfo();
					_processInfo = Xertz::SystemInfo::GetProcessInfo(_pid);
					_executeCheats = true;
					_cheatThread = std::thread(&Cheats::cheatRoutine, this);
				}
			}

			ImGui::SliderInt("Interval", &_perSecond, 1, 240);
		}
		ImGui::EndGroup();
	}
	ImGui::EndGroup();
}

void MungPlex::Cheats::cheatRoutine()
{
	_cheatError = false;
	sol::protected_function_result pfr;

	if (_cheatList)
	{
		for (const auto& cheat : _luaCheats)
		{
			pfr = _lua.safe_script(cheat.Lua, sol::script_pass_on_error);
			if (!pfr.valid())
			{
				sol_c_assert(!pfr.valid());
				const sol::error err = pfr;
				std::cout << err.what() << std::endl;
				_executeCheats = false;
				_cheatError = true;
				return;
			}
		}

		while (_executeCheats)
		{
			for (const auto& cheat : _luaCheats)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1000 / _perSecond));
				_lua.safe_script(cheat.Lua, sol::script_pass_on_error);
			}
		}
	}
	else
	{
		pfr = _lua.safe_script(_textCheatLua, sol::script_pass_on_error);

		if (!pfr.valid())
		{
			sol_c_assert(!pfr.valid());
			const sol::error err = pfr;
			std::cout << err.what() << std::endl;
			_executeCheats = false;
			_cheatError = true;
			return;
		}

		while (_executeCheats)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1000 / _perSecond));
			_lua.safe_script(_textCheatLua, sol::script_pass_on_error);
		}
	}
}

void MungPlex::Cheats::SetGameID(const char* ID)
{
	GetInstance()._currentGameID = MorphText::Utf8_To_Utf16LE(ID);
}

void MungPlex::Cheats::initCheatFile()
{
	_currentCheatFile = _documentsPath + L"\\MungPlex\\Cheats\\" + _currentGameID + L".json";
	if (!std::filesystem::exists(_currentCheatFile))
	{
		std::ofstream file(_currentCheatFile, std::ios::binary);

		if (file.is_open())
		{
			file << "\xEF\xBB\xBF"; //write BOM
			file << _placeholderCheatFile;
			file.close();
		}
	}
	
	_luaCheats.clear();
	_markedCheats.clear();
	_checkBoxIDs.clear();
	std::ifstream inFile;
	std::string jsonstr;
	inFile.open(_currentCheatFile);

	if (inFile)
	{
		std::string buffer;
		while (std::getline(inFile, buffer))
		{
			jsonstr.append(buffer).append("\n");
		}
	}
	inFile.close();
	
	try
	{
		auto doc = nlohmann::json::parse(jsonstr);
		auto& cheats = doc["Cheats"];

		for (int i = 0; i < cheats.size(); ++i)
		{
			int id = cheats[i]["ID"].get<int>();
			bool checked = cheats[i]["Checked"].get<bool>();
			std::string title = cheats[i]["Title"].get<std::string>();
			std::string hacker = cheats[i]["Hacker"].get<std::string>();
			std::string lua = cheats[i]["Lua"].get<std::string>();
			std::string description = cheats[i]["Description"].get<std::string>();
			_luaCheats.emplace_back(LuaCheat(id, checked, title, hacker, lua, description));
			_markedCheats.emplace_back(false);
			_checkBoxIDs.emplace_back("##cheat_" + std::to_string(_luaCheats[i].ID));
		}

		if (!_markedCheats.empty())
		{
			_markedCheats[0] = true;
			copyCheatToInformationBox(0);
		}
	}
	catch (const nlohmann::json::parse_error& exception)
	{
		std::cerr << "Failed parsing Lua Cheat: " << exception.what() << std::endl;
		return;
	}
}

int MungPlex::Cheats::luaExceptionHandler(lua_State* L, const sol::optional<const std::exception&> exception, const sol::string_view description)
{
	std::cout << "An exception occurred";
	if (exception)
	{
		std::cout << "\nError: ";
		const std::exception& ex = *exception;
		std::cout << ex.what() << std::endl;
	}
	else
	{
		std::cout << "\nDetails: ";
		std::cout.write(description.data(),
			static_cast<std::streamsize>(description.size()));
		std::cout << std::endl;
	}

	return sol::stack::push(L, description);
}

int MungPlex::Cheats::getRangeIndex(const uint64_t address) const
{
	for (int i = 0; i < _regions.size(); ++i)
	{
		if (address >= _regions[i].Base && address < _regions[i].Base + _regions[i].Size)
			return i;
	}

	return -1;
}

void MungPlex::Cheats::copyCheatToInformationBox(const int index)
{
	strcpy_s(_textCheatTitle, _luaCheats[index].Title.c_str());
	strcpy_s(_textCheatHacker, _luaCheats[index].Hacker.c_str());
	strcpy_s(_textCheatLua, _luaCheats[index].Lua.c_str());
	strcpy_s(_textCheatDescription, _luaCheats[index].Description.c_str());
}

void MungPlex::Cheats::copyCheatToList(const int index)
{
	if (index == -1)
	{
		_luaCheats.emplace_back(LuaCheat(!_luaCheats.empty() ? _luaCheats.back().ID + 1 : 0,
			true,
			_textCheatTitle,
			_textCheatHacker,
			_textCheatLua,
			_textCheatDescription));
	}
	else
	{
		_luaCheats[index].Title = _textCheatTitle;
		_luaCheats[index].Hacker = _textCheatHacker;
		_luaCheats[index].Lua = _textCheatLua;
		_luaCheats[index].Description = _textCheatDescription;
	}
}

bool MungPlex::Cheats::saveCheatList() const
{
	std::ofstream file(_currentCheatFile, std::ios::binary);
	const bool isOpen = file.is_open();

	if (isOpen)
	{
		nlohmann::json jsonData;

		if (!_luaCheats.empty())
		{
			for (const auto& cheat : _luaCheats) {
				nlohmann::json cheatJson;
				cheatJson["ID"] = cheat.ID;
				cheatJson["Checked"] = cheat.Checked;
				cheatJson["Title"] = cheat.Title;
				cheatJson["Hacker"] = cheat.Hacker;
				cheatJson["Lua"] = cheat.Lua;
				cheatJson["Description"] = cheat.Description;

				jsonData["Cheats"].emplace_back(cheatJson);
			}
		}
		else
		{
			jsonData["Cheats"].emplace_back("");
		}

		file << "\xEF\xBB\xBF"; //write BOM
		file << jsonData.dump(4);
		file.close();
	}

	return isOpen;
}

void MungPlex::Cheats::deleteCheat(const uint16_t index)
{
	if (index >= _luaCheats.size())
		return;

	_luaCheats.erase(_luaCheats.begin() + index);
}

void MungPlex::Cheats::refreshModuleList()
{
	lua_State* L = _lua.lua_state();
	const int moduleCount = _processInfo.GetModuleList().size();
	lua_createtable(L, 0, moduleCount);

	for (int i = 0; i < moduleCount; ++i)
	{
		lua_pushinteger(L, _processInfo.GetModuleList()[i].second);
		lua_setfield(L, -2, MorphText::Utf16LE_To_Utf8(_processInfo.GetModuleList()[i].first).c_str());
	}
	lua_setglobal(L, "Modules");
}