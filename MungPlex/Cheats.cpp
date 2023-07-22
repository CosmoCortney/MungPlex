#include "Cheats.h"

#include "Settings.h"

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
		std::string nr = i < 10 ? std::to_string(0).append(std::to_string(i)) : std::to_string(i);
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
	//initCheatFile();
}

bool MungPlex::Cheats::readBool(const uint64_t address)
{
	return readInt8(address) != 0;
}

int8_t MungPlex::Cheats::readInt8(const uint64_t address)
{
	return GetInstance()._isBigEndian ? readInt64(address) >> 56 : readInt64(address);
}

uint8_t MungPlex::Cheats::readUInt8(const uint64_t address)
{
	return readInt8(address);
}

int16_t MungPlex::Cheats::readInt16(const uint64_t address)
{
	return GetInstance()._isBigEndian ? readInt64(address) >> 48 : readInt64(address);
}

uint16_t MungPlex::Cheats::readUInt16(const uint64_t address)
{
	return readInt16(address);
}

int32_t MungPlex::Cheats::readInt32(const uint64_t address)
{
	return GetInstance()._isBigEndian ? readInt64(address) >> 32 : readInt64(address);
}

uint32_t MungPlex::Cheats::readUInt32(const uint64_t address)
{
	return readInt32(address);
}

int64_t MungPlex::Cheats::readInt64(const uint64_t address)
{
	int64_t readValue = 0;
	const int rangeIndex = GetInstance().getRangeIndex(address);

	if (rangeIndex == -1)
		return 0;

	const void* readAddress = static_cast<char*>(GetInstance()._regions[rangeIndex].BaseLocationProcess) + address - GetInstance()._regions[rangeIndex].Base;
	GetInstance()._processInfo.ReadExRAM(&readValue, readAddress, 8);
	if (GetInstance()._isBigEndian) readValue = Xertz::SwapBytes<int64_t>(readValue);
	return readValue;
}

uint64_t MungPlex::Cheats::readUInt64(const uint64_t address)
{
	return readInt64(address);
}

float MungPlex::Cheats::readFloat(const uint64_t address)
{
	float readValue = 0.0f;
	const int rangeIndex = GetInstance().getRangeIndex(address);

	if (rangeIndex == -1)
		return 0.0f;

	const void* readAddress = static_cast<char*>(GetInstance()._regions[rangeIndex].BaseLocationProcess) + address - GetInstance()._regions[rangeIndex].Base;
	GetInstance()._processInfo.ReadExRAM(&readValue, readAddress, 4);
	if (GetInstance()._isBigEndian) readValue = Xertz::SwapBytes<float>(readValue);
	return readValue;
}

double MungPlex::Cheats::readDouble(const uint64_t address)
{
	double readValue = 0.0;
	const int rangeIndex = GetInstance().getRangeIndex(address);

	if (rangeIndex == -1)
		return 0.0;

	const void* readAddress = static_cast<char*>(GetInstance()._regions[rangeIndex].BaseLocationProcess) + address - GetInstance()._regions[rangeIndex].Base;
	GetInstance()._processInfo.ReadExRAM(&readValue, readAddress, 8);
	if (GetInstance()._isBigEndian) readValue = Xertz::SwapBytes<double>(readValue);
	return readValue;
}

