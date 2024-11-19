#pragma once
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/atomic.hpp>
#include "MemoryViewer.hpp"
#include "Discord.hpp"

namespace MungPlex
{
    class MemoryViewer;

    class Connection
    {
    public:
        static void DrawWindow();
        static bool IsConnected();
        static void SetConnectedStatus(const bool isConnected);
        static DiscordRPC GetDiscordRichPresence();

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
        int _selectedEmulatorIndex = 0;
        int _selectedProcessIndex = 0;
        int _selectedApplicationProcessIndex = 0;
        std::string _connectionMessage = "Not connected...";
        std::vector<MemoryViewer> _memoryViewers;
        DiscordRPC _discord;
        boost::thread _checkConnectionThread;
        boost::atomic<bool> _checkConnectionThreadFlag = false;

        void drawConnectionSelect();
        void startConnectionCheck();
        void checkConnection();
        void drawEmulatorsTabItem();
        void drawAppTabItem();
        void drawConsoleTabItem();
        void drawDisconnectButton();
        void drawProcessControl();
    };
}