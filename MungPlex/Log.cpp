#include "Log.h"
#include <ctime> 
#include <fstream>
#include "Settings.h"
#include <stdio.h>

std::string MungPlex::Log::_logMessage;

bool MungPlex::Log::init()
{
	LPSTR str = new CHAR[512];
	GetModuleFileName(NULL, str, 512);
	_logPath = str;
	delete[] str;

	while (_logPath.back() != '\\')
		_logPath.pop_back();

	if (!std::filesystem::is_directory(_logPath))
		return _logToFile = false;

	_logPath.append("\\log.txt");
	clear(true);
	_logFile = new std::fstream(_logPath, std::ios::app);

	if (!_logFile->is_open())
		return _logToFile = false;

	_logToFile = true;
	*_logFile << "\xEF\xBB\xBF"; //write BOM
	return _logToFile;
}

MungPlex::Log::~Log()
{ 
	GetInstance()._logFile->close(); 
	delete GetInstance()._logFile;
}

void MungPlex::Log::clear(const bool deleteFileOnly)
{
	_logMessage.clear();

	if (_logToFile)
	{
		if (!deleteFileOnly)
		{
			_logFile->clear();
			_logFile->close();
		}

		if(std::filesystem::exists(_logPath))
			std::filesystem::remove(_logPath);
	}
}

void MungPlex::Log::DrawWindow()
{
	ImGui::Begin("Log");

	if (!ImGui::CollapsingHeader("Show/Hide Log"))
	{
		ImGui::End();
		return;
	}

	SetUpInputTextMultiline("", (char*)_logMessage.c_str(), _logMessage.size(), 1.0f, 0.9f, ImGuiInputTextFlags_ReadOnly);

	if (ImGui::Button("Clear Log"))
	{
		GetInstance().clear();
		GetInstance()._initialized = false;
	}

	ImGui::SameLine();

	if (ImGui::Checkbox("Log to file", &GetInstance()._logToFile))
	{

	}

	ImGui::End();
}

void MungPlex::Log::LogInformation(const char* text, const bool appendToLast, const int indentation)
{
	if (GetInstance()._logToFile)
		if (!GetInstance()._initialized)
			GetInstance()._initialized = GetInstance().init();

	std::string appendingStr;

	if (appendToLast)
	{
		if (indentation == 0)
		{
			appendingStr = " " + std::string(text);
		}
		else
		{
			appendingStr = "\n" + std::string(indentation, ' ');
		}

		_logMessage.append(appendingStr);
	}
	else
	{
		if (_logMessage.size())
			appendingStr.append("\n");
			
		std::time_t time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		appendingStr.append(std::to_string(std::localtime(&time)->tm_hour) + ':');
		appendingStr.append(std::to_string(std::localtime(&time)->tm_min) + ':');
		appendingStr.append(std::to_string(std::localtime(&time)->tm_sec) + ": ");
		appendingStr.append(std::string(text));
		_logMessage.append(appendingStr);
	}

	if (GetInstance()._logToFile)
	{
		if (GetInstance()._initialized)
			*GetInstance()._logFile << appendingStr;

		GetInstance()._logFile->flush();
	}
}


void MungPlex::Log::LogInformation(const std::string& text, const bool appendToLast, const int indentation)
{
	LogInformation(text.c_str(), appendToLast);
}