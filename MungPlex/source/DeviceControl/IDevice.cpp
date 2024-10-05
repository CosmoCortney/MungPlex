#include "IDevice.hpp"
#include "../../HelperFunctions.hpp"

const std::vector<std::pair<std::string, int>> MungPlex::IDevice::s_DeviceTypes =
{
	//{ "XInput", XINPUT},
	{ "Lovense", LOVENSE}
};

const std::vector<std::pair<std::string, int>> MungPlex::IDevice::s_ValueTypes =
{
	{ "Int 8 (1 Byte)", INT8 },
	{ "Int 16 (2 Bytes)", INT16 },
	{ "Int 32 (4 Bytes)", INT32 },
	{ "Int 64 (8 Bytes)", INT64 },
	{ "Float Single", FLOAT },
	{ "Float Double", DOUBLE },
	{ "Bool", BOOL }
};
