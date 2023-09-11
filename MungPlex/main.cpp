#include <iostream>
#include "MungPlexConfig.h"
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include"Settings.h"
#include "Connection.h"
#include "ProcessInformation.h"
#include"Search.h"
#include"Cheats.h"
#include"Log.h"
#include "PointerSearch.h"

void key_callback(GLFWwindow* window, const int key, const int scancode, const int action, const int mode) //keep unused parameters since that signature is required for glfwSetKeyCallback
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

int main()
{
	if (!glfwInit())
	{
		return EXIT_FAILURE;
	}

	std::string windowTitle("MungPlex ");
	windowTitle.append(std::to_string(MungPlex_VERSION_MAJOR) + "." + std::to_string(MungPlex_VERSION_MINOR) + "." + std::to_string(MungPlex_VERSION_PATCH));

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	auto& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	ImGui::StyleColorsDark();

	const auto window = glfwCreateWindow(1280, 720, windowTitle.c_str(), nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);

	glfwSwapInterval(1);

	ImGuiStyle& style = ImGui::GetStyle();
	//if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		const int styleID = MungPlex::Settings::GetGeneralSettings().Style;

		if (styleID)
		{
			MungPlex::ColorScheme colors = MungPlex::Settings::GetColorScheme(styleID);
			style.Colors[ImGuiCol_WindowBg] = colors.Background;
			style.Colors[ImGuiCol_Text] = colors.Text;
			style.Colors[ImGuiCol_TextDisabled] = colors.TextDisabled;
			style.Colors[ImGuiCol_ChildBg] = colors.ChildBG;
			style.Colors[ImGuiCol_PopupBg] = colors.PopUpBG;
			style.Colors[ImGuiCol_FrameBg] = colors.FrameBG;
			style.Colors[ImGuiCol_FrameBgHovered] = colors.FrameBGHovered;
			style.Colors[ImGuiCol_FrameBgActive] = colors.FrameBGActive;
			style.Colors[ImGuiCol_TitleBg] = colors.TitleBG;
			style.Colors[ImGuiCol_TitleBgActive] = colors.TitleBGActive;
			style.Colors[ImGuiCol_TitleBgCollapsed] = colors.TitleBGCollapsed;
			style.Colors[ImGuiCol_ScrollbarBg] = colors.ScrollbarBg;
			style.Colors[ImGuiCol_ScrollbarGrab] = colors.ScrollbarGrab;
			style.Colors[ImGuiCol_ScrollbarGrabHovered] = colors.ScrollbarGrabHovered;
			style.Colors[ImGuiCol_ScrollbarGrabActive] = colors.ScrollbarGrabActive;
			style.Colors[ImGuiCol_CheckMark] = colors.CheckMark;
			style.Colors[ImGuiCol_SliderGrab] = colors.SliderGrab;
			style.Colors[ImGuiCol_SliderGrabActive] = colors.SliderGrabActive;
			style.Colors[ImGuiCol_Button] = colors.Button;
			style.Colors[ImGuiCol_ButtonHovered] = colors.ButtonHovered;
			style.Colors[ImGuiCol_ButtonActive] = colors.ButtonActive;
			style.Colors[ImGuiCol_Header] = colors.Header;
			style.Colors[ImGuiCol_HeaderHovered] = colors.HeaderHovered;
			style.Colors[ImGuiCol_HeaderActive] = colors.HeaderActive;
			style.Colors[ImGuiCol_Separator] = colors.Separator;
			style.Colors[ImGuiCol_SeparatorHovered] = colors.SeparatorHovered;
			style.Colors[ImGuiCol_SeparatorActive] = colors.SeparatorActive;
			style.Colors[ImGuiCol_ResizeGrip] = colors.ResizeGrip;
			style.Colors[ImGuiCol_ResizeGripHovered] = colors.ResizeGripHovered;
			style.Colors[ImGuiCol_ResizeGripActive] = colors.ResizeGripActive;
			style.Colors[ImGuiCol_Tab] = colors.Tab;
			style.Colors[ImGuiCol_TabHovered] = colors.TabHovered;
			style.Colors[ImGuiCol_TabActive] = colors.TabActive;
			style.Colors[ImGuiCol_TabUnfocused] = colors.TabUnfocused;
			style.Colors[ImGuiCol_TabUnfocusedActive] = colors.TabUnfocusedActive;
			style.Colors[ImGuiCol_DockingPreview] = colors.DockingPreview;
			style.Colors[ImGuiCol_DockingEmptyBg] = colors.DockingEmptyBG;
			style.Colors[ImGuiCol_TableHeaderBg] = colors.TableHeaderBg;
		}
	}

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();
	ImVec4 clear_color = ImVec4(0.2f, 0.2f, 0.2f, 1.00f);
	ImFontConfig cfg;
	static const ImWchar icons_ranges[] = { 0x0000, 0xf3ff, 0 };

#ifndef NDEBUG
	bool show_demo_window = true;
	bool fontLoaded = io.Fonts->AddFontFromFileTTF("F:\\Workspace\\MungPlex\\MungPlex\\resources\\NotoSansJP-Black.ttf", MungPlex::Settings::GetGeneralSettings().Scale * 20.0f, &cfg, io.Fonts->GetGlyphRangesJapanese());
#else
	bool show_demo_window = false;
	bool fontLoaded = io.Fonts->AddFontFromFileTTF("resources\\NotoSansJP-Black.ttf", MungPlex::Settings::GetGeneralSettings().Scale * 20.0f, &cfg, io.Fonts->GetGlyphRangesJapanese());
#endif

	const auto version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
	std::cout << "OpenGL Version: " << version << std::endl;
	
	style.ScaleAllSizes(MungPlex::Settings::GetGeneralSettings().Scale);
	MungPlex::Log::LogInformation("Started MungPlex");

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (show_demo_window)
		{
			ImGui::ShowDemoWindow(&show_demo_window);
		}

		MungPlex::Settings::DrawWindow();
		MungPlex::ProcessInformation::DrawWindow();
		MungPlex::Connection::DrawWindow();
		MungPlex::Search::DrawWindow();
		MungPlex::Cheats::DrawWindow();
		MungPlex::Log::DrawWindow();
		MungPlex::PointerSearch::DrawWindow();

		if (!MungPlex::Connection::IsOpen())
			break;

		static bool setWindowFocused = true;
		if (setWindowFocused)
		{
			ImGui::SetWindowFocus(MungPlex::Settings::GetGeneralSettings().Windows[MungPlex::Settings::GetGeneralSettings().DefaultWindowSelect].c_str());
			setWindowFocused = false;
		}

		ImGui::Render();

		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			const auto backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}

		glfwSwapBuffers(window);
	}
	
	return EXIT_SUCCESS;
}
