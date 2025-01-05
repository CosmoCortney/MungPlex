#include "HelperFunctions.hpp"
#include "RegionCombo.hpp"

MungPlex::RegionCombo::RegionCombo(const std::string& label, const bool printLabel, const std::vector<SystemRegion>& regionVec)
	: ICombo(label, printLabel)
{
	SetItems(regionVec);
}

MungPlex::RegionCombo::RegionCombo(const RegionCombo& other)
{
	assign(other);
}

MungPlex::RegionCombo& MungPlex::RegionCombo::operator=(const RegionCombo& other)
{
	assign(other);
	return *this;
}

MungPlex::RegionCombo::RegionCombo(RegionCombo&& other) noexcept
{
	assign(other);
}

MungPlex::RegionCombo& MungPlex::RegionCombo::operator=(RegionCombo&& other) noexcept
{
	assign(other);
	return *this;
}

const MungPlex::SystemRegion& MungPlex::RegionCombo::GetRegionAt(const uint64_t index)
{
	isInRange(index);
	return _regionVec[index];
}

const MungPlex::SystemRegion& MungPlex::RegionCombo::GetSelectedRegion() const
{
	return _regionVec[_selectedIndex];
}

void MungPlex::RegionCombo::SetItems(const std::vector<SystemRegion>& regionVec, const bool setIndexLast)
{
	_regionVec = regionVec;
	assignPointers();

	if (setIndexLast)
		SetSelectedByIndex(_stringPointers.size() - 1);
	else
		SetSelectedByIndex(0);
}

void MungPlex::RegionCombo::PopBack(const uint64_t count)
{
	if (count <= 0)
		throw "Error: Cannot pop-back 0 or less elements.";
	else if (count > _stringPointers.size())
		throw "Error: Cannot pop-back elements than exist.";

	const uint64_t size = _stringPointers.size();

	for (int64_t i = size; i > size - count; --i)
	{
		_stringPointers.pop_back();
		_regionVec.pop_back();
	}

	if (_selectedIndex >= _stringPointers.size())
		_selectedIndex = _stringPointers.size() - 1;
}

void MungPlex::RegionCombo::Clear()
{
	_selectedIndex = 0;
	_stringPointers.clear();
	_regionVec.clear();
}

void MungPlex::RegionCombo::PushBack(const SystemRegion& stringIdPair)
{
	_regionVec.push_back(stringIdPair);
	_stringPointers.push_back(_regionVec.back().Label.c_str());
}

void MungPlex::RegionCombo::assign(const RegionCombo& other)
{
	_helpText = other._helpText;
	_id = other._id;
	_label = other._label;
	_printLabel = other._printLabel;
	_showHelpText = other._showHelpText;
	_selectedIndex = other._selectedIndex;
	_slotsOnIndexChanged = other._slotsOnIndexChanged;
	_regionVec = other._regionVec;
	assignPointers();
}

void MungPlex::RegionCombo::assignPointers()
{
	_stringPointers.clear();
	_stringPointers.reserve(_regionVec.size());

	for (auto& elem : _regionVec)
		_stringPointers.push_back(elem.Label.c_str());
}