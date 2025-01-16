#pragma once
#include <boost/asio.hpp>
#include <string>

namespace MungPlex
{
	static std::string GetRegionFromBigSRegionCode(const char code)
	{
		switch (code)
		{
			case 'U':
				return "NTSC-U";
			case 'E':
				return "PAL";
			case 'P': case 'A':	case 'K':
				return "NTSC-J";
			default:
				return "Any/UNK";
		}
	}
}