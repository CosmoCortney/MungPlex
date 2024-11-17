#pragma once
#include <initializer_list>
#include <string>
#include <type_traits>
#include <vector>

namespace MungPlex
{
    class IPairs
    {
    public:
        const char* GetCString(const uint32_t index) const;
        const char* const* GetData() const;
        const uint32_t GetCount() const;
        const std::string& GetLabelStdString() const;
        const char* GetLabelCString() const;
        const std::string& GetComboLabelStdString() const;
        const char* GetComboLabelCString() const;

    protected:
        std::vector<const char*> _stringPointers;
        std::string _label;
        std::string _comboLabel;

        void setLabel(const std::string label);

    private:

    };

    class StringIdPairs: public IPairs
    {
    public:
        StringIdPairs(const std::vector<std::string>& strings, const std::vector<int32_t>& ids, const std::string& label);

        const std::string& GetStdString(const uint32_t index) const;
        const std::string& GetStdStringById(const int32_t id) const;
        const int GetId(const uint32_t index) const;
        const int GetIndexById(const int id) const;
        void PushBack(const std::string& str, const int id);
        void PopBack(const uint32_t count);
        void Clear();

    private:
        std::vector<std::string> _strings;
        std::vector<int> _ids;
    };

    class ProcessInforPairs: public IPairs
    {
    public:
        ProcessInforPairs(const std::string& label);

        void RefreshProcessInfo();

    private:
    };

    class ProcessApplicationInforPairs: public IPairs
    {
    public:
        ProcessApplicationInforPairs(const std::string& label);

        void RefreshApplicationProcessInfo();

    private:

    };
    /*
    class RegionPairs: public IPairs
    {
    public:
        RegionPairs(const std::string& label);
        const SystemRegion& GetRegion(const uint32_t index) const;
        const std::vector<SystemRegion>& GetRegions();
        void SetRegions(const std::vector<SystemRegion>& regions);

    private:
        std::vector<SystemRegion> _regions;
    };*/
}