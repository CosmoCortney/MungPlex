#pragma once
#include <boost/asio.hpp>
#include "IView.hpp"

namespace MungPlex
{
    class WaveView : public IView
    {
    private:
        enum Modifiers : int
        {
            ADD,
            MUL,
            SUPPO
        };

        InputFloat<float> _inputDelta = InputFloat<float>("Delta:", true, 0.0f, 0.0001f, 0.01f, 8);
        std::vector<float> _plotVals;
        float _plotMin = -1.0f;
        float _plotMax = 1.0f;
        float _plotMinWithDelta = -1.0f;
        float _plotMaxWithDelta = 1.0f;
        std::string _plotBuf = std::string(64, '\0');
        InputText _inputExtremes = InputText("Extremes:");
        std::vector<float> _extremes;
        std::vector<float> _ogWaveTableCopy;
        std::vector<float> _modifiedValues;
        uint64_t _waveTableOffset = 0;
        int32_t _modifierType = MUL;
        int32_t _regionIndex = -1;
        std::string _countStr = "0";

        void assign(const WaveView& other);
        void drawSearchSetup(const float itemWidth, const float itemHeight);
        void drawValueSetup(const float itemWidth, const float itemHeight, const int type) override {};
        void drawPlotArea(const float itemWidth, const float itemHeight, const int type) override;
        void manageProcessValueThread() override {};
        void processWavetableThread(const std::function<void()>& func);
        void parseExtremes();
        void setOriginalPlotRange();
		void setModifiedPlotRange();
        void searchWave();
        void modify();
        void reset();
        void processValue() override {};

    public:
        WaveView(const int id);
        WaveView(const int id, const nlohmann::json elem);
        WaveView(const WaveView& other);
        WaveView& operator=(const WaveView& other);
        WaveView(WaveView&& other) noexcept;
        WaveView& operator=(WaveView&& other) noexcept;

        void Draw() override;
        nlohmann::json GetJSON() override;
    };
}