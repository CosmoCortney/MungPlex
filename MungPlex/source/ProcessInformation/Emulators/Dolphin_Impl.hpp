#pragma once
#include <boost/asio.hpp>
#include "IEmulator.hpp"

namespace MungPlex
{
	class Dolphin : public IEmulator
	{
	public:
		bool Init(const Xertz::ProcessInfo& process, std::vector<GameEntity>& entities, std::vector<SystemRegion>& regions);

	private:

	};
}