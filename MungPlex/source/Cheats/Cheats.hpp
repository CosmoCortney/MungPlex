#pragma once
#define SOL_ALL_SAFETIES_ON 1
#include "Connection.hpp"
#include "sol/sol.hpp"
#include "WidgetHelpers.hpp"

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
    public:
        static void DrawWindow();
        static void InitCheatFile();

    private:
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
            TITLE_LENGTH = 256,
            HACKER_LENGTH = 128,
            CHEAT_LENGTH = 1024 * 64,
            DESCRIPTION_LENGTH = 1024 * 16
        };

        enum class ButtonsGcnController
        {
            DPAD_LEFT = 0x0001, DPAD_RIGHT = 0x0002, DPAD_DOWN = 0x0004, DPAD_UP = 0x0008, Z = 0x0010,
            R = 0x0020, L = 0x0040, A = 0x0100, B = 0x0200, X = 0x0400, Y = 0x8000, START = 0x1000
        };

        enum class ButtonsGcnGba
        {
            A = 0x01, B = 0x02, START = 0x08, DPAD_RIGHT = 0x10, DPAD_LEFT = 0x20, DPAD_UP = 0x40, DPAD_DOWN = 0x80,
           // R , L ,
        };

        enum class ButtonsWiiRemoteNunchuck
        {
            DPAD_LEFT = 0x0001, DPAD_RIGHT = 0x0002, DPAD_DOWN = 0x0004, DPAD_UP = 0x0008, PLUS = 0x0010, TWO = 0x0100,
            ONE = 0x0200, B = 0x0400, A = 0x0800, MINUS = 0x1000, Z_NUNCHUCK = 0x2000, C = 0x4000, HOME = 0x8000
        };

        enum class ButtonsWiiClassicController
        {
            DPAD_UP = 0x0001, DPAD_LEFT = 0x0002, ZR = 0x0004, X = 0x0008, A = 0x0010, Y = 0x0020, B = 0x0040,
            ZL = 0x0080, R = 0x0200, PLUS = 0x0400, MINUS = 0x1000, L = 0x2000, DPAD_DOWN = 0x4000, DPAD_RIGHT = 0x8000
            
        };

        static const StringIdPairs _cheatTypes;
        InputText _cheatTitleInput = InputText("Title:", true, "", TITLE_LENGTH);
        InputText _hackerInput = InputText("Hacker(s):", true, "", HACKER_LENGTH);
        InputTextMultiline _textCheatLuaInput = InputTextMultiline("Text Cheat:", true, "", CHEAT_LENGTH);
        InputTextMultiline _textCheatDescriptionInput = InputTextMultiline("Description:", true, "", DESCRIPTION_LENGTH);
        std::wstring _cheatListPath;
        sol::state _lua{};
        int _perSecond = 60;
        bool _executeCheats = false;
        bool _cheatList = false;
        std::thread _cheatThread;
        bool _cheatError = false;
        std::wstring _documentsPath;
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
        int _selectedCheatType = 0;

        static int luaExceptionHandler(lua_State* L, sol::optional<const std::exception&> exception, sol::string_view description);
        void copyCheatToInformationBox(int index);
        bool copyCheatToList(int index);
        bool saveCheatList() const;
        void deleteCheat(uint16_t index);
        void refreshModuleList();
        void DrawCheatList(); //top-left
        void DrawCheatInformation(); //top-right
        void DrawControl(); //bottom left
        void updateConnectionInfo();
        void drawCheatConverter();
        void cheatRoutine();
        bool convertToLua();

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
        static void fillAndSlideInt8(const uint64_t address, const int64_t addressIncrement, const int8_t value, const int8_t valueIncrement, const uint8_t count);
        static void fillAndSlideInt16(const uint64_t address, const int64_t addressIncrement, const int16_t value, const int16_t valueIncrement, const uint16_t count);
        static void fillAndSlideInt32(const uint64_t address, const int64_t addressIncrement, const int32_t value, const int32_t valueIncrement, const uint32_t count);
        static void fillAndSlideInt64(const uint64_t address, const int64_t addressIncrement, const int64_t value, const int64_t valueIncrement, const uint32_t count);
        static void fillAndSlideFloat(const uint64_t address, const int64_t addressIncrement, const float value, const float valueIncrement, const uint32_t count);
        static void fillAndSlideDouble(const uint64_t address, const int64_t addressIncrement, const double value, const double valueIncrement, const uint32_t count);

        static bool isInRange(const uint64_t ptr, const uint64_t start, const uint64_t end);
        static uint64_t getModuleAddress(const char* moduleName);
        static void copyMemory(const uint64_t source, const uint64_t destination, const uint32_t size);

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
	};
}