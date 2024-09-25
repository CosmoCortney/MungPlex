#pragma once
//#include <libftdi1/ftdi.h>
//#include <libusb-1.0/libusb.h>
#include <string>

namespace MungPlex
{
	class USBGecko
	{
	public:
		USBGecko() = default;
		bool Init();


	private:
	//	struct ftdi_context* _ftdi;
		//int _ret;
		//std::string _buf = "Hello FTDI";



	};
}