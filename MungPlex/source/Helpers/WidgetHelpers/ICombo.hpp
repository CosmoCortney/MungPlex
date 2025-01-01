#pragma once
#include <boost/asio.hpp>
#include "IWidget.hpp"
#include <utility>

namespace MungPlex
{
    class ICombo : public IWidget
    {
    public:
        ICombo() = default;
        ICombo(const std::string& label, const bool printLabel = true) : IWidget(label, printLabel) {}
        virtual bool Draw(const float paneWidth = 0.25f, const float labelPortion = 0.4f) = 0;
        const uint64_t GetCount() const;
        const uint64_t GetSelectedIndex() const;
        void SetSelectedByIndex(const uint64_t index);
        const char* GetCStringAt(const uint64_t index);
        const char* GetSelectedCString() const;
        const char* const* GetCStringPointers() const;
        virtual const std::string& GetStdStringAt(const uint64_t index) = 0;
        virtual const std::string& GetSelectedStdString() const = 0;
        virtual uint32_t GetSelectedId() const = 0;
        virtual void PopBack(const uint64_t count) = 0;
        virtual void Clear() = 0;
        void ConnectOnIndexChangedSlot(const Slot slot);

    protected:
        std::vector<const char*> _stringPointers{};
        uint64_t _selectedIndex = 0;
        std::vector<Slot> _slotsOnIndexChanged{};

        void isInRange(const uint64_t index);
        void callOnIndexChangedSlots();

    private:
    };
}