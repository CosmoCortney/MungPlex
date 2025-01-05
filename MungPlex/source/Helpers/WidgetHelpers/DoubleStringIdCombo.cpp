#include "DoubleStringIdCombo.hpp"

MungPlex::DoubleStringIdCombo::DoubleStringIdCombo(const std::string& label, const bool printLabel, const std::vector<VecType>& doubleStringIdPairVec)
	: ICombo(label, printLabel)
{
	SetItems(doubleStringIdPairVec);
}

MungPlex::DoubleStringIdCombo::DoubleStringIdCombo(const DoubleStringIdCombo& other)
{
	assign(other);
}

MungPlex::DoubleStringIdCombo& MungPlex::DoubleStringIdCombo::operator=(const DoubleStringIdCombo& other)
{
	assign(other);
	return *this;
}

MungPlex::DoubleStringIdCombo::DoubleStringIdCombo(DoubleStringIdCombo&& other) noexcept
{
	assign(other);
}

MungPlex::DoubleStringIdCombo& MungPlex::DoubleStringIdCombo::operator=(DoubleStringIdCombo&& other) noexcept
{
	assign(other);
	return *this;
}

const std::string& MungPlex::DoubleStringIdCombo::GetStdStringAt(const uint64_t index)
{
	isInRange(index);
	return std::get<0>(_doubleStringIdPairVec[index]);
}

const std::string& MungPlex::DoubleStringIdCombo::GetSelectedStdString() const
{
	return std::get<0>(_doubleStringIdPairVec[_selectedIndex]);
}

std::string& MungPlex::DoubleStringIdCombo::GetIdentifierStdStringAt(const uint64_t index)
{
	isInRange(index);
	return std::get<1>(_doubleStringIdPairVec[index]);
}

const std::string& MungPlex::DoubleStringIdCombo::GetSelectedIdentifierStdString() const
{
	return std::get<1>(_doubleStringIdPairVec[_selectedIndex]);
}

void MungPlex::DoubleStringIdCombo::SetSelectedById(const int32_t id)
{
	for (uint64_t i = 0; i < _doubleStringIdPairVec.size(); ++i)
	{
		if (std::get<int32_t>(_doubleStringIdPairVec[i]) == id)
		{
			SetSelectedByIndex(i);
			return;
		}
	}
}

void MungPlex::DoubleStringIdCombo::SetItems(const std::vector<VecType>& doubleStringIdPairVec, const bool setIndexLast)
{
	_doubleStringIdPairVec = doubleStringIdPairVec;
	assignPointers();

	if (setIndexLast)
		SetSelectedByIndex(_doubleStringIdPairVec.size() - 1);
	else
		SetSelectedByIndex(0);
}

int32_t MungPlex::DoubleStringIdCombo::GetSelectedId() const
{
	return std::get<int32_t>(_doubleStringIdPairVec[_selectedIndex]);
}

void MungPlex::DoubleStringIdCombo::PopBack(const uint64_t count)
{
	if (count <= 0)
		throw "Error: Cannot pop-back 0 or less elements.";
	else if (count > _stringPointers.size())
		throw "Error: Cannot pop-back elements than exist.";

	const uint64_t size = _stringPointers.size();

	for (int64_t i = size; i > size - count; --i)
	{
		_stringPointers.pop_back();
		_doubleStringIdPairVec.pop_back();
	}

	if (_selectedIndex >= _stringPointers.size())
		_selectedIndex = _stringPointers.size() - 1;
}

void MungPlex::DoubleStringIdCombo::Clear()
{
	_selectedIndex = 0;
	_stringPointers.clear();
	_doubleStringIdPairVec.clear();
}

void MungPlex::DoubleStringIdCombo::PushBack(const std::tuple<std::string, std::string, int32_t>& stringIdPair)
{
	_doubleStringIdPairVec.push_back(stringIdPair);
	assignPointers();
}

int MungPlex::DoubleStringIdCombo::GetIdAt(const uint64_t index)
{
	isInRange(index);
	return std::get<int32_t>(_doubleStringIdPairVec[index]);
}

void MungPlex::DoubleStringIdCombo::assign(const DoubleStringIdCombo& other)
{
	_helpText = other._helpText;
	_id = other._id;
	_label = other._label;
	_printLabel = other._printLabel;
	_showHelpText = other._showHelpText;
	_selectedIndex = other._selectedIndex;
	_slotsOnIndexChanged = other._slotsOnIndexChanged;
	_doubleStringIdPairVec = other._doubleStringIdPairVec;
	assignPointers();
}

void MungPlex::DoubleStringIdCombo::assignPointers()
{
	_stringPointers.clear();
	_stringPointers.reserve(_doubleStringIdPairVec.size());

	for (const auto& elem : _doubleStringIdPairVec)
		_stringPointers.push_back(std::get<0>(elem).c_str());
}