#pragma once
#include "imgui.h"
#include "MemoryViewer.hpp"

namespace MungPlex
{
	class MemoryViewer;

	class ContextMenuHelper
	{
	public:
		static void DrawWindow();
		static std::vector<MemoryViewer>& GetMemoryViews();
		static void UpdateMemoryViewerList();

	private:
		ContextMenuHelper();
		~ContextMenuHelper();
		void OpenGithubLink();
		void OpenGithubWikiLink();
		void openMemoryVierwer();
		void drawViewMenuItems();

		static ContextMenuHelper& GetInstance() {
			static ContextMenuHelper Instance;
			return Instance;
		}

		uint32_t _memViewerCount = 0;
		std::vector<MemoryViewer> _memoryViewers;
	};
}