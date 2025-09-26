#include "CheatConvert.hpp"
#include <iostream>
#include "HelperFunctions.hpp"
#include "../../Log.hpp"
#include "StringHelpers.hpp"

std::vector<std::pair<uint32_t, uint32_t>> MungPlex::CheatConvert::_dissectedCheat;
std::string MungPlex::CheatConvert::_lua;
std::vector<int> MungPlex::CheatConvert::_layerCount;

bool MungPlex::CheatConvert::GcnArToLua()
{
	if (!DissectCheat())
		return false;

	_lua.clear();

	for (int elem = 0; elem < _dissectedCheat.size(); ++elem)
	{
		std::pair<uint32_t, uint32_t>& line = _dissectedCheat[elem];
		
		if(!_layerCount.empty())
			_layerCount.back() -= 1;

		if (line.first == 0) //FillSide or Terminator
		{
			uint32_t address = 0x80000000 | (line.second & 0x01FFFFFF);
			std::string addressStr;
			std::stringstream stream;
			stream << std::hex << address;
			stream >> std::hex >> addressStr;
			GcnArCodeTypes type = static_cast<GcnArCodeTypes>(*(reinterpret_cast<uint8_t*>(&line.second) + 3));
			line = _dissectedCheat[++elem];
			uint32_t val = line.first;
			std::string writeStr;
			int leftJustifyValue = 8;
			std::string offsetStr = "i * 4";

			if (type == GcnArCodeTypes::INCREMENT_8)
			{
				writeStr = "WriteInt8(";
				std::string offsetStr = "i";
				val &= 0xFF;
				leftJustifyValue = 2;
			}
			else if(type == GcnArCodeTypes::INCREMENT_16)
			{
				writeStr = "WriteInt16(";
				std::string offsetStr = "i * 2";
				val &= 0xFFFF;
				leftJustifyValue = 4;
			}
			else
				writeStr = "WriteInt32(";

			int8_t valInc = line.second >> 24;
			uint8_t wCount = line.second >> 16;
			int16_t addrInc = line.second & 0xFFFF;
			
			_lua.append(generateIndent() + "for i = 0, " + ToHexString(wCount, 2, true) + " do\n");
			_lua.append(generateIndent() + "  " + writeStr + ToHexString(address, 8, true) + " + i * " + ToHexString(addrInc, 4, true) + ", " + ToHexString(val, leftJustifyValue, true) + " + i * " + ToHexString(valInc, 2, true) + ")\n");
			_lua.append(generateIndent() + "end\n");
		}
		else
		{
			std::pair<uint32_t, uint32_t>& line = _dissectedCheat[elem];
			uint32_t address = 0x80000000 | (line.first & 0x01FFFFFF);
			std::string addressStr;
			std::stringstream stream;
			stream << std::hex << address;
			stream >> std::hex >> addressStr;
			GcnArCodeTypes type = static_cast<GcnArCodeTypes>(*(reinterpret_cast<uint8_t*>(&line.first) + 3));

			switch (type)
			{
				case GcnArCodeTypes::WRITE_8: case GcnArCodeTypes::WRITE_8H: 
				case GcnArCodeTypes::WRITE_16: case GcnArCodeTypes::WRITE_16H: 
				case GcnArCodeTypes::WRITE_32: case GcnArCodeTypes::WRITE_32H:
				{
					uint32_t count = line.second;
					uint32_t val = line.second;
					std::string writeStr;
					int leftJustifyValue = 8;
					std::string incExpr = " + i, ";

					if (type == GcnArCodeTypes::WRITE_8 || type == GcnArCodeTypes::WRITE_8H)
					{
						count >>= 8;
						val &= 0xFF;
						writeStr = "WriteInt8(";
						leftJustifyValue = 2;
					}
					else if (type == GcnArCodeTypes::WRITE_16 || type == GcnArCodeTypes::WRITE_16H)
					{
						count >>= 16;
						val &= 0xFFFF;
						writeStr = "WriteInt16(";
						leftJustifyValue = 4;
						incExpr = " + i * 2, ";
					}
					else
					{
						count = 0;
						writeStr = "WriteInt32(";
					}

					if (count)
					{
						_lua.append(generateIndent() + "for i = 0, " + ToHexString(count, 2, true) + " do\n");
						_lua.append(generateIndent() + "  " + writeStr + ToHexString(address, 8, true) + incExpr + ToHexString(val, leftJustifyValue, true) + ")\n");
						_lua.append(generateIndent() + "end\n");
					}
					else
						_lua.append(generateIndent() + writeStr + ToHexString(address, 8, true) + ", " + ToHexString(val, leftJustifyValue, true) + ")\n");
				} break;
				case GcnArCodeTypes::STRUCT_WRITE_8: case GcnArCodeTypes::STRUCT_WRITE_8H: 
				case GcnArCodeTypes::STRUCT_WRITE_16: case GcnArCodeTypes::STRUCT_WRITE_16H: 
				case GcnArCodeTypes::STRUCT_WRITE_32: case GcnArCodeTypes::STRUCT_WRITE_32H:
				{
					uint32_t val = line.second;
					uint32_t offset = val;
					std::string writeStr;
					int leftJustifyValue = 8;

					if (type == GcnArCodeTypes::STRUCT_WRITE_8 || type == GcnArCodeTypes::STRUCT_WRITE_8H)
					{
						val &= 0xFF;
						offset >>= 8;
						writeStr = "WriteInt8(";
						leftJustifyValue = 2;
					}
					else if (type == GcnArCodeTypes::STRUCT_WRITE_16 || type == GcnArCodeTypes::STRUCT_WRITE_16H)
					{
						val &= 0xFFFF;
						offset = (offset >>= 16) * 2;
						writeStr = "WriteInt16(";
						leftJustifyValue = 4;
					}
					else
						writeStr = "WriteInt32(";

					_lua.append(generateIndent() + "ptr = ReadUInt32(" + ToHexString(address, 8, true) + ")\n");
				
					if (type != GcnArCodeTypes::STRUCT_WRITE_32 && type != GcnArCodeTypes::STRUCT_WRITE_32H)
						_lua.append(generateIndent() + "ptr = ptr + " + ToHexString(offset, 2, true) + "\n");

					_lua.append(generateIndent() + "if IsInRange(ptr, 0x80000000, 0x818000000) then\n");
					_lua.append(generateIndent() + "  " + writeStr + "ptr, " + ToHexString(val, leftJustifyValue, true) + ")\n");
					_lua.append(generateIndent() + "end\n");
				} break;
				case GcnArCodeTypes::INCREMENT_8: case GcnArCodeTypes::INCREMENT_8H:
				case GcnArCodeTypes::INCREMENT_16: case GcnArCodeTypes::INCREMENT_16H:
				case GcnArCodeTypes::INCREMENT_32: case GcnArCodeTypes::INCREMENT_32H:
				case GcnArCodeTypes::INCREMENT_FLOAT: case GcnArCodeTypes::INCREMENT_FLOATH:
				{
					int32_t val = line.second;
					std::string writeStr;
					std::string readStr;
					int leftJustifyValue = 8;

					if (type == GcnArCodeTypes::INCREMENT_8 || type == GcnArCodeTypes::INCREMENT_8H)
					{
						val &= 0xFF;
						readStr = "ReadInt8(";
						writeStr = "WriteInt8(";
						leftJustifyValue = 2;
					}
					else if (type == GcnArCodeTypes::INCREMENT_16 || type == GcnArCodeTypes::INCREMENT_16H)
					{
						val &= 0xFFFF;
						readStr = "ReadInt16(";
						writeStr = "WriteInt16(";
						leftJustifyValue = 4;
					}
					else if (type == GcnArCodeTypes::INCREMENT_32 || type == GcnArCodeTypes::INCREMENT_32H)
					{
						readStr = "ReadInt32(";
						writeStr = "WriteInt32(";
					}
					else
					{
						readStr = "ReadFloat(";
						writeStr = "WriteFloat(";
					}

					_lua.append(generateIndent() + "val = " + readStr + ToHexString(address, 8, true) + ")\n");

					if (type == GcnArCodeTypes::INCREMENT_FLOAT || type == GcnArCodeTypes::INCREMENT_FLOATH)
						_lua.append(generateIndent() + "val = val + " + std::to_string(static_cast<float>(val)) + "\n");
					else
						_lua.append(generateIndent() + "val = val + " + ToHexString(val, leftJustifyValue, true) + "\n");

					_lua.append(generateIndent() + writeStr + ToHexString(address, 8, true) + ", val)\n");
				} break;
				default: //all conditionals
				{
					bool signedFlag = true;
					std::string opStr;
					int32_t val = line.second;
					std::string writeStr;
					std::string readStr;
					int leftJustifyValue = 8;
					std::string valStr;

					// r/w setup
					switch (type)
					{
					case GcnArCodeTypes::IF_EQ_NL_8: case GcnArCodeTypes::IF_EQ_NL_8H:
					case GcnArCodeTypes::IF_EQ_N2L_8: case GcnArCodeTypes::IF_EQ_N2L_8H:
					case GcnArCodeTypes::IF_EQ_AU_8: case GcnArCodeTypes::IF_EQ_AU_8H:
					case GcnArCodeTypes::IF_EQ_ALL_8: case GcnArCodeTypes::IF_EQ_ALL_8H:
					case GcnArCodeTypes::IF_NE_NL_8: case GcnArCodeTypes::IF_NE_NL_8H:
					case GcnArCodeTypes::IF_NE_N2L_8: case GcnArCodeTypes::IF_NE_N2L_8H:
					case GcnArCodeTypes::IF_NE_AU_8: case GcnArCodeTypes::IF_NE_AU_8H:
					case GcnArCodeTypes::IF_NE_ALL_8: case GcnArCodeTypes::IF_NE_ALL_8H:
					case GcnArCodeTypes::IF_LT_NL_8: case GcnArCodeTypes::IF_LT_NL_8H:
					case GcnArCodeTypes::IF_LT_N2L_8: case GcnArCodeTypes::IF_LT_N2L_8H:
					case GcnArCodeTypes::IF_LT_AU_8: case GcnArCodeTypes::IF_LT_AU_8H:
					case GcnArCodeTypes::IF_LT_ALL_8: case GcnArCodeTypes::IF_LT_ALL_8H:
					case GcnArCodeTypes::IF_GT_NL_8: case GcnArCodeTypes::IF_GT_NL_8H:
					case GcnArCodeTypes::IF_GT_N2L_8: case GcnArCodeTypes::IF_GT_N2L_8H:				
					case GcnArCodeTypes::IF_GT_AU_8: case GcnArCodeTypes::IF_GT_AU_8H:
					case GcnArCodeTypes::IF_GT_ALL_8: case GcnArCodeTypes::IF_GT_ALL_8H:
					case GcnArCodeTypes::IF_AND_NL_8: case GcnArCodeTypes::IF_AND_NL_8H:
					case GcnArCodeTypes::IF_AND_N2L_8: case GcnArCodeTypes::IF_AND_N2L_8H:
					case GcnArCodeTypes::IF_AND_AU_8: case GcnArCodeTypes::IF_AND_AU_8H:
					case GcnArCodeTypes::IF_AND_ALL_8: case GcnArCodeTypes::IF_AND_ALL_8H:
						val &= 0xFF;
						readStr = "ReadInt8(";
						writeStr = "WriteInt8(";
						leftJustifyValue = 2;
						break;				
					case GcnArCodeTypes::IF_LT_NL_U8: case GcnArCodeTypes::IF_LT_NL_U8H:
					case GcnArCodeTypes::IF_LT_N2L_U8: case GcnArCodeTypes::IF_LT_N2L_U8H:
					case GcnArCodeTypes::IF_LT_AU_U8: case GcnArCodeTypes::IF_LT_AU_U8H:
					case GcnArCodeTypes::IF_LT_ALL_U8: case GcnArCodeTypes::IF_LT_ALL_U8H:
					case GcnArCodeTypes::IF_GT_NL_U8: case GcnArCodeTypes::IF_GT_NL_U8H:
					case GcnArCodeTypes::IF_GT_N2L_U8: case GcnArCodeTypes::IF_GT_N2L_U8H:
					case GcnArCodeTypes::IF_GT_AU_U8: case GcnArCodeTypes::IF_GT_AU_U8H:
					case GcnArCodeTypes::IF_GT_ALL_U8: case GcnArCodeTypes::IF_GT_ALL_U8H:
						val &= 0xFF;
						readStr = "ReadUInt8(";
						writeStr = "WriteInt8(";
						leftJustifyValue = 2;
						break;
					case GcnArCodeTypes::IF_EQ_NL_16: case GcnArCodeTypes::IF_EQ_NL_16H:
					case GcnArCodeTypes::IF_EQ_N2L_16: case GcnArCodeTypes::IF_EQ_N2L_16H:
					case GcnArCodeTypes::IF_EQ_AU_16: case GcnArCodeTypes::IF_EQ_AU_16H:
					case GcnArCodeTypes::IF_EQ_ALL_16: case GcnArCodeTypes::IF_EQ_ALL_16H:
					case GcnArCodeTypes::IF_NE_NL_16: case GcnArCodeTypes::IF_NE_NL_16H:
					case GcnArCodeTypes::IF_NE_N2L_16: case GcnArCodeTypes::IF_NE_N2L_16H:
					case GcnArCodeTypes::IF_NE_AU_16: case GcnArCodeTypes::IF_NE_AU_16H:
					case GcnArCodeTypes::IF_NE_ALL_16: case GcnArCodeTypes::IF_NE_ALL_16H:
					case GcnArCodeTypes::IF_LT_NL_16: case GcnArCodeTypes::IF_LT_NL_16H:
					case GcnArCodeTypes::IF_LT_N2L_16: case GcnArCodeTypes::IF_LT_N2L_16H:
					case GcnArCodeTypes::IF_LT_AU_16: case GcnArCodeTypes::IF_LT_AU_16H:
					case GcnArCodeTypes::IF_LT_ALL_16: case GcnArCodeTypes::IF_LT_ALL_16H:
					case GcnArCodeTypes::IF_GT_NL_16: case GcnArCodeTypes::IF_GT_NL_16H:
					case GcnArCodeTypes::IF_GT_N2L_16: case GcnArCodeTypes::IF_GT_N2L_16H:
					case GcnArCodeTypes::IF_GT_AU_16: case GcnArCodeTypes::IF_GT_AU_16H:
					case GcnArCodeTypes::IF_GT_ALL_16: case GcnArCodeTypes::IF_GT_ALL_16H:
					case GcnArCodeTypes::IF_AND_NL_16: case GcnArCodeTypes::IF_AND_NL_16H:
					case GcnArCodeTypes::IF_AND_N2L_16: case GcnArCodeTypes::IF_AND_N2L_16H:
					case GcnArCodeTypes::IF_AND_AU_16: case GcnArCodeTypes::IF_AND_AU_16H:
					case GcnArCodeTypes::IF_AND_ALL_16: case GcnArCodeTypes::IF_AND_ALL_16H:
						val &= 0xFFFF;
						readStr = "ReadInt16(";
						writeStr = "WriteInt16(";
						leftJustifyValue = 4;
						break;
					case GcnArCodeTypes::IF_LT_NL_U16: case GcnArCodeTypes::IF_LT_NL_U16H:
					case GcnArCodeTypes::IF_LT_N2L_U16: case GcnArCodeTypes::IF_LT_N2L_U16H:
					case GcnArCodeTypes::IF_LT_AU_U16: case GcnArCodeTypes::IF_LT_AU_U16H:
					case GcnArCodeTypes::IF_LT_ALL_U16: case GcnArCodeTypes::IF_LT_ALL_U16H:
					case GcnArCodeTypes::IF_GT_NL_U16: case GcnArCodeTypes::IF_GT_NL_U16H:
					case GcnArCodeTypes::IF_GT_N2L_U16: case GcnArCodeTypes::IF_GT_N2L_U16H:
					case GcnArCodeTypes::IF_GT_AU_U16: case GcnArCodeTypes::IF_GT_AU_U16H:
					case GcnArCodeTypes::IF_GT_ALL_U16: case GcnArCodeTypes::IF_GT_ALL_U16H:
						val &= 0xFFFF;
						readStr = "ReadUInt16(";
						writeStr = "WriteUInt16(";
						leftJustifyValue = 4;
						break;
					case GcnArCodeTypes::IF_LT_NL_U32: case GcnArCodeTypes::IF_LT_NL_U32H:
					case GcnArCodeTypes::IF_LT_N2L_U32: case GcnArCodeTypes::IF_LT_N2L_U32H:
					case GcnArCodeTypes::IF_LT_AU_U32: case GcnArCodeTypes::IF_LT_AU_U32H:
					case GcnArCodeTypes::IF_LT_ALL_U32: case GcnArCodeTypes::IF_LT_ALL_U32H:
					case GcnArCodeTypes::IF_GT_NL_U32: case GcnArCodeTypes::IF_GT_NL_U32H:
					case GcnArCodeTypes::IF_GT_N2L_U32: case GcnArCodeTypes::IF_GT_N2L_U32H:
					case GcnArCodeTypes::IF_GT_AU_U32: case GcnArCodeTypes::IF_GT_AU_U32H:
					case GcnArCodeTypes::IF_GT_ALL_U32: case GcnArCodeTypes::IF_GT_ALL_U32H:
						readStr = "ReadUInt32(";
						writeStr = "WriteUInt32(";
						break;
					default:
						readStr = "ReadInt32(";
						writeStr = "WriteInt32(";
					}

					//signed flag
					switch (type)
					{
					case GcnArCodeTypes::IF_LT_NL_U8: case GcnArCodeTypes::IF_LT_NL_U8H:
					case GcnArCodeTypes::IF_LT_NL_U16: case GcnArCodeTypes::IF_LT_NL_U16H:
					case GcnArCodeTypes::IF_LT_NL_U32: case GcnArCodeTypes::IF_LT_NL_U32H:
					case GcnArCodeTypes::IF_LT_N2L_U8: case GcnArCodeTypes::IF_LT_N2L_U8H:
					case GcnArCodeTypes::IF_LT_N2L_U16: case GcnArCodeTypes::IF_LT_N2L_U16H:
					case GcnArCodeTypes::IF_LT_N2L_U32: case GcnArCodeTypes::IF_LT_N2L_U32H:
					case GcnArCodeTypes::IF_LT_AU_U8: case GcnArCodeTypes::IF_LT_AU_U8H:
					case GcnArCodeTypes::IF_LT_AU_U16: case GcnArCodeTypes::IF_LT_AU_U16H:
					case GcnArCodeTypes::IF_LT_AU_U32: case GcnArCodeTypes::IF_LT_AU_U32H:
					case GcnArCodeTypes::IF_LT_ALL_U8: case GcnArCodeTypes::IF_LT_ALL_U8H:
					case GcnArCodeTypes::IF_LT_ALL_U16: case GcnArCodeTypes::IF_LT_ALL_U16H:
					case GcnArCodeTypes::IF_LT_ALL_U32: case GcnArCodeTypes::IF_LT_ALL_U32H:
					case GcnArCodeTypes::IF_GT_NL_U8: case GcnArCodeTypes::IF_GT_NL_U8H:
					case GcnArCodeTypes::IF_GT_NL_U16: case GcnArCodeTypes::IF_GT_NL_U16H:
					case GcnArCodeTypes::IF_GT_NL_U32: case GcnArCodeTypes::IF_GT_NL_U32H:
					case GcnArCodeTypes::IF_GT_N2L_U8: case GcnArCodeTypes::IF_GT_N2L_U8H:
					case GcnArCodeTypes::IF_GT_N2L_U16: case GcnArCodeTypes::IF_GT_N2L_U16H:
					case GcnArCodeTypes::IF_GT_N2L_U32: case GcnArCodeTypes::IF_GT_N2L_U32H:
					case GcnArCodeTypes::IF_GT_AU_U8: case GcnArCodeTypes::IF_GT_AU_U8H:
					case GcnArCodeTypes::IF_GT_AU_U16: case GcnArCodeTypes::IF_GT_AU_U16H:
					case GcnArCodeTypes::IF_GT_AU_U32: case GcnArCodeTypes::IF_GT_AU_U32H:
					case GcnArCodeTypes::IF_GT_ALL_U8: case GcnArCodeTypes::IF_GT_ALL_U8H:
					case GcnArCodeTypes::IF_GT_ALL_U16: case GcnArCodeTypes::IF_GT_ALL_U16H:
					case GcnArCodeTypes::IF_GT_ALL_U32: case GcnArCodeTypes::IF_GT_ALL_U32H:
						signedFlag = false;
						break;
					}

					// comp ops
					switch (type)
					{
					case GcnArCodeTypes::IF_EQ_NL_8: case GcnArCodeTypes::IF_EQ_NL_8H:
					case GcnArCodeTypes::IF_EQ_NL_16: case GcnArCodeTypes::IF_EQ_NL_16H:
					case GcnArCodeTypes::IF_EQ_NL_32: case GcnArCodeTypes::IF_EQ_NL_32H:
					case GcnArCodeTypes::IF_EQ_N2L_8: case GcnArCodeTypes::IF_EQ_N2L_8H:
					case GcnArCodeTypes::IF_EQ_N2L_16: case GcnArCodeTypes::IF_EQ_N2L_16H:
					case GcnArCodeTypes::IF_EQ_N2L_32: case GcnArCodeTypes::IF_EQ_N2L_32H:
					case GcnArCodeTypes::IF_EQ_AU_8: case GcnArCodeTypes::IF_EQ_AU_8H:
					case GcnArCodeTypes::IF_EQ_AU_16: case GcnArCodeTypes::IF_EQ_AU_16H:
					case GcnArCodeTypes::IF_EQ_AU_32: case GcnArCodeTypes::IF_EQ_AU_32H:
					case GcnArCodeTypes::IF_EQ_ALL_8: case GcnArCodeTypes::IF_EQ_ALL_8H:
					case GcnArCodeTypes::IF_EQ_ALL_16: case GcnArCodeTypes::IF_EQ_ALL_16H:
					case GcnArCodeTypes::IF_EQ_ALL_32: case GcnArCodeTypes::IF_EQ_ALL_32H:
						opStr = "==";
						break;
					case GcnArCodeTypes::IF_NE_NL_8: case GcnArCodeTypes::IF_NE_NL_8H:
					case GcnArCodeTypes::IF_NE_NL_16: case GcnArCodeTypes::IF_NE_NL_16H:
					case GcnArCodeTypes::IF_NE_NL_32: case GcnArCodeTypes::IF_NE_NL_32H:
					case GcnArCodeTypes::IF_NE_N2L_8: case GcnArCodeTypes::IF_NE_N2L_8H:
					case GcnArCodeTypes::IF_NE_N2L_16: case GcnArCodeTypes::IF_NE_N2L_16H:
					case GcnArCodeTypes::IF_NE_N2L_32: case GcnArCodeTypes::IF_NE_N2L_32H:
					case GcnArCodeTypes::IF_NE_AU_8: case GcnArCodeTypes::IF_NE_AU_8H:
					case GcnArCodeTypes::IF_NE_AU_16: case GcnArCodeTypes::IF_NE_AU_16H:
					case GcnArCodeTypes::IF_NE_AU_32: case GcnArCodeTypes::IF_NE_AU_32H:
					case GcnArCodeTypes::IF_NE_ALL_8: case GcnArCodeTypes::IF_NE_ALL_8H:
					case GcnArCodeTypes::IF_NE_ALL_16: case GcnArCodeTypes::IF_NE_ALL_16H:
					case GcnArCodeTypes::IF_NE_ALL_32: case GcnArCodeTypes::IF_NE_ALL_32H:
						opStr = "!=";
						break;
					case GcnArCodeTypes::IF_GT_NL_8: case GcnArCodeTypes::IF_GT_NL_8H:
					case GcnArCodeTypes::IF_GT_NL_16: case GcnArCodeTypes::IF_GT_NL_16H:
					case GcnArCodeTypes::IF_GT_NL_32: case GcnArCodeTypes::IF_GT_NL_32H:
					case GcnArCodeTypes::IF_GT_N2L_8: case GcnArCodeTypes::IF_GT_N2L_8H:
					case GcnArCodeTypes::IF_GT_N2L_16: case GcnArCodeTypes::IF_GT_N2L_16H:
					case GcnArCodeTypes::IF_GT_N2L_32: case GcnArCodeTypes::IF_GT_N2L_32H:
					case GcnArCodeTypes::IF_GT_AU_8: case GcnArCodeTypes::IF_GT_AU_8H:
					case GcnArCodeTypes::IF_GT_AU_16: case GcnArCodeTypes::IF_GT_AU_16H:
					case GcnArCodeTypes::IF_GT_AU_32: case GcnArCodeTypes::IF_GT_AU_32H:
					case GcnArCodeTypes::IF_GT_ALL_8: case GcnArCodeTypes::IF_GT_ALL_8H:
					case GcnArCodeTypes::IF_GT_ALL_16: case GcnArCodeTypes::IF_GT_ALL_16H:
					case GcnArCodeTypes::IF_GT_ALL_32: case GcnArCodeTypes::IF_GT_ALL_32H:
					case GcnArCodeTypes::IF_GT_NL_U8: case GcnArCodeTypes::IF_GT_NL_U8H:
					case GcnArCodeTypes::IF_GT_NL_U16: case GcnArCodeTypes::IF_GT_NL_U16H:
					case GcnArCodeTypes::IF_GT_NL_U32: case GcnArCodeTypes::IF_GT_NL_U32H:
					case GcnArCodeTypes::IF_GT_N2L_U8: case GcnArCodeTypes::IF_GT_N2L_U8H:
					case GcnArCodeTypes::IF_GT_N2L_U16: case GcnArCodeTypes::IF_GT_N2L_U16H:
					case GcnArCodeTypes::IF_GT_N2L_U32: case GcnArCodeTypes::IF_GT_N2L_U32H:
					case GcnArCodeTypes::IF_GT_AU_U8: case GcnArCodeTypes::IF_GT_AU_U8H:
					case GcnArCodeTypes::IF_GT_AU_U16: case GcnArCodeTypes::IF_GT_AU_U16H:
					case GcnArCodeTypes::IF_GT_AU_U32: case GcnArCodeTypes::IF_GT_AU_U32H:
					case GcnArCodeTypes::IF_GT_ALL_U8: case GcnArCodeTypes::IF_GT_ALL_U8H:
					case GcnArCodeTypes::IF_GT_ALL_U16: case GcnArCodeTypes::IF_GT_ALL_U16H:
					case GcnArCodeTypes::IF_GT_ALL_U32: case GcnArCodeTypes::IF_GT_ALL_U32H:
						opStr = ">";
						break;
					case GcnArCodeTypes::IF_AND_NL_8: case GcnArCodeTypes::IF_AND_NL_8H:
					case GcnArCodeTypes::IF_AND_NL_16: case GcnArCodeTypes::IF_AND_NL_16H:
					case GcnArCodeTypes::IF_AND_NL_32: case GcnArCodeTypes::IF_AND_NL_32H:
					case GcnArCodeTypes::IF_AND_N2L_8: case GcnArCodeTypes::IF_AND_N2L_8H:
					case GcnArCodeTypes::IF_AND_N2L_16: case GcnArCodeTypes::IF_AND_N2L_16H:
					case GcnArCodeTypes::IF_AND_N2L_32: case GcnArCodeTypes::IF_AND_N2L_32H:
					case GcnArCodeTypes::IF_AND_AU_8: case GcnArCodeTypes::IF_AND_AU_8H:
					case GcnArCodeTypes::IF_AND_AU_16: case GcnArCodeTypes::IF_AND_AU_16H:
					case GcnArCodeTypes::IF_AND_AU_32: case GcnArCodeTypes::IF_AND_AU_32H:
					case GcnArCodeTypes::IF_AND_ALL_8: case GcnArCodeTypes::IF_AND_ALL_8H:
					case GcnArCodeTypes::IF_AND_ALL_16: case GcnArCodeTypes::IF_AND_ALL_16H:
					case GcnArCodeTypes::IF_AND_ALL_32: case GcnArCodeTypes::IF_AND_ALL_32H:
						opStr = "and";
						break;
					default:
						opStr = "<";
					}

					if (signedFlag)
						valStr = ToHexString(val, leftJustifyValue, true);
					else
					{
						uint32_t uVal = val;
						valStr = ToHexString(uVal, leftJustifyValue, true);
					}

					_lua.append(generateIndent() + "if " + readStr + ToHexString(address, 8, true) + ") " + opStr + " " + valStr + " then\n");
				
					//level count
					switch (type)
					{
						case GcnArCodeTypes::IF_EQ_N2L_8: case GcnArCodeTypes::IF_EQ_N2L_8H:
						case GcnArCodeTypes::IF_EQ_N2L_16: case GcnArCodeTypes::IF_EQ_N2L_16H:
						case GcnArCodeTypes::IF_EQ_N2L_32: case GcnArCodeTypes::IF_EQ_N2L_32H:
						case GcnArCodeTypes::IF_NE_N2L_8: case GcnArCodeTypes::IF_NE_N2L_8H:
						case GcnArCodeTypes::IF_NE_N2L_16: case GcnArCodeTypes::IF_NE_N2L_16H:
						case GcnArCodeTypes::IF_NE_N2L_32: case GcnArCodeTypes::IF_NE_N2L_32H:
						case GcnArCodeTypes::IF_LT_N2L_8: case GcnArCodeTypes::IF_LT_N2L_8H:
						case GcnArCodeTypes::IF_LT_N2L_16: case GcnArCodeTypes::IF_LT_N2L_16H:
						case GcnArCodeTypes::IF_LT_N2L_32: case GcnArCodeTypes::IF_LT_N2L_32H:
						case GcnArCodeTypes::IF_GT_N2L_8: case GcnArCodeTypes::IF_GT_N2L_8H:
						case GcnArCodeTypes::IF_GT_N2L_16: case GcnArCodeTypes::IF_GT_N2L_16H:
						case GcnArCodeTypes::IF_GT_N2L_32: case GcnArCodeTypes::IF_GT_N2L_32H:
						case GcnArCodeTypes::IF_LT_N2L_U8: case GcnArCodeTypes::IF_LT_N2L_U8H:
						case GcnArCodeTypes::IF_LT_N2L_U16: case GcnArCodeTypes::IF_LT_N2L_U16H:
						case GcnArCodeTypes::IF_LT_N2L_U32: case GcnArCodeTypes::IF_LT_N2L_U32H:
						case GcnArCodeTypes::IF_GT_N2L_U8: case GcnArCodeTypes::IF_GT_N2L_U8H:
						case GcnArCodeTypes::IF_GT_N2L_U16: case GcnArCodeTypes::IF_GT_N2L_U16H:
						case GcnArCodeTypes::IF_GT_N2L_U32: case GcnArCodeTypes::IF_GT_N2L_U32H:
						case GcnArCodeTypes::IF_AND_N2L_8: case GcnArCodeTypes::IF_AND_N2L_8H:
						case GcnArCodeTypes::IF_AND_N2L_16: case GcnArCodeTypes::IF_AND_N2L_16H:
						case GcnArCodeTypes::IF_AND_N2L_32: case GcnArCodeTypes::IF_AND_N2L_32H:
							_layerCount.push_back(2);
							break;
						case GcnArCodeTypes::IF_EQ_AU_8: case GcnArCodeTypes::IF_EQ_AU_8H:
						case GcnArCodeTypes::IF_EQ_AU_16: case GcnArCodeTypes::IF_EQ_AU_16H:
						case GcnArCodeTypes::IF_EQ_AU_32: case GcnArCodeTypes::IF_EQ_AU_32H:
						case GcnArCodeTypes::IF_NE_AU_8: case GcnArCodeTypes::IF_NE_AU_8H:
						case GcnArCodeTypes::IF_NE_AU_16: case GcnArCodeTypes::IF_NE_AU_16H:
						case GcnArCodeTypes::IF_NE_AU_32: case GcnArCodeTypes::IF_NE_AU_32H:
						case GcnArCodeTypes::IF_LT_AU_8: case GcnArCodeTypes::IF_LT_AU_8H:
						case GcnArCodeTypes::IF_LT_AU_16: case GcnArCodeTypes::IF_LT_AU_16H:
						case GcnArCodeTypes::IF_LT_AU_32: case GcnArCodeTypes::IF_LT_AU_32H:
						case GcnArCodeTypes::IF_GT_AU_8: case GcnArCodeTypes::IF_GT_AU_8H:
						case GcnArCodeTypes::IF_GT_AU_16: case GcnArCodeTypes::IF_GT_AU_16H:
						case GcnArCodeTypes::IF_GT_AU_32: case GcnArCodeTypes::IF_GT_AU_32H:
						case GcnArCodeTypes::IF_LT_AU_U8: case GcnArCodeTypes::IF_LT_AU_U8H:
						case GcnArCodeTypes::IF_LT_AU_U16: case GcnArCodeTypes::IF_LT_AU_U16H:
						case GcnArCodeTypes::IF_LT_AU_U32: case GcnArCodeTypes::IF_LT_AU_U32H:
						case GcnArCodeTypes::IF_GT_AU_U8: case GcnArCodeTypes::IF_GT_AU_U8H:
						case GcnArCodeTypes::IF_GT_AU_U16: case GcnArCodeTypes::IF_GT_AU_U16H:
						case GcnArCodeTypes::IF_GT_AU_U32: case GcnArCodeTypes::IF_GT_AU_U32H:
						case GcnArCodeTypes::IF_AND_AU_8: case GcnArCodeTypes::IF_AND_AU_8H:
						case GcnArCodeTypes::IF_AND_AU_16: case GcnArCodeTypes::IF_AND_AU_16H:
						case GcnArCodeTypes::IF_AND_AU_32: case GcnArCodeTypes::IF_AND_AU_32H:
						{
							bool found = false;

							for (int x = elem; x < _dissectedCheat.size(); ++x)
							{
								if (_dissectedCheat[x].second != 0x40000000)
									continue;

								_layerCount.push_back(x-1);
								found = true;
								break;
							}

							if (found)
								break;

							_layerCount.push_back(_dissectedCheat.size() - 1);
							Log::LogInformation("Warning: Missing terminator (00000000 40000000)");
						}
						break;
						case GcnArCodeTypes::IF_EQ_ALL_8: case GcnArCodeTypes::IF_EQ_ALL_8H:
						case GcnArCodeTypes::IF_EQ_ALL_16: case GcnArCodeTypes::IF_EQ_ALL_16H:
						case GcnArCodeTypes::IF_EQ_ALL_32: case GcnArCodeTypes::IF_EQ_ALL_32H:
						case GcnArCodeTypes::IF_NE_ALL_8: case GcnArCodeTypes::IF_NE_ALL_8H:
						case GcnArCodeTypes::IF_NE_ALL_16: case GcnArCodeTypes::IF_NE_ALL_16H:
						case GcnArCodeTypes::IF_NE_ALL_32: case GcnArCodeTypes::IF_NE_ALL_32H:
						case GcnArCodeTypes::IF_LT_ALL_8: case GcnArCodeTypes::IF_LT_ALL_8H:
						case GcnArCodeTypes::IF_LT_ALL_16: case GcnArCodeTypes::IF_LT_ALL_16H:
						case GcnArCodeTypes::IF_LT_ALL_32: case GcnArCodeTypes::IF_LT_ALL_32H:
						case GcnArCodeTypes::IF_GT_ALL_8: case GcnArCodeTypes::IF_GT_ALL_8H:
						case GcnArCodeTypes::IF_GT_ALL_16: case GcnArCodeTypes::IF_GT_ALL_16H:
						case GcnArCodeTypes::IF_GT_ALL_32: case GcnArCodeTypes::IF_GT_ALL_32H:
						case GcnArCodeTypes::IF_LT_ALL_U8: case GcnArCodeTypes::IF_LT_ALL_U8H:
						case GcnArCodeTypes::IF_LT_ALL_U16: case GcnArCodeTypes::IF_LT_ALL_U16H:
						case GcnArCodeTypes::IF_LT_ALL_U32: case GcnArCodeTypes::IF_LT_ALL_U32H:
						case GcnArCodeTypes::IF_GT_ALL_U8: case GcnArCodeTypes::IF_GT_ALL_U8H:
						case GcnArCodeTypes::IF_GT_ALL_U16: case GcnArCodeTypes::IF_GT_ALL_U16H:
						case GcnArCodeTypes::IF_GT_ALL_U32: case GcnArCodeTypes::IF_GT_ALL_U32H:
						case GcnArCodeTypes::IF_AND_ALL_8: case GcnArCodeTypes::IF_AND_ALL_8H:
						case GcnArCodeTypes::IF_AND_ALL_16: case GcnArCodeTypes::IF_AND_ALL_16H:
						case GcnArCodeTypes::IF_AND_ALL_32: case GcnArCodeTypes::IF_AND_ALL_32H:
							_layerCount.push_back(_dissectedCheat.size() - 1);
							break;
						default: //next line only
							_layerCount.push_back(1);
					}
				} break;
			}
		}

		if (_layerCount.empty())
			continue;

		if (_layerCount.back() == 0)
		{
			_layerCount.pop_back();
			_lua.append("end\n");
		}
	}



	return true;
}

