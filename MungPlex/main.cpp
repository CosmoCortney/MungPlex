#include <boost/asio.hpp>
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "Cheats.hpp"
#include "Connection.hpp"
#include "DataConversion.hpp"
#include "DeviceControl.hpp"
#include "GLFW/glfw3.h"
#include "HelperFunctions.hpp"
#include "ContextMenuHelper.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include <iostream>
#include "Log.hpp"
#include "MungPlexConfig.hpp"
#include "PointerSearch.hpp"
#include "ProcessInformation.hpp"
#include "Search.hpp"
#include "Settings.hpp"
#include "WatchControl.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

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
	const std::string temp = std::string(MungPlex::Settings::GetGeneralSettings().DocumentsPath) + R"(\MungPlex\Search)";
	const std::wstring path = MorphText::Convert<std::string, std::wstring>(temp, MT::UTF8, MT::UTF16LE);

	if (std::filesystem::is_directory(path))
	{
		for (const auto& entry : std::filesystem::directory_iterator(path))
			std::filesystem::remove_all(entry.path());
	}
	else
	{
		const std::string err = "Error clearing Search Path: " + MorphText::Convert<std::wstring, std::string>(path, MorphText::UTF16LE, MorphText::UTF8);
#ifndef NDEBUG
		std::cout << err << '\n';
#endif
		MungPlex::Log::LogInformation(err);
	}
}

int main()
{
	if (!glfwInit())
	{
		return EXIT_FAILURE;
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	auto& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	ImGui::StyleColorsDark();

	std::string windowTitle = MungPlex::GetWindowTitleBase();
	const auto window = glfwCreateWindow(1280, 720, windowTitle.c_str(), nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glfwSwapInterval(1);
	MungPlex::ProcessInformation::SetWindowRef(window);

	ImGuiStyle& style = ImGui::GetStyle();
	MungPlex::Settings::InitSettings();
	clearSearchResultsDir();

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

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::SetNextWindowPos(viewport->WorkPos);

		int displayWidth, displayHeight;
		glfwGetWindowSize(window, &displayWidth, &displayHeight);
		ImGui::SetNextWindowSize(ImVec2(displayWidth,displayHeight));

		//Container window
		ImGui::Begin("MungPlex", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id);
		ImGui::End();

		MungPlex::WatchControl::DrawWindow();
		MungPlex::Settings::DrawWindow();
		MungPlex::ProcessInformation::DrawWindow();
		MungPlex::Connection::DrawWindow();
		MungPlex::Search::DrawWindow();
		MungPlex::Cheats::DrawWindow();
		MungPlex::Log::DrawWindow();
		MungPlex::PointerSearch::DrawWindow();
		MungPlex::DataConversion::DrawWindow();
		MungPlex::DeviceControl::DrawWindow();
		MungPlex::ContextMenuHelper::DrawWindow();

		for (int i = 0; i < MungPlex::ContextMenuHelper::GetMemoryViews().size(); ++i)
		{
			if (!MungPlex::ContextMenuHelper::GetMemoryViews()[i].IsOpen())
			{
				MungPlex::ContextMenuHelper::UpdateMemoryViewerList();
				i = -1;
				continue;
			}

			MungPlex::ContextMenuHelper::GetMemoryViews()[i].DrawWindow();
		}

		if (show_demo_window)
		{
			ImGui::ShowDemoWindow(&show_demo_window);
		}

		static bool setWindowFocused = true;
		if (setWindowFocused)
		{
			ImGui::SetWindowFocus(MungPlex::Settings::GetGeneralSettings().Windows[MungPlex::Settings::GetGeneralSettings().DefaultWindowSelect].c_str());
			setWindowFocused = false;
		}

		ImGui::Render();

		
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			const auto backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}

		glfwGetFramebufferSize(window, &displayWidth, &displayHeight);
		glViewport(0, 0, displayWidth, displayHeight);
		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}
	
	clearSearchResultsDir();
	return EXIT_SUCCESS;
}
