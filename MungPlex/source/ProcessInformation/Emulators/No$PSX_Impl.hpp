#pragma once
#include "IEmulator.hpp"

namespace MungPlex
{
	class No$PSX : public IEmulator
	{
	public:
		bool Init(const Xertz::ProcessInfo& process, std::vector<GameEntity>& gameEntities, RegionCombo::Type& systemRegions);

	private:

	};
}