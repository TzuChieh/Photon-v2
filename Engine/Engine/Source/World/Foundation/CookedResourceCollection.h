#pragma once

#include "World/Foundation/CookedResourceCollectionBase.h"
#include "Utility/TUniquePtrVector.h"
#include "Math/Transform/Transform.h"
#include "World/Foundation/CookedGeometry.h"
#include "World/Foundation/CookedMotion.h"
#include "Utility/Concurrent/TSynchronized.h"
#include "Utility/traits.h"
#include "SDL/SdlResourceId.h"
#include "Core/Intersection/PrimitiveMetadata.h"
#include "Core/Intersection/Intersectable.h"
#include "Core/Intersection/DataStructure/TIndexedPolygonBuffer.h"
#include "Core/Emitter/Emitter.h"
#include "World/Foundation/CookedNamedResource.h"

#include <Common/logging.h>

#include <utility>

namespace ph
{

PH_DEFINE_EXTERNAL_LOG_GROUP(CookedResourceCollection, World);

/*! @brief Provides thread-safe cooked data creation and storage.
All methods are thread-safe to call, however manipulating the created data is not thread-safe unless
stated explicitly.
*/
class CookedResourceCollection : public CookedResourceCollectionBase
{
public:
	std::string getStats() const override;

	template<typename... DeducedArgs>
	[[nodiscard]]
	PrimitiveMetadata* makeMetadata(DeducedArgs&&... args)
	{
		return makeCookedResource<PrimitiveMetadata>(
			m_metadatas, std::forward<DeducedArgs>(args)...);
	}

	template<CDerived<math::Transform> TransformType, typename... DeducedArgs>
	[[nodiscard]]
	TransformType* makeTransform(DeducedArgs&&... args)
	{
		return makeCookedResource<TransformType>(
			m_transforms, std::forward<DeducedArgs>(args)...);
	}

	template<CDerived<Intersectable> IntersectableType, typename... DeducedArgs>
	[[nodiscard]]
	IntersectableType* makeIntersectable(DeducedArgs&&... args)
	{
		return makeCookedResource<IntersectableType>(
			m_intersectables, std::forward<DeducedArgs>(args)...);
	}

	template<CDerived<Intersectable> IntersectableType>
	[[nodiscard]]
	IntersectableType* copyIntersectable(IntersectableType intersectable)
	{
		return makeIntersectable<IntersectableType>(std::move(intersectable));
	}

	template<CDerived<Emitter> EmitterType, typename... DeducedArgs>
	[[nodiscard]]
	EmitterType* makeEmitter(DeducedArgs&&... args)
	{
		return makeCookedResource<EmitterType>(
			m_emitters, std::forward<DeducedArgs>(args)...);
	}

	template<typename... DeducedArgs>
	[[nodiscard]]
	IndexedTriangleBuffer* makeTriangleBuffer(DeducedArgs&&... args)
	{
		return makeCookedResource<IndexedTriangleBuffer>(
			m_triangleBuffers, std::forward<DeducedArgs>(args)...);
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
	TSynchronized<CookedNamedResource>& getNamed();

	const CookedGeometry* getGeometry(const SdlResourceId id) const;
	const CookedMotion* getMotion(const SdlResourceId id) const;

private:
	TSynchronized<TUniquePtrVector<PrimitiveMetadata>> m_metadatas;
	TSynchronized<TUniquePtrVector<math::Transform>> m_transforms;
	TSynchronized<TUniquePtrVector<Intersectable>> m_intersectables;
	TSynchronized<TUniquePtrVector<Emitter>> m_emitters;
	TSynchronized<TUniquePtrVector<IndexedTriangleBuffer>> m_triangleBuffers;
	TSynchronized<TSdlResourceIdMap<CookedGeometry>> m_idToGeometry;
	TSynchronized<TSdlResourceIdMap<CookedMotion>> m_idToMotion;
	TSynchronized<CookedNamedResource> m_namedResource;
};

}// end namespace ph
