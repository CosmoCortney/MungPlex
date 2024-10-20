#pragma once
#include "HelperFunctions.hpp"
#include "nlohmann/json.hpp"
#include <string>
#include <vector>

namespace MungPlex
{
    class IView
    {
    public:
        enum ViewTypes
        {
            INTEGRAL, FLOAT, BOOL, MOUSEPIANO, MAP2D, MAP3D, COLOR
        };

        static const std::vector<std::pair<std::string, int>> s_IntTypes;
        static const std::vector<std::pair<std::string, int>> s_FloatTypes;
        static const std::vector<std::pair<std::string, int>> s_SuperiorTypes;

        int GetID();
        nlohmann::json GetBasicJSON();
        void SetBasicMembers(const nlohmann::json elem);
        bool DrawSetup(const float itemWidth, const float itemHeight, const int type);
        virtual void Draw() = 0;
        virtual nlohmann::json GetJSON() = 0;

    protected:
        std::wstring _moduleW = std::wstring(32, '\0');
        std::string _module = std::string(32, '\0');
        std::string _pointerPathText = std::string(128, '\0');
        FloorString _label = FloorString("", 128);
        std::vector<int64_t> _pointerPath;
        bool _useModulePath = false;
        uint64_t _moduleAddress = 0;
        bool _freeze = false;
        bool _active = false;
        int _id;
        bool _delete = false;
        std::string _idText;
        uint64_t _rangeMin = 0;
        uint64_t _rangeMax = 0;
        int _typeSelect = 0;

        virtual void drawValueSetup(const float itemWidth, const float itemHeight, const int type) = 0;
        virtual void drawPlotArea(const float itemWidth, const float itemHeight, const int type) = 0;

    private:
        bool drawGeneralSetup(const float itemWidth, const float itemHeight, const int type);
        void drawPointerPathSetup(const float itemWidth, const float itemHeight, const int type);
    };
}