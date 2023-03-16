#pragma once

#include "World/Foundation/CookedResourceCollectionBase.h"
#include "Utility/TUniquePtrVector.h"
#include "Utility/Concurrent/TSynchronized.h"
#include "Utility/traits.h"
#include "DataIO/SDL/SdlResourceId.h"
#include "Common/logging.h"
#include "World/Foundation/TransientVisualElement.h"

#include <utility>

namespace ph
{

PH_DEFINE_EXTERNAL_LOG_GROUP(TransientResourceCache, World);

/*! @brief Provides thread-safe cooked data creation and storage.
Similar to `TransientVisualElement`, this cache will be cleared before rendering starts. 
All methods are thread-safe to call, however manipulating the created data is not thread-safe unless
stated explicitly.
*/
class TransientResourceCache : public CookedResourceCollectionBase
{
public:
	std::string getStats() const override;

	template<typename... DeducedArgs>
	TransientVisualElement* makeVisualElement(const SdlResourceId id, DeducedArgs&&... args)
	{
		return makeCookedResourceWithID(m_idToVisualElement, id, std::forward<DeducedArgs>(args)...);
	}

	const TransientVisualElement* getVisualElement(const SdlResourceId id) const;

private:
	TSynchronized<TSdlResourceIdMap<TransientVisualElement>> m_idToVisualElement;
};

}// end namespace ph
