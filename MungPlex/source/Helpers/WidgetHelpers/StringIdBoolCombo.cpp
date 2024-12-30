#include "imgui.h"
#include "StringIdBoolCombo.hpp"

MungPlex::StringIdBoolCombo::StringIdBoolCombo(const std::string& label, const bool printLabel, const Type& stringIdBoolPairVec)
	: ICombo(label, printLabel)
{
	_stringPointers.reserve(stringIdBoolPairVec.size());
	_stringIdBoolPairVec = stringIdBoolPairVec;
	assignPointers();
}

MungPlex::StringIdBoolCombo::StringIdBoolCombo(const StringIdBoolCombo& other)
{
	assign(other);
}

MungPlex::StringIdBoolCombo& MungPlex::StringIdBoolCombo::operator=(const StringIdBoolCombo& other)
{
	assign(other);
	return *this;
}

MungPlex::StringIdBoolCombo::StringIdBoolCombo(StringIdBoolCombo&& other) noexcept
{
	assign(other);
}

MungPlex::StringIdBoolCombo& MungPlex::StringIdBoolCombo::operator=(StringIdBoolCombo&& other) noexcept
{
	assign(other);
	return *this;
}

bool MungPlex::StringIdBoolCombo::Draw(const float paneWidth, const float labelPortion)
{
	static bool indexChanged = false;
	DrawLabel(_label.c_str(), paneWidth, labelPortion, _printLabel, _showHelpText ? _helpText.c_str() : nullptr);
	return ImGui::Combo(_id.c_str(), reinterpret_cast<int*>(&_selectedIndex), _stringPointers.data(), _stringPointers.size());

	if (indexChanged)
		callOnIndexChangedSlots();

	return indexChanged;
}

const std::string& MungPlex::StringIdBoolCombo::GetStdStringAt(const uint64_t index)
{
	isInRange(index);
	return std::get<0>(_stringIdBoolPairVec[index]);
}

const std::string& MungPlex::StringIdBoolCombo::GetSelectedStdString() const
{
	return std::get<0>(_stringIdBoolPairVec[_selectedIndex]);
}

bool MungPlex::StringIdBoolCombo::GetBoolByIndex(const uint64_t index)
{
	isInRange(index);
	return std::get<bool>(_stringIdBoolPairVec[index]);
}

bool MungPlex::StringIdBoolCombo::GetSelectedBool() const
{
	return std::get<bool>(_stringIdBoolPairVec[_selectedIndex]);
}

void MungPlex::StringIdBoolCombo::SetSelectedById(const uint32_t id)
{
	for (uint64_t i = 0; i < _stringIdBoolPairVec.size(); ++i)
	{
		if (std::get<uint32_t>(_stringIdBoolPairVec[i]) == id)
		{
			SetSelectedByIndex(i);
			return;
		}
	}
}

void MungPlex::StringIdBoolCombo::SetItems(const Type& stringIdBoolPairVec)
{
	*this = StringIdBoolCombo(_label, _printLabel, stringIdBoolPairVec);
}

uint32_t MungPlex::StringIdBoolCombo::GetSelectedId() const
{
	return std::get<uint32_t>(_stringIdBoolPairVec[_selectedIndex]);
}

void MungPlex::StringIdBoolCombo::PopBack(const uint64_t count)
{
	if (count <= 0)
		throw "Error: Cannot pop-back 0 or less elements.";
	else if (count > _stringPointers.size())
		throw "Error: Cannot pop-back elements than exist.";

	const uint64_t size = _stringPointers.size();

	for (int64_t i = size; i > size - count; --i)
	{
		_stringPointers.pop_back();
		_stringIdBoolPairVec.pop_back();
	}

	if (_selectedIndex >= _stringPointers.size())
		_selectedIndex = _stringPointers.size() - 1;
}

void MungPlex::StringIdBoolCombo::Clear()
{
	_selectedIndex = 0;
	_stringPointers.clear();
	_stringIdBoolPairVec.clear();
}

void MungPlex::StringIdBoolCombo::PushBack(const std::tuple<std::string, uint32_t, bool>& stringIdPair)
{
	_stringIdBoolPairVec.push_back(stringIdPair);
	assignPointers();
}

void MungPlex::StringIdBoolCombo::assign(const StringIdBoolCombo& other)
{
	_helpText = other._helpText;
	_id = other._id;
	_label = other._label;
	_printLabel = other._printLabel;
	_showHelpText = other._showHelpText;
	_selectedIndex = other._selectedIndex;
	_slotsOnIndexChanged = other._slotsOnIndexChanged;
	_stringIdBoolPairVec = other._stringIdBoolPairVec;
	assignPointers();
}

void MungPlex::StringIdBoolCombo::assignPointers()
{
	for (const auto& elem : _stringIdBoolPairVec)
		_stringPointers.push_back(std::get<0>(elem).c_str());
}