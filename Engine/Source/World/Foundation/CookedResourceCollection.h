#pragma once

#include "Utility/INoCopyAndMove.h"
#include "Utility/TUniquePtrVector.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Math/Transform/Transform.h"
#include "World/Foundation/CookedGeometry.h"
#include "Utility/Concurrent/TSynchronized.h"
#include "Utility/traits.h"
#include "DataIO/SDL/SdlResourceId.h"
#include "Common/logging.h"
#include "Core/Intersectable/Intersectable.h"

#include <vector>
#include <utility>
#include <memory>
#include <unordered_map>
#include <string_view>

namespace ph
{

PH_DEFINE_EXTERNAL_LOG_GROUP(CookedResourceCollection, World);

/*! @brief Provides thread-safe cooked data creation and storage.
*/
class CookedResourceCollection final : private INoCopyAndMove
{
public:
	template<typename... DeducedArgs>
	PrimitiveMetadata* makeMetadata(DeducedArgs&&... args)
	{
		return makeCookedResource(m_metadatas, std::forward<DeducedArgs>(args)...);
	}

	template<CDerived<math::Transform> TransformType, typename... DeducedArgs>
	TransformType* makeTransform(DeducedArgs&&... args)
	{
		return makeCookedResource(m_transforms, std::forward<DeducedArgs>(args)...);
	}

	template<CDerived<Intersectable> IntersectableType, typename... DeducedArgs>
	IntersectableType* makeIntersectable(DeducedArgs&&... args)
	{
		return makeCookedResource(m_intersectables, std::forward<DeducedArgs>(args)...);
	}

	template<typename... DeducedArgs>
	CookedGeometry* makeGeometry(const SdlResourceId id, DeducedArgs&&... args)
	{
		return makeCookedResourceByID(m_idToGeometry, id, std::forward<DeducedArgs>(args)...);
	}

private:
	template<typename CookedType>
	using TSdlResourceIdMap = std::unordered_map<SdlResourceId, std::unique_ptr<CookedType>>;

	template<typename CookedType, typename... DeducedArgs>
	static CookedType* makeCookedResource(
		TSynchronized<TUniquePtrVector<CookedType>>& resources,
		DeducedArgs&&... args)
	{
		// Create resource in separate expression since no lock is required yet
		auto newResource = std::make_unique<CookedType>(std::forward<DeducedArgs>(args)...);

		// Arrow accessor to take advantage of auto synchronization
		return (&resources)->add(std::move(newResource));
	}

	template<typename CookedType, typename... DeducedArgs>
	static CookedType* makeCookedResourceByID(
		TSynchronized<TSdlResourceIdMap<CookedType>>& idToResource,
		const SdlResourceId id, 
		DeducedArgs&&... args)
	{
		// Create resource in separate expression since no lock is required yet
		auto newResource = std::make_unique<CookedType>(std::forward<DeducedArgs>(args)...);

		CookedType* resourcePtr = nullptr;

		idToResource.locked(
			[id, &resourcePtr, newResource = std::move(newResource)](auto& idToResource)
			{
				auto findResult = idToResource.find(id);
				if(findResult == idToResource.end())
				{
					resourcePtr = newResource.get();
					idToResource[id] = std::move(newResource);
				}
				else
				{
					PH_LOG(CookedResourceCollection,
						"overwriting existing cooked resource (id: {})", id);

					// Clear the content of existing resource while keeping its pointer valid
					resourcePtr = (*findResult).get();
					*resourcePtr = std::move(*newResource);
				}
			});

		return resourcePtr;
	}

	TSynchronized<TUniquePtrVector<PrimitiveMetadata>> m_metadatas;
	TSynchronized<TUniquePtrVector<math::Transform>> m_transforms;
	TSynchronized<TUniquePtrVector<Intersectable>> m_intersectables;
	TSynchronized<TSdlResourceIdMap<CookedGeometry>> m_idToGeometry;
};

}// end namespace ph
