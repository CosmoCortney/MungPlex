/*
Legal Disclaimer: Unauthorized Modifications and Use of Software
By using, distributing, or modifying this open-source netplay system (hereafter referred to as "Software"), you acknowledge and agree to the following terms:
No Unauthorized Modifications: This Software is provided for educational and personal use only. Any modification, alteration, or misuse of the Software for malicious purposes, including but not limited to hacking, unauthorized access, or exploitation of other users' systems or data, is strictly prohibited and may be in violation of local, state, and international laws.
Legal Consequences: Unauthorized use or tampering with the Software to cause harm, breach security, or disrupt networks may subject you to civil liability and criminal prosecution under applicable computer fraud, cybersecurity, and anti-hacking laws.
Disclaimer of Liability: The authors and contributors of this Software bear no responsibility or liability for any damage, loss, or legal action resulting from any illegal, unethical, or malicious use of the Software. By modifying or misusing the Software in any unauthorized manner, you assume all risks and legal responsibilities for your actions.
Monitoring and Enforcement: The authors reserve the right to cooperate with law enforcement agencies and affected parties in investigating any misuse of the Software. This may include providing relevant information and evidence regarding unauthorized activities to proper authorities.
By proceeding to modify, use, or distribute this Software, you agree to abide by these terms. Any attempt to use the Software for hacking or other malicious activities will result in immediate legal action to the fullest extent of the law.
*/

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
			MungPlex::Log::LogInformation(MungPlex::GetLogMessage(MungPlex::LogMessages::NetplayNoInternetError));
			return;
		}
		if (GetInstance().IsConnected || GetInstance().IsHosting || GetInstance().IsInGame) {
			MungPlex::Log::LogInformation(MungPlex::GetLogMessage(MungPlex::LogMessages::NetplayAlreadyConnectedToServer));
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
		MungPlex::Log::LogInformation(MungPlex::GetLogMessage(MungPlex::LogMessages::NetplayAttemptingHostNotConnected));
		return;
	}
	if (GetInstance().IsInGame) {
		MungPlex::Log::LogInformation(MungPlex::GetLogMessage(MungPlex::LogMessages::NetplayAttemptingHostAlreadyInGame));
		return;
	}
	if (GetInstance().IsHosting) {
		MungPlex::Log::LogInformation(MungPlex::GetLogMessage(MungPlex::LogMessages::NetplayAttemptingHostAlreadyHosting));
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
		MungPlex::Log::LogInformation(MungPlex::GetLogMessage(MungPlex::LogMessages::NetplayJoinGameAttemptDisconnected));
		return;
	}
	if (GetInstance().IsInGame) {
		MungPlex::Log::LogInformation(MungPlex::GetLogMessage(MungPlex::LogMessages::NetplayJoinGameAttemptAlreadyInOne));
		return;
	}
	if (GetInstance().IsHosting) {
		MungPlex::Log::LogInformation(MungPlex::GetLogMessage(MungPlex::LogMessages::NetplayJoinGameAttemptButHosting));
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
		MungPlex::Log::LogInformation(MungPlex::GetLogMessage(MungPlex::LogMessages::NetplayDisconnectAttemptButAlreadyDisconnected));
		return;
	}

	try {
		GetInstance().Websocket.close(boost::beast::websocket::close_code::normal);
		GetInstance().IsConnected = false;
		GetInstance().IsHosting = false;
		GetInstance().IsInGame = false;
		MungPlex::Log::LogInformation(MungPlex::GetLogMessage(MungPlex::LogMessages::NetplaySuccessfullyConnectedToServer));
	}
	catch (const std::exception& e) {
		MungPlex::Log::LogInformation(MungPlex::GetLogMessage(MungPlex::LogMessages::NetplayDisconnectAttemptButAlreadyDisconnected, e.what()));
	}
}
void MungPlex::WebsocketClient::LeaveGame() {
	try {
		if (!GetInstance().IsInGame) {
			MungPlex::Log::LogInformation(MungPlex::GetLogMessage(MungPlex::LogMessages::NetplayLeaveGameInGame));
			return;
		}
		if (!GetInstance().IsConnected) {
			MungPlex::Log::LogInformation(MungPlex::GetLogMessage(MungPlex::LogMessages::NetplayLeaveGameDisconnected));
			return;
		}
		if (GetInstance().IsHosting) {
			MungPlex::Log::LogInformation(MungPlex::GetLogMessage(MungPlex::LogMessages::NetplayLeaveGameHosting));
			return;
		}
		GetInstance().IsInGame = false;
		GetInstance().IsHosting = false;

		GetInstance().SendRequestFromEnum(MungPlex::WebsocketClient::DISCONNECT_FROM_GAME_REQUEST);
	}
	catch (const std::exception& e) {
		MungPlex::Log::LogInformation(MungPlex::GetLogMessage(MungPlex::LogMessages::NetplayDisconnectAttemptButAlreadyDisconnected, e.what()));
	}
}

void MungPlex::WebsocketClient::SendBinaryData(std::vector<uint8_t> _buffer) {
	try {
		boost::asio::const_buffer buffer(_buffer.data(), _buffer.size());
		GetInstance().Websocket.write(buffer);
	}
	catch (std::exception& e) {
		MungPlex::Log::LogInformation(MungPlex::GetLogMessage(MungPlex::LogMessages::NetplayFailedToSendBinaryData, e.what()));
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
			MungPlex::Log::LogInformation(MungPlex::GetLogMessage(MungPlex::LogMessages::NetplayPingDebug));
#endif
		}
	}
	catch (const std::exception& e) {
		MungPlex::Log::LogInformation(MungPlex::GetLogMessage(MungPlex::LogMessages::NetplayPingFailure, e.what()));
	}
}

void MungPlex::WebsocketClient::StopPingTimer() {
	try {
		GetInstance().StopPinging.store(true);
		boost::system::error_code ec;
		GetInstance().PingTimer.cancel(ec);
	}
	catch (std::exception& e) {
		MungPlex::Log::LogInformation(MungPlex::GetLogMessage(MungPlex::LogMessages::NetplayPingStopFailure));
	}
}
void MungPlex::WebsocketClient::StartPingTimer() {
	try {
		GetInstance().StopPinging.store(false);
		GetInstance().PingTimer.expires_after(std::chrono::seconds(GetInstance().PING_INTERVAL));

		GetInstance().PingTimer.async_wait([](const boost::system::error_code& ec) {
			if (ec) {
				MungPlex::Log::LogInformation(MungPlex::GetLogMessage(MungPlex::LogMessages::NetplayPingTimerError, ec.message().c_str()));
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