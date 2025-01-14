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
        Map3dView(const int id, const nlohmann::json& elem);
        Map3dView(const Map3dView& other);
        Map3dView& operator=(const Map3dView& other);
        Map3dView(Map3dView&& other) noexcept;
        Map3dView& operator=(Map3dView&& other) noexcept;

        void Draw();
        nlohmann::json GetJSON();

    private:
        enum PlotTypes { SCATTER, MESH };

        //general setup
        static const std::vector<StringIdCombo::VecType> _plotTypes;
        StringIdCombo _plotTypeSelectCombo = StringIdCombo("Type:", false, _plotTypes);
        std::vector<StringIdCombo::VecType> _items;
        StringIdCombo _itemSelectCombo = StringIdCombo("Layer:", true);
        bool _clippingOn = false;
        float _markerSize = 6.0f;
        std::array<bool, 3> _axesFlipFlags = { false, false, false };

        //pointer setup
        InputVectorInt<uint64_t> _rangeBeginnings = InputVectorInt<uint64_t>("Safe Range:", true, {});
        InputVectorInt<uint64_t> _rangeEnds = InputVectorInt<uint64_t>(" - ", false, {});
        std::vector<std::wstring> _moduleWVec;
        InputVectorText _moduleInputs = InputVectorText("Module:", true, {}, 128);
        InputVectorText _pointerPathInputs = InputVectorText("Pointer Path:", true, {}, 256);
        std::vector<std::vector<int64_t>> _pointerPathVecVec;
        std::vector<bool> _useModulePathVec;
        std::vector<uint64_t> _moduleAddressVec;

        //value setup
        inline static const ImVec4 _defaultMeshFaceColor = { 0.564706f, 0.0f, 0.831373f, 1.0f };
        inline static const ImVec4 _defaultMeshLineColor = { 1.0f, 1.0f, 1.0f, 1.0f };
        inline static const ImVec4 _defaultMeshMarkerColor = { 0.0f, 0.827451f, 1.0f, 1.0f };
        inline static const ImVec4 _colorDisable = { 0.0f, 0.0f, 0.0f, 0.0f };
        InputVectorText _objPaths = InputVectorText("OBJ Path:", true, {}, 512);
        std::vector<ImVec4> _fillColorVec;
        std::vector<ImVec4> _lineColorVec;
        std::vector<ImVec4> _markerColorVec;
        std::vector<std::shared_ptr<ImPlot3DPoint>> _meshes;
        std::vector<uint32_t> _mesheVertCounts;
        std::vector<std::vector<uint32_t>> _meshesIndecies;
        std::vector<std::array<bool, 3>> _colorPickerEnablerVec;
        std::vector<std::vector<std::vector<float>>> _coordinatesVecVecVec;
        InputVectorInt<uint64_t> _scatterCounts = InputVectorInt<uint64_t>("Count:", true, {});
        InputVectorInt<uint64_t> _scatterOffsets = InputVectorInt<uint64_t>("Offset:", true, {}, 4, 16);
        std::vector<ImVec4> _scatterColorVec;
        InputVectorText _plotNames = InputVectorText("Name:", true, {}, 64);
        double _axisLimit = 0.0;
        bool _setAxisLimit = false;
        static const std::vector<StringIdCombo::VecType> _markerTypes;
        StringIdCombo _markerTypeSelectCombo = StringIdCombo("Marker Type:", false, _markerTypes);
        std::vector<int32_t> _markerTypeSelects;
        std::vector<std::array<float, 3>> _markerOffset;
        InputVectorInt<int64_t> _coordinateDisplacements = InputVectorInt<int64_t>("Displacement:", true, {}, 4, 16);
        std::vector<bool> _trackLines;
        std::vector<std::vector<std::vector<std::vector<float>>>> _linesVecVecVecVec;
        std::vector<uint32_t> _frameCount;
        std::vector<bool> _linePlotRotate;
        std::vector<std::string> _linePlotNames;
        static const uint32_t _maxFrames = 1200;

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
        void setItemIndices(const uint64_t index);
        void resizeLinesVec(const uint64_t index);
        std::vector<std::vector<float>> imVec4VecToStdVec(const std::vector<ImVec4>& vec4);
        void deleteItem(const uint64_t index);
    };
}