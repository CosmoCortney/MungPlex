#include"WatchControl.h"
#include"Helperfunctions.h"
#include"ProcessInformation.h"

const std::vector<std::pair<std::string, int>> MungPlex::WatchControl::IntegralView::s_IntTypes = 
{
	{ "Int 8", ImGuiDataType_S8 },
	{ "UInt 8", ImGuiDataType_U8 },
	{ "Int 16", ImGuiDataType_S16 },
	{ "UInt 16", ImGuiDataType_U16 },
	{ "Int 32", ImGuiDataType_S32 },
	{ "UInt 32", ImGuiDataType_U32 },
	{ "Int 64", ImGuiDataType_S64 },
	{ "UInt 64", ImGuiDataType_U64 }
};


Xertz::ProcessInfo MungPlex::WatchControl::View::s_Process;

void MungPlex::WatchControl::DrawWindow()
{
	ImGui::Begin("Watch & Control");

	GetInstance().drawList();

	ImGui::End();
}

void MungPlex::WatchControl::drawList()
{
	ImGui::BeginChild("Watch List");
	{



		if (ImGui::Button("Test"))
		{
			_views.emplace_back(INTEGRAL, IntegralView(_views.size() + 1, "801B96EC, 1E8", ImGuiDataType_S8, 0x3f800000, 0x42000000, 0, HEX | PLOTTING));
			


			/*	_views.emplace_back([]()->IntegralView
					{
						IntegralView IV;
				IV.Val = 45623;
				return IV;
					}());



				_views.emplace_back([]()->MousePiano
					{
						MousePiano MP;
				MP.Freeze = { true, false};
				return MP;
			}());



			}*/
		}

		for (int i = 0; i < _views.size(); ++i)
		{
			switch (_views[i].first)
			{
			case INTEGRAL:
				std::get<IntegralView>(_views[i].second).Draw();
				break;
			}
		}
	}
	ImGui::EndChild();
}


MungPlex::WatchControl::IntegralView::IntegralView(const int id, const std::string& pointerPathText, const uint8_t subtype, const int64_t min, const int64_t max, const int64_t val, const int32_t integralViewFlags, const std::string& module, const Xertz::ProcessInfo* process)
{
	_moduleW = MorphText::Utf8_To_Utf16LE(module);
	_moduleW.resize(32);
	_module = module;
	_module.resize(32);
	//_pointerPath = pointerPath;
	_pointerPath.resize(128);
	_useModulePath = integralViewFlags & USE_MODULEPATH;
	_plotting = integralViewFlags & PLOTTING;
	_freeze = integralViewFlags & FREEZE;
	_active = integralViewFlags & ACTIVE;
	_val = val;
	_typeSelect = subtype;
	_hex = integralViewFlags & HEX;
	_id = id;
	_idText = "##" + std::to_string(id);

	if (process != nullptr)
	{
		s_Process = *process;
	}

	std::stringstream stream;
	std::string item;
	stream << pointerPathText;

	while (std::getline(stream, item, ','))
		_pointerPath.push_back(stoll(item));

}

