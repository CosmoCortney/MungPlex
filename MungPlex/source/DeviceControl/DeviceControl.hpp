#pragma once
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "GLFW/glfw3.h"
#include "DeviceLovense_Impl.hpp"
#include "nlohmann/json.hpp"
#include "Xertz.hpp"

namespace MungPlex
{
	class DeviceControl
	{
    public:
        static void DrawWindow();
        static void InitWatchFile();
        static void DeleteItem(const int id);

    private:
        DeviceControl() = default;
        ~DeviceControl() = default;
        DeviceControl(const DeviceControl&) = delete;
        DeviceControl(DeviceControl&&) = delete;
        void operator=(const DeviceControl&) = delete;
        void operator=(DeviceControl&&) = delete;
        static DeviceControl& GetInstance()
        {
            static DeviceControl Instance;
            return Instance;
        }

        std::vector<std::pair<int, std::variant</*DeviceXInput, */DeviceLovense>>> _devices;
        std::vector<int> _deviceIds;

        void drawList();
        bool saveList();

	};
}