#pragma once
#include <boost/asio.hpp>
#include "discord-game-sdk/discord_game_sdk.h"
#include "discord-game-sdk/discord.h"
#include "MemoryViewer.h"

namespace MungPlex
{
    class MemoryViewer;

    class Connection
    {
    public:
        static void DrawWindow();
        static bool IsConnected();
        static void SetRichPresenceState(const std::string& action);
        static void InitRichPresence();
        static void StopRichPresence();
        static std::vector<MemoryViewer>& GetMemoryViews();

    private:
        Connection() = default;
        ~Connection() = default;
        Connection(const Connection&) = delete;
        Connection(Connection&&) = delete;
        void operator=(const Connection&) = delete;
        void operator=(Connection&&) = delete;
        static Connection& GetInstance()
        {
            static Connection Instance;
            return Instance;
        }

        bool _connected = false;
        uint32_t _memViewerCount = 0;
        int _selectedEmulatorIndex = 0;
        int _selectedProcessIndex = 0;
        int _selectedApplicationProcessIndex = 0;
        std::string _connectionMessage = "Not connected...";
        std::vector<MemoryViewer> _memoryViewers;
        discord::Core* _core;
        discord::Result _result;
        discord::Activity _activity;
        std::string _richPresenceDetails;

        void drawConnectionSelect();
        void memoryViewerButton();
        void startConnectionCheck();
        void checkConnection();
        static void getDiscordActivityResult(discord::Result result);
        static void logDiscordProblem(const discord::LogLevel level, const std::string message);
    };
}