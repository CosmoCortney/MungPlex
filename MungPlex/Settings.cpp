#include"Settings.h"

MungPlex::Settings::Settings()
{
	std::ifstream inFile;
	inFile.open(SettingsJSON);
	std::string buffer;
	std::string jsonstr;

	if (inFile)
	{
		while (std::getline(inFile, buffer))
		{
			jsonstr.append(buffer).append("\n");
		}
	}
	inFile.close();

	try
	{
		auto doc = nlohmann::json::parse(jsonstr);
		auto& settings = doc["Settings"];
		strcpy(_documentsPath, settings[0]["General"][0]["DocumentsPath"].get<std::string>().c_str());
		_scale = settings[0]["General"][0]["Scale"].get<float>();
		
		if (!std::filesystem::is_directory(_documentsPath))
		{
			PWSTR tmp = new wchar_t[512];
			SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &tmp);
			strcpy(_documentsPath, MorphText::Utf16LE_To_Utf8(tmp).c_str());
			CoTaskMemFree(tmp);
		}
	}
	catch (const nlohmann::json::parse_error& exception)
	{
		std::cerr << "Failed parsing settings file: " << exception.what() << std::endl;
		return;
	}
}


void MungPlex::Settings::DrawWindow()
{
	ImGui::Begin("Settings");
	
	GetInstance().drawGeneralSettings();
	GetInstance().drawSearchSettings();
	GetInstance().drawCheatSettings();

	ImGui::Button("Save");
	ImGui::SameLine();
	ImGui::Button("Default");
	ImGui::End();
}

void MungPlex::Settings::drawGeneralSettings()
{
	float groupWidth = ImGui::GetContentRegionAvail().x / _scale;
	ImGui::PushItemWidth(groupWidth);
	ImGui::SeparatorText("General Settings");
	ImGui::BeginChild("child", ImVec2(groupWidth, groupWidth), true);

	ImGui::InputText("Documents Path", GetInstance()._documentsPath, 512);

	ImGui::SliderFloat("UI Scale", &_scale, 1.0f, 3.0f);

	ImGui::EndChild();

}

void MungPlex::Settings::drawSearchSettings()
{
	static bool dummy;
	static int dummyi;
	float groupWidth = ImGui::GetContentRegionAvail().x / _scale;
	ImGui::BeginChild("child", ImVec2(groupWidth, groupWidth), true);
	ImGui::SeparatorText("Search Settings");
	ImGui::LabelText("label1", "Default Selected Data Type");
	ImGui::SameLine();
	//MungPlex::SetUpCombo("#DataTypeComb", std::vector<int>(0), 0 ));
	ImGui::Checkbox("Signed values by default", &dummy);

	ImGui::SameLine();
	ImGui::Checkbox("Case Senstive by default", &dummy);

	ImGui::SameLine();
	ImGui::Checkbox("Color Wheel by default", &dummy);

	ImGui::InputInt("Default Alignment", &dummyi);

	ImGui::SameLine();
	ImGui::Checkbox("Values are hex by default", &dummy);

	ImGui::SameLine();
	ImGui::Checkbox("Cached Searches by default", &dummy);

	ImGui::EndChild();
}

void MungPlex::Settings::drawCheatSettings()
{
	float groupWidth = ImGui::GetContentRegionAvail().x / _scale;
	ImGui::BeginChild("child", ImVec2(groupWidth, groupWidth), true);
	ImGui::SeparatorText("Cheats Settings");
	ImGui::EndChild();
}