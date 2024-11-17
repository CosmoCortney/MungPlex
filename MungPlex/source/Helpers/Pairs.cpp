#include "Pairs.hpp"
#include "Xertz.hpp"

const char* MungPlex::IPairs::GetCString(const uint32_t index) const
{
	return _stringPointers[index];
}

const char* const* MungPlex::IPairs::GetData() const
{
	return _stringPointers.data();
}

const uint32_t MungPlex::IPairs::GetCount() const
{
	return _stringPointers.size();
}

const std::string& MungPlex::IPairs::GetLabelStdString() const
{
	return _label;
}

const char* MungPlex::IPairs::GetLabelCString() const
{
	return _label.c_str();;
}

const std::string& MungPlex::IPairs::GetComboLabelStdString() const
{
	return _comboLabel;
}

const char* MungPlex::IPairs::GetComboLabelCString() const
{
	return _comboLabel.c_str();
}

void MungPlex::IPairs::setLabel(const std::string label)
{
	_label = label;
	_comboLabel = "##" + label;
}

MungPlex::StringIdPairs::StringIdPairs(const std::vector<std::string>& strings, const std::vector<int32_t>& ids, const std::string& label)
{
	if (strings.size() != ids.size())
		throw "Error: Vectors of different count!";

	_strings = strings;
	_ids = ids;
	setLabel(label);

	for (auto& str : _strings)
		_stringPointers.push_back(str.c_str());
}

const std::string& MungPlex::StringIdPairs::GetStdString(const uint32_t index) const
{
	return _strings[index];
}

const std::string& MungPlex::StringIdPairs::GetStdStringById(const int32_t id) const
{
	for (int i = 0; i < _ids.size(); ++i)
		if (id == _ids[i])
			return _strings[i];

	return "";
}

const int MungPlex::StringIdPairs::GetId(const uint32_t index) const
{
	return _ids[index];
}

const int MungPlex::StringIdPairs::GetIndexById(const int id) const
{
	for (int i = 0; i < _ids.size(); ++i)
		if (_ids[i] == id)
			return i;

	return -1;
}

void MungPlex::StringIdPairs::PushBack(const std::string& str, const int id)
{
	_strings.push_back(str);
	_stringPointers.push_back(_strings.back().c_str());
	_ids.push_back(id);
}

void MungPlex::StringIdPairs::PopBack(const uint32_t count)
{
	for (int i = _ids.size() - 1; i < _ids.size() - count - 1; --i)
	{
		_strings.pop_back();
		_stringPointers.pop_back();
		_ids.pop_back();
	}
}

void MungPlex::StringIdPairs::Clear()
{
	_stringPointers.clear();
	_strings.clear();
	_ids.clear();
}

MungPlex::ProcessInforPairs::ProcessInforPairs(const std::string& label)
{
	setLabel(label);
	RefreshProcessInfo();
}

void MungPlex::ProcessInforPairs::RefreshProcessInfo()
{
	_stringPointers.clear();
	Xertz::SystemInfo::RefreshProcessInfoList();

	for (auto& processInfo : Xertz::SystemInfo::GetProcessInfoList())
		_stringPointers.push_back(processInfo.GetProcessName().c_str());
}

MungPlex::ProcessApplicationInforPairs::ProcessApplicationInforPairs(const std::string& label)
{
	setLabel(label);
	RefreshApplicationProcessInfo();
}

void MungPlex::ProcessApplicationInforPairs::RefreshApplicationProcessInfo()
{
	_stringPointers.clear();
	Xertz::SystemInfo::RefreshApplicationProcessInfoList();

	for (auto& processInfo : Xertz::SystemInfo::GetApplicationProcessInfoList())
		_stringPointers.push_back(processInfo.GetProcessName().c_str());
}
/*
MungPlex::RegionPairs::RegionPairs(const std::string& label)
{
	setLabel(label);
}

const MungPlex::SystemRegion& MungPlex::RegionPairs::GetRegion(const uint32_t index) const
{
	return _regions[index];
}

const std::vector<MungPlex::SystemRegion>& MungPlex::RegionPairs::GetRegions()
{
	return _regions;
}

void MungPlex::RegionPairs::SetRegions(const std::vector<SystemRegion>& regions)
{
	_regions = regions;
}
*/