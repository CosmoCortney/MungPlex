#include "WidgetHelpers.hpp"

MungPlex::StringIdPairCombo::StringIdPairCombo(const std::vector<std::pair<std::string, int>>& pairs)
{
	_pairs = pairs;

	for (auto& pair : _pairs)
		_stringsPointers.push_back(pair.first.c_str());
}

MungPlex::StringIdPairCombo MungPlex::StringIdPairCombo::operator=(const StringIdPairCombo& other)
{
	_pairs = other._pairs;
	_stringsPointers = other._stringsPointers;
	return *this;
}

bool MungPlex::StringIdPairCombo::Draw(const std::string& label, int* currentSelect, const float paneWidth, const float labelPortion, bool printLabel, const char* helpText) const
{
	PrepareWidgetLabel(label.c_str(), paneWidth, labelPortion, printLabel, helpText);
	return ImGui::Combo(label.c_str(), currentSelect, _stringsPointers.data(), _pairs.size());
}

const char* MungPlex::StringIdPairCombo::GetCString(const uint32_t index) const
{
	return _stringsPointers[index];
}

const char* const* MungPlex::StringIdPairCombo::GetData() const
{
	return _stringsPointers.data();
}

const std::string& MungPlex::StringIdPairCombo::GetStdString(const uint32_t index) const
{
	return _pairs[index].first;
}

int MungPlex::StringIdPairCombo::GetId(const uint32_t index) const
{
	return _pairs[index].second;
}

uint32_t MungPlex::StringIdPairCombo::GetIndexById(const int id) const
{
	for (int i = 0; i < _pairs.size(); ++i)
		if (_pairs[i].second == id)
			return i;

	return 0;
}

uint32_t MungPlex::StringIdPairCombo::GetCount() const
{
	return _pairs.size();
}
