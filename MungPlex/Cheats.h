#pragma once

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"
#include <cassert>
#include "lua.h"
#include "GLFW/glfw3.h"
#include "Connection.h"
#include <thread>
#include <future>
#include <Windows.h>
#include <string>

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

        char _textCheatTitle[256];
        char _textCheatHacker[128];
        char _textCheatLua[1024 * 128];
        char _textCheatDescription[1024 * 16];
        std::wstring _cheatListPath;
        sol::state _lua{};
        bool _isBigEndian = false;
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

        
        int getRangeIndex(uint64_t address) const;
        void cheatRoutine();

        //legacy function to keep older cheats functioning
        static double readFromRAM(int type, uint64_t address);

        static bool readBool(uint64_t address);

        static int8_t readInt8(uint64_t address);

        static uint8_t readUInt8(uint64_t address);

        static int16_t readInt16(uint64_t address);

        static uint16_t readUInt16(uint64_t address);

        static int32_t readInt32(uint64_t address);

        static uint32_t readUInt32(uint64_t address);

        static int64_t readInt64(uint64_t address);

        static uint64_t readUInt64(uint64_t address);

        static float readFloat(uint64_t address);

        static double readDouble(uint64_t address);

        //legacy function to keep older cheats functioning
        static void writeToRAM(int type, uint64_t address, double value);

        static void writeBool(uint64_t address, bool value);

        static void writeInt8(uint64_t address, int8_t value);

        static void writeInt16(uint64_t address, int16_t value);

        static void writeInt32(uint64_t address, int32_t value);

        static void writeInt64(uint64_t address, int64_t value);

        static void writeFloat(uint64_t address, float value);

        static void writeDouble(uint64_t address, double value);

    public:
        static void DrawWindow();
        static void SetGameID(const char* ID);
        static void SetPlatform(const char* platform);
        static void InitCheatFile();
	};
}