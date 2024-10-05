#pragma once
#include "IDevice.hpp"
#include "LovenseToy.hpp"

namespace MungPlex
{
    class DeviceLovense : public IDevice
    {
    public:
        DeviceLovense();
        //DeviceLovense(const int id, const nlohmann::json elem);

        //int _typeID = DeviceTypes::UNK;
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
        //toy connection and info
        LovenseToy _lovenseToy;
        FloorString _token = FloorString("", 128);
        int _toyError = CLovenseToy::TOYERR_SUCCESS;

        //value
        float _maxF = 20.0f;
        double _maxD = 20.0;
        int32_t _maxI = 20;
        int64_t _maxL = 20;

        //pointer path
        bool _useModulePath = false;
        uint64_t _moduleAddress = 0;
        FloorString _module = FloorString("", 128);
        std::wstring _moduleW;
        std::string _pointerPathText = std::string(256, '\0');
        std::vector<int64_t> _pointerPath;
        uint64_t _rangeMin = 0;
        uint64_t _rangeMax = 0;

        //toy control
        int _vibrationValue = 0;
        int _plotCount = 128;
        int _valueType = INT32;
        std::vector<float> _plotVals = std::vector<float>(128);

        void drawToyConnectionOptions();
        void drawToyInfo();
        void drawValueTypeOptions();
        void drawPointerSettings();
        void controlToy();
    };
}