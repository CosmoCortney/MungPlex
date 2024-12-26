#include "imgui.h"
#include "InputText.hpp"

MungPlex::InputText::InputText(const std::string& label, const bool printLabel, const std::string text, const uint64_t maxLength, const ImGuiInputTextFlags textFlags)
	: IWidget(label, printLabel)
{
	_maxLength = maxLength;
	_text = text;
	_text.resize(_maxLength);
	_textFlags = textFlags;
}

MungPlex::InputText::InputText(const InputText& other)
{
	assign(other);
}

MungPlex::InputText& MungPlex::InputText::operator=(const InputText& other)
{
	assign(other);
	return *this;
}

MungPlex::InputText::InputText(InputText&& other) noexcept
{
	assign(other);
}

MungPlex::InputText& MungPlex::InputText::operator=(InputText&& other) noexcept
{
	assign(other);
	return *this;
}

bool MungPlex::InputText::Draw(const float paneWidth, const float labelPortion)
{
	static bool edited = false;
	DrawLabel(_label.c_str(), paneWidth, labelPortion, _printLabel, _showHelpText ? _helpText.c_str() : nullptr);
	edited = ImGui::InputText(_id.c_str(), _text.data(), _maxLength, _textFlags);
	ImGui::PopItemWidth();

	if (edited)
		callOnTextChangedSlots();

	return edited;
}

void MungPlex::InputText::SetText(const std::string& text)
{
	_text = text;
	_text.resize(_maxLength);
	callOnTextChangedSlots();
}

void MungPlex::InputText::AppendText(const std::string& text)
{
	std::string temp = _text.c_str(); //handle trailing zeros
	temp.append(text);
	_text = temp;
	_text.resize(_maxLength);
	callOnTextChangedSlots();
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
	_textFlags = other._textFlags;
	_slotsOnTextChanged = other._slotsOnTextChanged;
}

void MungPlex::InputText::callOnTextChangedSlots()
{
	for (Slot& slot : _slotsOnTextChanged)
		slot();
}

bool MungPlex::InputTextMultiline::Draw(const float width, const float height)
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

	edited = ImGui::InputTextMultiline(_id.c_str(), _text.data(), _maxLength, xy, _textFlags);

	if (edited)
		callOnTextChangedSlots();

	return edited;
}