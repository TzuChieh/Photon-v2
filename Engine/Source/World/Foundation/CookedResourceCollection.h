#pragma once

#include "Utility/INoCopyAndMove.h"
#include "Utility/TUniquePtrVector.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Math/Transform/Transform.h"
#include "World/Foundation/CookedGeometry.h"
#include "World/Foundation/CookedMotion.h"
#include "Utility/Concurrent/TSynchronized.h"
#include "Utility/traits.h"
#include "DataIO/SDL/SdlResourceId.h"
#include "Common/logging.h"
#include "Core/Intersectable/Intersectable.h"
#include "World/Foundation/CookedNamedResource.h"

#include <vector>
#include <utility>
#include <memory>
#include <unordered_map>
#include <string_view>
#include <string>
#include <format>

namespace ph
{

PH_DEFINE_EXTERNAL_LOG_GROUP(CookedResourceCollection, World);

/*! @brief Provides thread-safe cooked data creation and storage.
All methods are thread-safe to call, however manipulating the created data is not thread-safe unless
stated explicitly.
*/
class CookedResourceCollection final : private INoCopyAndMove
{
public:
	template<typename... DeducedArgs>
	PrimitiveMetadata* makeMetadata(DeducedArgs&&... args)
	{
		return makeCookedResource<PrimitiveMetadata>(
			m_metadatas, std::forward<DeducedArgs>(args)...);
	}

	template<CDerived<math::Transform> TransformType, typename... DeducedArgs>
	TransformType* makeTransform(DeducedArgs&&... args)
	{
		return makeCookedResource<TransformType>(
			m_transforms, std::forward<DeducedArgs>(args)...);
	}

	template<CDerived<Intersectable> IntersectableType, typename... DeducedArgs>
	IntersectableType* makeIntersectable(DeducedArgs&&... args)
	{
		return makeCookedResource<IntersectableType>(
			m_intersectables, std::forward<DeducedArgs>(args)...);
	}

	template<CDerived<Intersectable> IntersectableType>
	IntersectableType* copyIntersectable(IntersectableType intersectable)
	{
		return makeIntersectable<IntersectableType>(std::move(intersectable));
	}

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
	TSynchronized<CookedNamedResource>& getNamed()
	{
		return m_namedResource;
	}

	const CookedGeometry* getGeometry(const SdlResourceId id) const
	{
		return getCookedResourceByID(m_idToGeometry, id);
	}

	const CookedMotion* getMotion(const SdlResourceId id) const
	{
		return getCookedResourceByID(m_idToMotion, id);
	}

	std::string getStats() const
	{
		return std::format(
			"{} metadatas, {} transforms, {} intersectables, {} geometries, {} motions",
			m_metadatas->size(),
			m_transforms->size(), 
			m_intersectables->size(),
			m_idToGeometry->size(),
			m_idToMotion->size());
	}

private:
	template<typename CookedType>
	using TSdlResourceIdMap = std::unordered_map<SdlResourceId, std::unique_ptr<CookedType>>;

	template<typename DerivedType, typename BaseType, typename... DeducedArgs>
	static DerivedType* makeCookedResource(
		TSynchronized<TUniquePtrVector<BaseType>>& syncedResources,
		DeducedArgs&&... args)
	{
		// Create resource in separate expression since no lock is required yet
		auto newResource = std::make_unique<DerivedType>(std::forward<DeducedArgs>(args)...);

		DerivedType* resourcePtr = nullptr;
		syncedResources.locked(
			[&resourcePtr, &newResource](auto& resources)
			{
				resourcePtr = resources.add(std::move(newResource));
			});

		return resourcePtr;
	}

	template<typename CookedType, typename... DeducedArgs>
	static CookedType* makeCookedResourceWithID(
		TSynchronized<TSdlResourceIdMap<CookedType>>& syncedIdToResource,
		const SdlResourceId id, 
		DeducedArgs&&... args)
	{
		// Create resource in separate expression since no lock is required yet
		auto newResource = std::make_unique<CookedType>(std::forward<DeducedArgs>(args)...);

		CookedType* resourcePtr = nullptr;
		syncedIdToResource.locked(
			[id, &resourcePtr, &newResource](auto& idToResource)
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
					resourcePtr = findResult->second.get();
					*resourcePtr = std::move(*newResource);
				}
			});

		return resourcePtr;
	}

	template<typename CookedType, typename... DeducedArgs>
	static const CookedType* getCookedResourceByID(
		const TSynchronized<TSdlResourceIdMap<CookedType>>& syncedIdToResource,
		const SdlResourceId id)
	{
		const CookedType* resourcePtr = nullptr;
		syncedIdToResource.constLocked(
			[id, &resourcePtr](const auto& idToResource)
			{
				auto findResult = idToResource.find(id);
				if(findResult != idToResource.end())
				{
					resourcePtr = findResult->second.get();
				}
			});

		return resourcePtr;
	}

	TSynchronized<TUniquePtrVector<PrimitiveMetadata>> m_metadatas;
	TSynchronized<TUniquePtrVector<math::Transform>> m_transforms;
	TSynchronized<TUniquePtrVector<Intersectable>> m_intersectables;
	TSynchronized<TSdlResourceIdMap<CookedGeometry>> m_idToGeometry;
	TSynchronized<TSdlResourceIdMap<CookedMotion>> m_idToMotion;
	TSynchronized<CookedNamedResource> m_namedResource;
};

}// end namespace ph
