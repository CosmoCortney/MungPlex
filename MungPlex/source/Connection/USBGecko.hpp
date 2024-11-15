/*
This implementation of USB Gecko support is heavily based on the works of the Gecko dNET project. 
Special thanks to...
	dcx2, hawkeye2777, MasterofGalaxies, Link: Gecko dNET application
	Nuke: Original USB Gecko
	webhdx: USB Gecko (Type-C) reblica
	Y.S.: Original codehandler
	brkirch: Continuation on Gecko OS
The Project can be found here: https://github.com/MasterofGalaxies/geckowii
*/

#pragma once
#include <iostream>
#include <string>
#include "ftd2xx.h"
#include <windows.h>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>

namespace MungPlex
{
	class USBGecko
	{
	public:
		static boost::mutex s_AccessMutex;

		enum GeckoCommands : uint8_t
		{
			cmd_poke08 = 0x01,
			cmd_poke16 = 0x02,
			cmd_pokemem = 0x03,
			cmd_readmem = 0x04,
			cmd_pause = 0x06,
			cmd_unfreeze = 0x07,
			cmd_breakpoint = 0x09,
			cmd_breakpointx = 0x10,
			cmd_sendregs = 0x2F,
			cmd_getregs = 0x30,
			cmd_cancelbp = 0x38,
			cmd_sendcheats = 0x40,
			cmd_upload = 0x41,
			cmd_hook = 0x42,
			cmd_hookpause = 0x43,
			cmd_step = 0x44,
			cmd_status = 0x50,
			cmd_cheatexec = 0x60,
			cmd_nbreakpoint = 0x89,
			cmd_version = 0x99
		};

		enum GCCommands : uint8_t
		{
			GCBPHit = 0x11,
			GCNewVer = 0x80,
			GCACK = 0xAA,
			GCRETRY = 0xBB,
			GCFAIL = 0xCC,
			GCDONE = 0xFF
		};

		enum RvlHookType : uint8_t
		{
			VI,
			RvlRemote,
			GCNPad
		};

		enum RvlStatus {
			Running,
			Paused,
			Breakpoint,
			Loader,
			Unknown
		};

		enum RvlLanguage : uint8_t
		{
			NoOverride,
			Japanese,
			English,
			German,
			French,
			Spanish,
			Italian,
			Dutch,
			ChineseSimplified,
			ChineseTraditional,
			Korean
		};

		enum RvlPatches : uint8_t
		{
			NoPatches,
			PAL60,
			VIDTV,
			PAL60VIDTV,
			NTSC,
			NTSCVIDTV,
			PAL50,
			PAL50VIDTV
		};

		USBGecko() = default;
		~USBGecko();
		FT_STATUS Init();
		FT_STATUS Connect();
		FT_STATUS Reset();
		FT_STATUS Disconnect();
		FT_STATUS Read(char* buf, const uint64_t rangeStart, const uint64_t readSize);
		FT_STATUS Write(char* buf, const uint64_t rangeStart, const uint64_t writeSize);
		FT_STATUS Unfreeze();
		RvlStatus GetCurrentStatus();
		bool IsConnectedAndReady();

		template <typename dataType> FT_STATUS Poke(const dataType pokeVal, const uint64_t address)
		{
			boost::lock_guard<boost::mutex> lock(s_AccessMutex);
			static FT_STATUS ftStatus = 0;
			static uint32_t pokeAddress = 0;
			pokeAddress = address;
			static uint64_t bytesWritten = 0;
			static std::vector<char> pokeAddrValPair(8);
			*reinterpret_cast<uint32_t*>(pokeAddrValPair.data()) = std::byteswap(pokeAddress);
			*reinterpret_cast<dataType*>(&pokeAddrValPair[8 - sizeof(dataType)]) = pokeVal;

			if constexpr (std::is_same_v<dataType, int8_t> || std::is_same_v<dataType, uint8_t>)
			{
				if ((ftStatus = sendGeckoCommand(cmd_poke08)) != FT_OK)
					return ftStatus;
			}
			else if constexpr (std::is_same_v<dataType, int16_t> || std::is_same_v<dataType, uint16_t>)
			{
				if ((ftStatus = sendGeckoCommand(cmd_poke16)) != FT_OK)
					return ftStatus;
			}
			else if constexpr (std::is_same_v<dataType, int32_t> || std::is_same_v<dataType, uint32_t> || std::is_same_v<dataType, float>)
			{
				if ((ftStatus = sendGeckoCommand(cmd_pokemem)) != FT_OK)
					return ftStatus;
			}
			else if constexpr (std::is_same_v<dataType, int64_t> || std::is_same_v<dataType, uint64_t> || std::is_same_v<dataType, double>)
			{
				static std::vector<char> longVal(8);
				*reinterpret_cast<dataType*>(longVal.data()) = pokeVal;

				ftStatus = Poke<uint32_t>(*reinterpret_cast<uint32_t*>(longVal.data()), address);

				if (ftStatus != FT_OK)
					return ftStatus;

				return Poke<uint32_t>(*reinterpret_cast<uint32_t*>(&longVal[4]), address + 4);
			}
			else
				return FT_OTHER_ERROR; //invalid datatype

			ftStatus = geckoWrite(pokeAddrValPair.data(), 8, reinterpret_cast<LPDWORD>(&bytesWritten));

			if (ftStatus != FT_OK)
			{
#ifndef NDEBUG
				std::cout << "Failed to poke value of size " << sizeof(dataType) << '\n';
#endif
			}
				
			return ftStatus;
		}

	private:
		std::string _serialNumber = "GECKUSB0";
		FT_HANDLE _ftdiHandle = nullptr;
		int _ftdiDeviceNumber = -1;
		const uint32_t _packetSize = 0xF800;
		const uint32_t _uplPacketSize = 0xF80;
		bool _rvlMode = false;
		bool _connectedAndReady = false;

		FT_STATUS findUsbGecko();
		FT_STATUS openUsbGecko();
		FT_STATUS closeUsbGecko();
		FT_STATUS setLatencyTimeouts(const uint64_t readTimeout, const uint64_t writeTimeout);
		FT_STATUS setLatencyTimer(const uint8_t ucLatency);
		FT_STATUS setUsbParameters(const uint64_t inTransferSize, const uint64_t outTransferSize);
		FT_STATUS resetDevice();
		FT_STATUS purge(const uint32_t mask);
		FT_STATUS sendGeckoCommand(uint8_t cmd);
		FT_STATUS getCommandResponce(char* out);
		FT_STATUS geckoSetMemoryRegion(const uint32_t memoryStart, const uint32_t memoryEnd);
		FT_STATUS geckoRead(char* buf, const uint64_t readSize, LPDWORD bytesReceived);
		FT_STATUS geckoWrite(char* buf, const uint64_t writeSize, LPDWORD bytesWritten);
		FT_STATUS dump(char* buf, const uint32_t memoryStart, const uint32_t memoryEnd);
		FT_STATUS upload(char* buf, const uint32_t memoryStart, const uint32_t memoryEnd);
		void wait(const int milliseconds) const;
	};
}