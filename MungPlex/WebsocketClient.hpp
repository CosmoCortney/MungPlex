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
        static void ConnectToGame(std::string GameID, std::string GamePassword);
        static void SendMemoryChange(std::string MemoryAddress, std::string NewValue, MungPlex::PrimitiveType Datatype);
        static void SendServerMessage(std::string &message);
        static void HandleMessages();
        static void Disconnect();
    private:
        const std::string WebsocketHostURL = "ws://localhost:8765";
        const std::string WebsocketHostURLPath = "/";
        bool IsConnected = false;
        std::string GameID;
        std::string GamePassword;
        IoContext IoC;
        WebSocketStream Websocket;
        TcpResolver Resolver;

        WebsocketClient();
        ~WebsocketClient();

        static WebsocketClient& GetInstance()
        {
            static WebsocketClient Instance;
            return Instance;
        }
    };
}