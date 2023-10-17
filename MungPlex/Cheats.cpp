#include "Cheats.h"
#include "Settings.h"
#include <Windows.h>
#include <string>
#include "Log.h"

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
	_lua.set_function("ReadArrayInt8", &readArrayInt8);
	_lua.set_function("ReadArrayInt16", &readArrayInt16);
	_lua.set_function("ReadArrayInt32", &readArrayInt32);
	_lua.set_function("ReadArrayInt64", &readArrayInt64);
	_lua.set_function("ReadArrayUInt8", &readArrayUInt8);
	_lua.set_function("ReadArrayUInt16", &readArrayUInt16);
	_lua.set_function("ReadArrayUInt32", &readArrayUInt32);
	_lua.set_function("ReadArrayUInt64", &readArrayUInt64);
	_lua.set_function("ReadArrayFloat", &readArrayFloat);
	_lua.set_function("ReadArrayDouble", &readArrayDouble);

	_lua.set_function("WriteToRAM", &writeToRAM);
	_lua.set_function("WriteBool", &writeBool);
	_lua.set_function("WriteInt8", &writeInt8);
	_lua.set_function("WriteInt16", &writeInt16);
	_lua.set_function("WriteInt32", &writeInt32);
	_lua.set_function("WriteInt64", &writeInt64);
	_lua.set_function("WriteFloat", &writeFloat);
	_lua.set_function("WriteDouble", &writeDouble);
	_lua.set_function("WriteArrayInt8", &writeArrayInt8);
	_lua.set_function("WriteArrayInt16", &writeArrayInt16);
	_lua.set_function("WriteArrayInt32", &writeArrayInt32);
	_lua.set_function("WriteArrayInt64", &writeArrayInt64);
	_lua.set_function("WriteArrayFloat", &writeArrayFloat);
	_lua.set_function("WriteArrayDouble", &writeArrayDouble);
	_lua.set_function("FillAndSlideInt8", &fillAndSlideInt8);
	_lua.set_function("FillAndSlideInt16", &fillAndSlideInt16);
	_lua.set_function("FillAndSlideInt32", &fillAndSlideInt32);
	_lua.set_function("FillAndSlideInt64", &fillAndSlideInt64);
	_lua.set_function("FillAndSlideFloat", &fillAndSlideFloat);
	_lua.set_function("FillAndSlideDouble", &fillAndSlideDouble);

	_lua.set_function("IsInRange", &isInRange);
	_lua.set_function("GetModuleAddress", &getModuleAddress);
	_lua.set_function("CopyMemory", &copyMemory);

	_lua.set_function("LogText", &logText);
	_lua.set_function("LogUInt8", &logUInt8);
	_lua.set_function("LogUInt16", &logUInt16);
	_lua.set_function("LogUInt32", &logUInt32);
	_lua.set_function("LogUInt64", &logUInt64);
	_lua.set_function("LogInt8", &logInt8);
	_lua.set_function("LogInt16", &logInt16);
	_lua.set_function("LogInt32", &logInt32);
	_lua.set_function("LogInt64", &logInt64);
	_lua.set_function("LogFloat", &logFloat);
	_lua.set_function("LogDouble", &logDouble);
	_lua.set_function("LogBool", &logBool);

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

	_cheatTypes.emplace_back("GameCuber ActionReplay", GCN_AR);
	//initCheatFile();
}

bool MungPlex::Cheats::readBool(const uint64_t address)
{
	return readInt8(address) != 0;
}

int8_t MungPlex::Cheats::readInt8(const uint64_t address)
{
	int8_t temp = 0;

	if (GetInstance()._reorderedMemory)
		temp = !GetInstance()._isBigEndian ? readInt64(address) : readInt64(address) >> 56;
	else
		temp = GetInstance()._isBigEndian ? readInt64(address) >> 56 : readInt64(address);

	return temp;
}

uint8_t MungPlex::Cheats::readUInt8(const uint64_t address)
{
	return readInt8(address);
}

int16_t MungPlex::Cheats::readInt16(const uint64_t address)
{
	int16_t temp = 0;

	if (GetInstance()._reorderedMemory)
		temp = !GetInstance()._isBigEndian ? readInt64(address) : readInt64(address) >> 48;
	else
		temp = GetInstance()._isBigEndian ? readInt64(address) >> 48 : readInt64(address);

	return temp;
}

