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
#include"DataConversion.h"
#include"HelperFunctions.h"
#include"WatchControl.h"
#define STB_IMAGE_IMPLEMENTATION
#include"stb/stb_image.h"

#ifndef NDEBUG
	#pragma comment(linker, "/SUBSYSTEM:console /ENTRY:mainCRTStartup")
#else
	#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

void key_callback(GLFWwindow* window, const int key, const int scancode, const int action, const int mode) //keep unused parameters since that signature is required for glfwSetKeyCallback
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

void clearSearchResultsDir()
{
	const std::wstring path(MorphText::Utf8_To_Utf16LE(MungPlex::Settings::GetGeneralSettings().DocumentsPath).append(L"\\MungPlex\\Search"));
	std::wcout << path;

	if (std::filesystem::is_directory(path))
	{
		for (const auto& entry : std::filesystem::directory_iterator(path))
			std::filesystem::remove_all(entry.path());
	}
}

int main()
{
	if (!glfwInit())
	{
		return EXIT_FAILURE;
	}

	clearSearchResultsDir();

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
			style.Colors[ImGuiCol_ModalWindowDimBg] = colors.WindowDim;
			style.Colors[ImGuiCol_PlotLines] = colors.PlotLines;
			style.Colors[ImGuiCol_PlotLinesHovered] = colors.PlotLinesHovered;
			style.Colors[ImGuiCol_PlotHistogram] = colors.PlotHistogram;
			style.Colors[ImGuiCol_PlotHistogramHovered] = colors.PlotHistogramHovered;
		}
	}

	int channels;

	GLFWimage images[3];
	images[0].height = images[0].width = 16;
	images[1].height = images[1].width = 32;
	images[2].height = images[2].width = 48;
	std::string imgPath = MungPlex::GetResourcesFilePath("img\\icon\\").generic_string();
	
	if (std::filesystem::is_directory(imgPath))
	{
		images[0].pixels = stbi_load((imgPath + "icon16.png").data(), &images[0].width, &images[0].height, &channels, 4);
		images[1].pixels = stbi_load((imgPath + "icon32.png").data(), &images[1].width, &images[1].height, &channels, 4);
		images[2].pixels = stbi_load((imgPath + "icon48.png").data(), &images[2].width, &images[2].height, &channels, 4);
		glfwSetWindowIcon(window, 3, images);
	}

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();
	ImVec4 clear_color = ImVec4(0.2f, 0.2f, 0.2f, 1.00f);
	ImFontConfig cfg;

#ifndef NDEBUG
	bool show_demo_window = true;
#else
	bool show_demo_window = false;
#endif

	const bool fontsLoaded = MungPlex::LoadFonts(io, cfg, MungPlex::Settings::GetGeneralSettings().Scale * 20.0f);
	std::cout << "Font successfully loaded: " << fontsLoaded << std::endl;

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
		MungPlex::DataConversion::DrawWindow();
		MungPlex::WatchControl::DrawWindow();

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
	
	clearSearchResultsDir();
	return EXIT_SUCCESS;
}
