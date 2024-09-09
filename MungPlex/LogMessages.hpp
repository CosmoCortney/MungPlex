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
		};

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
	};



	// THESE NEED TO BE ORDER 0 ONWARDS FOR FAST SORTING!!!!!!!!!! :)
	const std::vector<std::pair<std::string, int>> LogMessages::IntegerToMessageMappings = {
		{"[Netplay] Quit goofing off! You can't disband a game you aren't hosting! We have checks server-side too, if you get around this! Good Luck!", LogMessages::NetplayDisbandGameWhenNotHosting},
		{"[Netplay] Connected to netplay server successfully!", LogMessages::NetplaySuccessfullyConnectedToServer},
		{"[Netplay] Failed to connect to netplay server!!! Error: %s", LogMessages::NetplayServerConnectionError}
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

		char buffer[512];

		va_start(args, _Enum);
		int ret = std::vsnprintf(buffer, sizeof(buffer), logMessage.c_str(), args);
		va_end(args);

		if (ret < 0) {
			return "Error formatting message.";
		}

		return std::string(buffer);
	}
}
