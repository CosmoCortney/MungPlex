#include "Yuzu_Impl.hpp"

bool MungPlex::Yuzu::Init(const Xertz::ProcessInfo& process, std::vector<GameEntity>& gameEntities, std::vector<SystemRegion>& systemRegions)
{
	ProcessInformation::SetMiscProcessInfo("Yuzu", false, false, 8, 4);
	loadSystemInformationJSON("SWITCH", systemRegions);
	std::wstring wTitleBuf(512, L'\0');
	std::wstring gTitle;
	std::wstring version;

	//get title
	for (HWND wHandle : Xertz::SystemInfo::GetWindowHandleList())
	{
		GetWindowTextW(wHandle, wTitleBuf.data(), 512);

		if (wTitleBuf.find(L"yuzu") == std::wstring::npos)
			continue;

		if (std::ranges::count(wTitleBuf, '|') < 2)
			continue;

		int posStart = wTitleBuf.find(L"|");
		wTitleBuf = wTitleBuf.substr(posStart + 1);
		int posEnd = wTitleBuf.find(L"(");
		gTitle = wTitleBuf.substr(0, posEnd);

		while (gTitle.front() == 0x0020)
			gTitle = gTitle.substr(1, gTitle.size());

		while (gTitle.back() == 0x0020)
			gTitle = gTitle.substr(0, gTitle.size() - 1);

		_gameName = MT::Convert<std::wstring, std::string>(gTitle, MT::UTF16LE, MT::UTF8);
		//std::cout << _gameName << std::endl;

		posStart = wTitleBuf.find(L"|");
		version = wTitleBuf.substr(posStart + 1);
		posEnd = version.find(L"|");
		version = version.substr(0, posEnd);

		while (version.front() == 0x0020)
			version = version.substr(1, version.size());

		while (version.back() == 0x0020)
			version = version.substr(0, version.size() - 1);

		break;
	}

	//get title id
	bool idFound = false;
	for (const auto& region : ProcessInformation::GetRegionList())
	{
		uint64_t regionSize = region.GetRegionSize();
		if (regionSize < 0x20000 || regionSize >= 0x200000)
			continue;

		std::vector<uint8_t> buf(regionSize);
		process.ReadMemoryFast(buf.data(), region.GetBaseAddress<void*>(), regionSize);

		for (int i = 0; i < regionSize; i += 4)
		{
			if (*reinterpret_cast<uint64_t*>(&buf[i]) != *reinterpret_cast<uint64_t*>(gTitle.data()))
				continue;

			for (int prefixOffset = 0; prefixOffset < 0x100; ++prefixOffset)
			{
				if (buf.size() <= i + prefixOffset + 4)
					break;

				static int prependSize = 0;

				if (*reinterpret_cast<uint32_t*>(&buf[i + prefixOffset]) == 0x00780030)
					prependSize = 4;
				else if (*reinterpret_cast<uint16_t*>(&buf[i + prefixOffset]) == 0x0028)
					prependSize = 2;
				else
					continue;

				std::wstring tempID = reinterpret_cast<wchar_t*>(&buf[i + prefixOffset + prependSize]);
				_connectionCheckValue = *reinterpret_cast<int*>(tempID.data());
				_connectionCheckPtr = region.GetBaseAddress<char*>() + i + prefixOffset + prependSize;

				if (prependSize == 2)
					tempID = tempID.substr(0, tempID.find(L')'));

				if (tempID.size() != 16)
					continue;

				idFound = true;
				_rpcGameID = _gameID = MT::Convert<std::wstring, std::string>(tempID + L"-" + version, MT::UTF16LE, MT::UTF8);
				break;
			}

			if (!idFound)
				continue;

			break;
		}

		if (idFound)
			break;
	}

	//get RAM
	for (const auto& region : ProcessInformation::GetRegionList())
	{
		const uint64_t rSize = region.GetRegionSize();

		if (rSize != 0x100000000)
			continue;

		systemRegions[0].BaseLocationProcess = region.GetBaseAddress<void*>();
		_platformID = ProcessInformation::SWITCH;
		return true;
	}

	return false;
}
