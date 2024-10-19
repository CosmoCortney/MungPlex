#pragma once
#include "IEmulator.hpp"

namespace MungPlex
{
	class Mesen : public IEmulator
	{
	public:
		bool Init(const Xertz::ProcessInfo& process, std::vector<GameEntity>& gameEntities, std::vector<SystemRegion>& systemRegions);

	private:

	};
}