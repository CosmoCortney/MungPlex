#include "WebsocketClient.hpp"
#include <iostream>
#include <stdint.h>
#include <sstream>
#include <iostream>
#include <thread>
#include <boost/asio/executor_work_guard.hpp>
#include <vector>
#include "../Log.hpp"


MungPlex::WebsocketClient::WebsocketClient()
	: Websocket(IoC), Resolver(IoC), PingTimer(IoC), WorkGuard(boost::asio::make_work_guard(IoC))
{
	IoThread = std::thread([this]() {
		IoC.run();
		});
}

MungPlex::WebsocketClient::~WebsocketClient()
{
	//Realistically never called, but here just in case
	if (IsConnected) {
		Disconnect();
	}
	StopPingTimer();
	IoC.stop();  
	WorkGuard.reset();
	if (IoThread.joinable()) {
		IoThread.join();  
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
		
		//Async recursive functions
		GetInstance().StartPingTimer();
		GetInstance().HandleMessages();
		
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
	GetInstance().GameID = _GameID;
	GetInstance().GamePassword = _Password;
	GetInstance().SendBinaryData(BufferToSend);
}

void MungPlex::WebsocketClient::Disconnect() {
	if (!GetInstance().IsConnected) {
		MungPlex::Log::LogInformation("[Netplay] Already disconnected from server");
		return;
	}

	try {
		GetInstance().Websocket.close(boost::beast::websocket::close_code::normal);
		GetInstance().IsConnected = false;
		GetInstance().IsHosting = false;
		GetInstance().IsInGame = false;
		MungPlex::Log::LogInformation("[Netplay] Successfully disconnect from netplay session.");
	}
	catch (const std::exception& e) {
		MungPlex::Log::LogInformation("[Netplay] Failed to disconnect from netplay session!!! Better unplug your router!");
	}
}
void MungPlex::WebsocketClient::LeaveGame() {
	GetInstance().IsInGame = false;
	GetInstance().IsHosting = false;
}

void MungPlex::WebsocketClient::SendBinaryData(std::vector<uint8_t> _buffer) {
	boost::asio::const_buffer buffer(_buffer.data(), _buffer.size());
	GetInstance().Websocket.write(buffer);
}


void MungPlex::WebsocketClient::PingServer() {
	try {
		if (GetInstance().IsConnected) {
			std::vector<uint8_t> _buffer(2);
			_buffer[0] = PING_REQUEST;
			GetInstance().SendBinaryData(_buffer);
			MungPlex::Log::LogInformation("Ping sent to server.");
		}
	}
	catch (const std::exception& e) {
		MungPlex::Log::LogInformation("Failed to send ping: " + std::string(e.what()));
	}
}

void MungPlex::WebsocketClient::StopPingTimer() {
	GetInstance().StopPinging.store(true);
	boost::system::error_code ec;
	GetInstance().PingTimer.cancel(ec);  
}
void MungPlex::WebsocketClient::StartPingTimer() {

	GetInstance().StopPinging.store(false);
	GetInstance().PingTimer.expires_after(std::chrono::seconds(GetInstance().PING_INTERVAL));

	GetInstance().PingTimer.async_wait([](const boost::system::error_code& ec) {
		if (ec) {
			MungPlex::Log::LogInformation("Ping timer error: " + ec.message());
			return;
		}

		GetInstance().PingServer();
		GetInstance().StartPingTimer();  
		});
}
void MungPlex::WebsocketClient::HandleMessages() {
	if (!GetInstance().IsConnected) {
		MungPlex::Log::LogInformation("Not connected to WebSocket server, cannot receive messages.");
		return;
	}

	GetInstance().Websocket.async_read(
		GetInstance()._WebsocketBuffer, 
		[](boost::system::error_code ec, std::size_t bytes_transferred) {
			if (ec) {
				MungPlex::Log::LogInformation("[Netplay] Error receiving message from netplay server! If this happens often, try restarting your game ");
				return;
			}
#ifndef NDEBUG
			MungPlex::Log::LogInformation("[Netplay] Message received. Bytes transferred: " + std::to_string(bytes_transferred));
#endif
			auto buffer_data = boost::beast::buffers_to_string(GetInstance()._WebsocketBuffer.data());

			GetInstance().ProcessMessages(buffer_data);

			GetInstance()._WebsocketBuffer.consume(bytes_transferred);
			GetInstance().HandleMessages();
		});
}

void MungPlex::WebsocketClient::ProcessMessages(const std::string& message) {
	if (message.empty()) {
		MungPlex::Log::LogInformation("[Netplay] Received empty message from server. If this happens often, try restarting your game!");
		return;
	}
	std::vector<uint8_t> MessageBytes(message.begin(), message.end());
	uint8_t MessageType = MessageBytes[0];

	switch (MessageType) {
	//Yes, the brackets for case: are needed
	case RECEIVED_LOBBY_ID: {
		// Host's game was created successfully and this is sending them the ID to it, since it is a number between 1000-9999
		GetInstance().IsConnected = true;
		GetInstance().IsInGame = true;
		GetInstance().IsHosting = true;
		if (MessageBytes.size() < 5) {
			MungPlex::Log::LogInformation("[Netplay] Received GameID was too short. Try restarting your game!");
		}
		std::string ConcatenatedBytes;
		ConcatenatedBytes += static_cast<char>(MessageBytes[1]);
		ConcatenatedBytes += static_cast<char>(MessageBytes[2]);
		ConcatenatedBytes += static_cast<char>(MessageBytes[3]);
		ConcatenatedBytes += static_cast<char>(MessageBytes[4]);

		GetInstance().GameID = ConcatenatedBytes;

		MungPlex::Log::LogInformation("Received game id: " + ConcatenatedBytes);
		break;

	}
	case SUCCESSFULLY_JOINED_GAME:
		MungPlex::Log::LogInformation("[Netplay] Successfully joined Game#" + GetInstance().GameID + " with password " + GetInstance().GamePassword);
		GetInstance().IsInGame = true;
		GetInstance().IsConnected = true;
		//Definitly not hosting if they just joined a game
		GetInstance().IsHosting = false;
		break;

	case PING_REQUEST:
		break;
	case SERVER_PONG:
		// See SERVER_PONG definition for more details
		break;
	default:
#ifndef NDEBUG
		MungPlex::Log::LogInformation("[Netplay] Received unknown message type: " + std::to_string(MessageType));
#endif
		break;
	}
}
