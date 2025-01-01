#pragma once
#include <boost/asio.hpp>
#include "IView.hpp"
#include <utility>

namespace MungPlex
{
    class MousePianoView : public IView
    {
    private:
        std::vector<std::pair<std::string, bool>> _switches;
        std::array<std::string, 32> _checkBoxIDs;
        std::array<std::string, 32> _checkBoxTextIDs;
        std::string _idText;

        void assign(const MousePianoView& other);
        void drawValueSetup(const float itemWidth, const float itemHeight, const int type);
        void assignCheckBoxIDs();
        void drawPlotArea(const float itemWidth, const float itemHeight, const int type);
        void processValue();
        void manageProcessValueThread();

    public:
        MousePianoView(const int id);
        MousePianoView(const int id, const nlohmann::json elem);
        MousePianoView(const MousePianoView& other);
        MousePianoView& operator=(const MousePianoView& other);
        MousePianoView(MousePianoView&& other) noexcept;
        MousePianoView& operator=(MousePianoView&& other) noexcept;

        void Draw();
        nlohmann::json GetJSON();
    };

}