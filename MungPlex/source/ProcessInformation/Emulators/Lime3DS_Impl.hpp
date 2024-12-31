#pragma once
#include "IEmulator.hpp"

namespace MungPlex
{
	class Lime3DS : public IEmulator
	{
	public:
		bool Init(const Xertz::ProcessInfo& process, std::vector<GameEntity>& entities, RegionCombo::Type& regions);

	private:

	};
}