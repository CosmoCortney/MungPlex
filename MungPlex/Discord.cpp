#include "Discord.hpp"
#include <iostream>
#include "Log.hpp"

void MungPlex::DiscordRPC::getActivityResult(discord::Result result)
{
	if (result == discord::Result::Ok)
		return;

	std::string err = "Error handling Discord Rich PResence (err-code: " + std::to_string((int)result) + ")";
	std::cout << err << std::endl;
	Log::LogInformation(err);
}

void MungPlex::DiscordRPC::logProblem(const discord::LogLevel level, const std::string message)
{
	std::cout << "discord error level: " << (int)level << " - " << message << std::endl;
}

void MungPlex::DiscordRPC::SetRichPresenceState(const std::string& action)
{
	if (_core == nullptr)
		return;

	_activity.SetState(action.c_str());
	_core->ActivityManager().UpdateActivity(_activity, getActivityResult);
}

void MungPlex::DiscordRPC::SetRichPresenceDetails(const std::string& details)
{
	_richPresenceDetails = details;
}

void MungPlex::DiscordRPC::InitRichPresence()
{
	if (_core != nullptr)
		_core->~Core();

	discord::Core::Create(1175421760892567552, DiscordCreateFlags_Default, &_core);
	_core->SetLogHook(discord::LogLevel::Debug, logProblem);
	_activity.SetApplicationId(1175421760892567552);
	//_activity.SetName("Test Name");
	_activity.SetDetails(_richPresenceDetails.c_str());
	_activity.SetType(discord::ActivityType::Playing);
	_activity.SetInstance(true);
	_activity.GetTimestamps().SetStart(time(NULL));
	_activity.GetAssets().SetLargeImage("icon1024");
	_core->ActivityManager().UpdateActivity(_activity, getActivityResult);
}

void MungPlex::DiscordRPC::StopRichPresence()
{
	if (_core != nullptr)
		_core->~Core();
}

void MungPlex::DiscordRPC::CheckGameState(const bool connected)
{
	if (connected && _core != nullptr)
	{
		if (_core->RunCallbacks() != discord::Result::Ok)
			_core->~Core();
	}
	else if (_core != nullptr)
	{
		_core->~Core();
	}
}
