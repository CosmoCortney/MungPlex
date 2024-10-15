#pragma once
#include "IView.hpp"

namespace MungPlex
{
    class BoolView : public IView
    {
    private:
        bool _val = false;
        void drawValueSetup(const float itemWidth, const float itemHeight, const int type);
        void drawPlotArea(const float itemWidth, const float itemHeight, const int type);

    public:
        BoolView(const int id);
        BoolView(const int id, const nlohmann::json elem);
        void Draw();
        nlohmann::json GetJSON();
    };
}