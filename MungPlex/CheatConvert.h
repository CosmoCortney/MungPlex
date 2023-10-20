#pragma once
#include <iostream>
#include <stdio.h>
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "examples/libs/emscripten/emscripten_mainloop_stub.h"
#include <string>
#include<vector>

namespace MungPlex
{
	class CheatConvert
	{
	private:
		CheatConvert() {}
		~CheatConvert() {}
		CheatConvert(const CheatConvert&) = delete;
		CheatConvert(CheatConvert&&) = delete;
		void operator=(const CheatConvert&) = delete;
		void operator=(CheatConvert&&) = delete;
		static CheatConvert& GetInstance()
		{
			static CheatConvert Instance;
			return Instance;
		}

		enum class GcnArCodeTypes
		{
			WRITE_8 = 0x00, WRITE_16 = 0x02, WRITE_32 = 0x04,
			STRUCT_WRITE_8 = 0x40, STRUCT_WRITE_16 = 0x42, STRUCT_WRITE_32 = 0x44,
			FILL_SLIDE_8 = 0x80, FILL_SLIDE_16 = 0x82, FILL_SLIDE_32 = 0x84,
			INCREMENT_8 = 0x80, INCREMENT_16 = 0x82, INCREMENT_32 = 0x84, INCREMENT_FLOAT = 0x86,
			IF_EQ_NL_8 = 0x08, IF_EQ_NL_16 = 0x0A, IF_EQ_NL_32 = 0x0C,
			IF_EQ_N2L_8 = 0x48, IF_EQ_N2L_16 = 0x4A, IF_EQ_N2L_32 = 0x4C,
			IF_EQ_AU_8 = 0x88, IF_EQ_AU_16 = 0x8A, IF_EQ_AU_32 = 0x8C,
			IF_EQ_ALL_8 = 0xC8, IF_EQ_ALL_16 = 0xCA, IF_EQ_ALL_32 = 0xCC,
			IF_NE_NL_8 = 0x10, IF_NE_NL_16 = 0x12, IF_NE_NL_32 = 0x14,
			IF_NE_N2L_8 = 0x50, IF_NE_N2L_16 = 0x52, IF_NE_N2L_32 = 0x54,
			IF_NE_AU_8 = 0x90, IF_NE_AU_16 = 0x92, IF_NE_AU_32 = 0x94,
			IF_NE_ALL_8 = 0xD0, IF_NE_ALL_16 = 0xD2, IF_NE_ALL_32 = 0xD4,
			IF_LT_NL_8 = 0x18, IF_LT_NL_16 = 0x1A, IF_LT_NL_32 = 0x1C,
			IF_LT_N2L_8 = 0x58, IF_LT_N2L_16 = 0x5A, IF_LT_N2L_32 = 0x5C,
			IF_LT_AU_8 = 0x98, IF_LT_AU_16 = 0x9A, IF_LT_AU_32 = 0x9C,
			IF_LT_ALL_8 = 0xD8, IF_LT_ALL_16 = 0xDA, IF_LT_ALL_32 = 0xDC,
			IF_GT_NL_8 = 0x20, IF_GT_NL_16 = 0x22, IF_GT_NL_32 = 0x24,
			IF_GT_N2L_8 = 0x60, IF_GT_N2L_16 = 0x62, IF_GT_N2L_32 = 0x64,
			IF_GT_AU_8 = 0xA0, IF_GT_AU_16 = 0xA2, IF_GT_AU_32 = 0xA4,
			IF_GT_ALL_8 = 0xE0, IF_GT_ALL_16 = 0xE2, IF_GT_ALL_32 = 0xE4,
			IF_LT_NL_U8 = 0x28, IF_LT_NL_U16 = 0x2A, IF_LT_NL_U32 = 0x2C,
			IF_LT_N2L_U8 = 0x68, IF_LT_N2L_U16 = 0x6A, IF_LT_N2L_U32 = 0x6C,
			IF_LT_AU_U8 = 0xA8, IF_LT_AU_U16 = 0xAA, IF_LT_AU_U32 = 0xAC,
			IF_LT_ALL_U8 = 0xE8, IF_LT_ALL_U16 = 0xEA, IF_LT_ALL_U32 = 0xEC,
			IF_GT_NL_U8 = 0x30, IF_GT_NL_U16 = 0x32, IF_GT_NL_U32 = 0x34,
			IF_GT_N2L_U8 = 0x70, IF_GT_N2L_U16 = 0x72, IF_GT_N2L_U32 = 0x74,
			IF_GT_AU_U8 = 0xB0, IF_GT_AU_U16 = 0xB2, IF_GT_AU_U32 = 0xB4,
			IF_GT_ALL_U8 = 0xF0, IF_GT_ALL_U16 = 0xF2, IF_GT_ALL_U32 = 0xF4,
			IF_AND_NL_8 = 0x38, IF_AND_NL_16 = 0x3A, IF_AND_NL_32 = 0x3C,
			IF_AND_N2L_8 = 0x78, IF_AND_N2L_16 = 0x7A, IF_AND_N2L_32 = 0x7C,
			IF_AND_AU_8 = 0xB8, IF_AND_AU_16 = 0xBA, IF_AND_AU_32 = 0xBC,
			IF_AND_ALL_8 = 0xF8, IF_AND_ALL_16 = 0xFA, IF_AND_ALL_32 = 0xFC,
			WRITE_8H = 0x01, WRITE_16H = 0x03, WRITE_32H = 0x05,
			STRUCT_WRITE_8H = 0x41, STRUCT_WRITE_16H = 0x43, STRUCT_WRITE_32H = 0x45,//
			FILL_SLIDE_8H = 0x81, FILL_SLIDE_16H = 0x83, FILL_SLIDE_32H = 0x85,
			INCREMENT_8H = 0x81, INCREMENT_16H = 0x83, INCREMENT_32H = 0x85, INCREMENT_FLOATH = 0x87,//
			IF_EQ_NL_8H = 0x09, IF_EQ_NL_16H = 0x0B, IF_EQ_NL_32H = 0x0D,
			IF_EQ_N2L_8H = 0x49, IF_EQ_N2L_16H = 0x4B, IF_EQ_N2L_32H = 0x4FFD,
			IF_EQ_AU_8H = 0x89, IF_EQ_AU_16H = 0x8B, IF_EQ_AU_32H = 0x8D,
			IF_EQ_ALL_8H = 0xC9, IF_EQ_ALL_16H = 0xCB, IF_EQ_ALL_32H = 0xCD,
			IF_NE_NL_8H = 0x11, IF_NE_NL_16H = 0x13, IF_NE_NL_32H = 0x15,
			IF_NE_N2L_8H = 0x51, IF_NE_N2L_16H = 0x53, IF_NE_N2L_32H = 0x55,
			IF_NE_AU_8H = 0x91, IF_NE_AU_16H = 0x93, IF_NE_AU_32H = 0x95,
			IF_NE_ALL_8H = 0xD1, IF_NE_ALL_16H = 0xD3, IF_NE_ALL_32H = 0xD5,
			IF_LT_NL_8H = 0x19, IF_LT_NL_16H = 0x1B, IF_LT_NL_32H = 0x1D,
			IF_LT_N2L_8H = 0x59, IF_LT_N2L_16H = 0x5B, IF_LT_N2L_32H = 0x5D,
			IF_LT_AU_8H = 0x99, IF_LT_AU_16H = 0x9B, IF_LT_AU_32H = 0x9D,
			IF_LT_ALL_8H = 0xD9, IF_LT_ALL_16H = 0xDB, IF_LT_ALL_32H = 0xDD,
			IF_GT_NL_8H = 0x21, IF_GT_NL_16H = 0x23, IF_GT_NL_32H = 0x25,
			IF_GT_N2L_8H = 0x61, IF_GT_N2L_16H = 0x63, IF_GT_N2L_32H = 0x65,
			IF_GT_AU_8H = 0xA1, IF_GT_AU_16H = 0xA3, IF_GT_AU_32H = 0xA5,
			IF_GT_ALL_8H = 0xE1, IF_GT_ALL_16H = 0xE3, IF_GT_ALL_32H = 0xE5,
			IF_LT_NL_U8H = 0x29, IF_LT_NL_U16H = 0x2B, IF_LT_NL_U32H = 0x2D,
			IF_LT_N2L_U8H = 0x69, IF_LT_N2L_U16H = 0x6B, IF_LT_N2L_U32H = 0x6D,
			IF_LT_AU_U8H = 0xA9, IF_LT_AU_U16H = 0xAB, IF_LT_AU_U32H = 0xAD,
			IF_LT_ALL_U8H = 0xE9, IF_LT_ALL_U16H = 0xEB, IF_LT_ALL_U32H = 0xED,
			IF_GT_NL_U8H = 0x31, IF_GT_NL_U16H = 0x33, IF_GT_NL_U32H = 0x35,
			IF_GT_N2L_U8H = 0x71, IF_GT_N2L_U16H = 0x73, IF_GT_N2L_U32H = 0x75,
			IF_GT_AU_U8H = 0xB1, IF_GT_AU_U16H = 0xB3, IF_GT_AU_U32H = 0xB5,
			IF_GT_ALL_U8H = 0xF1, IF_GT_ALL_U16H = 0xF3, IF_GT_ALL_U32H = 0xF5,
			IF_AND_NL_8H = 0x39, IF_AND_NL_16H = 0x3B, IF_AND_NL_32H = 0x3D,
			IF_AND_N2L_8H = 0x79, IF_AND_N2L_16H = 0x7B, IF_AND_N2L_32H = 0x7D,
			IF_AND_AU_8H = 0xB9, IF_AND_AU_16H = 0xBB, IF_AND_AU_32H = 0xBD,
			IF_AND_ALL_8H = 0xF9, IF_AND_ALL_16H = 0xFB, IF_AND_ALL_32H = 0xFD,
			TERMINATOR = 0x40,
		};

		static std::vector<std::pair<uint32_t, uint32_t>> _dissectedCheat;
		std::string _oldSchoolCheat = std::string(0x1800, 0);
		static std::string _lua;
		static std::vector<int> _layerCount;

		void logConversionError(const int lineCount, const std::string& line);
		static std::string generateIndent();

	public:
		static bool GcnArToLua();
		static bool DissectCheat();
		static std::string& GetOldSchoolCheat();
		static std::string GetLuaCheat();

		enum CheatTypes
		{
			GCN_AR, WII_GECKO, WIIU_CAFE
		};
	};
}