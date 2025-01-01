#pragma once
#include "ICombo.hpp"
#include <tuple>

namespace MungPlex
{
    class DoubleStringIdCombo : public ICombo
    {
    public:
        typedef std::tuple<std::string, std::string, int32_t> VecType;
        DoubleStringIdCombo(const std::string& label, const bool printLabel) : ICombo(label, printLabel) {}
        DoubleStringIdCombo(const std::string& label, const bool printLabel, const std::vector<VecType>& doubleStringIdPairVec);
        DoubleStringIdCombo(const DoubleStringIdCombo& other);
        DoubleStringIdCombo& operator=(const DoubleStringIdCombo& other);
        DoubleStringIdCombo(DoubleStringIdCombo&& other) noexcept;
        DoubleStringIdCombo& operator=(DoubleStringIdCombo&& other) noexcept;
        const std::string& GetStdStringAt(const uint64_t index);
        const std::string& GetSelectedStdString() const;
        std::string& GetIdentifierStdStringAt(const uint64_t index);
        const std::string& GetSelectedIdentifierStdString() const;
        void SetSelectedById(const int32_t id);
        void SetItems(const std::vector<VecType>& stringIdPairVec);
        int32_t GetSelectedId() const;
        void PopBack(const uint64_t count);
        void Clear();
        void PushBack(const std::tuple<std::string, std::string, int32_t>& stringIdPair);
        int GetIdAt(const uint64_t index);

    protected:

    private:
        void assign(const DoubleStringIdCombo& other);
        void assignPointers();

        std::vector<VecType> _doubleStringIdPairVec;
    };
}