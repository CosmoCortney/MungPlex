#pragma once
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

        void drawValueSetup(const float itemWidth, const float itemHeight, const int type);
        void assignCheckBoxIDs();
        void drawPlotArea(const float itemWidth, const float itemHeight, const int type);

    public:
        MousePianoView(const int id);
        MousePianoView(const int id, const nlohmann::json elem);
        void Draw();
        nlohmann::json GetJSON();
    };

}