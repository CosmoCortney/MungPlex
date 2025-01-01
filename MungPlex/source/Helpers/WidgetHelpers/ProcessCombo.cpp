#include "ProcessCombo.hpp"

MungPlex::ProcessCombo::ProcessCombo(const std::string& label, const bool printLabel, const bool isAppliction)
	: ICombo(label, printLabel)
{
	_isApplication = isAppliction;
}

MungPlex::ProcessCombo::ProcessCombo(const ProcessCombo& other)
{
	assign(other);
}

MungPlex::ProcessCombo& MungPlex::ProcessCombo::operator=(const ProcessCombo& other)
{
	assign(other);
	return *this;
}

MungPlex::ProcessCombo::ProcessCombo(ProcessCombo&& other) noexcept
{
	assign(other);
}

MungPlex::ProcessCombo& MungPlex::ProcessCombo::operator=(ProcessCombo&& other) noexcept
{
	assign(other);
	return *this;
}

void MungPlex::ProcessCombo::RefreshProcessInfo()
{
	if(_isApplication)
		Xertz::SystemInfo::RefreshApplicationProcessInfoList();
	else
		Xertz::SystemInfo::RefreshProcessInfoList();

	assignPointers();
}

const PROCESS_INFO& MungPlex::ProcessCombo::GetSelectedProcess() const
{
	if (_isApplication)
		return Xertz::SystemInfo::GetApplicationProcessInfoList()[_selectedIndex];

	return Xertz::SystemInfo::GetProcessInfoList()[_selectedIndex];
}

const PROCESS_INFO& MungPlex::ProcessCombo::GetProcessAt(const uint64_t index)
{
	isInRange(index);

	if (_isApplication)
		return Xertz::SystemInfo::GetApplicationProcessInfoList()[index];

	return Xertz::SystemInfo::GetProcessInfoList()[index];
}

const std::string& MungPlex::ProcessCombo::GetStdStringAt(const uint64_t index)
{
	isInRange(index);
	
	if (_isApplication)
		return Xertz::SystemInfo::GetApplicationProcessInfoList()[index].GetProcessName();

	return Xertz::SystemInfo::GetProcessInfoList()[index].GetProcessName();
}

const std::string& MungPlex::ProcessCombo::GetSelectedStdString() const
{
	if (_isApplication)
		return Xertz::SystemInfo::GetApplicationProcessInfoList()[_selectedIndex].GetProcessName();

	return Xertz::SystemInfo::GetProcessInfoList()[_selectedIndex].GetProcessName();
}

int32_t MungPlex::ProcessCombo::GetSelectedId() const
{
	if (_isApplication)
		return Xertz::SystemInfo::GetApplicationProcessInfoList()[_selectedIndex].GetPID();

	return Xertz::SystemInfo::GetProcessInfoList()[_selectedIndex].GetPID();
}

void MungPlex::ProcessCombo::PopBack(const uint64_t count)
{
	throw "Error: Unsupported override function called.";
}

void MungPlex::ProcessCombo::Clear()
{
	throw "Error: Unsupported override function called.";
}

void MungPlex::ProcessCombo::assign(const ProcessCombo& other)
{
	_helpText = other._helpText;
	_id = other._id;
	_label = other._label;
	_printLabel = other._printLabel;
	_showHelpText = other._showHelpText;
	_selectedIndex = other._selectedIndex;
	_slotsOnIndexChanged = other._slotsOnIndexChanged;
	RefreshProcessInfo();
}

void MungPlex::ProcessCombo::assignPointers()
{
	_stringPointers.clear();

	if (_isApplication)
	{
		for (auto& processInfo : Xertz::SystemInfo::GetApplicationProcessInfoList())
			_stringPointers.push_back(processInfo.GetProcessName().c_str());

		return;
	}
		
	for (auto& processInfo : Xertz::SystemInfo::GetProcessInfoList())
		_stringPointers.push_back(processInfo.GetProcessName().c_str());
}