#include "Pairs.hpp"

const char* MungPlex::IPairs::GetCString(const uint32_t index) const
{
	return _stringPointers[index];
}

const char* const* MungPlex::IPairs::GetData() const
{
	return _stringPointers.data();
}

const uint64_t MungPlex::IPairs::GetCount() const
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
		throw "Error: Vectors of different sizes!";

	_stringPointers.reserve(strings.size());
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

	return _dummy;
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

void MungPlex::StringIdPairs::PopBack(const int64_t count)
{
	if (count <= 0)
		throw "Error: Cannot pop-back 0 or less elements.";

	const int64_t size = static_cast<int64_t>(_ids.size());

	for (int64_t i = size - 1; i < size - count - 1; --i)
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
	Clear();
	_regions = regions;

	for (auto& region : _regions)
		_stringPointers.push_back(region.Label.c_str());
}

void MungPlex::RegionPairs::Clear()
{
	_regions.clear();
	_stringPointers.clear();
}

MungPlex::DoubleStringIdPairs::DoubleStringIdPairs(const std::vector<std::string>& entityNames, const std::vector<std::string>& labelList, const std::vector<int32_t>& ids, const std::string& label)
{
	if (entityNames.size() != ids.size() || ids.size() != labelList.size())
		throw "Error: Vectors of different sizes!";

	_stringPointers.reserve(labelList.size());
	_entityNames = entityNames;
	_labelList = labelList;
	_ids = ids;
	setLabel(label);

	for (auto& str : _labelList)
		_stringPointers.push_back(str.c_str());
}

const std::string& MungPlex::DoubleStringIdPairs::GetStdStringLabel(const uint32_t index) const
{
	return _labelList[index];
}

const std::string& MungPlex::DoubleStringIdPairs::GetStdStringEntity(const uint32_t index) const
{
	return _entityNames[index];
}

const std::string& MungPlex::DoubleStringIdPairs::GetStdStringLabelById(const int32_t id) const
{
	for (int i = 0; i < _ids.size(); ++i)
		if (id == _ids[i])
			return _labelList[i];

	return _dummy;
}

const std::string& MungPlex::DoubleStringIdPairs::GetStdStrinEntityById(const int32_t id) const
{
	for (int i = 0; i < _ids.size(); ++i)
		if (id == _ids[i])
			return _entityNames[i];

	return _dummy;
}

const int MungPlex::DoubleStringIdPairs::GetId(const uint32_t index) const
{
	return _ids[index];
}

const int MungPlex::DoubleStringIdPairs::GetIndexById(const int id) const
{
	for (int i = 0; i < _ids.size(); ++i)
		if (_ids[i] == id)
			return i;

	return -1;
}

void MungPlex::DoubleStringIdPairs::PushBack(const std::string& entity, const std::string& label, const int id)
{
	_entityNames.push_back(entity);
	_labelList.push_back(label);
	_stringPointers.push_back(_labelList.back().c_str());
	_ids.push_back(id);
}

void MungPlex::DoubleStringIdPairs::PopBack(const int64_t count)
{
	if (count <= 0)
		throw "Error: Cannot pop-back 0 or less elements.";

	const int64_t size = static_cast<int64_t>(_ids.size());

	for (int64_t i = size - 1; i < size - count - 1; --i)
	{
		_entityNames.pop_back();
		_labelList.pop_back();
		_stringPointers.pop_back();
		_ids.pop_back();
	}
}

void MungPlex::DoubleStringIdPairs::Clear()
{
	_stringPointers.clear();
	_labelList.clear();
	_entityNames.clear();
	_ids.clear();
}

MungPlex::StringIdBoolPairs::StringIdBoolPairs(const std::vector<std::string>& strings, const std::vector<int32_t>& ids, const std::vector<bool>& flags, const std::string& label)
	: MungPlex::StringIdPairs(strings, ids, label)
{
	if (flags.size() != ids.size())
		throw "Error: Vectors of different sizes!";

	_flags = flags;
}

void MungPlex::StringIdBoolPairs::PushBack(const std::string& str, const int id, const bool flag)
{
	_strings.push_back(str);
	_stringPointers.push_back(_strings.back().c_str());
	_ids.push_back(id);
	_flags.push_back(flag);
}

void MungPlex::StringIdBoolPairs::PopBack(const int64_t count)
{
	if (count <= 0)
		throw "Error: Cannot pop-back 0 or less elements.";

	const int64_t size = static_cast<int64_t>(_ids.size());

	for (int64_t i = size - 1; i < size - count - 1; --i)
	{
		_strings.pop_back();
		_stringPointers.pop_back();
		_ids.pop_back();
		_flags.pop_back();
	}
}

bool MungPlex::StringIdBoolPairs::GetFlag(const uint32_t index) const
{
	return _flags[index];
}

void MungPlex::StringIdBoolPairs::Clear()
{
	_stringPointers.clear();
	_strings.clear();
	_ids.clear();
	_flags.clear();
}