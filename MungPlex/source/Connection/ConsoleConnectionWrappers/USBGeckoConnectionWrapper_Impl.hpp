#pragma once
#include <boost/asio.hpp>
#include "IConsoleConnectionWrapper.hpp"
#include "RegionCombo.hpp"

namespace MungPlex
{
	class USBGeckoConnectionWrapper : public IConsoleConnectionWrapper
	{
	public:
		bool Init(USBGecko* usbGecko, std::vector<GameEntity>& gameEntities, RegionCombo::Type& systemRegions);

	private:
	};
}