uint16_t MungPlex::Cheats::readUInt16(const uint64_t address)
{
	return readInt16(address);
}

int32_t MungPlex::Cheats::readInt32(const uint64_t address)
{
	int temp = 0;

	if (GetInstance()._reorderedMemory)
		temp = !GetInstance()._isBigEndian ? readInt64(address) : readInt64(address) >> 32;
	else
		temp = GetInstance()._isBigEndian ? readInt64(address) >> 32 : readInt64(address);

	return temp;
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

	void* readAddress = static_cast<char*>(GetInstance()._regions[rangeIndex].BaseLocationProcess) + address - GetInstance()._regions[rangeIndex].Base;
	
	if (GetInstance()._reorderedMemory)
		ReadFromReorderedRangeEx<int64_t>(GetInstance()._processInfo, &readValue, readAddress);
	else
		GetInstance()._processInfo.ReadExRAM(&readValue, readAddress, 8);
	
	if (GetInstance()._isBigEndian)
		readValue = Xertz::SwapBytes<int64_t>(readValue);
	
	return readValue;
}

uint64_t MungPlex::Cheats::readUInt64(const uint64_t address)
{
	return readInt64(address);
}

float MungPlex::Cheats::readFloat(const uint64_t address)
{
	int32_t temp = 0;
	temp = readInt32(address);
	return *(float*)&temp;
}

double MungPlex::Cheats::readDouble(const uint64_t address)
{
	int64_t temp = 0;
	temp = readInt64(address);
	return *(double*)&temp;
}

sol::table MungPlex::Cheats::readArrayInt8(const uint64_t address, const uint32_t size)
{
	sol::table table = GetInstance()._lua.create_table(size);

	for (int i = 0; i < size; ++i)
		table[i+1] = readInt8(address+i);

	return table;
}

sol::table MungPlex::Cheats::readArrayUInt8(const uint64_t address, const uint32_t size)
{
	sol::table table = GetInstance()._lua.create_table(size);

	for (int i = 0; i < size; ++i)
		table[i + 1] = readUInt8(address + i);

	return table;
}

sol::table MungPlex::Cheats::readArrayInt16(const uint64_t address, const uint32_t size)
{
	sol::table table = GetInstance()._lua.create_table(size);

	for (int i = 0; i < size; ++i)
		table[i + 1] = readInt16(address + i*2);

	return table;
}

sol::table MungPlex::Cheats::readArrayUInt16(const uint64_t address, const uint32_t size)
{
	sol::table table = GetInstance()._lua.create_table(size);

	for (int i = 0; i < size; ++i)
		table[i + 1] = readUInt16(address + i*2);

	return table;
}

sol::table MungPlex::Cheats::readArrayInt32(const uint64_t address, const uint32_t size)
{
	sol::table table = GetInstance()._lua.create_table(size);

	for (int i = 0; i < size; ++i)
		table[i + 1] = readInt32(address + i*4);

	return table;
}

sol::table MungPlex::Cheats::readArrayUInt32(const uint64_t address, const uint32_t size)
{
	sol::table table = GetInstance()._lua.create_table(size);

	for (int i = 0; i < size; ++i)
		table[i + 1] = readUInt32(address + i*4);

	return table;
}

sol::table MungPlex::Cheats::readArrayInt64(const uint64_t address, const uint32_t size)
{
	sol::table table = GetInstance()._lua.create_table(size);

	for (int i = 0; i < size; ++i)
		table[i + 1] = readInt64(address + i*8);

	return table;
}

sol::table MungPlex::Cheats::readArrayUInt64(const uint64_t address, const uint32_t size)
{
	sol::table table = GetInstance()._lua.create_table(size);

	for (int i = 0; i < size; ++i)
		table[i + 1] = readUInt64(address + i*8);

	return table;
}

sol::table MungPlex::Cheats::readArrayFloat(const uint64_t address, const uint32_t size)
{
	sol::table table = GetInstance()._lua.create_table(size);

	for (int i = 0; i < size; ++i)
		table[i + 1] = readFloat(address + i*4);

	return table;
}

