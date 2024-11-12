#pragma once
#include "IConsoleConnectionWrapper.hpp"

namespace MungPlex
{
	class USBGeckoConnectionWrapper : public IConsoleConnectionWrapper
	{
	public:
		bool Init(USBGecko* usbGecko, std::vector<GameEntity>& gameEntities, std::vector<SystemRegion>& systemRegions);

	private:
	};
}