#pragma once
#include "HelperFunctions.hpp"
#include <string>
#include <utility>
#include <vector>
#include "WidgetHelpers.hpp"

namespace MungPlex
{
    class IDevice
    {
    public:
        enum DeviceTypes
        {
            XINPUT, LOVENSE, TOBII, UNK = -1
        };
        static const StringIdCombo::Type s_DeviceTypes;
        static const StringIdCombo::Type s_ValueTypes;
        int GetID();
        virtual void Draw() = 0;
        virtual nlohmann::json GetJSON() = 0;

    protected:
        int _deviceTypeID = DeviceTypes::UNK;
        int _deviceId = -1;
        std::string _idText;
        InputText _nameInput = InputText("Name:", true, "", 128);
        bool _delete = false;
        bool _active = false;

    };
}