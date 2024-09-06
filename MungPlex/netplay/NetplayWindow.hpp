#pragma once
#include <string>
#include <stdint.h>
#include "imgui.h"
#include "GLFW/glfw3.h"

namespace MungPlex {
	class NetplayWindow {
	public:
		static void DrawWindow();
	private:
		NetplayWindow();
		~NetplayWindow();
		void HostButtonClicked();
		static int ClampIntInputValue(int value, int min, int max);
		static void JoinButtonClicked();
		static const uint8_t MaxPasswordLength = 255;
		static char PasswordInputTextBuffer[MaxPasswordLength];
		static int GameIDInput;
		static NetplayWindow& GetInstance()
		{
			static NetplayWindow Instance;
			return Instance;
		}
	};
}