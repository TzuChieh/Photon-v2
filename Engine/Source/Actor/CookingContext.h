#pragma once

#include "Core/Intersectable/Intersectable.h"
#include "Actor/CookedUnit.h"

#include <vector>
#include <memory>
#include <type_traits>

namespace ph
{

class CookingContext final
{
public:
	inline void addBackend(std::unique_ptr<Intersectable> intersectable)
	{
		m_backendIntersectables.push_back(std::move(intersectable));
	}

	inline CookedUnit toCooked()
	{
		CookedUnit cookedUnit;
		for(auto& isable : m_backendIntersectables)
		{
			cookedUnit.intersectables.push_back(std::move(isable));
		}
		clear();

		return cookedUnit;
	}

	inline void clear()
	{
		m_backendIntersectables.clear();
	}

private:
	std::vector<std::unique_ptr<Intersectable>> m_backendIntersectables;
};

}// end namespace ph