void MungPlex::Cheats::writeToRAM(const int type, const uint64_t address, double value)
{
	uint64_t writeValue;
	const int rangeIndex = GetInstance().getRangeIndex(address);

	if (rangeIndex == -1)
		return;

	void* writeAddress = static_cast<char*>(GetInstance()._regions[rangeIndex].BaseLocationProcess) + address - GetInstance()._regions[rangeIndex].Base;

	switch (type)
	{
	case BOOL:
		writeValue = static_cast<int64_t>(value) != 0;
		GetInstance()._processInfo.WriteExRAM(&writeValue, writeAddress, 1);
		return;
	case INT8:
		writeValue = static_cast<int8_t>(value);
		GetInstance()._processInfo.WriteExRAM(&writeValue, writeAddress, 1);
		return;
	case INT16:
		writeValue = static_cast<int16_t>(value);
		if (GetInstance()._isBigEndian) writeValue = Xertz::SwapBytes<int16_t>(writeValue);
		GetInstance()._processInfo.WriteExRAM(&writeValue, writeAddress, 2);
		return;
	case INT32:
		writeValue = static_cast<int32_t>(value);
		if (GetInstance()._isBigEndian) writeValue = Xertz::SwapBytes<int32_t>(writeValue);
		GetInstance()._processInfo.WriteExRAM(&writeValue, writeAddress, 4);
		return;
	case INT64:
		writeValue = static_cast<int64_t>(value);
		if (GetInstance()._isBigEndian) writeValue = Xertz::SwapBytes<int64_t>(writeValue);
		GetInstance()._processInfo.WriteExRAM(&writeValue, writeAddress, 8);
		return;
	case FLOAT: {
		auto temp = static_cast<float>(value);
		writeValue = *reinterpret_cast<int32_t*>(&temp);
		if (GetInstance()._isBigEndian) writeValue = Xertz::SwapBytes<int32_t>(writeValue);
		GetInstance()._processInfo.WriteExRAM(&writeValue, writeAddress, 4);
	} return;
	case DOUBLE: {
		writeValue = *reinterpret_cast<int64_t*>(&value);
		if (GetInstance()._isBigEndian) writeValue = Xertz::SwapBytes<int64_t>(writeValue);
		GetInstance()._processInfo.WriteExRAM(&writeValue, writeAddress, 8);
	}
	}
}

void MungPlex::Cheats::writeBool(const uint64_t address, const bool value)
{
	const int rangeIndex = GetInstance().getRangeIndex(address);

	if (rangeIndex == -1)
		return;

	void* writeAddress = static_cast<char*>(GetInstance()._regions[rangeIndex].BaseLocationProcess) + address - GetInstance()._regions[rangeIndex].Base;
	GetInstance()._processInfo.WriteExRAM(&value, writeAddress, 1);
}

void MungPlex::Cheats::writeInt8(const uint64_t address, int8_t value)
{
	const int rangeIndex = GetInstance().getRangeIndex(address);

	if (rangeIndex == -1)
		return;

	void* writeAddress = static_cast<char*>(GetInstance()._regions[rangeIndex].BaseLocationProcess) + address - GetInstance()._regions[rangeIndex].Base;
	GetInstance()._processInfo.WriteExRAM(&value, writeAddress, 1);
}

void MungPlex::Cheats::writeInt16(const uint64_t address, int16_t value)
{
	const int rangeIndex = GetInstance().getRangeIndex(address);

	if (rangeIndex == -1)
		return;

	void* writeAddress = static_cast<char*>(GetInstance()._regions[rangeIndex].BaseLocationProcess) + address - GetInstance()._regions[rangeIndex].Base;
	if (GetInstance()._isBigEndian) value = Xertz::SwapBytes<int16_t>(value);
	GetInstance()._processInfo.WriteExRAM(&value, writeAddress, 2);
}

void MungPlex::Cheats::writeInt32(const uint64_t address, int32_t value)
{
	const int rangeIndex = GetInstance().getRangeIndex(address);

	if (rangeIndex == -1)
		return;

	void* writeAddress = static_cast<char*>(GetInstance()._regions[rangeIndex].BaseLocationProcess) + address - GetInstance()._regions[rangeIndex].Base;
	if (GetInstance()._isBigEndian) value = Xertz::SwapBytes<int32_t>(value);
	GetInstance()._processInfo.WriteExRAM(&value, writeAddress, 4);
}

void MungPlex::Cheats::writeInt64(const uint64_t address, int64_t value)
{
	const int rangeIndex = GetInstance().getRangeIndex(address);

	if (rangeIndex == -1)
		return;

	void* writeAddress = static_cast<char*>(GetInstance()._regions[rangeIndex].BaseLocationProcess) + address - GetInstance()._regions[rangeIndex].Base;
	if (GetInstance()._isBigEndian) value = Xertz::SwapBytes<int64_t>(value);
	GetInstance()._processInfo.WriteExRAM(&value, writeAddress, 8);
}

void MungPlex::Cheats::writeFloat(const uint64_t address, float value)
{
	const int rangeIndex = GetInstance().getRangeIndex(address);

	if (rangeIndex == -1)
		return;

	void* writeAddress = static_cast<char*>(GetInstance()._regions[rangeIndex].BaseLocationProcess) + address - GetInstance()._regions[rangeIndex].Base;
	if (GetInstance()._isBigEndian) value = Xertz::SwapBytes<float>(value);
	GetInstance()._processInfo.WriteExRAM(&value, writeAddress, 4);
}

