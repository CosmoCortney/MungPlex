#pragma once
#include "ICombo.hpp"
#include <tuple>

namespace MungPlex
{
    class StringIdBoolCombo : public ICombo
    {
    public:
        typedef std::tuple<std::string, int32_t, bool> VecType;
        StringIdBoolCombo(const std::string& label, const bool printLabel) : ICombo(label, printLabel) {}
        StringIdBoolCombo(const std::string& label, const bool printLabel, const std::vector<VecType>& stringIdBoolPairVec);
        StringIdBoolCombo(const StringIdBoolCombo& other);
        StringIdBoolCombo& operator=(const StringIdBoolCombo& other);
        StringIdBoolCombo(StringIdBoolCombo&& other) noexcept;
        StringIdBoolCombo& operator=(StringIdBoolCombo&& other) noexcept;
        const std::string& GetStdStringAt(const uint64_t index);
        const std::string& GetSelectedStdString() const;
        bool GetBoolByIndex(const uint64_t index);
        bool GetSelectedBool() const;
        void SetSelectedById(const int32_t id);
        void SetItems(const std::vector<VecType>& stringIdPairVec);
        int32_t GetSelectedId() const;
        void PopBack(const uint64_t count);
        void Clear();
        void PushBack(const VecType& stringIdPair);

    private:
        void assign(const StringIdBoolCombo& other);
        void assignPointers();

        std::vector<VecType> _stringIdBoolPairVec;
    };
}