sol::table MungPlex::Cheats::readArrayDouble(const uint64_t address, const uint32_t size)
{
	sol::table table = GetInstance()._lua.create_table(size);

	for (int i = 0; i < size; ++i)
		table[i + 1] = readDouble(address + i*8);

	return table;
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
	writeInt8(address, value);
}

void MungPlex::Cheats::writeInt8(const uint64_t address, int8_t value)
{
	const int rangeIndex = GetInstance().getRangeIndex(address);

	if (rangeIndex == -1)
		return;

	void* writeAddress = static_cast<char*>(GetInstance()._regions[rangeIndex].BaseLocationProcess) + address - GetInstance()._regions[rangeIndex].Base;
	
	if (GetInstance()._reorderedMemory)
		WriteToReorderedRangeEx<int8_t>(GetInstance()._processInfo, &value, writeAddress);
	else
		GetInstance()._processInfo.WriteExRAM(&value, writeAddress, 1);
}

void MungPlex::Cheats::writeInt16(const uint64_t address, int16_t value)
{
	GetInstance().writeValue<int16_t>(address, value);
}

void MungPlex::Cheats::writeInt32(const uint64_t address, int32_t value)
{
	GetInstance().writeValue<int32_t>(address, value);
}

void MungPlex::Cheats::writeInt64(const uint64_t address, int64_t value)
{
	GetInstance().writeValue<int64_t>(address, value);
}

void MungPlex::Cheats::writeFloat(const uint64_t address, float value)
{
	GetInstance().writeValue<float>(address, value);
}

void MungPlex::Cheats::writeDouble(const uint64_t address, double value)
{
	GetInstance().writeValue<double>(address, value);
}

void MungPlex::Cheats::writeArrayInt8(const uint64_t address, const sol::table arr)
{
	if (!arr.valid())
		return;

	for (int i = 1; i <= arr.size(); ++i)
	{
		GetInstance().writeValue<int8_t>(address + i - 1, arr[i]);
	}
}

void MungPlex::Cheats::writeArrayInt16(const uint64_t address, const sol::table arr)
{
	if (!arr.valid())
		return;

	for (int i = 1; i <= arr.size(); ++i)
	{
		GetInstance().writeValue<int16_t>(address + i*2 - 2, arr[i]);
	}
}

void MungPlex::Cheats::writeArrayInt32(const uint64_t address, const sol::table arr)
{
	if (!arr.valid())
		return;

	for (int i = 1; i <= arr.size(); ++i)
	{
		GetInstance().writeValue<int32_t>(address + i*4 - 4, arr[i]);
	}
}

void MungPlex::Cheats::writeArrayInt64(const uint64_t address, const sol::table arr)
{
	if (!arr.valid())
		return;

	for (int i = 1; i <= arr.size(); ++i)
	{
		GetInstance().writeValue<int64_t>(address + i*8 - 8, arr[i]);
	}
}

void MungPlex::Cheats::writeArrayFloat(const uint64_t address, const sol::table arr)
{
	if (!arr.valid())
		return;

	for (int i = 1; i <= arr.size(); ++i)
	{
		GetInstance().writeValue<float>(address + i * 4 - 4, arr[i]);
	}
}

void MungPlex::Cheats::writeArrayDouble(const uint64_t address, const sol::table arr)
{
	if (!arr.valid())
		return;

	for (int i = 1; i <= arr.size(); ++i)
	{
		GetInstance().writeValue<double>(address + i * 8 - 8, arr[i]);
	}
}

void MungPlex::Cheats::fillAndSlideInt8(const uint64_t address, const int64_t addressIncrement, const int8_t value, const int8_t valueIncrement, const uint8_t count)
{
	uint64_t addr = address;
	int8_t val = value;

	for (uint32_t i = 0; i < count; ++i)
	{
		writeInt8(addr, val);
		addr += addressIncrement;
		val += valueIncrement;
	}
}

void MungPlex::Cheats::fillAndSlideInt16(const uint64_t address, const int64_t addressIncrement, const int16_t value, const int16_t valueIncrement, const uint16_t count)
{
	uint64_t addr = address;
	int16_t val = value;

	for (uint32_t i = 0; i < count; ++i)
	{
		writeInt16(addr, val);
		addr += addressIncrement;
		val += valueIncrement;
	}
}

