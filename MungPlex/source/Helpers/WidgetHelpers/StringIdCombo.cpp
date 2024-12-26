#include "imgui.h"
#include "StringIdCombo.hpp"

MungPlex::StringIdCombo::StringIdCombo(const std::string& label, const bool printLabel, const std::vector<std::pair<std::string, uint32_t>>& stringIdPairVec)
	: ICombo(label, printLabel)
{
	_stringPointers.reserve(stringIdPairVec.size());
	_stringIdPairVec = stringIdPairVec;

	for (auto& item : _stringIdPairVec)
		_stringPointers.push_back(item.first.c_str());
}

bool MungPlex::StringIdCombo::Draw(const float paneWidth, const float labelPortion)
{
	static bool indexChanged = false;
	DrawLabel(_label.c_str(), paneWidth, labelPortion, _printLabel, _showHelpText ? _helpText.c_str() : nullptr);
	return ImGui::Combo(_id.c_str(), reinterpret_cast<int*>(&_selectedIndex), _stringPointers.data(), _stringPointers.size());

	if (indexChanged)
		callOnIndexChangedSlots();

	return indexChanged;
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

uint32_t MungPlex::StringIdCombo::GetSelectedId() const
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

void MungPlex::StringIdCombo::PushBack(const std::pair<std::string, uint32_t>& stringIdPair)
{
	_stringIdPairVec.push_back(stringIdPair);
	_stringPointers.push_back(_stringIdPairVec.back().first.c_str());
}