#include "WebsocketClient.hpp"
#include <iostream>
#include <stdint.h>
#include <sstream>
#include <vector>

MungPlex::WebsocketClient::WebsocketClient()
    : Websocket(IoC), Resolver(IoC) 
{
}

MungPlex::WebsocketClient::~WebsocketClient()
{
    if (IsConnected) {
        Disconnect();
    }
}

void MungPlex::WebsocketClient::ConnectToGame(std::string GameID, std::string GamePassword) {
    GetInstance().GameID = GameID;
    GetInstance().GamePassword = GamePassword;

    try {
        std::string full_url = GetInstance().WebsocketHostURL;

        std::string host_and_port = full_url.substr(5); 

        std::string host = host_and_port.substr(0, host_and_port.find(':'));
        std::string port = host_and_port.substr(host_and_port.find(':') + 1);

        TcpResolver::results_type results = GetInstance().Resolver.resolve(host, port);

        boost::asio::connect(GetInstance().Websocket.next_layer(), results.begin(), results.end());

        GetInstance().Websocket.handshake(host_and_port, GetInstance().WebsocketHostURLPath);

        GetInstance().IsConnected = true;
        std::cout << ":) Connected to server successfully!" << std::endl;

    }
    catch (const std::exception& e) {
        std::cerr << ":( Failed to connect: " << e.what() << std::endl;
        GetInstance().IsConnected = false;
    }
}



void MungPlex::WebsocketClient::SendMemoryChange(std::string MemoryAddress, std::string NewValue, MungPlex::PrimitiveType Datatype) {
    if (!GetInstance().IsConnected) {
        std::cerr << "Not connected to the server." << std::endl;
        return;
    }

    std::vector<uint8_t> DataToSend;
    uint8_t DataTypeUint8 = static_cast<uint8_t>(Datatype);
    DataToSend.push_back(DataTypeUint8);

}

void MungPlex::WebsocketClient::HandleMessages() {
    if (!GetInstance().IsConnected) {
        std::cerr << "Not connected to the server." << std::endl;
        return;

        try {
            BeastFlatBuffer buffer;
            GetInstance().Websocket.read(buffer);
            std::cout << ":) Received: " << boost::beast::make_printable(buffer.data()) << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << ":( Error receiving message: " << e.what() << std::endl;
        }
    }
}

void MungPlex::WebsocketClient::Disconnect() {
    if (!GetInstance().IsConnected) {
        std::cerr << "Already disconnected." << std::endl;
        return;
    }

    try {
        GetInstance().Websocket.close(boost::beast::websocket::close_code::normal);
        GetInstance().IsConnected = false;
        std::cout << ":) Disconnected from the server." << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << ":( Failed to disconnect: " << e.what() << std::endl;
    }
}

void MungPlex::WebsocketClient::SendServerMessage(std::string& message) {
    try {
        GetInstance().Websocket.write(boost::asio::buffer(message));
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to send message: " << e.what() << std::endl;
    }
}