#pragma once
#include <boost/asio.hpp>
#include "IView.hpp"

namespace MungPlex
{
    class BoolView : public IView
    {
    private:
        bool _val = false;
        void assign(const BoolView& other);
        void drawValueSetup(const float itemWidth, const float itemHeight, const int type);
        void drawPlotArea(const float itemWidth, const float itemHeight, const int type);
        void processValue();
        void manageProcessValueThread();

    public:
        BoolView(const int id);
        BoolView(const int id, const nlohmann::json elem);
        BoolView(const BoolView& other);
        BoolView& operator=(const BoolView& other);
        BoolView(BoolView&& other) noexcept;
        BoolView& operator=(BoolView&& other) noexcept;
        
        void Draw();
        nlohmann::json GetJSON();
    };
}