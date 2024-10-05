#pragma once
#include <string>
#include <utility>
#include <vector>

namespace MungPlex
{
    class IDevice
    {
    protected:
        //int _typeID = DeviceTypes::UNK;
        int _deviceId = -1;
        std::string _idText;
        std::string _name = std::string(128, '\0');
        std::string _label = std::string(128, '\0');
        bool _connected = false;
        bool _delete = false;
        bool _active = false;
        int _valueTypeIndex = 2;

    public:
        enum DeviceTypes
        {
            XINPUT, LOVENSE, TOBII, UNK = -1
        };
        static const std::vector<std::pair<std::string, int>> s_DeviceTypes;
        static const std::vector<std::pair<std::string, int>> s_ValueTypes;
        int GetID();
        const bool IsConnected();
    };
}