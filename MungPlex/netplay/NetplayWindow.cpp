#include "imgui.h"
#include "GLFW/glfw3.h"
#include "NetplayWindow.hpp"
#include "WebsocketClient.hpp"

char MungPlex::NetplayWindow::PasswordInputTextBuffer[MungPlex::NetplayWindow::MaxPasswordLength];
int MungPlex::NetplayWindow::GameIDInput = 1000;

int MungPlex::NetplayWindow::ClampIntInputValue(int value, int minValue, int maxValue) {
	if (value < minValue) return minValue;
	if (value > maxValue) return maxValue;
	return value;
}

void MungPlex::NetplayWindow::DrawWindow()
{
	if (ImGui::Begin("Netplay")) {
		if (MungPlex::WebsocketClient::IsConnected && !MungPlex::WebsocketClient::IsHosting && !MungPlex::WebsocketClient::IsInGame) {
			if (ImGui::InputText(" ", GetInstance().PasswordInputTextBuffer, MaxPasswordLength)) {

			}
			ImGui::SameLine();
			if (ImGui::Button("Host Netplay")) {
				MungPlex::WebsocketClient::HostGame(GetInstance().PasswordInputTextBuffer);
			}
			ImGui::Text("Game#");
			ImGui::SameLine();
			if (ImGui::InputInt("Integer Input", &GetInstance().GameIDInput)) {
				GetInstance().GameIDInput = GetInstance().ClampIntInputValue(GetInstance().GameIDInput, 1000, 9999);
			}
			if (ImGui::Button("Join Game")) {
				MungPlex::WebsocketClient::JoinGame(&std::to_string(GetInstance().GameIDInput)[0], GetInstance().PasswordInputTextBuffer);
			}
		}
	}
	if (MungPlex::WebsocketClient::IsInGame && !MungPlex::WebsocketClient::IsHosting && MungPlex::WebsocketClient::IsConnected) {
		if (ImGui::Button("Disconnect")) {
			MungPlex::WebsocketClient::LeaveGame();
		}
	}
	ImGui::End();
}


MungPlex::NetplayWindow::NetplayWindow()
{
}

MungPlex::NetplayWindow::~NetplayWindow()
{
}
