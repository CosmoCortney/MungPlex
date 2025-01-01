#include "IDevice.hpp"
#include "HelperFunctions.hpp"

inline const std::vector<MungPlex::StringIdCombo::VecType> MungPlex::IDevice::s_DeviceTypes =
{
	{
		{ "Lovense", LOVENSE }
	}
};

inline const std::vector<MungPlex::StringIdCombo::VecType> MungPlex::IDevice::s_ValueTypes =
{
	{
		{ "Int 8 (1 Byte)", INT8 },
		{ "Int 16 (2 Bytes)", INT16 },
		{ "Int 32 (4 Bytes)", INT32 },
		{ "Int 64 (8 Bytes)", INT64, },
		{ "Float Single", FLOAT },
		{ "Float Double", DOUBLE },
		{ "Bool",  BOOL }
	}
};

int MungPlex::IDevice::GetID()
{
	return _deviceId;
}
