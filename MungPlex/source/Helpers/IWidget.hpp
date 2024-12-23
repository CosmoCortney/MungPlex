#pragma once
#include <functional>
#include <string>

namespace MungPlex
{
	class IWidget
	{
	public:
		IWidget() = default;
		IWidget(const std::string& label, const bool printLabel = true);
		void SetLabel(const std::string& label);
		void SetHelpText(const std::string& helpText, const bool show = true);
		void SetShowHelpText(const bool show);
		typedef std::function<void()> Slot;

	protected:
		std::string _helpText = "";
		std::string _id = "";
		std::string _label = "";
		bool _printLabel = true;
		bool _showHelpText = false;

		void DrawHelpMarker(const char* desc);
		void DrawLabel(const char* label, const float paneWidth, const float labelPortion, bool printLabel, const char* helpText = nullptr);
	};
}