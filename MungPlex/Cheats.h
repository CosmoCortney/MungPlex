#pragma once

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"
#include <cassert>
#include "lua.h"
#include "GLFW/glfw3.h"
#include <thread>
#include <future>
#include "Connection.h"
#include "ProcessInformation.h"

namespace MungPlex
{
    struct LuaCheat
    {
        int ID = -1;
        bool Checked = false;
        std::string Title;
        std::string Hacker;
        std::string Lua;
        std::string Description;
    };

    class Cheats
	{
        Cheats();

        ~Cheats(){}

        Cheats(const Cheats&) = delete;
        Cheats(Cheats&&) = delete;
        void operator=(const Cheats&) = delete;
        void operator=(Cheats&&) = delete;
        static Cheats& GetInstance()
        {
            static Cheats Instance;
            return Instance;
        }

        enum StrLengths
        {
            TITLE = 256,
            HACKER = 128,
            CHEAT = 1024 * 64,
            DESCRIPTION = 1024 * 16
        };

        enum CheatTypes
        {
            GCN_AR, WII_GECKO, WIIU_CAFE
        };

        std::vector<std::pair<std::string, int>> _cheatTypes;
        std::string _textCheatTitle = std::string(TITLE, 0);
        std::string _textCheatHacker = std::string(HACKER, 0);
        std::string _textCheatLua = std::string(CHEAT, 0);
        std::string _textCheatDescription = std::string(DESCRIPTION, 0);
        std::wstring _cheatListPath;
        sol::state _lua{};
        bool _isBigEndian = false;
        bool _reorderedMemory = false;
        int _pid = 0;
        std::vector<SystemRegion> _regions{};
        int _perSecond = 60;
        bool _executeCheats = false;
        bool _cheatList = false;
        std::thread _cheatThread;
        PROCESS_INFO _processInfo;
        bool _cheatError = false;
        std::wstring _documentsPath;
        std::wstring _currentGameID;
        std::wstring _currentPlatform;
        std::wstring _currentCheatFile;
        std::string _currentCheatListFile;
        std::string _placeholderCheatFile = "{\"Cheats\":[]}";
        std::vector<LuaCheat> _luaCheats{};
        std::vector<bool> _markedCheats{};
        std::vector<std::string> _checkBoxIDs{};
        bool _unsavedChangesTextCheat = false;
        bool _unsavedChangesCheatList = false;
        int _selectedID = -1;
        bool _disableEditButtons = false;
        std::stringstream _logStream;

        static int luaExceptionHandler(lua_State* L, sol::optional<const std::exception&> exception, sol::string_view description);
        void copyCheatToInformationBox(int index);
        void copyCheatToList(int index);
        bool saveCheatList() const;
        void deleteCheat(uint16_t index);
        void refreshModuleList();
        void DrawCheatList(); //top-left
        void DrawCheatInformation(); //top-right
        void DrawControl(); //bottom left
        void updateConnectionInfo();
        void drawCheatConverter();
        int getRangeIndex(uint64_t address) const;
        void cheatRoutine();

        static double readFromRAM(int type, uint64_t address); //legacy function to keep older cheats functioning
        static bool readBool(const uint64_t address);
        static int8_t readInt8(const uint64_t address);
        static uint8_t readUInt8(const uint64_t address);
        static int16_t readInt16(const uint64_t address);
        static uint16_t readUInt16(const uint64_t address);
        static int32_t readInt32(const uint64_t address);
        static uint32_t readUInt32(const uint64_t address);
        static int64_t readInt64(const uint64_t address);
        static uint64_t readUInt64(const uint64_t address);
        static float readFloat(const uint64_t address);
        static double readDouble(const uint64_t address);
        static sol::table readArrayInt8(const uint64_t address, const uint32_t size);
        static sol::table readArrayUInt8(const uint64_t address, const uint32_t size);
        static sol::table readArrayInt16(const uint64_t address, const uint32_t size);
        static sol::table readArrayUInt16(const uint64_t address, const uint32_t size);
        static sol::table readArrayInt32(const uint64_t address, const uint32_t size);
        static sol::table readArrayUInt32(const uint64_t address, const uint32_t size);
        static sol::table readArrayInt64(const uint64_t address, const uint32_t size);
        static sol::table readArrayUInt64(const uint64_t address, const uint32_t size);
        static sol::table readArrayFloat(const uint64_t address, const uint32_t size);
        static sol::table readArrayDouble(const uint64_t address, const uint32_t size);

        static void writeToRAM(int type, uint64_t address, double value); //legacy function to keep older cheats functioning
        static void writeBool(const uint64_t address, bool value);
        static void writeInt8(const uint64_t address, int8_t value);
        static void writeInt16(const uint64_t address, int16_t value);
        static void writeInt32(const uint64_t address, int32_t value);
        static void writeInt64(const uint64_t address, int64_t value);
        static void writeFloat(const uint64_t address, float value);
        static void writeDouble(const uint64_t address, double value);
        static void writeArrayInt8(const uint64_t address, const sol::table arr);
        static void writeArrayInt16(const uint64_t address, const sol::table arr);
        static void writeArrayInt32(const uint64_t address, const sol::table arr);
        static void writeArrayInt64(const uint64_t address, const sol::table arr);
        static void writeArrayFloat(const uint64_t address, const sol::table arr);
        static void writeArrayDouble(const uint64_t address, const sol::table arr);
        template <typename dataType> void writeValue(uint64_t writeAddress, dataType writeValue)
        {
            if (_isBigEndian)
                writeValue = Xertz::SwapBytes(writeValue);

            int8_t* writeValAddr = reinterpret_cast<int8_t*>(&writeValue);

            for (int i = 0; i < sizeof(dataType); ++i)
                writeInt8(writeAddress + i, *(writeValAddr + i));
        }

        static bool isInRange(const uint64_t ptr, const uint64_t start, const uint64_t end);
        static uint64_t getModuleAddress(const char* moduleName);

        static void logText(const char* text);
        static void logUInt8(const uint8_t value, const bool hex = false);
        static void logUInt16(const uint16_t value, const bool hex = false);
        static void logUInt32(const uint32_t value, const bool hex = false);
        static void logUInt64(const uint64_t value, const bool hex = false);
        static void logInt8(const int8_t value, const bool hex = false);
        static void logInt16(const int16_t value, const bool hex = false);
        static void logInt32(const int32_t value, const bool hex = false);
        static void logInt64(const int64_t value, const bool hex = false);
        static void logFloat(const float value);
        static void logDouble(const double value);
        static void logBool(const bool value);

    public:
        static void DrawWindow();
        static void SetGameID(const char* ID);
        static void SetPlatform(const char* platform);
        static void SetBigEndian(const bool isBE);
        static void SetReorderedMemory(const bool reordered);
        static void InitCheatFile();
	};
}