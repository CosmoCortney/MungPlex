#include <iostream>
#include "MungPlexConfig.h"
#include <stdio.h>
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "examples/libs/emscripten/emscripten_mainloop_stub.h"
#include<string>
#include"Settings.h"
#include "Connection.h"
#include "ProcessInformation.h"
#include"Xertz.h"
#include"Search.h"
#include"HelperFunctions.h"
#include"Cheats.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

int main(int argc, char* argv[])
{
	/* sol2 example code, can be removed */
	sol::state lua;
	int x = 0;
	lua.set_function("beep", [&x] { ++x; });
	lua.script("beep()");
	assert(x == 1);

	if (!glfwInit())
		return EXIT_FAILURE;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	ImGui::StyleColorsDark();

	GLFWwindow* window = glfwCreateWindow(720, 480, "MungPlex", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);

	glfwSwapInterval(1);

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

#ifndef NDEBUG
	bool show_demo_window = true;
#else
	bool show_demo_window = false;
#endif
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	float SCALE = 2.0f;
	ImFontConfig cfg;
	cfg.SizePixels = 10 * SCALE;
	static const ImWchar icons_ranges[] = { 0x0000, 0xf3ff, 0 };
	// While developing, manually copy the resources folder into the output directory where the EXE resides, otherwise this won't be resolvable
	bool fontLoaded = io.Fonts->AddFontFromFileTTF("resources\\NotoSansJP-Black.ttf", 30, &cfg, io.Fonts->GetGlyphRangesJapanese());

	const char* version = (const char*)glGetString(GL_VERSION);
	std::cout << "OpenGL Version: " << version << std::endl;

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		MungPlex::Settings::DrawWindow();
		MungPlex::ProcessInformation::DrawWindow();
		MungPlex::Connection::DrawWindow();
		MungPlex::Search::DrawWindow();
		MungPlex::Cheats::DrawWindow();

		ImGui::Render();

		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}

		glfwSwapBuffers(window);
	}
	
	return 0;
}
