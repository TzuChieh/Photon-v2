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
	inline CookingContext& addBackend(std::unique_ptr<Intersectable> intersectable)
	{
		m_backendIntersectables.push_back(std::move(intersectable));

		return *this;
	}

	inline CookingContext& addCookedUnit(CookedUnit&& cookedUnit)
	{
		m_cookedUnits.push_back(std::move(cookedUnit));

		return *this;
	}

	inline CookedUnit retrieveCookedBackends()
	{
		CookedUnit cookedUnit;
		for(auto& isable : m_backendIntersectables)
		{
			cookedUnit.intersectables.push_back(std::move(isable));
		}
		m_backendIntersectables.clear();

		return cookedUnit;
	}

	inline CookedUnit retrieveCookedUnits()
	{
		CookedUnit results;
		for(auto& unit : m_cookedUnits)
		{
			results.add(std::move(unit));
		}
		m_cookedUnits.clear();

		return results;
	}

private:
	std::vector<std::unique_ptr<Intersectable>> m_backendIntersectables;
	std::vector<CookedUnit>                     m_cookedUnits; 
};

}// end namespace ph