void MungPlex::WatchControl::IntegralView::Draw()
{
	float itemWidth = ImGui::GetContentRegionAvail().x;
	static float itemHeigh = 80.0f * Settings::GetGeneralSettings().Scale;;
	static int stepSmall = 1;
	static int stepFast = 10;
	static int stepFastHex = 16;
	std::string moduleName = _idText + "Module";
	std::string modulCheckName = "Use Module" + _idText;
	//ImGui::PushItemWidth(xy.x * 0.1f);
	//ImGui::Pushitem(xy.x * 0.1f);

	ImGui::BeginGroup();

	std::string intTypeChildName = "IntTypeChild" + _idText;
	std::string signedName = "Signed" + _idText;
	ImGui::BeginChild(intTypeChildName.c_str(), ImVec2(itemWidth * 0.15f, itemHeigh), true);
	SetUpCombo("Int Type" + _idText, s_IntTypes, _typeSelect, 1.0f);
	ImGui::Checkbox(signedName.c_str(), &_signed);
	//ImGui::PushItemWidth(widthPerElement);
	ImGui::EndChild();
	ImGui::SameLine();


	std::string pathChildName = "pathChild" + _idText;
	ImGui::BeginChild(pathChildName.c_str(), ImVec2(itemWidth * 0.2f, itemHeigh), true);
	{
		if (_useModulePath)
		{
			ImGui::BeginGroup();
				ImGui::Checkbox(modulCheckName.c_str(), &_useModulePath);
				//ImGui::PushItemWidth(widthPerElement * 0.1f);
				if (ImGui::InputText(moduleName.c_str(), _module.data(), _module.size()))
					_moduleW = MorphText::Utf8_To_Utf16LE(_module);
			ImGui::EndGroup();
			ImGui::SameLine();
		}

		ImGui::BeginGroup();
		{
			ImGui::Text("Pointer Path");
			if (!_useModulePath)
			{
				ImGui::SameLine();
				ImGui::Checkbox(modulCheckName.c_str(), &_useModulePath);
			}
			//ImGui::PushItemWidth(widthPerElement * 0.1f);
			ImGui::InputText("##io", _pointerPathText.data(), _pointerPathText.size());
		}
		ImGui::EndGroup();
	}
	ImGui::EndChild();
	ImGui::SameLine();

	std::string valueChildName = "valueChild" + _idText;
	std::string freezeValueName = "Freeze" + _idText;
	std::string hexName = "Hex" + _idText;
	std::string useSliderName = "Slider" + _idText;
	std::string scalarName = _idText + "Scalar";
	std::string plottingName = "Plotting" + _idText;

	ImGui::BeginChild(valueChildName.c_str(), ImVec2(itemWidth * 0.2f, itemHeigh), true);
		ImGui::Text("Value");
		ImGui::SameLine();
		ImGui::Checkbox(freezeValueName.c_str(), &_freeze);
		ImGui::SameLine();
		ImGui::Checkbox(hexName.c_str(), &_hex);
		ImGui::SameLine();
		ImGui::Checkbox(plottingName.c_str(), &_plotting);
		ImGui::SameLine();
		//ImGui::PushItemWidth(widthPerElement);
		ImGui::InputScalar(scalarName.c_str(), _typeSelect, &_val, &stepSmall);
	ImGui::EndChild();
	ImGui::SameLine();

	if (_plotting)
	{
		std::string plottingChildName = "childPlotting" + _idText;
		ImGui::BeginChild(valueChildName.c_str(), ImVec2(itemWidth * 0.2f, itemHeigh), true);
		ImGui::PlotHistogram("##values", _plotVals.data(), 64, 0, NULL, 0.0f, 1.0f, ImVec2(1.0f, 1.0f));
			
		ImGui::EndChild();
	}



	ImGui::EndGroup();
}

void* MungPlex::WatchControl::View::GetCurrentPointer()
{
	int64_t ptr = 0;

	if (_useModulePath)
		ptr = s_Process.GetModuleAddress(_moduleW);
	
	for (int i = 0; i < _pointerPath.size()-1; ++i)
	{
		ptr += _pointerPath[i];

		if (ProcessInformation::GetProcessType() == ProcessInformation::EMULATOR)
		{
			int regionIndex = ProcessInformation::GetRegionIndex(ptr);

			if (regionIndex > 0)
			{
				SystemRegion& region = ProcessInformation::GetSystemRegionList()[regionIndex];
				s_Process.ReadExRAM(&ptr, reinterpret_cast<char*>(region.BaseLocationProcess) + ptr - region.Base, ProcessInformation::GetAddressWidth());
			}
			else
				ptr = 0;

			break;
		}
		else
		{
			s_Process.ReadExRAM(&ptr, reinterpret_cast<void*>(ptr), ProcessInformation::GetAddressWidth());
		}
	}
	
	return reinterpret_cast<void*>(ptr + _pointerPath.back());
}