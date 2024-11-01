#pragma once
#include <boost/asio.hpp>
#include "MemoryViewer.hpp"
#include "Discord.hpp"
#include "USBGecko.hpp"

namespace MungPlex
{
    class MemoryViewer;

    class Connection
    {
    public:
        static void DrawWindow();
        static bool IsConnected();
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

        enum ConnectionTypeIDs
        {
            CON_USBGecko
        };

        std::vector<std::pair<std::string, int>> _connectionTypes
        {
            { "USB Gecko", CON_USBGecko }
        };
        bool _connected = false;
        int _selectedEmulatorIndex = 0;
        int _selectedProcessIndex = 0;
        int _selectedApplicationProcessIndex = 0;
        std::string _connectionMessage = "Not connected...";
        std::vector<MemoryViewer> _memoryViewers;
        DiscordRPC _discord;
        USBGecko _usbGecko;

        void drawConnectionSelect();
        void startConnectionCheck();
        void checkConnection();
    };
}