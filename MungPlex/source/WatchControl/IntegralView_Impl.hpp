#pragma once
#include <boost/asio.hpp>
#include "IView.hpp"

namespace MungPlex
{
    class IntegralView : public IView
    {
    private:
        int64_t _val = 0;
        bool _hex = false;
        int _plotCount = 128;
        std::vector<float> _plotVals;
        uint64_t _plotMin = 0;
        uint64_t _plotMax = 255;
        float _plotMinF = 0.0f;
        float _plotMaxF = 0.0f;
        std::string _plotBuf = std::string(64, '\0');
        std::string _formatPlot;
        std::string _format;
        void assign(const IntegralView& other);
        void drawValueSetup(const float itemWidth, const float itemHeight, const int type);
        void drawPlotArea(const float itemWidth, const float itemHeight, const int type);
        void setFormatting();
        void processValue();
        void manageProcessValueThread();

    public:
        IntegralView(const int id);
        IntegralView(const int id, const nlohmann::json elem);
        IntegralView(const IntegralView& other);
        IntegralView& operator=(const IntegralView& other);
        IntegralView(IntegralView&& other) noexcept;
        IntegralView& operator=(IntegralView&& other) noexcept;

        void Draw();
        nlohmann::json GetJSON();
    };
}