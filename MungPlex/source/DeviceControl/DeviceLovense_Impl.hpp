#pragma once
#include <boost/atomic.hpp>
#include <boost/thread.hpp>
#include "IDevice.hpp"
#include "LovenseToy.hpp"

namespace MungPlex
{
    class DeviceLovense : public IDevice
    {
    public:
        DeviceLovense(const int id);
        DeviceLovense(const int id, const nlohmann::json& json);
        ~DeviceLovense();
        DeviceLovense(const DeviceLovense& other);
        DeviceLovense& operator=(const DeviceLovense& other);
        DeviceLovense(DeviceLovense&& other) noexcept;
        DeviceLovense& operator=(DeviceLovense&& other) noexcept;

        bool _connected = false;

        void Draw();
        nlohmann::json GetJSON();

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
        //toy connection and info
        LovenseToy _lovenseToy;
        FloorString _token = FloorString("", 128);
        int _toyError = CLovenseToy::TOYERR_SUCCESS;

        //value
        int _valueType = INT32;
        int _valueTypeIndex = 0;
        float _maxF = 20.0f;
        double _maxD = 20.0;
        int32_t _maxI = 20;
        int64_t _maxL = 20;

        //pointer path
        bool _useModulePath = false;
        uint64_t _moduleAddress = 0;
        FloorString _module = FloorString("", 128);
        std::wstring _moduleW;
        FloorString _pointerPathText = FloorString("", 256);
        std::vector<int64_t> _pointerPath;
        uint64_t _rangeMin = 0;
        uint64_t _rangeMax = 0;

        //toy control
        int _vibrationValue = 0;
        int _previousVibrationValue = 0;
        const int _plotCount = 128;
        const uint32_t _toyControlIntervalMilliseconds = 20;
        boost::thread _toyControlThread;
        static boost::atomic<bool> _toyControlThreadFlag;
        std::vector<float> _plotVals = std::vector<float>(128);
        bool _abortPlot = false;

        void test();
        void drawToyConnectionOptions();
        void drawToyInfo();
        void drawValueTypeOptions();
        void drawPointerSettings();
        void controlToy();
        void plotValues();
        void assign(const DeviceLovense& other);
    };
}