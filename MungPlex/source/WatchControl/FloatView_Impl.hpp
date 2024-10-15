#pragma once
#include "IView.hpp"

namespace MungPlex
{
    class FloatView : public IView
    {
    private:
        double _val = 0.0;
        bool DoublePrecision;
        bool _useSlider = false;
        int64_t _min;
        int64_t _max;
        int _plotCount = 128;
        std::vector<float> _plotVals;
        float _plotMin = 0.0f;
        float _plotMax = 0.0f;
        std::string _plotBuf = std::string(64, '\0');
        void drawValueSetup(const float itemWidth, const float itemHeight, const int type);
        void drawPlotArea(const float itemWidth, const float itemHeight, const int type);

    public:
        FloatView(const int id);
        FloatView(const int id, const nlohmann::json elem);
        void Draw();
        nlohmann::json GetJSON();
    };
}