void MungPlex::Cheats::fillAndSlideInt32(const uint64_t address, const int64_t addressIncrement, const int32_t value, const int32_t valueIncrement, const uint32_t count)
{
	uint64_t addr = address;
	int32_t val = value;

	for (uint32_t i = 0; i < count; ++i)
	{
		writeInt32(addr, val);
		addr += addressIncrement;
		val += valueIncrement;
	}
}

void MungPlex::Cheats::fillAndSlideInt64(const uint64_t address, const int64_t addressIncrement, const int64_t value, const int64_t valueIncrement, const uint32_t count)
{
	uint64_t addr = address;
	int64_t val = value;

	for (uint32_t i = 0; i < count; ++i)
	{
		writeInt64(addr, val);
		addr += addressIncrement;
		val += valueIncrement;
	}
}

void MungPlex::Cheats::fillAndSlideFloat(const uint64_t address, const int64_t addressIncrement, const float value, const float valueIncrement, const uint32_t count)
{
	uint64_t addr = address;
	float val = value;

	for (uint32_t i = 0; i < count; ++i)
	{
		writeFloat(addr, val);
		addr += addressIncrement;
		val += valueIncrement;
	}
}

void MungPlex::Cheats::fillAndSlideDouble(const uint64_t address, const int64_t addressIncrement, const double value, const double valueIncrement, const uint32_t count)
{
	uint64_t addr = address;
	double val = value;

	for (uint32_t i = 0; i < count; ++i)
	{
		writeDouble(addr, val);
		addr += addressIncrement;
		val += valueIncrement;
	}
}

void MungPlex::Cheats::copyMemory(const uint64_t source, const uint64_t destination, const uint32_t size)
{
	const int sourceIndex = GetInstance().getRangeIndex(source);
	const int destinationIndex = GetInstance().getRangeIndex(destination);

	if (sourceIndex == -1 || GetInstance().getRangeIndex(destinationIndex + size) == -1)
		return;

	void* src = static_cast<char*>(GetInstance()._regions[sourceIndex].BaseLocationProcess) + source - GetInstance()._regions[sourceIndex].Base;
	void* dest = static_cast<char*>(GetInstance()._regions[destinationIndex].BaseLocationProcess) + destination - GetInstance()._regions[destinationIndex].Base;

	char* buf = new char[size];
	Xertz::SystemInfo::GetProcessInfo(GetInstance()._pid).ReadExRAM(buf, src, size);
	Xertz::SystemInfo::GetProcessInfo(GetInstance()._pid).WriteExRAM(buf, dest, size);
	delete[] buf;
}

bool MungPlex::Cheats::isInRange(const uint64_t ptr, const uint64_t start, const uint64_t end)
{
	return ptr >= start && ptr < end;
}

uint64_t MungPlex::Cheats::getModuleAddress(const char* moduleName)
{
	return Xertz::SystemInfo::GetProcessInfo(GetInstance()._pid).GetModuleAddress(MorphText::Utf8_To_Utf16LE(moduleName));
}

void MungPlex::Cheats::logText(const char* text)
{
	Log::LogInformation(text);
}

void MungPlex::Cheats::logUInt8(const uint8_t value, const bool hex)
{
	GetInstance()._logStream.str(std::string());
	uint32_t temp = value;

	if(hex)
		GetInstance()._logStream << std::hex << temp;
	else
		GetInstance()._logStream << std::dec << temp;

	Log::LogInformation(GetInstance()._logStream.str());
}

void MungPlex::Cheats::logUInt16(const uint16_t value, const bool hex)
{
	GetInstance()._logStream.str(std::string());

	if (hex)
		GetInstance()._logStream << std::hex << value;
	else
		GetInstance()._logStream << std::dec << value;

	Log::LogInformation(GetInstance()._logStream.str());
}

void MungPlex::Cheats::logUInt32(const uint32_t value, const bool hex)
{
	GetInstance()._logStream.str(std::string());

	if (hex)
		GetInstance()._logStream << std::hex << value;
	else
		GetInstance()._logStream << std::dec << value;

	Log::LogInformation(GetInstance()._logStream.str());
}

