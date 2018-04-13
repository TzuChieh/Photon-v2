#pragma once

#include "Core/Intersectable/Intersectable.h"
#include "Actor/CookedUnit.h"

#include <vector>
#include <memory>
#include <type_traits>

namespace ph
{

// TODO: backends should be with CookedUnit

class CookedDataStorage;

class CookingContext final
{
public:
	inline CookingContext& addBackend(std::unique_ptr<Intersectable> intersectable)
	{
		m_backendIntersectables.push_back(std::move(intersectable));

		return *this;
	}

	void claimBackendData(CookedDataStorage& storage);

private:
	std::vector<std::unique_ptr<Intersectable>> m_backendIntersectables;
};

}// end namespace ph