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
	DrawLabel(label.c_str(), paneWidth, labelPortion, printLabel, helpText);
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

MungPlex::InputText::InputText(const std::string& label, const std::string text, const uint64_t maxLength, const bool printLabel, const ImGuiInputTextFlags flags)
{
	_label = label;
	_id = "##" + label;
	_text = text;
	_maxLength = maxLength;
	_text.resize(_maxLength);
	_printLabel = printLabel;
	_flags = flags;
}

MungPlex::InputText::InputText(const InputText& other)
{
	assign(other);
}

MungPlex::InputText& MungPlex::InputText::operator=(const InputText& other)
{
	return *this;
}

MungPlex::InputText::InputText(InputText&& other) noexcept
{
	assign(other);
}

MungPlex::InputText& MungPlex::InputText::operator=(InputText&& other) noexcept
{
	return *this;
}

bool MungPlex::InputText::Draw(const float paneWidth, const float labelPortion)
{
	static bool edited = false;
	DrawLabel(_label.c_str(), paneWidth, labelPortion, _printLabel, _showHelpText ? _helpText.c_str() : nullptr);
	edited = ImGui::InputText(_id.c_str(), _text.data(), _maxLength, _flags);
	ImGui::PopItemWidth();
	return edited;
}

void MungPlex::InputText::SetText(const std::string& text)
{
	_text = text;
	_text.resize(_maxLength);

	for (Slot& slot : _slotsOnTextChanged)
		slot();
}

void MungPlex::InputText::SetLabel(const std::string& label)
{
	_label = label;
	_id = "##" + label;
}

void MungPlex::InputText::SetHelpText(const std::string& helpText, const bool show)
{
	_helpText = helpText;
	_showHelpText = show;
}

void MungPlex::InputText::SetShowHelpText(const bool show)
{
	_showHelpText = show;
}

void MungPlex::InputText::SetMaxLength(const uint64_t maxLength)
{
	_maxLength = maxLength;
	_text.resize(_maxLength);
}

std::string MungPlex::InputText::GetStdString() const
{
	return _text;
}

std::string MungPlex::InputText::GetStdStringNoZeros() const
{
	return _text.c_str();
}

const char* MungPlex::InputText::GetCString() const
{
	return _text.c_str();
}

char* MungPlex::InputText::GetData()
{
	return _text.data();
}

uint64_t MungPlex::InputText::GetMaxLength() const
{
	return _maxLength;
}

void MungPlex::InputText::ConnectOnTextChanged(const Slot slot)
{
	_slotsOnTextChanged.emplace_back(slot);
}

void MungPlex::InputText::assign(const InputText& other)
{
	_text = other._text;
	_helpText = other._helpText;
	_id = other._id;
	_label = other._label;
	_maxLength = other._maxLength;
	_printLabel = other._printLabel;
	_showHelpText = other._showHelpText;
	_flags = other._flags;
	_slotsOnTextChanged = other._slotsOnTextChanged;
}