void MungPlex::Cheats::writeDouble(const uint64_t address, double value)
{
	const int rangeIndex = GetInstance().getRangeIndex(address);

	if (rangeIndex == -1)
		return;

	void* writeAddress = static_cast<char*>(GetInstance()._regions[rangeIndex].BaseLocationProcess) + address - GetInstance()._regions[rangeIndex].Base;
	if (GetInstance()._isBigEndian) value = Xertz::SwapBytes<double>(value);
	GetInstance()._processInfo.WriteExRAM(&value, writeAddress, 8);
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
	const ImVec2 childXY = { ImGui::GetContentRegionAvail().x * 0.333f, ImGui::GetContentRegionAvail().y * 0.8f};
	ImGui::BeginGroup();
	{
		ImGui::SeparatorText("Cheat List");

		ImGui::BeginChild("child_CheatList", childXY, true);
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
	const ImVec2 childXY = { ImGui::GetContentRegionAvail().x * 0.333f, ImGui::GetContentRegionAvail().y * 0.8f };
	
	ImGui::BeginGroup();
	{
		ImGui::SeparatorText("Cheat Information");
		
		if (SetUpInputText("Title:", _textCheatTitle, IM_ARRAYSIZE(_textCheatTitle), 1.0f, 0.15f))
		{
			_unsavedChangesTextCheat = true;
		}

		if(SetUpInputText("Hacker(s):", _textCheatHacker, IM_ARRAYSIZE(_textCheatHacker), 1.0f, 0.15f))
		{
			_unsavedChangesTextCheat = true;
		}
		
		static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
		
		if(SetUpInputTextMultiline("Lua Cheat:", _textCheatLua, IM_ARRAYSIZE(_textCheatLua), 1.0f, 0.55f, flags))
		{
			_unsavedChangesTextCheat = true;
		}
		
		if(SetUpInputTextMultiline("Description:", _textCheatDescription, IM_ARRAYSIZE(_textCheatDescription), 1.0f, 0.35f, flags))
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
	const float groupWidth = ImGui::GetContentRegionAvail().x / scale;

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
			SetUpSliderInt("Interval:", &_perSecond, 1, 240, "%d", 0.5f, 0.15f);

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

double MungPlex::Cheats::readFromRAM(const int type, const uint64_t address)
{
	int64_t readValue = 0;
	double returnValue;
	const int rangeIndex = GetInstance().getRangeIndex(address);

	if (rangeIndex == -1)
		return returnValue = NAN;

	const void* readAddress = static_cast<char*>(GetInstance()._regions[rangeIndex].BaseLocationProcess) + address - GetInstance()._regions[rangeIndex].Base;

	switch (type)
	{
	case INT8:
		GetInstance()._processInfo.ReadExRAM(&readValue, readAddress, 1);
		returnValue = static_cast<double>(*reinterpret_cast<int8_t*>(&readValue));
		break;
	case BOOL:
		GetInstance()._processInfo.ReadExRAM(&readValue, readAddress, 1);
		returnValue = readValue != 0;
		break;
	case INT16:
		GetInstance()._processInfo.ReadExRAM(&readValue, readAddress, 2);
		if (GetInstance()._isBigEndian) readValue = Xertz::SwapBytes<int16_t>(readValue);
		returnValue = static_cast<double>(*reinterpret_cast<int16_t*>(&readValue));
		break;
	case INT32:
		GetInstance()._processInfo.ReadExRAM(&readValue, readAddress, 4);
		if (GetInstance()._isBigEndian) readValue = Xertz::SwapBytes<int32_t>(readValue);
		returnValue = static_cast<double>(*reinterpret_cast<int32_t*>(&readValue));
		break;
	case INT64:
		GetInstance()._processInfo.ReadExRAM(&readValue, readAddress, 8);
		if (GetInstance()._isBigEndian) readValue = Xertz::SwapBytes<int64_t>(readValue);
		returnValue = static_cast<double>(readValue);
		break;
	case FLOAT:
		GetInstance()._processInfo.ReadExRAM(&readValue, readAddress, 4);
		if (GetInstance()._isBigEndian) readValue = Xertz::SwapBytes<int32_t>(readValue);
		returnValue = static_cast<double>(*reinterpret_cast<float*>(&readValue));
		break;
	default://DOUBLE
		GetInstance()._processInfo.ReadExRAM(&readValue, readAddress, 8);
		if (GetInstance()._isBigEndian) readValue = Xertz::SwapBytes<int64_t>(readValue);
		returnValue = *reinterpret_cast<double*>(&readValue);
	}

	return returnValue;
}

void MungPlex::Cheats::SetGameID(const char* ID)
{
	GetInstance()._currentGameID = MorphText::Utf8_To_Utf16LE(ID);
}

void MungPlex::Cheats::SetPlatform(const char* platform)
{
	GetInstance()._currentPlatform = MorphText::Utf8_To_Utf16LE(platform);
}

void MungPlex::Cheats::InitCheatFile()
{
	GetInstance()._currentCheatFile = GetInstance()._documentsPath + L"\\MungPlex\\Cheats\\" + GetInstance()._currentPlatform + L"\\" + GetInstance()._currentGameID + L".json";
	if (!std::filesystem::exists(GetInstance()._currentCheatFile))
	{
		std::ofstream file(GetInstance()._currentCheatFile, std::ios::binary);

		if (file.is_open())
		{
			file << "\xEF\xBB\xBF"; //write BOM
			file << GetInstance()._placeholderCheatFile;
		}
	}
	
	GetInstance()._luaCheats.clear();
	GetInstance()._markedCheats.clear();
	GetInstance()._checkBoxIDs.clear();
	memset(GetInstance()._textCheatTitle, 0, strlen(GetInstance()._textCheatTitle));
	memset(GetInstance()._textCheatHacker, 0, strlen(GetInstance()._textCheatHacker));
	memset(GetInstance()._textCheatLua, 0, strlen(GetInstance()._textCheatLua));
	memset(GetInstance()._textCheatDescription, 0, strlen(GetInstance()._textCheatDescription));

	std::string jsonstr;
	{
		std::ifstream inFile;
		inFile.open(GetInstance()._currentCheatFile);

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
		auto& cheats = doc["Cheats"];

		for (size_t i = 0; i < cheats.size(); ++i)
		{
			auto id = cheats[i]["ID"].get<int>();
			auto checked = cheats[i]["Checked"].get<bool>();
			auto title = cheats[i]["Title"].get<std::string>();
			auto hacker = cheats[i]["Hacker"].get<std::string>();
			auto lua = cheats[i]["Lua"].get<std::string>();
			auto description = cheats[i]["Description"].get<std::string>();
			GetInstance()._luaCheats.emplace_back(LuaCheat(id, checked, title, hacker, lua, description));
			GetInstance()._markedCheats.emplace_back(false);
			GetInstance()._checkBoxIDs.emplace_back("##cheat_" + std::to_string(GetInstance()._luaCheats[i].ID));
		}

		if (!GetInstance()._markedCheats.empty())
		{
			GetInstance()._markedCheats[0] = true;
			GetInstance().copyCheatToInformationBox(0);
		}
	}
	catch (const nlohmann::json::parse_error& exception)
	{
		std::cerr << "Failed parsing Lua Cheat: " << exception.what() << std::endl;
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
	for (size_t i = 0; i < _regions.size(); ++i)
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
			for (const auto& [ID, Checked, Title,
				Hacker, Lua, Description] : _luaCheats) {
				nlohmann::json cheatJson;
				cheatJson["ID"] = ID;
				cheatJson["Checked"] = Checked;
				cheatJson["Title"] = Title;
				cheatJson["Hacker"] = Hacker;
				cheatJson["Lua"] = Lua;
				cheatJson["Description"] = Description;

				jsonData["Cheats"].emplace_back(cheatJson);
			}
		}
		else
		{
			jsonData["Cheats"].emplace_back("");
		}

		file << "\xEF\xBB\xBF"; //write BOM
		file << jsonData.dump(4);
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

void MungPlex::Cheats::updateConnectionInfo()
{
	_isBigEndian = ProcessInformation::UnderlyingIsBigEndian();
	_pid = ProcessInformation::GetPID();
	_regions = ProcessInformation::GetRegions();
	refreshModuleList();
}