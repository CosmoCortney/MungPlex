#include "imgui.h"
#include "InputVectorText.hpp"

MungPlex::InputVectorText::InputVectorText(const std::string& label, const bool printLabel, const std::vector<std::string>& texts, const uint64_t maxLength, const ImGuiInputTextFlags textFlags)
	: IWidget(label, printLabel)
{
	_maxLength = maxLength;
	_texts = texts;
	_textFlags = textFlags;
	SetMaxLength(_maxLength);
}

MungPlex::InputVectorText::InputVectorText(const InputVectorText& other)
{
	assign(other);
}

MungPlex::InputVectorText& MungPlex::InputVectorText::operator=(const InputVectorText& other)
{
	assign(other);
	return *this;
}

MungPlex::InputVectorText::InputVectorText(InputVectorText&& other) noexcept
{
	assign(other);
}

MungPlex::InputVectorText& MungPlex::InputVectorText::operator=(InputVectorText&& other) noexcept
{
	assign(other);
	return *this;
}

bool MungPlex::InputVectorText::Draw(const float paneWidth, const float labelPortion)
{
	static bool edited = false;
	DrawLabel(_label.c_str(), paneWidth, labelPortion, _printLabel, _showHelpText ? _helpText.c_str() : nullptr);
	edited = ImGui::InputText(_id.c_str(), _texts[_selectedIndex].data(), _maxLength, _textFlags);
	ImGui::PopItemWidth();

	if (edited)
		callOnTextChangedSlots();

	return edited;
}

void MungPlex::InputVectorText::SelectByIndex(const uint64_t index)
{
	isInRange(index);
	_selectedIndex = index;
}

void MungPlex::InputVectorText::SetSelectedValue(const std::string& text)
{
	_texts[_selectedIndex] = text;
	_texts[_selectedIndex].resize(_maxLength);
	callOnTextChangedSlots();
}

void MungPlex::InputVectorText::SetValueAt(const std::string& text, const uint64_t index)
{
	isInRange(index);
	_texts[index] = text;
	_texts[index].resize(_maxLength);
	callOnTextChangedSlots();
}

std::string MungPlex::InputVectorText::GetSelectedStdString() const
{
	return _texts[_selectedIndex];
}

std::string MungPlex::InputVectorText::GetStdStringAt(const uint64_t index)
{
	isInRange(index);
	return _texts[index];
}

std::string MungPlex::InputVectorText::GetSelectedStdStringNoZeros() const
{
	return _texts[_selectedIndex].c_str();
}

std::string MungPlex::InputVectorText::GetStdStringNoZerosAt(const uint64_t index)
{
	isInRange(index);
	return _texts[index].c_str();
}

const char* MungPlex::InputVectorText::GetSelectedCString() const
{
	return _texts[_selectedIndex].c_str();
}

const char* MungPlex::InputVectorText::GetCStringAt(const uint64_t index)
{
	isInRange(index);
	return _texts[index].c_str();
}

char* MungPlex::InputVectorText::GetSelectedData()
{
	return _texts[_selectedIndex].data();
}

char* MungPlex::InputVectorText::GetDataAt(const uint64_t index)
{
	isInRange(index);
	return _texts[index].data();
}

uint64_t MungPlex::InputVectorText::GetCount() const
{
	return _texts.size();
}

void MungPlex::InputVectorText::AppendTextAt(const std::string& text, const uint64_t index)
{
	isInRange(index);
	std::string temp = _texts[index].c_str(); //handle trailing zeros
	temp.append(text);
	_texts[index] = temp;
	_texts[index].resize(_maxLength);
	callOnTextChangedSlots();
}

void MungPlex::InputVectorText::AppendSelectedText(const std::string& text)
{
	std::string temp = _texts[_selectedIndex].c_str(); //handle trailing zeros
	temp.append(text);
	_texts[_selectedIndex] = temp;
	_texts[_selectedIndex].resize(_maxLength);
	callOnTextChangedSlots();
}

void MungPlex::InputVectorText::SetLabel(const std::string& label)
{
	_label = label;
	_id = "##" + label;
}

void MungPlex::InputVectorText::SetHelpText(const std::string& helpText, const bool show)
{
	_helpText = helpText;
	_showHelpText = show;
}

void MungPlex::InputVectorText::SetShowHelpText(const bool show)
{
	_showHelpText = show;
}

void MungPlex::InputVectorText::SetMaxLength(const uint64_t maxLength)
{
	_maxLength = maxLength;

	for(auto& text : _texts)
		text.resize(_maxLength);
}

uint64_t MungPlex::InputVectorText::GetMaxLength() const
{
	return _maxLength;
}

void MungPlex::InputVectorText::PushBack(const std::string text)
{
	_texts.push_back(text);
	SetMaxLength(_maxLength);
}

void MungPlex::InputVectorText::ConnectOnTextChanged(const Slot slot)
{
	_slotsOnTextChanged.emplace_back(slot);
}

void MungPlex::InputVectorText::assign(const InputVectorText& other)
{
	_texts = other._texts;
	_helpText = other._helpText;
	_id = other._id;
	_label = other._label;
	_maxLength = other._maxLength;
	_printLabel = other._printLabel;
	_showHelpText = other._showHelpText;
	_textFlags = other._textFlags;
	_slotsOnTextChanged = other._slotsOnTextChanged;
	_selectedIndex = other._selectedIndex;
}

void MungPlex::InputVectorText::callOnTextChangedSlots()
{
	for (Slot& slot : _slotsOnTextChanged)
		slot();
}

void MungPlex::InputVectorText::isInRange(const uint64_t index)
{
	if (index >= _texts.size())
		throw "Error: Index out of range!";
}

bool MungPlex::InputVectorTextMultiline::Draw(const float width, const float height)
{
	static bool edited = false;
	static ImVec2 xy = { 0.0f, 0.0f };
	xy.x = ImGui::GetContentRegionAvail().x * width;
	xy.y = ImGui::GetContentRegionAvail().y * height;

	if (_printLabel)
		ImGui::Text(_label.c_str());

	if (_showHelpText)
	{
		if (_printLabel)
			ImGui::SameLine();

		DrawHelpMarker(_helpText.c_str());
	}

	edited = ImGui::InputTextMultiline(_id.c_str(), _texts[_selectedIndex].data(), _maxLength, xy, _textFlags);

	if (edited)
		callOnTextChangedSlots();

	return edited;
}