#pragma once
#include <string>
#include <atomic>
#include <stdint.h>
#include <thread>
#include "MungPlexConfig.hpp"
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/executor_work_guard.hpp>


typedef boost::beast::flat_buffer WebsocketBuffer;
typedef boost::beast::websocket::stream<boost::asio::ip::tcp::socket> WebSocketStream;
typedef boost::asio::io_context IoContext;
typedef boost::asio::ip::tcp::resolver TcpResolver;

namespace MungPlex {
    class WebsocketClient {
    public:
        static enum RequestTypeByteMap {
            HOST_GAME_REQUEST = 12,
            JOIN_GAME_REQUEST = 11,
            MEMORY_CHANGE_ADDRESS_REQUEST = 10,
            DISCONNECT_FROM_GAME_REQUEST = 9,
            HOST_SHUTDOWN_REQUEST = 8,
            PING_REQUEST = 7,
        };
        static void ConnectToWebsocket();
        static void Disconnect();
        static void JoinGame(char* GameID, char* Password);
        static void LeaveGame();
        static void HostGame(char* Password);
        static void SendBinaryData(std::vector<uint8_t> _buffer);
        //Only use this for requests that don't need extra data, such as pings or disconnect requests
        static void SendRequestFromEnum(MungPlex::WebsocketClient::RequestTypeByteMap _type);
        static void HandleMessages();
        static void ProcessMessages(const std::string& message);
        static void StartPingTimer();
        static void StopPingTimer();
        static bool CheckForInternetConnection();
        static void DisbandGame();
        static bool IsConnected;
        static bool IsInGame;
        static bool IsHosting;
        static bool HasInternet;
    private:
        const std::string WebsocketHostURL = "ws://localhost:8765";
        const std::string WebsocketHostURLPath = "/";
        //Can be replaced by the actual domain later
        const std::string InternetCheckURL = "lawnmeower.de";  //DO NOT PUT "www", "https://", or "http://" before the URL!!!!!!!!!
        const std::string InternetCheckURLPort = "80"; //443 is https port.  80 is http port.  I found that port 80 worked better
        std::string GameID;
        std::string GamePassword;
        IoContext IoC;
        WebSocketStream Websocket;
        WebsocketBuffer _WebsocketBuffer;
        TcpResolver Resolver;
        const uint8_t PING_INTERVAL = 5; // Seconds
        boost::asio::steady_timer PingTimer;
        boost::asio::executor_work_guard<IoContext::executor_type> WorkGuard;
        std::thread IoThread;
        // Made this atomic just in case! This should hopefully prevent threading issues :)
        std::atomic<bool> StopPinging;
        static void PingServer();
        WebsocketClient();
        ~WebsocketClient();

        static enum RecieveTypeBytemap {
            RECEIVED_LOBBY_ID = 12,
            // I have no idea why the first byte of server pong is 69.  Prehaps if we plan to shrink these types into a half a byte we can look into this more.  We will just ignore these in the ProcessMessages function for the time being.  This value will probably change when we switch to C#
            SERVER_PONG = 69,
            SUCCESSFULLY_JOINED_GAME = 11,
            SUCCESSFUL_DISBAND = 10,
            SUCCESSFUL_LEAVE_GAME = 9,
        };

        static WebsocketClient& GetInstance()
        {
            static WebsocketClient Instance;
            return Instance;
        }
    };
}