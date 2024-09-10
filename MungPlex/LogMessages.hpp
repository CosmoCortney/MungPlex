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
			NetplayPingThreadStartFailure = 23,
			NetplayHandleMessagesDisconnected = 24,
			NetplayHandleMessageFailure = 25,
			NetplayHandleMessageError = 26,
			NetplayRecievedEmptyMessage = 27,
			NetplayHostGameIdTooShort = 28,
			NetplayReceivedGameId = 30,
			NetplayPasswordTooShort = 31,
			NetplayJoinGameSuccess = 32,
			NetplayProcessMessageException = 33,
			NetplayDisbandGameWhenNotConnected = 34,
			NetplayDisbandGameNotInGame = 35,
			NetplayDisbandFailure = 36,
		}; 
		// THESE NEED TO BE ORDER 0 ONWARDS FOR FAST SORTING!!!!!!!!!! :)
		static inline const std::vector<std::pair<std::string, LogMessageIntegers>> IntegerToMessageMappings = {
			{"[Netplay] Quit goofing off! You can't disband a game you aren't hosting! We have checks server-side too, if you get around this! Good Luck!",  NetplayDisbandGameWhenNotHosting},
			{"[Netplay] Connected to netplay server successfully!",  NetplaySuccessfullyConnectedToServer},
			{"[Netplay] Failed to connect to netplay server!!! Error: %s",  NetplayServerConnectionError},
			{"[Netplay] You aren't connected to the internet!  Restart MungPlex to try again!",  NetplayNoInternetError},
			{"[Netplay] Already connected to netplay server!",  NetplayAlreadyConnectedToServer},
			{"[Netplay] You aren't connected to the netplay server and therefore you can't host! Try restarting!",  NetplayAttemptingHostNotConnected},
			{"[Netplay] You can't host a netplay session while already connected to one! Disconnect and try again!",  NetplayAttemptingHostAlreadyInGame},
			{"[Netplay] You are already hosting a netplay session!",  NetplayAttemptingHostAlreadyHosting},
			{"[Netplay] You cannot join a netplay session while you aren't connected! Restart to try again!",  NetplayJoinGameAttemptDisconnected},
			{"[Netplay] You cannot join a netplay session while you are already in a netplay session! Disconnect from it and try again!",  NetplayJoinGameAttemptAlreadyInOne},
			{"[Netplay] You cannot join a netplay session while you are hosting!  Change ownership of lobby or disband the session to continue!",  NetplayJoinGameAttemptButHosting},
			{"[Netplay] You are already disconnected from the netplay server!!!",  NetplayDisconnectAttemptButAlreadyDisconnected},
			{"[Netplay] Successfully disconnected from the netplay server!",  NetplaySuccessfullyDisconnected},
			{"[Netplay] Failed to disconnect from netplay server!!! Better unplug your router: %s",  NetplayDisconnectFailure},
			{"[Netplay] You are already in a game! Disconnect from it to connect to a different one!",  NetplayLeaveGameInGame},
			{"[Netplay] How did you trigger a leave! You aren't even connected! Please put the process to trigger this on the GitHub or Discord!",  NetplayLeaveGameDisconnected},
			{"[Netplay] You can't leave a game as a host! Transfer ownership or disband the lobby!",  NetplayLeaveGameHosting},
			{"[Netplay] Failed to leave game!!! %s",  NetplayLeaveGameFailure},
			{"[Netplay] Failed to send data to server!!!!!! %s",  NetplayFailedToSendBinaryData},
			{"[Netplay][Debug] Ping sent to server.",  NetplayPingDebug},
			{"[Netplay] Failed to stop ping thread! This should have no effect on MungPlex, don't worry!",  NetplayPingStopFailure},
			{"[Netplay] Ping send failure!!! You will be disconnected due to this!!! %s",  NetplayPingFailure},
			{"[Netplay] Ping timer error: %s", NetplayPingTimerError},
			{"[Netplay] Failed to start ping thread!!! This will likely cause you to be randomly disconnected from the server! If this becomes an issue, post this error to the github or discord! %s", NetplayPingThreadStartFailure},
			{"[Netplay] You cannot handle messages while disconnected! How did you receive one anyway...", NetplayHandleMessagesDisconnected},
			{"[Netplay] Failed to handle incoming message :(.  Error: %s", NetplayHandleMessageFailure},
			{"[Netplay] Failed to handle an incoming message.  This has likely caused a desync between you and the game.  You are being disconnected from your game for this :(.  Try restarting! %s", NetplayHandleMessageError},
			{"[Netplay] Received an empty mesasage from the server! Either parsing the message has gone wrong or the server is having issues. This should not affect you unless this becomes frequent!", NetplayRecievedEmptyMessage},
			{"[Netplay] Received a game id that could not be parsed correctly! Try again in a little bit!", NetplayHostGameIdTooShort},
			{"[Netplay] Success received Game ID! Game#%s", NetplayReceivedGameId},
			{"[Netplay] The password you entered is too short. It needs to be >1 characters!", NetplayPasswordTooShort},
			{"[Netplay] Successfully joined Game#%s with password: %s", NetplayJoinGameSuccess},
			{"[Netplay] Failed to process message from server!!! This has likely caused a desync and you are going to be disconnected! If this becomes an issue, report this error to the github or discord! %s", NetplayProcessMessageException},
			{"[Netplay] You seriously think you can disband a session while disconnected from the netplay server?", NetplayDisbandGameWhenNotConnected},
			{"[Netplay] You cannot disband a session when you aren't even in one!", NetplayDisbandGameNotInGame},
			{"[Netplay] Disbanding the session failed!  Just close MungPlex, it has the same effect, anyways, here's the error: %s", NetplayDisbandFailure},
		};
	};
}
