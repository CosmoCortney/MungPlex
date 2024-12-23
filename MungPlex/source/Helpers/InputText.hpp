#pragma once
#include "IWidget.hpp"

class MungPlex::IWidget;

namespace MungPlex
{

	class InputText : public IWidget
	{
	public:
		typedef std::function<void()> Slot;
		InputText(const std::string& label, const bool printLabel = true, const std::string text = "", const uint64_t maxLength = 256, const ImGuiInputTextFlags textFlags = ImGuiInputTextFlags_None);
		InputText(const InputText& other);
		InputText& operator=(const InputText& other);
		InputText(InputText&& other) noexcept;
		InputText& operator=(InputText&& other) noexcept;

		bool Draw(const float paneWidth = 0.25f, const float labelPortion = 0.4f);
		void SetText(const std::string& text);
		void AppendText(const std::string& text);
		void SetLabel(const std::string& label);
		void SetHelpText(const std::string& helpText, const bool show = true);
		void SetShowHelpText(const bool show);
		void SetMaxLength(const uint64_t maxLength);
		std::string GetStdString() const;
		std::string GetStdStringNoZeros() const;
		const char* GetCString() const;
		char* GetData();
		uint64_t GetMaxLength() const;
		void ConnectOnTextChanged(const Slot slot);

	protected:
		std::string _text = std::string(256, '\0');
		uint64_t _maxLength = 256;
		ImGuiInputTextFlags _textFlags = ImGuiInputTextFlags_None;
		std::vector<Slot> _slotsOnTextChanged{};

		void assign(const InputText& other);
		void callOnTextChangedSlots();
	};

	class InputTextMultiline : public InputText
	{
	public:
		explicit InputTextMultiline(const std::string& label, const bool printLabel = true, const std::string text = "", const uint64_t maxLength = 256, const ImGuiInputTextFlags textFlags = ImGuiInputTextFlags_None)
			: InputText(label, printLabel, text, maxLength, textFlags) {}
		explicit InputTextMultiline(const InputTextMultiline& other) : InputText(other) {}
		explicit InputTextMultiline(InputTextMultiline&& other) noexcept : InputText(other) {}
		bool Draw(const float paneWidth = 0.25f, const float labelPortion = 0.4f);
	};
}