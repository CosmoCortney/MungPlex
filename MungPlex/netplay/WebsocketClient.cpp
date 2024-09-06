#include "WebsocketClient.hpp"
#include <iostream>
#include <stdint.h>
#include <sstream>
#include <vector>
#include "../Log.hpp"


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

void MungPlex::WebsocketClient::ConnectToWebsocket() {
	try {
		if (GetInstance().IsConnected) {
			MungPlex::Log::LogInformation("Already connected to netplay server!");
			return;
		}
		std::string full_url = GetInstance().WebsocketHostURL;

		std::string host_and_port = full_url.substr(5);

		std::string host = host_and_port.substr(0, host_and_port.find(':'));
		std::string port = host_and_port.substr(host_and_port.find(':') + 1);

		TcpResolver::results_type results = GetInstance().Resolver.resolve(host, port);

		boost::asio::connect(GetInstance().Websocket.next_layer(), results.begin(), results.end());

		GetInstance().Websocket.handshake(host_and_port, GetInstance().WebsocketHostURLPath);

		GetInstance().IsConnected = true;
		MungPlex::Log::LogInformation("Connected to netplay server successfully!");
	}
	catch (const std::exception& e) {
		MungPlex::Log::LogInformation("Failed to connect to netplay server!");
		GetInstance().IsConnected = false;
	}
}

void MungPlex::WebsocketClient::HostGame(char* Password) {
	if (!GetInstance().IsConnected) {
		MungPlex::Log::LogInformation("Not connected to netplay server!");
		return;
	}
	if (GetInstance().IsInGame) {
		MungPlex::Log::LogInformation("You are already in a netplay session!");
		return;
	}
	if (GetInstance().IsHosting) {
		MungPlex::Log::LogInformation("You are already hosting a netplay session!");
		return;
	}
	GetInstance().GamePassword = Password;
	GetInstance().IsInGame = true;
	GetInstance().IsHosting = true;
	uint8_t PasswordLength = static_cast<uint8_t>(strlen(Password));
	std::vector<uint8_t> BufferToSend(1 + PasswordLength); 
	BufferToSend[0] = HOST_GAME_REQUEST;
	memcpy(&BufferToSend[1], Password, PasswordLength);
	GetInstance().SendBinaryData(BufferToSend);
}

void MungPlex::WebsocketClient::JoinGame(char* _GameID, char* _Password) {
	if (!GetInstance().IsConnected) {
		MungPlex::Log::LogInformation("Not connected to netplay server!");
		return;
	}
	if (GetInstance().IsInGame) {
		MungPlex::Log::LogInformation("You are already in a game! Disconnect from your current one to ");
		return;
	}
	if (GetInstance().IsHosting) {
		MungPlex::Log::LogInformation("You are hosting a game!  Disconnect from it to join a different one");
		return;
	}
	uint8_t PasswordLength = static_cast<uint8_t>(strlen(_Password));
	// The _GameID is always 4 characters, so we don't have to calculate it's length.  Theoritically, we could get the game ID down to only 2 bytes, since the max unsigned 2 byte integer is ~65000, but join game requests don't happen often, so this optimization can wait.
	std::vector<uint8_t> BufferToSend(5 + PasswordLength);
	BufferToSend[0] = JOIN_GAME_REQUEST;
	memcpy(&BufferToSend[1], _GameID, 4);
	memcpy(&BufferToSend[5], _Password, PasswordLength);
	GetInstance().SendBinaryData(BufferToSend);
}

void MungPlex::WebsocketClient::Disconnect() {
	if (!GetInstance().IsConnected) {
		MungPlex::Log::LogInformation("Already disconnected from server");
		return;
	}

	try {
		GetInstance().Websocket.close(boost::beast::websocket::close_code::normal);
		GetInstance().IsConnected = false;
		GetInstance().IsHosting = false;
		GetInstance().IsInGame = false;
		MungPlex::Log::LogInformation("Successfully disconnect from netplay session.");
	}
	catch (const std::exception& e) {
		MungPlex::Log::LogInformation("Failed to disconnect from netplay session");
	}
}
void MungPlex::WebsocketClient::LeaveGame() {
	//Leave game logic
	GetInstance().IsInGame = false;
	GetInstance().IsHosting = false;
}

void MungPlex::WebsocketClient::SendBinaryData(std::vector<uint8_t> _buffer) {
	boost::asio::const_buffer buffer(_buffer.data(), _buffer.size());
	GetInstance().Websocket.write(buffer);
}