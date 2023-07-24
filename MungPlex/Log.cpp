#include "Log.h"
#include"HelperFunctions.h"
#include <ctime> 

void MungPlex::Log::DrawWindow()
{
	ImGui::Begin("Log");

	if (!ImGui::CollapsingHeader("Show/Hide Log"))
	{
		ImGui::End();
		return;
	}

	SetUpInputTextMultiline("", (char*)GetInstance()._logMessage.c_str(), IM_ARRAYSIZE(GetInstance()._logMessage.c_str()), 1.0f, 0.9f, ImGuiInputTextFlags_ReadOnly);

	if (ImGui::Button("Clear Log"))
	{
		GetInstance()._logMessage.clear();
	}

	ImGui::End();
}

void MungPlex::Log::LogInformation(const char* text)
{
	std::time_t time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	GetInstance()._logMessage.append(std::to_string(std::localtime(&time)->tm_hour) + ':');
	GetInstance()._logMessage.append(std::to_string(std::localtime(&time)->tm_min) + ':');
	GetInstance()._logMessage.append(std::to_string(std::localtime(&time)->tm_sec) + ": ");
	GetInstance()._logMessage.append(std::string(text) + '\n');
}