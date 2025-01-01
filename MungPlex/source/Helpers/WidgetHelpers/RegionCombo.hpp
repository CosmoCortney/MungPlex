#pragma once
#include "ICombo.hpp"
#include "SystemRegion.hpp"

namespace MungPlex
{
    class RegionCombo : public ICombo
    {
    public:
        RegionCombo(const std::string& label, const bool printLabel) : ICombo(label, printLabel) {}
        RegionCombo(const std::string& label, const bool printLabel, const std::vector<SystemRegion>& regionVec);
        RegionCombo(const RegionCombo& other);
        RegionCombo& operator=(const RegionCombo& other);
        RegionCombo(RegionCombo&& other) noexcept;
        RegionCombo& operator=(RegionCombo&& other) noexcept;
        const std::string& GetStdStringAt(const uint64_t index) { throw _err; }
        const std::string& GetSelectedStdString() const { throw _err; }
        int32_t GetSelectedId() const { throw _err; }
        const SystemRegion& GetRegionAt(const uint64_t index);
        const SystemRegion& GetSelectedRegion() const;
        void SetItems(const std::vector<SystemRegion>& regionVec);
        void PopBack(const uint64_t count);
        void Clear();
        void PushBack(const SystemRegion& stringIdPair);

    private:
        void assign(const RegionCombo& other);
        void assignPointers();

        std::vector<SystemRegion> _regionVec{};
        const std::string _err = "Function not defined.";
    };
}