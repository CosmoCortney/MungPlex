#pragma once
#include <string>
#include "MungPlexConfig.hpp"
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_context.hpp>

typedef boost::beast::flat_buffer BeastFlatBuffer;
typedef boost::beast::websocket::stream<boost::asio::ip::tcp::socket> WebSocketStream;
typedef boost::asio::io_context IoContext;
typedef boost::asio::ip::tcp::resolver TcpResolver;

namespace MungPlex {
    class WebsocketClient {
    public:
        static void ConnectToWebsocket();
        static void HandleMessages();
        static void Disconnect();
        static void JoinGame(char* GameID, char* Password);
        static void LeaveGame();
        static void HostGame(char* Password);
        static void SendBinaryData(std::vector<uint8_t> _buffer);
        bool IsConnected = false;
        bool IsInGame = false;
        bool IsHosting = false;
    private:
        const std::string WebsocketHostURL = "ws://localhost:8765";
        const std::string WebsocketHostURLPath = "/";
        std::string GameID;
        std::string GamePassword;
        IoContext IoC;
        WebSocketStream Websocket;
        TcpResolver Resolver;

        WebsocketClient();
        ~WebsocketClient();
       
        static enum RequestTypeByteMap {
            HOST_GAME_REQUEST = 12,
            JOIN_GAME_REQUEST = 11,
            MEMORY_CHANGE_ADDRESS_REQUEST = 10,
            DISCONNECT_FROM_GAME_REQUEST = 9,
            HOST_SHUTDOWN_REQUEST = 8
        };

        static enum RecieveTypeBytemap {

        };

        static WebsocketClient& GetInstance()
        {
            static WebsocketClient Instance;
            return Instance;
        }
    };
}