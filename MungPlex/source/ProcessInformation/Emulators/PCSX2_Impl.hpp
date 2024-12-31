#pragma once
#include "IEmulator.hpp"

namespace MungPlex
{
	class PCSx2 : public IEmulator
	{
	public:
		bool Init(const Xertz::ProcessInfo& process, std::vector<GameEntity>& gameEntities, RegionCombo::Type& systemRegions);

	private:

	};
}