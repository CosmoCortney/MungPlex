#pragma once
#include <boost/asio.hpp>
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "examples/libs/emscripten/emscripten_mainloop_stub.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "GLFW/glfw3.h"
#include <iostream>
#include "LovenseToy.hpp"
#include "MungPlexConfig.hpp"
#include "nlohmann/json.hpp"
#include <stdio.h>
#include <string>
#include <utility>
#include <variant>
#include <vector>
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

        enum DeviceTypes
        {
            XINPUT, LOVENSE, TOBII, UNK = -1
        };

        class Device
        {
        public:
            int _typeID = DeviceTypes::UNK;
            int _deviceId = -1;
            std::string _idText;
            std::string _name = std::string(128, '\0');
            std::string _label = std::string(128, '\0');
            bool _connected = false;
            bool _delete = false;
            bool _active = false;
            static const std::vector<std::pair<std::string, int>> s_DeviceTypes;
            static const std::vector<std::pair<std::string, int>> s_ValueTypes;
            int _valueTypeIndex = 2;

            int GetID();
            const char* GetName();
            const bool IsConnected();
            void DrawSetup(const float itemWidth, const float itemHeight, const int type);
        };

        class DeviceLovense : public Device
        {
        public:
            DeviceLovense();
            //DeviceLovense(const int id, const nlohmann::json elem);

            int _typeID = DeviceTypes::UNK;
            std::string _name = std::string(128, '\0');
            bool _connected = false;

            void Draw();
            template<typename T> int ScaleValue(const T val, const T max)
            {
                double factor;

                if constexpr (std::is_same_v<T, double>)
                    factor = max / 20.1;
                else if constexpr (std::is_same_v<T, float>)
                    factor = static_cast<double>(max) / 20.1f;
                else if constexpr (std::is_same_v<T, bool>)
                    return val ? 20 : 0;
                else 
                    factor = static_cast<double>(max) / 20.1;

                int res = val / factor;
                std::cout << "res: " << res << ", val: " << val << ", factor: " << factor << std::endl;
                return std::clamp(res, 0, 20);
            }

        private:
            LovenseToy _lovenseToy;
            FloorString _token = FloorString("", 128);
            float _maxF = 20.0f;
            double _maxD = 20.0;
            int32_t _maxI = 20;
            int64_t _maxL = 20;
            int _vibrationValue = 0;
            bool _useModulePath = false;
            uint64_t _moduleAddress = 0;
            FloorString _module = FloorString("", 128);
            std::wstring _moduleW;
            std::string _pointerPathText = std::string(256, '\0');
            std::vector<int64_t> _pointerPath;
            uint64_t _rangeMin = 0;
            uint64_t _rangeMax = 0;
            int _plotCount = 128;
            std::vector<float> _plotVals = std::vector<float>(128);
            void* getCurrentPointer();
        };

        std::vector<std::pair<int, std::variant</*DeviceXInput, */DeviceLovense>>> _devices;
        std::vector<int> _deviceIds;

        void drawList();
        bool saveList();

	};
}