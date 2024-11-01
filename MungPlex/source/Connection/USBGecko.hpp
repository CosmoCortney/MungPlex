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
#include <string>
#include "ftd2xx.h"
#include <windows.h>

namespace MungPlex
{
	class USBGecko
	{
	public:
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
		FT_STATUS Init();
		FT_STATUS Connect();
		FT_STATUS Reset();


	private:
		std::string _serialNumber = "GECKUSB0";
		FT_HANDLE _ftdiHandle = nullptr;
		int _ftdiDeviceNumber = -1;
		const uint32_t _packetSize = 0xF800;
		const uint32_t _uplPacketSize = 0xF80;
		bool _rvlMode = false;

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
		bool memoryArgsInRange(const uint32_t memoryStart, const uint32_t memoryEnd);
		FT_STATUS sendDumpInformation(const uint32_t memoryStart = 0x80000000, const uint32_t memoryEnd = 0x817FFFFF);
		FT_STATUS geckoRead(char* buf, const uint64_t readSize, LPDWORD bytesReceived);
		FT_STATUS geckoWrite(char* buf, const uint64_t writeSize, LPDWORD bytesWritten);
		FT_STATUS dump(const uint32_t memoryStart = 0x80000000, const uint32_t memoryEnd = 0x817FFFFF);
		
	};
}