#pragma once
#include "ICombo.hpp"

namespace MungPlex
{
    class RegionCombo : public ICombo
    {
    public:
        struct SystemRegion
        {
            std::string Label;
            uint64_t Base;
            uint64_t Size;
            void* BaseLocationProcess = nullptr;
        };

        typedef std::vector<RegionCombo::SystemRegion> Type;
        //RegionCombo() = default;
        RegionCombo(const std::string& label, const bool printLabel) : ICombo(label, printLabel) {}
        RegionCombo(const std::string& label, const bool printLabel, const Type& regionVec);
        //~RegionCombo() = default;
        RegionCombo(const RegionCombo& other);
        RegionCombo& operator=(const RegionCombo& other);
        RegionCombo(RegionCombo&& other) noexcept;
        RegionCombo& operator=(RegionCombo&& other) noexcept;
        const std::string& GetStdStringAt(const uint64_t index) { throw _err; }
        const std::string& GetSelectedStdString() const { throw _err; }
        uint32_t GetSelectedId() const { throw _err; }

        bool Draw(const float paneWidth = 0.25f, const float labelPortion = 0.4f);
        const RegionCombo::SystemRegion& GetRegionAt(const uint64_t index);
        const RegionCombo::SystemRegion& GetSelectedRegion() const;
        void SetItems(const Type& regionVec);
        void PopBack(const uint64_t count);
        void Clear();
        void PushBack(const RegionCombo::SystemRegion& stringIdPair);
        
    protected:

    private:
        void assign(const RegionCombo& other);
        void assignPointers();

        Type _regionVec{};
       // std::vector<std::string> _labelVec;
        const std::string _err = "Function not defined.";
    };
}