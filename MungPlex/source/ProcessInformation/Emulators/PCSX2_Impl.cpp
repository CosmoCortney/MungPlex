#include "PCSX2_Impl.hpp"

bool MungPlex::PCSx2::Init(const Xertz::ProcessInfo& process, std::vector<GameEntity>& gameEntities, std::vector<SystemRegion>& systemRegions)
{
	ProcessInformation::SetMiscProcessInfo("Pcsx2", false, false, 4, 4);
	REGION_LIST& regions = ProcessInformation::GetRegionList();
	bool idFound = false;
	static const uint64_t ramFlag = 0x6461655256413F2E;

	for (int i = 0; i < regions.size(); ++i)
	{
	jmp:
		if (regions[i].GetRegionSize() != 0x80000)
			continue;

		++i;
		for (int j = 0; j < 20; ++j)
			if (regions[i + j].GetRegionSize() != 0x1000)
			{
				++i;
				goto jmp;
			}

		for (int k = i; k < 0x1F80; ++k)
			if (regions[k].GetAllocationProtect() != PAGE_READWRITE)
				regions[k].SetProtect(ProcessInformation::GetHandle(), PAGE_READWRITE);

		loadSystemInformationJSON("PS2", systemRegions);
		systemRegions[0].BaseLocationProcess = regions[i].GetBaseAddress<char*>();

		uint32_t bufSize = 0x1000000;
		uint64_t* exeAddr = reinterpret_cast<uint64_t*>(process.GetModuleAddress(L"pcsx2-qt.exe"));
		std::vector<uint64_t> buf(bufSize / sizeof(uint64_t));
		process.ReadMemoryFast(buf.data(), exeAddr, bufSize);

		for (int offset = 0; offset < bufSize / sizeof(uint64_t); ++offset)
		{
			if (buf[offset] != ramFlag)
				continue;

			for (uint64_t idOff = 1; idOff < 0x20; ++idOff)
			{
				_rpcGameID = _gameID = reinterpret_cast<char*>(&buf[offset + idOff]);

				if (_gameID.front() != 'S')
					continue;

				idFound = true;
				_connectionCheckValue = *reinterpret_cast<int*>(&buf[offset + idOff]);
				_connectionCheckPtr = exeAddr + offset + idOff;
				
				switch (_gameID[2])
				{
				case 'U':
					_gameRegion = "NTSC-U";
					break;
				case 'E':
					_gameRegion = "PAL";
					break;
				case 'P': case 'A':	case 'K':
					_gameRegion = "NTSC-J";
					break;
				default:
					_gameRegion = "Any/UNK";
				}

				break;
			}

			break;
		}

		break;
	}

	if (!idFound)
		return false;

	//get title
	DWORD pid;
	std::wstring wTitleBuf(512, L'\0');

	for (HWND wHandle : Xertz::SystemInfo::GetWindowHandleList())
	{
		GetWindowThreadProcessId(wHandle, &pid);

		if (pid != process.GetPID())
			continue;

		_gameName.clear();
		GetWindowTextW(wHandle, wTitleBuf.data(), 512);
		_gameName = MT::Convert<std::wstring, std::string>(wTitleBuf.c_str(), MT::UTF16LE, MT::UTF8);
		
		if (_gameName.empty())
			continue;

		if (_gameName.find("pcsx2") != std::string::npos)
			continue;

		_platformID = ProcessInformation::PS2;
		return true;
	}

	return false;
}
