#pragma once
#include <string>

namespace MungPlex
{
	static std::string GetRegionFromBigNRegionCode(const char code)
	{
		switch (code)
		{
		case 'A':
			return "Any";
		case 'C':
			return "China";
		case 'D':
			return "Germany";
		case 'E':
			return "USA";
		case 'F':
			return "France";
		case 'H':
			return "Netherlands";
		case 'I':
			return "Italy";
		case 'J':
			return "Japan";
		case 'K':
			return "Korea";
		case 'L':
			return "JPN-PAL";
		case 'M':
			return "USA-PAL";
		case 'N':
			return "JPN-USA";
		case 'P': case 'X': case 'Y': case 'Z':
			return "Europe";
		case 'Q':
			return "JPN-KOR";
		case 'R':
			return "Russia";
		case 'S':
			return "Spain";
		case 'T':
			return "USA-KOR";
		case 'U':
			return "Australia";
		case 'V':
			return "Scandinavia";
		case 'W':
			return "Taiwan/Hong Kong/Macau";
		default://
			return "Unknown";
		}
	}
}