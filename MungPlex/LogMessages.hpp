#pragma once
#include <string>
#include <vector>
#include <cstdarg>
#include <utility>
#include <algorithm>
#include <iostream>

namespace MungPlex {
	class LogMessages {
	public:
		static int CountPlaceholders(const std::string& str) {
			const std::string placeholder = "%s";
			int count = 0;
			size_t pos = str.find(placeholder);
			while (pos != std::string::npos) {
				++count;
				pos = str.find(placeholder, pos + placeholder.size());
			}
			return count;
		}
		static const std::vector<std::pair<std::string, int>> IntegerToMessageMappings;
		enum LogMessageIntegers {
			NetplayDisbandGameWhenNotHosting = 0,
			NetplaySuccessfullyConnectedToServer = 1,
			NetplayServerConnectionError = 2,
			NetplayNoInternetError = 3,
			NetplayAlreadyConnectedToServer = 4,
			NetplayAttemptingHostNotConnected = 5,
			NetplayAttemptingHostAlreadyInGame = 6,
			NetplayAttemptingHostAlreadyHosting = 7,
			NetplayJoinGameAttemptDisconnected = 8,
			NetplayJoinGameAttemptAlreadyInOne = 9,
			NetplayJoinGameAttemptButHosting = 10,
			NetplayDisconnectAttemptButAlreadyDisconnected = 11,
			NetplaySuccessfullyDisconnected = 12,
			NetplayDisconnectFailure = 13,
			NetplayLeaveGameInGame = 14,
			NetplayLeaveGameDisconnected = 15,
			NetplayLeaveGameHosting = 16,
			NetplayLeaveGameFailure = 17,
			NetplayFailedToSendBinaryData = 18,
			NetplayPingDebug = 19,
			NetplayPingFailure = 20,
			NetplayPingStopFailure = 21,
			NetplayPingTimerError = 22,
		};
	};



	// THESE NEED TO BE ORDER 0 ONWARDS FOR FAST SORTING!!!!!!!!!! :)
	const std::vector<std::pair<std::string, int>> LogMessages::IntegerToMessageMappings = {
		{"[Netplay] Quit goofing off! You can't disband a game you aren't hosting! We have checks server-side too, if you get around this! Good Luck!", LogMessages::NetplayDisbandGameWhenNotHosting},
		{"[Netplay] Connected to netplay server successfully!", LogMessages::NetplaySuccessfullyConnectedToServer},
		{"[Netplay] Failed to connect to netplay server!!! Error: %s", LogMessages::NetplayServerConnectionError},
		{"[Netplay] You aren't connected to the internet!  Restart MungPlex to try again!", LogMessages::NetplayNoInternetError},
		{"[Netplay] Already connected to netplay server!", LogMessages::NetplayAlreadyConnectedToServer},
		{"[Netplay] You aren't connected to the netplay server and therefore you can't host! Try restarting!", LogMessages::NetplayAttemptingHostNotConnected},
		{"[Netplay] You can't host a netplay session while already connected to one! Disconnect and try again!", LogMessages::NetplayAttemptingHostAlreadyInGame},
		{"[Netplay] You are already hosting a netplay session!", LogMessages::NetplayAttemptingHostAlreadyHosting},
		{"[Netplay] You cannot join a netplay session while you aren't connected! Restart to try again!", LogMessages::NetplayJoinGameAttemptDisconnected},
		{"[Netplay] You cannot join a netplay session while you are already in a netplay session! Disconnect from it and try again!", LogMessages::NetplayJoinGameAttemptAlreadyInOne},
		{"[Netplay] You cannot join a netplay session while you are hosting!  Change ownership of lobby or disband the session to continue!", LogMessages::NetplayJoinGameAttemptButHosting},
		{"[Netplay] You are already disconnected from the netplay server!!!", LogMessages::NetplayDisconnectAttemptButAlreadyDisconnected},
		{"[Netplay] Successfully disconnected from the netplay server!", LogMessages::NetplaySuccessfullyConnectedToServer},
		{"[Netplay] Failed to disconnect from netplay server!!! Better unplug your router: %s", LogMessages::NetplayDisconnectFailure},
		{"[Netplay] You are already in a game! Disconnect from it to connect to a different one!", LogMessages::NetplayLeaveGameInGame},
		{"[Netplay] How did you trigger a leave! You aren't even connected! Please put the process to trigger this on the GitHub or Discord!", LogMessages::NetplayLeaveGameDisconnected},
		{"[Netplay] You can't leave a game as a host! Transfer ownership or disband the lobby!", LogMessages::NetplayLeaveGameHosting},
		{"[Netplay] Failed to leave game!!! %s", LogMessages::NetplayLeaveGameFailure},
		{"[Netplay] Failed to send data to server!!!!!! %s", LogMessages::NetplayFailedToSendBinaryData},
		{"[Netplay][Debug] Ping sent to server.", LogMessages::NetplayPingDebug},
		{"[Netplay] Failed to stop ping thread! This should have no effect on MungPlex, don't worry!", LogMessages::NetplayPingStopFailure},
		{"[Netplay] Ping send failure!!! You will be disconnected due to this!!! %s", LogMessages::NetplayPingFailure},
		{"[Netplay] Ping timer error: %s", LogMessages::NetplayPingTimerError},
	};








	// Accepts as many const char* arguments as you want, but it is highly recommended (Needed) that you use the same number of arguments as the LogMessage you choose corresponds to, or number of %s's.
	// Messing up will result in garbage data being read, and potentially with some luck, the BEL sound being logged! ;)  
	std::string GetLogMessage(LogMessages::LogMessageIntegers _Enum, ...) {
		va_list args;
		std::string logMessage;

		auto it = std::lower_bound(LogMessages::IntegerToMessageMappings.begin(), LogMessages::IntegerToMessageMappings.end(), _Enum,
			[](const std::pair<std::string, int>& pair, int value) {
				return pair.second < value;
			});

		if (it != LogMessages::IntegerToMessageMappings.end() && it->second == _Enum) {
			logMessage = it->first;
		}
		else {
			return "Invalid LogMessageEnum";
		}

		char buffer[1024];

		va_start(args, _Enum);
		int ret = std::vsnprintf(buffer, sizeof(buffer), logMessage.c_str(), args);
		va_end(args);

		if (ret < 0) {
			return "Error formatting message.";
		}

		return std::string(buffer);
	}
}
