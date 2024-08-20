#pragma once
#include "discord-game-sdk/discord_game_sdk.h"
#include "discord-game-sdk/discord.h"
#include <string>

namespace MungPlex
{
	class DiscordRPC
	{
	public:
		DiscordRPC() = default;
		~DiscordRPC() = default;
		void SetRichPresenceState(const std::string& action);
		void SetRichPresenceDetails(const std::string& details);
		void InitRichPresence();
		void StopRichPresence();
		void CheckGameState(const bool connected);

	private:
		static void getActivityResult(discord::Result result);
		static void logProblem(const discord::LogLevel level, const std::string message);
		discord::Core* _core;
		discord::Result _result;
		discord::Activity _activity;
		std::string _richPresenceDetails;
	};
}