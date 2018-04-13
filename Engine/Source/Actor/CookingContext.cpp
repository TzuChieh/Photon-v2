#include "Actor/CookingContext.h"
#include "Actor/CookedDataStorage.h"

namespace ph
{

void CookingContext::claimBackendData(CookedDataStorage& storage)
{
	storage.add(std::move(m_backendIntersectables));

	m_backendIntersectables.clear();
}

}// end namespace ph