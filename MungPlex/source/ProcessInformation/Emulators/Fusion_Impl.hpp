#pragma once
#include "IEmulator.hpp"

namespace MungPlex
{
	class Fusion : public IEmulator
	{
	public:
		bool Init(const Xertz::ProcessInfo& process, std::vector<GameEntity>& entities, RegionCombo::Type& regions);

	private:

	};
}