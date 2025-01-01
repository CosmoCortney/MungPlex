#pragma once
#include <boost/asio.hpp>
#include "HelperFunctions.hpp"
#include "nlohmann/json.hpp"
#include <boost/thread.hpp>
#include <boost/atomic.hpp>
#include "WidgetHelpers.hpp"

namespace MungPlex
{
    class IView
    {
    public:
        enum ViewTypes
        {
            INTEGRAL, FLOAT, BOOL, MOUSEPIANO, MAP2D, MAP3D, COLOR
        };

        static const std::vector<StringIdCombo::VecType> s_IntTypes;
        static const std::vector<StringIdCombo::VecType> s_FloatTypes;
        static const std::vector<StringIdCombo::VecType> s_SuperiorTypes;

        int GetID();
        nlohmann::json GetBasicJSON();
        void SetBasicMembers(const nlohmann::json elem);
        bool DrawSetup(const float itemWidth, const float itemHeight, const int type);
        virtual void Draw() = 0;
        virtual nlohmann::json GetJSON() = 0;

    protected:
        std::wstring _moduleW = std::wstring(32, '\0');
        InputText _moduleInput = InputText("Module:", false, "", 32);
        InputText _pointerPathInput = InputText("Pointer Path:", true, "", 256);
        InputText _labelInput = InputText("Title:", true, "", 128);
        StringIdCombo _floatTypeSelectCombo = StringIdCombo("Float Type:", true, IView::s_FloatTypes);
        StringIdCombo _intTypeSelectCombo = StringIdCombo("Int Type:", true, IView::s_IntTypes);
        std::vector<int64_t> _pointerPath;
        bool _useModulePath = false;
        uint64_t _moduleAddress = 0;
        bool _freeze = false;
        bool _active = false;
        bool _enableSignal = false;
        bool _disableSignal = false;
        int _id;
        bool _delete = false;
        std::string _idText;
        uint64_t _rangeMin = 0;
        uint64_t _rangeMax = 0;
        int _typeSelect = 0;
        boost::thread _processValueThread;
        boost::atomic<bool> _processValueThreadFlag = false;

        virtual void drawValueSetup(const float itemWidth, const float itemHeight, const int type) = 0;
        virtual void drawPlotArea(const float itemWidth, const float itemHeight, const int type) = 0;
        virtual void processValue() = 0;
        virtual void manageProcessValueThread() = 0;

    private:
        bool drawGeneralSetup(const float itemWidth, const float itemHeight, const int type);
        void drawPointerPathSetup(const float itemWidth, const float itemHeight, const int type);
    };
}