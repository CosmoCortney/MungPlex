#pragma once
#define IMGUI_IMPL_WIN32_DISABLE_GAMEPAD 0
#define MungPlex_VERSION_MAJOR 2
#define MungPlex_VERSION_MINOR 0
#define IMGUI_DEFINE_MATH_OPERATORS
#define SELECT_EMULATOR 0
#define SELECT_NATIVE_APP 1
#define SELECT_REMOTE_SYSTEM 2
#define DOLPHIN 0
#define PROJECT64 1
#define NO_CONNECTION "No connection..."
#define CONNECTED_DOLPHIN_GAMECUBE "Connected to Dolphin (GameCube)"
#define CONNECTED_DOLPHIN_WII "Connected to Dolphin (Wii)"
#define CONNECTED_DOLPHIN_WIIWARE "Connected to Dolphin (WiiWare/Channel)"
#define CONNECTED_PROJECT64 "Connected to Project 64"
#ifndef NDEBUG
#define SystemInfoJSON "F:\\Workspace\\MungPlex\\MungPlex\\resources\\SystemInformations.json5"
#else
#define SystemInfoJSON "resources\\SystemInformations.json"
#endif


namespace MungPlex
{
    enum ValueType
    {
        PRIMITIVE = 0,
        ARRAY = 1,
        TEXT = 2,
        COLOR = 3
    };

    enum PrimitiveType
    {
        INT8 = 0,
        INT16 = 1,
        INT32 = 2,
        INT64 = 3,
        FLOAT = 4,
        DOUBLE = 5,
    };

    enum TextType
    {
        ASCII = 0,
        UTF8 = 1,
        UTF16 = 2,
        UTF32 = 3,
        SHIFT_JIS = 4
    };
}