void MungPlex::Cheats::logUInt64(const uint64_t value, const bool hex)
{
	GetInstance()._logStream.str(std::string());

	if (hex)
		GetInstance()._logStream << std::hex << value;
	else
		GetInstance()._logStream << std::dec << value;

	Log::LogInformation(GetInstance()._logStream.str());
}

void MungPlex::Cheats::logInt8(const int8_t value, const bool hex)
{
	GetInstance()._logStream.str(std::string());
	int32_t temp = value;

	if (hex)
		GetInstance()._logStream << std::hex << (temp & 0xFF);
	else
		GetInstance()._logStream << std::dec << temp;

	Log::LogInformation(GetInstance()._logStream.str());
}

void MungPlex::Cheats::logInt16(const int16_t value, const bool hex)
{
	GetInstance()._logStream.str(std::string());

	if (hex)
		GetInstance()._logStream << std::hex << value;
	else
		GetInstance()._logStream << std::dec << value;

	Log::LogInformation(GetInstance()._logStream.str());
}

void MungPlex::Cheats::logInt32(const int32_t value, const bool hex)
{
	GetInstance()._logStream.str(std::string());

	if (hex)
		GetInstance()._logStream << std::hex << value;
	else
		GetInstance()._logStream << std::dec << value;

	Log::LogInformation(GetInstance()._logStream.str());
}

void MungPlex::Cheats::logInt64(const int64_t value, const bool hex)
{
	GetInstance()._logStream.str(std::string());

	if (hex)
		GetInstance()._logStream << std::hex << value;
	else
		GetInstance()._logStream << std::dec << value;

	Log::LogInformation(GetInstance()._logStream.str());
}

void MungPlex::Cheats::logFloat(const float value)
{
	GetInstance()._logStream.str(std::string());
	GetInstance()._logStream << value;
	Log::LogInformation(GetInstance()._logStream.str());
}

void MungPlex::Cheats::logDouble(const double value)
{
	GetInstance()._logStream.str(std::string());
	GetInstance()._logStream << value;
	Log::LogInformation(GetInstance()._logStream.str());
}

void MungPlex::Cheats::logBool(const bool value)
{
	Log::LogInformation(value ? "true" : "false");
}

void MungPlex::Cheats::DrawWindow()
{
	ImGui::Begin("Cheats");
	if (!Connection::IsConnected()) ImGui::BeginDisabled();
	{
		ImGui::BeginGroup();
			GetInstance().DrawCheatList();
#ifndef NDEBUG
			if (!Connection::IsConnected()) ImGui::EndDisabled();

			GetInstance().drawCheatConverter();

			if (!Connection::IsConnected()) ImGui::BeginDisabled();
#endif
		ImGui::EndGroup();

		ImGui::SameLine();

		GetInstance().DrawCheatInformation();
		GetInstance().DrawControl();
	}
	if (!Connection::IsConnected()) ImGui::EndDisabled();
	ImGui::End();
}

void MungPlex::Cheats::drawCheatConverter()
{
	const ImVec2 childXY = { ImGui::GetContentRegionAvail().x * 0.333f, ImGui::GetContentRegionAvail().y * 0.8f };
	static int cheatTypeSelect = GCN_AR;

	ImGui::BeginChild("Cheat Convert", childXY, true);
	{
		ImGui::SeparatorText("Cheat Convert");

		SetUpCombo("Cheat Format:", _cheatTypes, cheatTypeSelect, 1.0f, 0.333f);

	}
	ImGui::EndChild();
}

