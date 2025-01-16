#include "ICombo.hpp"
#include "imgui.h"

bool MungPlex::ICombo::Draw(const float paneWidth, const float labelPortion)
{
	static bool indexChanged = false;
	DrawLabel(_label.c_str(), paneWidth, labelPortion, _printLabel, _showHelpText ? _helpText.c_str() : nullptr);
	indexChanged = ImGui::Combo(_id.c_str(), reinterpret_cast<int*>(&_selectedIndex), _stringPointers.data(), _stringPointers.size());

	if (indexChanged)
		callOnIndexChangedSlots();

	return indexChanged;
}

const uint64_t MungPlex::ICombo::GetCount() const
{
	return _stringPointers.size();
}

const uint64_t MungPlex::ICombo::GetSelectedIndex() const
{
	return _selectedIndex;
}

void MungPlex::ICombo::SetSelectedByIndex(const uint64_t index)
{
	isInRange(index);
	_selectedIndex = index;
	callOnIndexChangedSlots();
}

const char* MungPlex::ICombo::GetCStringAt(const uint64_t index)
{
	isInRange(index);
	return _stringPointers[index];
}

const char* MungPlex::ICombo::GetSelectedCString() const
{
	return _stringPointers[_selectedIndex];
}

const char* const* MungPlex::ICombo::GetCStringPointers() const
{
	return _stringPointers.data();
}

void MungPlex::ICombo::ConnectOnIndexChangedSlot(const Slot slot)
{
	_slotsOnIndexChanged.emplace_back(slot);
}

void MungPlex::ICombo::isInRange(const uint64_t index)
{
	if (index >= _stringPointers.size())
		throw "Error: Index out of range!";
}

void MungPlex::ICombo::callOnIndexChangedSlots()
{
	for (Slot& slot : _slotsOnIndexChanged)
		slot();
}