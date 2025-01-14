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

const int32_t MungPlex::StringIdCombo::GetIdAt(const uint64_t index)
{
	isInRange(index);
	return _stringIdPairVec[index].second;
}

const std::string& MungPlex::StringIdCombo::GetSelectedStdString() const
{
	return _stringIdPairVec[_selectedIndex].first;
}

std::vector<int32_t> MungPlex::StringIdCombo::GetAllIds() const
{
	std::vector<int32_t> vec;

	for (const auto& item : _stringIdPairVec)
		vec.push_back(item.second);

	return vec;
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

void MungPlex::StringIdCombo::SetItems(const std::vector<VecType>& stringIdPairVec, const bool setIndexLast)
{
	_stringIdPairVec = stringIdPairVec;
	assignPointers();

	if(setIndexLast)
		SetSelectedByIndex(_stringIdPairVec.size() - 1);
	else
		SetSelectedByIndex(0);
}

void MungPlex::StringIdCombo::SetItemAt(const VecType& item, const uint64_t index)
{
	isInRange(index);
	_stringIdPairVec[index] = item;
	assignPointers();
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
	assignPointers();
}

void MungPlex::StringIdCombo::DeleteItemAt(const int64_t index)
{
	isInRange(index);
	_stringIdPairVec.erase(_stringIdPairVec.begin() + index);
	assignPointers();

	if (_selectedIndex >= _stringIdPairVec.size())
		_selectedIndex = _stringIdPairVec.size() - 1;
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
	_stringPointers.clear();
	_stringPointers.reserve(_stringIdPairVec.size());

	for (auto& elem : _stringIdPairVec)
		_stringPointers.push_back(elem.first.c_str());
}