std::string MungPlex::CheatConvert::generateIndent()
{
	std::string indent;

	for (int i = 0; i < _layerCount.size() * 2; ++i)
		indent.append(" ");

	return indent;
}

void MungPlex::CheatConvert::logConversionError(const int lineCount, const std::string& line)
{
	Log::LogInformation("Error Converting Code: improperly formatted line found at " + std::to_string(lineCount) + ": " + line);
}

bool MungPlex::CheatConvert::DissectCheat()
{
	_dissectedCheat.clear();
	std::string line, partStr;
	std::stringstream streamCheat, streamLine;
	streamCheat << GetInstance()._oldSchoolCheat.GetStdStringNoZeros();
	int lineCount = 0;
	bool left = true;
	uint32_t partInt = 0;

	while (std::getline(streamCheat, line, '\n'))
	{
		++lineCount;

		if (line.size() != 17)
		{
			GetInstance().logConversionError(lineCount, line);
			return false;
		}

		streamLine << line;
		_dissectedCheat.emplace_back();

		while (std::getline(streamLine, partStr, ' '))
		{
			if (partStr.size() != 8)
			{
				GetInstance().logConversionError(lineCount, line);
				return false;
			}

			for (char ch : partStr)
				if ((ch < '0' || ch > '9') && (ch < 'A' || ch > 'F') && (ch < 'a' || ch > 'f'))
				{
					GetInstance().logConversionError(lineCount, line);
					return false;
				}

			partInt = std::stoll(partStr, nullptr, 16);

			if (left)
			{
				_dissectedCheat.back().first = partInt;
				left = false;
			}
			else
			{
				_dissectedCheat.back().second = partInt;
				left = true;
			}
		}

		streamLine.str(std::string());
		streamLine.clear();
	}

	return true;
}

MungPlex::InputTextMultiline& MungPlex::CheatConvert::GetOldSchoolCheat()
{
	return GetInstance()._oldSchoolCheat;
}

std::string MungPlex::CheatConvert::GetLuaCheat()
{
	return _lua;
}