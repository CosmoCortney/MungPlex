#include "WebsocketClient.hpp"
#include <iostream>
#include <stdint.h>
#include <sstream>
#include <iostream>
#include <thread>
#include <boost/asio/executor_work_guard.hpp>
#include <vector>
#include "../Log.hpp"
#include "../LogMessages.hpp"

bool MungPlex::WebsocketClient::IsConnected = false;
bool MungPlex::WebsocketClient::IsInGame = false;
bool MungPlex::WebsocketClient::IsHosting = false;
bool MungPlex::WebsocketClient::HasInternet = true; // Assume they have internet by default.  The check for internet is one of the first functions to be called, so this shouldn't do anything regardless

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
		if (!GetInstance().CheckForInternetConnection()) {
			//Abort and don't allow netplay
			MungPlex::Log::LogInformation("[Netplay] You aren't connected to the internet!  Restart MungPlex to try again!");
			return;
		}
		if (GetInstance().IsConnected || GetInstance().IsHosting || GetInstance().IsInGame) {
			MungPlex::Log::LogInformation("[Netplay] Already connected to netplay server!");
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
		GetInstance().IsInGame = false;
		GetInstance().IsHosting = false;

		//Async recursive functions
		GetInstance().StartPingTimer();
		GetInstance().HandleMessages();

		MungPlex::Log::LogInformation(MungPlex::GetLogMessage(MungPlex::LogMessages::NetplaySuccessfullyConnectedToServer));
	}
	catch (const std::exception& e) {
		printf(e.what());
		MungPlex::Log::LogInformation(MungPlex::GetLogMessage(MungPlex::LogMessages::NetplayServerConnectionError, e.what()));
		GetInstance().IsConnected = false;
		GetInstance().IsInGame = false;
		GetInstance().IsHosting = false;
	}
}

void MungPlex::WebsocketClient::HostGame(char* Password) {
	if (!GetInstance().IsConnected) {
		MungPlex::Log::LogInformation("[Netplay] Not connected to netplay server!");
		return;
	}
	if (GetInstance().IsInGame) {
		MungPlex::Log::LogInformation("[Netplay] You are already in a netplay session!");
		return;
	}
	if (GetInstance().IsHosting) {
		MungPlex::Log::LogInformation("[Netplay] You are already hosting a netplay session!");
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
		MungPlex::Log::LogInformation("[Netplay] You are hosting a game!  Disconnect from it to join a different one");
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
	try {
		if (!GetInstance().IsInGame) {
			MungPlex::Log::LogInformation("[Netplay] You aren't in a game!!! How did you trigger a leave!!");
			return;
		}
		if (!GetInstance().IsConnected) {
			MungPlex::Log::LogInformation("[Netplay] Honestly how did you even trigger a leave?!");
			return;
		}
		if (GetInstance().IsHosting) {
			MungPlex::Log::LogInformation("[Netplay] You can't leave a game as a host! Try disbanding the session instead!");
			return;
		}
		GetInstance().IsInGame = false;
		GetInstance().IsHosting = false;

		GetInstance().SendRequestFromEnum(MungPlex::WebsocketClient::DISCONNECT_FROM_GAME_REQUEST);
	}
	catch (const std::exception& e) {
		MungPlex::Log::LogInformation("Failed to leave game!!! " + std::string(e.what()));
	}
}

void MungPlex::WebsocketClient::SendBinaryData(std::vector<uint8_t> _buffer) {
	try {
		boost::asio::const_buffer buffer(_buffer.data(), _buffer.size());
		GetInstance().Websocket.write(buffer);
	}
	catch (std::exception& e) {
		MungPlex::Log::LogInformation("Failed to send data to server!!! " + std::string(e.what()));
	}
}

void MungPlex::WebsocketClient::SendRequestFromEnum(MungPlex::WebsocketClient::RequestTypeByteMap _type)
{
	std::vector<uint8_t> Buffer(1);
	Buffer[0] = static_cast<uint8_t>(_type);
	GetInstance().SendBinaryData(Buffer);
}


void MungPlex::WebsocketClient::PingServer() {
	try {
		if (GetInstance().IsConnected) {
			GetInstance().SendRequestFromEnum(PING_REQUEST);
#ifndef NDEBUG
			MungPlex::Log::LogInformation("[Netplay][Debug] Ping sent to server.");
#endif
		}
	}
	catch (const std::exception& e) {
		MungPlex::Log::LogInformation("[Netplay] Failed to send ping! You might be disconnected due to this!!!");
	}
}

void MungPlex::WebsocketClient::StopPingTimer() {
	try {
		GetInstance().StopPinging.store(true);
		boost::system::error_code ec;
		GetInstance().PingTimer.cancel(ec);
	}
	catch (std::exception& e) {
		MungPlex::Log::LogInformation("Failed to stop ping thread! Hopefully this won't matter, but it becomes a constant, try reinstalling and posting the error to the github or discord! " + std::string(e.what()));
	}
}
void MungPlex::WebsocketClient::StartPingTimer() {
	try {
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
	catch (std::exception& e) {
		MungPlex::Log::LogInformation("Failed to start ping thread!!! This will likely cause you to be randomly disconnected from the server! If this becomes an issue, post this error to the github or discord! " + std::string(e.what()));
	}
}
void MungPlex::WebsocketClient::HandleMessages() {
	try {
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
				MungPlex::Log::LogInformation("[Netplay][Debug] Message received. Bytes transferred: " + std::to_string(bytes_transferred));
#endif
				auto buffer_data = boost::beast::buffers_to_string(GetInstance()._WebsocketBuffer.data());

				GetInstance().ProcessMessages(buffer_data);

				GetInstance()._WebsocketBuffer.consume(bytes_transferred);
				GetInstance().HandleMessages();
			});
	}
	catch (std::exception& e) {
		MungPlex::Log::LogInformation("Failed to handle an incoming message.  This has likely caused a desync between you and the game.  You are being disconnected from your game for this :(.  Try restarting!");
		GetInstance().LeaveGame();
	}
}

void MungPlex::WebsocketClient::ProcessMessages(const std::string& message) {
	try {
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
	catch (std::exception& e) {
		MungPlex::Log::LogInformation("Failed to process message from server!!! This has likely caused a desync and you are going to be disconnected! If this becomes an issue, report this error to the github or discord! " + std::string(e.what()));
	}
}
bool MungPlex::WebsocketClient::CheckForInternetConnection() {
	try {
		TcpResolver::results_type endpoints = GetInstance().Resolver.resolve(GetInstance().InternetCheckURL, GetInstance().InternetCheckURLPort);
		GetInstance().HasInternet = !endpoints.empty();
		return !endpoints.empty();
	}
	catch (const std::exception& e) {
		GetInstance().HasInternet = false;
		return false;
	}
}
void MungPlex::WebsocketClient::DisbandGame() {
	try {
		if (!GetInstance().IsHosting) {
			MungPlex::Log::LogInformation("w");
			return;
		}
		if (!GetInstance().IsConnected) {
			MungPlex::Log::LogInformation("[Netplay] You aren't even connected to the server! How do you expect to disband a session!");
			return;
		}
		if (!GetInstance().IsInGame) {
			MungPlex::Log::LogInformation("[Netplay] How do you except to disband a session you aren't in!");
		}

		GetInstance().SendRequestFromEnum(HOST_SHUTDOWN_REQUEST);
	}
	catch (std::exception& e) {
		MungPlex::Log::LogInformation("[Netplay] You disbanding the session failed.  Just restart MungPlex, it has the same effect! Error: " + std::string(e.what()));
	}
}