#include "IDevice.hpp"
#include "HelperFunctions.hpp"

inline const MungPlex::StringIdPairs MungPlex::IDevice::s_DeviceTypes =
{
	{ "Lovense" },
	{ LOVENSE },
	"Device Type:"
};

inline const MungPlex::StringIdPairs MungPlex::IDevice::s_ValueTypes =
{
	{ "Int 8 (1 Byte)", "Int 16 (2 Bytes)", "Int 32 (4 Bytes)", "Int 64 (8 Bytes)", "Float Single", "Float Double", "Bool" },
	{ INT8,             INT16,              INT32,              INT64,              FLOAT,          DOUBLE,         BOOL },
	"Value Type:"
};

int MungPlex::IDevice::GetID()
{
	return _deviceId;
}
