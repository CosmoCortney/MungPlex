#include "StringIdCombo.hpp"

MungPlex::StringIdCombo::StringIdCombo(const std::string& label, const bool printLabel, const std::vector<VecType>& stringIdPairVec)
	: ICombo(label, printLabel)
{
	SetItems(stringIdPairVec);
}

MungPlex::StringIdCombo::StringIdCombo(const StringIdCombo& other)
{
	assign(other);
}

MungPlex::StringIdCombo& MungPlex::StringIdCombo::operator=(const StringIdCombo& other)
{
	assign(other);
	return *this;
}

MungPlex::StringIdCombo::StringIdCombo(StringIdCombo&& other) noexcept
{
	assign(other);
}

MungPlex::StringIdCombo& MungPlex::StringIdCombo::operator=(StringIdCombo&& other) noexcept
{
	assign(other);
	return *this;
}

const std::string& MungPlex::StringIdCombo::GetStdStringAt(const uint64_t index)
{
	isInRange(index);
	return _stringIdPairVec[index].first;
}

const std::string& MungPlex::StringIdCombo::GetSelectedStdString() const
{
	return _stringIdPairVec[_selectedIndex].first;
}

void MungPlex::StringIdCombo::SetSelectedById(const uint64_t id)
{
	for (uint64_t i = 0; i < _stringIdPairVec.size(); ++i)
	{
		if (_stringIdPairVec[i].second == id)
		{
			SetSelectedByIndex(i);
			return;
		}
	}
}

void MungPlex::StringIdCombo::SetItems(const std::vector<VecType>& stringIdPairVec)
{
	_stringPointers.reserve(stringIdPairVec.size());
	_stringIdPairVec = stringIdPairVec;
	assignPointers();
	SetSelectedByIndex(0);
}

int32_t MungPlex::StringIdCombo::GetSelectedId() const
{
	return _stringIdPairVec[_selectedIndex].second;
}

void MungPlex::StringIdCombo::PopBack(const uint64_t count)
{
	if (count <= 0)
		throw "Error: Cannot pop-back 0 or less elements.";
	else if (count > _stringPointers.size())
		throw "Error: Cannot pop-back elements than exist.";

	const uint64_t size = _stringPointers.size();

	for (int64_t i = size; i > size - count; --i)
	{
		_stringPointers.pop_back();
		_stringIdPairVec.pop_back();
	}

	if (_selectedIndex >= _stringPointers.size())
		_selectedIndex = _stringPointers.size() - 1;
}

void MungPlex::StringIdCombo::Clear()
{
	_selectedIndex = 0;
	_stringPointers.clear();
	_stringIdPairVec.clear();
}

void MungPlex::StringIdCombo::PushBack(const VecType& stringIdPair)
{
	_stringIdPairVec.push_back(stringIdPair);
	_stringPointers.push_back(_stringIdPairVec.back().first.c_str());
}

void MungPlex::StringIdCombo::assign(const StringIdCombo& other)
{
	_helpText = other._helpText;
	_id = other._id;
	_label = other._label;
	_printLabel = other._printLabel;
	_showHelpText = other._showHelpText;
	_selectedIndex = other._selectedIndex;
	_slotsOnIndexChanged = other._slotsOnIndexChanged;
	_stringIdPairVec = other._stringIdPairVec;
	assignPointers();
}

void MungPlex::StringIdCombo::assignPointers()
{
	for (auto& elem : _stringIdPairVec)
		_stringPointers.push_back(elem.first.c_str());
}