#pragma once
#include "ICombo.hpp"

namespace MungPlex
{
    class StringIdCombo : public ICombo
    {
    public:
        StringIdCombo() = default;
        StringIdCombo(const std::string& label, const bool printLabel) : ICombo(label, printLabel) {}
        StringIdCombo(const std::string& label, const bool printLabel, const std::vector<std::pair<std::string, uint32_t>>& stringIdPairVec);
        bool Draw(const float paneWidth = 0.25f, const float labelPortion = 0.4f);
        const std::string& GetStdStringAt(const uint64_t index);
        const std::string& GetSelectedStdString() const;
        uint32_t GetSelectedId() const;
        void PopBack(const uint64_t count);
        void Clear();
        void PushBack(const std::pair<std::string, uint32_t>& stringIdPair);

    protected:

    private:
        std::vector<std::pair<std::string, uint32_t>> _stringIdPairVec{};
    };
}