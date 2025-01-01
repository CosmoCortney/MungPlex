#pragma once
#include <boost/asio.hpp>
#include "imgui.h"
#include "MemoryViewer.hpp"

namespace MungPlex
{
	class MemoryViewer;

	class ContextMenu
	{
	public:
		static void DrawWindow();
		static std::vector<MemoryViewer>& GetMemoryViews();
		static void UpdateMemoryViewerList();

	private:
		ContextMenu() = default;
		~ContextMenu() = default;
		void openMemoryVierwer();
		void drawViewMenuItems();
		void drawHelpMenuItems();
		void openWebPage(const std::string& url);

		static ContextMenu& GetInstance() {
			static ContextMenu Instance;
			return Instance;
		}

		uint32_t _memViewerCount = 0;
		std::vector<MemoryViewer> _memoryViewers;
	};
}