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
		
		if (ImGui::Button("Apply Cheats"))
		{
			_lua.script(_textCheatLua);
		}
		

	}
	ImGui::EndGroup();
}
