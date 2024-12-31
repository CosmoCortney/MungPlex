#pragma once
#include "IEmulator.hpp"

namespace MungPlex
{
	class Project64 : public IEmulator
	{
	public:
		bool Init(const Xertz::ProcessInfo& process, std::vector<GameEntity>& entities, RegionCombo::Type& regions);

	private:

	};
}