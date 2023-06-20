#include"Cheats.h"
static float scale = 2.0f;

void MungPlex::Cheats::DrawWindow()
{
	ImGui::Begin("Cheats");
	GetInstance().DrawCheatInformation();
	GetInstance().DrawControl();
	ImGui::End();
}

void MungPlex::Cheats::DrawCheatInformation()
{
	float groupWidth = ImGui::GetContentRegionAvail().x / scale;

	ImGui::BeginGroup();
	{
		ImGui::PushItemWidth(groupWidth);
		ImGui::SeparatorText("Cheat Information");

		static char buf[256];
		strcpy(buf, "dummy\0");
		ImGui::InputText("Title", buf, IM_ARRAYSIZE(buf));
		strcpy(buf, "dummy\0");
		ImGui::InputText("Hacker(s)", buf, IM_ARRAYSIZE(buf));

		_textCheatLua;
		_textCheatDescription;
		static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
		ImGui::InputTextMultiline("Lua Cheat", _textCheatLua, IM_ARRAYSIZE(_textCheatLua), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), flags);
		ImGui::InputTextMultiline("Description", _textCheatDescription, IM_ARRAYSIZE(_textCheatDescription), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), flags);

	}
	ImGui::EndGroup();
}

void MungPlex::Cheats::DrawControl()
{
	float groupWidth = ImGui::GetContentRegionAvail().x / scale;

	ImGui::BeginGroup();
	{
		ImGui::PushItemWidth(groupWidth);
		ImGui::SeparatorText("Cheat Control");

		static char buf[256];
		
		if (ImGui::Button(_executeCheats ? "Terminate Cheats" : "Apply Cheats"))
		{
			if (_cheatError)
			{
				_cheatThread.join();
				_cheatError = false;
			}

			if (_executeCheats)
			{
				_executeCheats = false;
				_cheatThread.join();
			}
			else
		{
			updateConnectionInfo();
				_processInfo = Xertz::SystemInfo::GetProcessInfo(_pid);
				_executeCheats = true;
				_cheatThread = std::thread(&Cheats::cheatRoutine, this);
			}
		}
		
		ImGui::SliderInt("Interval", &_perSecond, 1, 240);

	}
	ImGui::EndGroup();
}

void MungPlex::Cheats::cheatRoutine()
{
	_cheatError = false;
	sol::protected_function_result pfr = _lua.safe_script(_textCheatLua, sol::script_pass_on_error);

	if (!pfr.valid())
	{
		sol_c_assert(!pfr.valid());
		sol::error err = pfr;
		std::cout << err.what() << std::endl;
		_executeCheats = false;
		_cheatError = true;
		return;
	}

	while (_executeCheats)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1000 / _perSecond));
		_lua.safe_script(_textCheatLua, sol::script_pass_on_error);
	}
}
