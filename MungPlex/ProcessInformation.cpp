#include "ProcessInformation.h"

void MungPlex::ProcessInformation::DrawWindow()
{
	ImGui::Begin("Process Information");
	
	DrawMiscInformation();
	ImGui::Separator();
	DrawModuleList();
	ImGui::Separator();
	DrawRegionList();

	ImGui::End();
}

void MungPlex::ProcessInformation::RefreshData(int PID)
{
	GetInstance()._currentPID = PID;
	GetInstance()._modules = Xertz::SystemInfo::GetProcessInfo(PID).GetModuleList();
	GetInstance()._regions = Xertz::SystemInfo::GetProcessInfo(PID).GetRegionList();
}

void MungPlex::ProcessInformation::DrawModuleList()
{
	if (!ImGui::CollapsingHeader("Modules"))
		return;

	enum ContentsType { CT_Text, CT_FillButton };
	static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY;
	static bool display_headers = true;
	static int contents_type = CT_Text;
	std::wstring currentModule;

	if (!ImGui::BeginTable("Modules", 2, flags))
		return;
	
	ImGui::TableSetupColumn("Module");
	ImGui::TableSetupColumn("Base Address");
	ImGui::TableHeadersRow();

	currentModule.reserve(256);

	for (int row = 0; row < GetInstance()._modules.size(); ++row)
	{
		ImGui::TableNextRow();
		for (int column = 0; column < 2; ++column)
		{
			currentModule = GetInstance()._modules[row].first;
			ImGui::TableSetColumnIndex(column);
			char buf[256];

			if (column == 0)
			{
				sprintf(buf, std::string(currentModule.begin(), currentModule.end()).c_str());
			}
			else
			{
				sprintf(buf, "%llX", GetInstance()._modules[row].second);
			}

			if (contents_type == CT_Text)
				ImGui::TextUnformatted(buf);
			else if (contents_type == CT_FillButton)
				ImGui::Button(buf, ImVec2(-FLT_MIN, 0.0f));
		}
	}

	ImGui::EndTable();
}


void MungPlex::ProcessInformation::DrawRegionList()
{
	if (!ImGui::CollapsingHeader("Regions"))
		return;

	enum ContentsType { CT_Text, CT_FillButton };
	static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY;
	static bool display_headers = true;
	static int contents_type = CT_Text;
	std::wstring currentModule;
	
	if (!ImGui::BeginTable("Regions", 8, flags))
		return;

	ImGui::TableSetupColumn("Base Address (hex)");
	ImGui::TableSetupColumn("Allocation Address (hex)");
	ImGui::TableSetupColumn("Allocation Protect (hex)");
	ImGui::TableSetupColumn("Partition ID");
	ImGui::TableSetupColumn("Protection");
	ImGui::TableSetupColumn("Size");
	ImGui::TableSetupColumn("State");
	ImGui::TableSetupColumn("Type");
	ImGui::TableHeadersRow();

	currentModule.reserve(256);

	for (int row = 0; row < GetInstance()._regions.size(); ++row)
	{
		ImGui::TableNextRow();
		for (int column = 0; column < 8; ++column)
		{
			std::stringstream stream;
			ImGui::TableSetColumnIndex(column);
			char buf[256];

			switch(column)
			{
			case 0:
				sprintf(buf, "%llX", GetInstance()._regions[row].GetBaseAddress<uint64_t>());
				break;
			case 1:
				sprintf(buf, "%llX", GetInstance()._regions[row].GetAllocationBase<uint64_t>());
				break;
			case 2:
				sprintf(buf, "%u", GetInstance()._regions[row].GetAllocationProtect());
				break;
			case 3:
				sprintf(buf, "%u", GetInstance()._regions[row].GetPartitionId());
				break;
			case 4:
				sprintf(buf, "%llX", GetInstance()._regions[row].GetProtect());
				break;
			case 5:
				sprintf(buf, "%llX", GetInstance()._regions[row].GetRegionSize());
				break;
			case 6:
				sprintf(buf, "%llX", GetInstance()._regions[row].GetState());
				break;
			case 7:
				sprintf(buf, "%llX", GetInstance()._regions[row].GetType());
				break;
			}

			if (contents_type == CT_Text)
				ImGui::TextUnformatted(buf);
			else if (contents_type == CT_FillButton)
				ImGui::Button(buf, ImVec2(-FLT_MIN, 0.0f));
		}
	}

	ImGui::EndTable();
}

void MungPlex::ProcessInformation::DrawMiscInformation()
{
	char buf[2048];
	ImGui::Text("Misc. Information");

	ImGui::BeginGroup();

	std::wstring wstrTemp = Xertz::SystemInfo::GetProcessInfo(GetInstance()._currentPID).GetFilePath().c_str();
	strcpy(buf, std::string(wstrTemp.begin(), wstrTemp.end()).c_str());
	ImGui::InputText("Path", buf, IM_ARRAYSIZE(buf));

	strcpy(buf, std::to_string(GetInstance()._currentPID).c_str());
	ImGui::InputText("Process ID (dec)", buf, IM_ARRAYSIZE(buf));

	std::string strTemp = Xertz::SystemInfo::GetProcessInfo(GetInstance()._currentPID).IsX64() ? "Yes" : "No";
	strcpy(buf, strTemp.c_str());
	ImGui::InputText("Is x64", buf, IM_ARRAYSIZE(buf));

	ImGui::EndGroup();
}




