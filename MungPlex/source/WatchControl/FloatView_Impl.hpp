#pragma once
#include <boost/asio.hpp>
#include "IView.hpp"

namespace MungPlex
{
    class FloatView : public IView
    {
    private:
        Slider<double> _sliderD = Slider<double>("Adjust:", false, 0.0, -1.0, 1.0, ImGuiSliderFlags_None);
        Slider<float> _sliderF = Slider<float>("Adjust:", false, 0.0f, -1.0f, 1.0f, ImGuiSliderFlags_None);
        InputFloat<double> _inputD = InputFloat<double>("Value:", false, 0.0, 0.0, 0.0, 6);
        InputFloat<float> _inputF = InputFloat<float>("Value:", false, 0.0f, 0.0f, 0.0f, 6);
        bool DoublePrecision;
        bool _useSlider = false;
        int64_t _min;
        int64_t _max;
        int _plotCount = 128;
        std::vector<float> _plotVals;
        InputFloat<float> _plotMinF = InputFloat<float>("minF:", false, 0.0f, 0.0f, 0.0f, 6);
        InputFloat<float> _plotMaxF = InputFloat<float>("maxF:", false, 0.0f, 0.0f, 0.0f, 6);
        InputFloat<double> _plotMinD = InputFloat<double>("minD:", false, 0.0f, 0.0f, 0.0f, 6);
        InputFloat<double> _plotMaxD = InputFloat<double>("maxD:", false, 0.0f, 0.0f, 0.0f, 6);
        std::string _plotBuf = std::string(64, '\0');

        void assign(const FloatView& other);
        void drawValueSetup(const float itemWidth, const float itemHeight, const int type);
        void drawPlotArea(const float itemWidth, const float itemHeight, const int type);
        void processValue();
        void manageProcessValueThread();

    public:
        FloatView(const int id);
        FloatView(const int id, const nlohmann::json elem);
        FloatView(const FloatView& other);
        FloatView& operator=(const FloatView& other);
        FloatView(FloatView&& other) noexcept;
        FloatView& operator=(FloatView&& other) noexcept;

        void Draw();
        nlohmann::json GetJSON();
    };
}