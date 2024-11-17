#pragma once
#include "HelperFunctions.hpp"
#include <string>
#include <utility>
#include <vector>

namespace MungPlex
{
    class IDevice
    {
    public:
        enum DeviceTypes
        {
            XINPUT, LOVENSE, TOBII, UNK = -1
        };
        static const StringIdPairs s_DeviceTypes;
        static const StringIdPairs s_ValueTypes;
        int GetID();
        virtual void Draw() = 0;
        virtual nlohmann::json GetJSON() = 0;

    protected:
        int _deviceTypeID = DeviceTypes::UNK;
        int _deviceId = -1;
        std::string _idText;
        FloorString _name = FloorString("", 128);
        bool _delete = false;
        bool _active = false;

    };
}