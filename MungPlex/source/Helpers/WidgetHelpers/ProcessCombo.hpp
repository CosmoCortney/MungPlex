#pragma once
#include <boost/asio.hpp>
#include "ICombo.hpp"
#include "Xertz.hpp"

namespace MungPlex
{
    class ProcessCombo : public ICombo
    {
    public:
        //ProcessCombo() = default;
        ProcessCombo(const std::string& label, const bool printLabel, const bool isApplication);
        //~ProcessCombo() = default;
        ProcessCombo(const ProcessCombo& other);
        ProcessCombo& operator=(const ProcessCombo& other);
        ProcessCombo(ProcessCombo&& other) noexcept;
        ProcessCombo& operator=(ProcessCombo&& other) noexcept;

        bool Draw(const float paneWidth = 0.25f, const float labelPortion = 0.4f);
        void RefreshProcessInfo();
        const PROCESS_INFO& GetSelectedProcess() const;
        const PROCESS_INFO& GetProcessAt(const uint64_t index);
        const std::string& GetStdStringAt(const uint64_t index);
        const std::string& GetSelectedStdString() const;;
        uint32_t GetSelectedId() const;
        void PopBack(const uint64_t count);
        void Clear();

    protected:

    private:
        void assign(const ProcessCombo& other);
        void assignPointers();
        bool _isApplication = false;
    };
}