#pragma once
#include "ICombo.hpp"

namespace MungPlex
{
    class StringIdCombo : public ICombo
    {
    public:
        typedef std::pair<std::string, int32_t> VecType;
        StringIdCombo(const std::string& label, const bool printLabel) : ICombo(label, printLabel) {}
        StringIdCombo(const std::string& label, const bool printLabel, const std::vector<std::pair<std::string, int32_t>>& stringIdPairVec);
        StringIdCombo(const StringIdCombo& other);
        StringIdCombo& operator=(const StringIdCombo& other);
        StringIdCombo(StringIdCombo&& other) noexcept;
        StringIdCombo& operator=(StringIdCombo&& other) noexcept;
        const std::string& GetStdStringAt(const uint64_t index);
        const std::string& GetSelectedStdString() const;
        void SetSelectedById(const uint64_t id);
        void SetItems(const std::vector<std::pair<std::string, int32_t>>& stringIdPairVec, const bool setIndexLast = false);
        int32_t GetSelectedId() const;
        void PopBack(const uint64_t count);
        void Clear();
        void PushBack(const std::pair<std::string, int32_t>& stringIdPair);

    private:
        void assign(const StringIdCombo& other);
        void assignPointers();

        std::vector<VecType> _stringIdPairVec{};
    };
}