void MungPlex::Cheats::DrawCheatList()
{
	const ImVec2 childXY = { ImGui::GetContentRegionAvail().x * 0.333f, ImGui::GetContentRegionAvail().y * 0.8f /*0.333f*/ };
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
					_markedCheats[i] = !marked;
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
		static bool disableFlag = false;
		ImGui::SeparatorText("Cheat Information");
		
		if (_executeCheats) ImGui::BeginDisabled();
		{
			if (SetUpInputText("Title:", _textCheatTitle.data(), TITLE, 1.0f, 0.15f))
			{
				_unsavedChangesTextCheat = true;
			}

			if (SetUpInputText("Hacker(s):", _textCheatHacker.data(), HACKER, 1.0f, 0.15f))
			{
				_unsavedChangesTextCheat = true;
			}

			static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;

			if (SetUpInputTextMultiline("Lua Cheat:", _textCheatLua.data(), CHEAT, 1.0f, 0.55f, flags))
			{
				_unsavedChangesTextCheat = true;
			}

			if (SetUpInputTextMultiline("Description:", _textCheatDescription.data(), DESCRIPTION, 1.0f, 0.35f, flags))
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
				Log::LogInformation("Added Cheat to list");
			}

			ImGui::SameLine();

			if (_luaCheats.empty())
				_disableEditButtons = true;

			if (_disableEditButtons) ImGui::BeginDisabled();
			if (ImGui::Button("Update Entry"))
			{
				copyCheatToList(_selectedID);
				saveCheatList();
				_unsavedChangesTextCheat = false;
				disableFlag = false;
				Log::LogInformation((std::string("Updated Cheat ") + std::to_string(_selectedID)).c_str());
			}

			ImGui::SameLine();

			if (ImGui::Button("Delete Entry"))
			{
				deleteCheat(_selectedID);
				saveCheatList();
				_unsavedChangesTextCheat = false;

				if (_luaCheats.empty())
					disableFlag = true;

				Log::LogInformation((std::string("Deleted Cheat at ") + std::to_string(_selectedID)).c_str());
			}
			if (_disableEditButtons) ImGui::EndDisabled();
		}
		if (_executeCheats) ImGui::EndDisabled();

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
	ImGui::BeginGroup();
	{
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
					Log::LogInformation("Cheat(s) terminated");
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
		Log::LogInformation("Executing Cheat List");
		for (const auto& cheat : _luaCheats)
		{
			pfr = _lua.safe_script(cheat.Lua.c_str(), sol::script_pass_on_error);
			if (!pfr.valid())
			{
				sol_c_assert(!pfr.valid());
				const sol::error err = pfr;
				Log::LogInformation(err.what());
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
				_lua.safe_script(cheat.Lua.c_str(), sol::script_pass_on_error);
			}
		}
	}
	else
	{
		Log::LogInformation("Executing Text Cheat");
		pfr = _lua.safe_script(_textCheatLua.c_str(), sol::script_pass_on_error);

		if (!pfr.valid())
		{
			sol_c_assert(!pfr.valid());
			const sol::error err = pfr;
			Log::LogInformation(err.what());
			_executeCheats = false;
			_cheatError = true;
			return;
		}

		while (_executeCheats)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1000 / _perSecond));
			_lua.safe_script(_textCheatLua.c_str(), sol::script_pass_on_error);
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
	GetInstance()._textCheatTitle = std::string(TITLE, 0);
	GetInstance()._textCheatHacker = std::string(HACKER, 0);
	GetInstance()._textCheatLua = std::string(CHEAT, 0);
	GetInstance()._textCheatDescription = std::string(DESCRIPTION, 0);

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
	_textCheatTitle = _luaCheats[index].Title;
	_textCheatHacker = _luaCheats[index].Hacker;
	_textCheatLua = _luaCheats[index].Lua;
	_textCheatDescription = _luaCheats[index].Description;
	_textCheatTitle.resize(TITLE);
	_textCheatHacker.resize(HACKER);
	_textCheatLua.resize(CHEAT);
	_textCheatDescription.resize(DESCRIPTION);
}

void MungPlex::Cheats::copyCheatToList(const int index)
{
	if (index == -1)
	{
		for (LuaCheat& cheat : _luaCheats)
		{
			static int count = 2;

			if (cheat.Title.compare(_textCheatTitle) == 0)
			{
				_textCheatTitle.append(std::to_string(count));
				++count;
			}
		}

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
	_pid = ProcessInformation::GetPID();
	_regions = ProcessInformation::GetRegions();
	refreshModuleList();
}

void MungPlex::Cheats::SetBigEndian(const bool isBE)
{
	GetInstance()._isBigEndian = isBE;
}

void MungPlex::Cheats::SetReorderedMemory(const bool reordered)
{
	GetInstance()._reorderedMemory = reordered;
}