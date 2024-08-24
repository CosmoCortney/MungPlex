#pragma once
#include "imgui.h"

namespace MungPlex {
	class ContextMenuHelper {
	public:
		static void DrawWindow();

	private:
		ContextMenuHelper();
		~ContextMenuHelper();
		void OpenGithubLink();
		void OpenGithubWikiLink();
		static ContextMenuHelper& GetInstance() {
			static ContextMenuHelper Instance;
			return Instance;
		}
	};
}