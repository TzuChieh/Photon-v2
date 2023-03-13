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
	CookedGeometry* makeGeometry(const SdlResourceId id, DeducedArgs&&... args)
	{
		return makeCookedResourceWithID(m_idToGeometry, id, std::forward<DeducedArgs>(args)...);
	}

	template<typename... DeducedArgs>
	CookedMotion* makeMotion(const SdlResourceId id, DeducedArgs&&... args)
	{
		return makeCookedResourceWithID(m_idToMotion, id, std::forward<DeducedArgs>(args)...);
	}

	/*! @brief Get the named resource sub-storage.
	@return A thread-safe storage.
	*/
	TSynchronized<CookedNamedResource>& getNamed();

	const CookedGeometry* getGeometry(const SdlResourceId id) const;
	const CookedMotion* getMotion(const SdlResourceId id) const;

private:
	TSynchronized<TUniquePtrVector<PrimitiveMetadata>> m_metadatas;
	TSynchronized<TUniquePtrVector<math::Transform>> m_transforms;
	TSynchronized<TUniquePtrVector<Intersectable>> m_intersectables;
	TSynchronized<TUniquePtrVector<Emitter>> m_emitters;
	TSynchronized<TSdlResourceIdMap<CookedGeometry>> m_idToGeometry;
	TSynchronized<TSdlResourceIdMap<CookedMotion>> m_idToMotion;
	TSynchronized<CookedNamedResource> m_namedResource;
};

}// end namespace ph
