#pragma once
#include "ICombo.hpp"
#include <tuple>

namespace MungPlex
{
    class DoubleStringIdCombo : public ICombo
    {
    public:
        typedef std::vector<std::tuple<std::string, std::string, uint32_t>> Type;
        //StringIdCombo() = default;
        DoubleStringIdCombo(const std::string& label, const bool printLabel) : ICombo(label, printLabel) {}
        DoubleStringIdCombo(const std::string& label, const bool printLabel, const Type& doubleStringIdPairVec);
        //~StringIdCombo() = default;
        DoubleStringIdCombo(const DoubleStringIdCombo& other);
        DoubleStringIdCombo& operator=(const DoubleStringIdCombo& other);
        DoubleStringIdCombo(DoubleStringIdCombo&& other) noexcept;
        DoubleStringIdCombo& operator=(DoubleStringIdCombo&& other) noexcept;

        bool Draw(const float paneWidth = 0.25f, const float labelPortion = 0.4f);
        const std::string& GetStdStringAt(const uint64_t index);
        const std::string& GetSelectedStdString() const;
        std::string& GetIdentifierStdStringAt(const uint64_t index);
        const std::string& GetSelectedIdentifierStdString() const;
        void SetSelectedById(const uint32_t id);
        void SetItems(const Type& stringIdPairVec);
        uint32_t GetSelectedId() const;
        void PopBack(const uint64_t count);
        void Clear();
        void PushBack(const std::tuple<std::string, std::string, uint32_t>& stringIdPair);
        int GetIdAt(const uint64_t index);

    protected:

    private:
        void assign(const DoubleStringIdCombo& other);
        void assignPointers();

        Type _doubleStringIdPairVec;
    };
}