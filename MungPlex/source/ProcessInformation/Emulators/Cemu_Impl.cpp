#include "Cemu_Impl.hpp"

bool MungPlex::Cemu::Init(const Xertz::ProcessInfo& process, std::vector<GameEntity>& gameEntities, std::vector<SystemRegion>& systemRegions)
{
	ProcessInformation::SetMiscProcessInfo("Cemu", true, false, 4, 4);
	bool titleIDFound = false;

	for (const auto& region : ProcessInformation::GetRegionList())
	{
		if (region.GetRegionSize() != 0x4E000000)
			continue;

		loadSystemInformationJSON("WiiU", systemRegions);
		char* mainBase = region.GetBaseAddress<char*>();
		systemRegions[0].BaseLocationProcess = reinterpret_cast<void*>(mainBase);
		systemRegions[1].BaseLocationProcess = reinterpret_cast<void*>(mainBase + 0x0E000000);


		//old title id fetch code, just keep it in case it might be needed again
		/*if (!(region.GetRegionSize() == 0x1E000 && !titleIDFound))
			continue;

		const int bufSize = 0x6000;
		std::vector<char> buf(bufSize);
		_process.ReadMemoryFast(buf.data(), region.GetBaseAddress<void*>(), bufSize);

		for (int offset = 0; offset < bufSize; offset += 4)
		{
			if (*reinterpret_cast<int*>(buf.data() + offset) != 0x746F6F52)
				continue;

			for (int i = 0x9C; i <= 0xA4; i += 4)
			{
				uint32_t tempID = *reinterpret_cast<uint32_t*>(buf.data() + offset + i);
				tempID = Xertz::SwapBytes<uint32_t>(tempID);

				if (i == 0x9C && tempID != 0x00050000)
					break;

				_gameID.append(ToHexString(tempID, 8, false));

				if(i <= 0xA0)
					_gameID.append("-");

				titleIDFound = true;
			}
		}*/
	}

	//get title type id, title id and version
	std::wstring wTitleBuf(512, L'\0');

	for (HWND wHandle : Xertz::SystemInfo::GetWindowHandleList())
	{
		GetWindowTextW(wHandle, wTitleBuf.data(), 512);

		if (wTitleBuf.find(L"Cemu") == std::wstring::npos)
			continue;

		int pos = wTitleBuf.find(L"TitleId: ");

		if (pos == std::wstring::npos)
			continue;

		std::wstring wTitle = wTitleBuf.substr(pos + 9);
		_gameID = MT::Convert<std::wstring, std::string>(wTitle.substr(0, 17), MT::UTF16LE, MT::UTF8);
		wTitle = wTitle.substr(19);
		pos = wTitle.find(L"[");
		_gameRegion = MT::Convert<std::wstring, std::string>(wTitle.substr(pos + 1, 2), MT::UTF16LE, MT::UTF8);
		_gameName = MT::Convert<std::wstring, std::string>(wTitle.substr(0, pos - 1), MT::UTF16LE, MT::UTF8);
		wTitle = wTitle.substr(wTitle.find(L"v"));
		_rpcGameID = _gameID.append("-" + MT::Convert<std::wstring, std::string>(wTitle.substr(0, wTitle.find(L"]"))), MT::UTF16LE, MT::UTF8);
		//std::cout << _gameID << std::endl;
		//std::cout << _gameName << std::endl;
		_platformID = ProcessInformation::WIIU;
		return true && obtainGameEntities("WiiU", gameEntities);
	}

	return false;
}
