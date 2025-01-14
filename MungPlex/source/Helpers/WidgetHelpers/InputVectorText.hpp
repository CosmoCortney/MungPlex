#pragma once
#include "IWidget.hpp"

class MungPlex::IWidget;

namespace MungPlex
{

	class InputVectorText : public IWidget
	{
	public:
		typedef std::function<void()> Slot;
		InputVectorText(const std::string& label, const bool printLabel = true, const std::vector<std::string>& texts = {}, const uint64_t maxLength = 256, const ImGuiInputTextFlags textFlags = ImGuiInputTextFlags_None);
		InputVectorText(const InputVectorText& other);
		InputVectorText& operator=(const InputVectorText& other);
		InputVectorText(InputVectorText&& other) noexcept;
		InputVectorText& operator=(InputVectorText&& other) noexcept;

		bool Draw(const float paneWidth = 0.25f, const float labelPortion = 0.4f);
		void SelectByIndex(const uint64_t index);
		void SetSelectedValue(const std::string& text);
		void SetValueAt(const std::string& text, const uint64_t index);
		std::string GetSelectedStdString() const;
		std::string GetStdStringAt(const uint64_t index);
		std::string GetSelectedStdStringNoZeros() const;
		std::string GetStdStringNoZerosAt(const uint64_t index);
		const char* GetSelectedCString() const;
		const char* GetCStringAt(const uint64_t index);
		std::vector<std::string> GetAllStdString() const;
		std::vector<std::string> GetAllStdStringNoZeros() const;
		char* GetSelectedData();
		char* GetDataAt(const uint64_t index);
		uint64_t GetCount() const;
		void AppendTextAt(const std::string& text, const uint64_t index);
		void AppendSelectedText(const std::string& text);
		void SetLabel(const std::string& label);
		void SetHelpText(const std::string& helpText, const bool show = true);
		void SetShowHelpText(const bool show);
		void SetMaxLength(const uint64_t maxLength);
		uint64_t GetMaxLength() const;
		void PushBack(const std::string text);
		void ConnectOnTextChanged(const Slot slot);
		void Clear();
		void DeleteItemAt(const uint64_t index);

	protected:
		std::vector<std::string> _texts;
		uint64_t _selectedIndex = 0;
		uint64_t _maxLength = 256;
		ImGuiInputTextFlags _textFlags = ImGuiInputTextFlags_None;
		std::vector<Slot> _slotsOnTextChanged{};

		void assign(const InputVectorText& other);
		void callOnTextChangedSlots();
		void isInRange(const uint64_t index);
	};

	class InputVectorTextMultiline : public InputVectorText
	{
	public:
		explicit InputVectorTextMultiline(const std::string& label, const bool printLabel = true, const std::vector<std::string>& text = {}, const uint64_t maxLength = 256, const ImGuiInputTextFlags textFlags = ImGuiInputTextFlags_None)
			: InputVectorText(label, printLabel, text, maxLength, textFlags) {}
		explicit InputVectorTextMultiline(const InputVectorTextMultiline& other) : InputVectorText(other) {}
		explicit InputVectorTextMultiline(InputVectorTextMultiline&& other) noexcept : InputVectorText(other) {}
		bool Draw(const float paneWidth = 0.25f, const float labelPortion = 0.4f);
	};
}