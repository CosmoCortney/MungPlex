#pragma once
#include <boost/asio.hpp>
#include "implot.h"
#include "implot3d.h"
#include "IView.hpp"
#include <memory>

namespace MungPlex
{
    class Map3dView : public IView
    {
    public:
        Map3dView(const int id);
        Map3dView(const int id, const nlohmann::json elem);
        Map3dView(const Map3dView& other);
        Map3dView& operator=(const Map3dView& other);
        Map3dView(Map3dView&& other) noexcept;
        Map3dView& operator=(Map3dView&& other) noexcept;

        void Draw();
        nlohmann::json GetJSON();

    private:
        enum PlotTypes { SCATTER, MESH, LINE };

        //general setup
        static const std::vector<StringIdCombo::VecType> _plotTypes;
        StringIdCombo _plotTypeSelectCombo = StringIdCombo("Type:", false, _plotTypes);
        std::vector<StringIdCombo::VecType> _items;
        StringIdCombo _itemSelectCombo = StringIdCombo("Map Settings:", true);

        //pointer setup
        std::vector<std::wstring> _moduleWVec;
        std::vector<InputText> _moduleInputVec;
        std::vector<InputText> _pointerPathInputVec;
        std::vector<std::vector<int64_t>> _pointerPathVecVec;
        std::vector<bool> _useModulePathVec;
        std::vector<uint64_t> _moduleAddressVec;
        std::vector<uint64_t> _rangeMinVec;
        std::vector<uint64_t> _rangeMaxVec;

        //value setup
        const ImVec4 _defaultMeshFaceColor = { 0.564706f, 0.0f, 0.831373f, 1.0f };
        const ImVec4 _defaultMeshLineColor = { 1.0f, 1.0f, 1.0f, 1.0f };
        const ImVec4 _defaultMeshMarkerColor = { 0.0f, 0.827451f, 1.0f, 1.0f };
        const ImVec4 _colorDisable = { 0.0f, 0.0f, 0.0f, 0.0f };
        std::vector<InputText> _objPathInputVec;
        std::vector<ImVec4> _fillColorVec;
        std::vector<ImVec4> _lineColorVec;
        std::vector<ImVec4> _markerColorVec;
        std::vector<std::shared_ptr<ImPlot3DPoint>> _meshes;
        std::vector<uint32_t> _mesheVertCounts;
        std::vector<std::vector<uint32_t>> _meshesIndecies;
        std::vector<std::array<bool, 3>> _colorPickerEnablerVec;
        std::vector<std::vector<std::vector<float>>> _coordinatesVecVecVec;
        std::vector<InputInt<uint16_t>> _scatterCountVec;
        std::vector<InputInt<uint64_t>> _scatterOffsetVec;
        std::vector<ImVec4> _scatterColorVec;
        std::vector<std::string> _3dPlotNames;
        double _axisLimit = 0.0;
        bool _setAxisLimit = false;
        const static std::vector<StringIdCombo::VecType> _markerTypes;
        std::vector<StringIdCombo> _markerTypeSelectCombo;
        bool _clippingOn = false;

        //misc.
        boost::thread _processValueThread;
        boost::atomic<bool> _processValueThreadFlag = false;

        void assign(const Map3dView& other);
        void drawValueSetup(const float itemWidth, const float itemHeight, const int type = -1);
        void drawPlotArea(const float itemWidth, const float itemHeight, const int type = -1);
        void processValue();
        void manageProcessValueThread();        
        bool drawGeneralSetup(const float itemWidth, const float itemHeight);
        void drawPointerPathSetup(const float itemWidth, const float itemHeight);
        bool drawActiveCheckBox();
        bool loadOBJ(const std::string& path, std::shared_ptr<ImPlot3DPoint>& mesh, uint32_t& vertCount, std::vector<uint32_t>& indices);
        void initNewitem();
        void resizeCoordinatesVec(const uint64_t index, const uint64_t count);
    };
}