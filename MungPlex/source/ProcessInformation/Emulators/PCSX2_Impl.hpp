#pragma once
#include <boost/asio.hpp>
#include "IEmulator.hpp"

namespace MungPlex
{
	class PCSx2 : public IEmulator
	{
	public:
		bool Init(const Xertz::ProcessInfo& process, std::vector<GameEntity>& gameEntities, std::vector<SystemRegion>& systemRegions);

	private:

	};
}