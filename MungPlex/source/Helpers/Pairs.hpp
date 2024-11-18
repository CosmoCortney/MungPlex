#pragma once
#include <initializer_list>
#include <string>
#include <type_traits>
#include <vector>
#include "Xertz.hpp"

namespace MungPlex
{
    struct GameEntity
    {
        std::string Entity;
        uint64_t Location;
        std::string Datatype;
        int Size;
        bool Hex;
        std::string Value;
    };

    struct SystemRegion
    {
        std::string Label;
        uint64_t Base;
        uint64_t Size;
        void* BaseLocationProcess = nullptr;
    };

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

    protected:
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
  
    class RegionPairs: public IPairs
    {
    public:
        RegionPairs(const std::string& label);
        const SystemRegion& GetRegion(const uint32_t index) const;
        const std::vector<SystemRegion>& GetRegions();
        void SetRegions(const std::vector<SystemRegion>& regions);
        void Clear();

    private:
        std::vector<SystemRegion> _regions;
    };

    class DoubleStringIdPairs: public IPairs
    {
    public:
        DoubleStringIdPairs(const std::vector<std::string>& entityNames, const std::vector<std::string>& labelList, const std::vector<int32_t>& ids, const std::string& label);
        DoubleStringIdPairs(const DoubleStringIdPairs&) = delete;
        DoubleStringIdPairs& operator=(const DoubleStringIdPairs&) = delete;

        const std::string& GetStdStringLabel(const uint32_t index) const;
        const std::string& GetStdStringLabelById(const int32_t id) const;
        const std::string& GetStdStringEntity(const uint32_t index) const;
        const std::string& GetStdStrinEntityById(const int32_t id) const;
        const int GetId(const uint32_t index) const;
        const int GetIndexById(const int id) const;
        void PushBack(const std::string& entity, const std::string& label, const int id);
        void PopBack(const uint32_t count);
        void Clear();

    private:
        std::vector<std::string> _labelList;
        std::vector<std::string> _entityNames;
        std::vector<int> _ids;
    };

    class StringIdBoolPairs: public StringIdPairs
    {
    public:
        StringIdBoolPairs(const std::vector<std::string>& strings, const std::vector<int32_t>& ids, const std::vector<bool>& flags, const std::string& label);

        void PushBack(const std::string& str, const int id, const bool flag);
        void PopBack(const uint32_t count);
        bool GetFlag(const uint32_t index) const;
        void Clear();

    private:
        std::vector<bool> _flags;
    };
}