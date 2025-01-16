#include "RPCS3_Impl.hpp"

bool MungPlex::RPcS3::Init(const Xertz::ProcessInfo& process, std::vector<GameEntity>& gameEntities, std::vector<SystemRegion>& systemRegions)
{
	ProcessInformation::SetMiscProcessInfo("Rpcs3", true, false, 4, 4);
	loadSystemInformationJSON("PS3", systemRegions);
	systemRegions[0].BaseLocationProcess = reinterpret_cast<void*>(0x300010000);
	systemRegions[1].BaseLocationProcess = reinterpret_cast<void*>(0x330000000);
	systemRegions[2].BaseLocationProcess = reinterpret_cast<void*>(0x340000000);
	systemRegions[3].BaseLocationProcess = reinterpret_cast<void*>(0x350000000);

	uint32_t bufSize = 0x10000000;
	char* exeAddr = reinterpret_cast<char*>(process.GetModuleAddress(L"rpcs3.exe") + 0x5000000);
	std::vector<char> buf(bufSize);
	process.ReadMemoryFast(buf.data(), exeAddr, bufSize);
	bool gameIdFound = false;
	std::wstring gIdWindow;

	for (int offset = 0; offset < bufSize; offset += 4)
	{
		if (*reinterpret_cast<uint64_t*>(&buf[offset]) != 0x454D41475F335350)
			continue;

		_rpcGameID = _gameID = reinterpret_cast<char*>(&buf[offset - 0xE0]);
		_gameID.resize(9);
		gIdWindow = MT::Convert<std::string, std::wstring>(_gameID, MT::UTF8, MT::UTF16LE);
		_rpcGameID = _gameID.append("-[" + std::string(reinterpret_cast<char*>(&buf[offset - 0xA0])) + "]");
		gameIdFound = true;
		_connectionCheckValue = *reinterpret_cast<int*>(&buf[offset - 0xDC]);
		_connectionCheckPtr = exeAddr + offset - 0xDC;

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
	}

	if (!gameIdFound)
		return false;

	std::wstring gTitle;
	std::wstring wTitleBuf(512, L'\0');
	//get title
	for (HWND wHandle : Xertz::SystemInfo::GetWindowHandleList())
	{
		GetWindowTextW(wHandle, wTitleBuf.data(), 512);

		if (wTitleBuf.find(gIdWindow) == std::wstring::npos)
			continue;

		int posStart = wTitleBuf.find_last_of(L"|");
		wTitleBuf = wTitleBuf.substr(posStart + 1);
		int posEnd = wTitleBuf.find(L"[");
		gTitle = wTitleBuf.substr(0, posEnd);

		while (gTitle.front() == 0x0020)
			gTitle = gTitle.substr(1, gTitle.size());

		while (gTitle.back() == 0x0020)
			gTitle = gTitle.substr(0, gTitle.size() - 1);

		_gameName = MT::Convert<std::wstring, std::string>(gTitle, MT::UTF16LE, MT::UTF8);
		_platformID = ProcessInformation::PS3;
		return true;
	}

	return false;
}
