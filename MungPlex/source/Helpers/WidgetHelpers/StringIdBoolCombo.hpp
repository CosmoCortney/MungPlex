#pragma once
#include "ICombo.hpp"
#include <tuple>

namespace MungPlex
{
    class StringIdBoolCombo : public ICombo
    {
    public:
        typedef std::vector<std::tuple<std::string, uint32_t, bool>> Type;
        //StringIdCombo() = default;
        StringIdBoolCombo(const std::string& label, const bool printLabel) : ICombo(label, printLabel) {}
        StringIdBoolCombo(const std::string& label, const bool printLabel, const Type& stringIdBoolPairVec);
        //~StringIdCombo() = default;
        StringIdBoolCombo(const StringIdBoolCombo& other);
        StringIdBoolCombo& operator=(const StringIdBoolCombo& other);
        StringIdBoolCombo(StringIdBoolCombo&& other) noexcept;
        StringIdBoolCombo& operator=(StringIdBoolCombo&& other) noexcept;

        bool Draw(const float paneWidth = 0.25f, const float labelPortion = 0.4f);
        const std::string& GetStdStringAt(const uint64_t index);
        const std::string& GetSelectedStdString() const;
        bool GetBoolByIndex(const uint64_t index);
        bool GetSelectedBool() const;
        void SetSelectedById(const uint32_t id);
        void SetItems(const Type& stringIdPairVec);
        uint32_t GetSelectedId() const;
        void PopBack(const uint64_t count);
        void Clear();
        void PushBack(const std::tuple<std::string, uint32_t, bool>& stringIdPair);

    protected:

    private:
        void assign(const StringIdBoolCombo& other);
        void assignPointers();

        Type _stringIdBoolPairVec;
    };
}