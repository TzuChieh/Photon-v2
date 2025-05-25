#pragma once

#include "Engine/World/Foundation/CookedResourceCollectionBase.h"
#include "Engine/Utility/TUniquePtrVector.h"
#include "Engine/Math/Transform/Transform.h"
#include "Engine/World/Foundation/CookedGeometry.h"
#include "Engine/World/Foundation/CookedMaterial.h"
#include "Engine/World/Foundation/CookedMotion.h"
#include "Engine/Utility/Concurrent/TSynchronized.h"
#include "Engine/Utility/traits.h"
#include "Engine/SDL/SdlResourceId.h"
#include "Engine/Core/Intersection/PrimitiveMetadata.h"
#include "Engine/Core/Intersection/Intersectable.h"
#include "Engine/Core/Intersection/DataStructure/TIndexedPolygonBuffer.h"
#include "Engine/Core/Emitter/Emitter.h"
#include "Engine/Core/SurfaceBehavior/SurfaceOptics.h"
#include "Engine/Core/VolumeBehavior/VolumeOptics.h"
#include "Engine/World/Foundation/CookedNamedResource.h"

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
	~CookedResourceCollection() override;

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

	template<CDerived<SurfaceOptics> SurfaceOpticsType, typename... DeducedArgs>
	[[nodiscard]]
	SurfaceOpticsType* makeSurfaceOptics(DeducedArgs&&... args)
	{
		return makeCookedResource<SurfaceOpticsType>(
			m_surfaceOptics, std::forward<DeducedArgs>(args)...);
	}

	template<CDerived<VolumeOptics> VolumeOpticsType, typename... DeducedArgs>
	[[nodiscard]]
	VolumeOpticsType* makeVolumeOptics(DeducedArgs&&... args)
	{
		return makeCookedResource<VolumeOpticsType>(
			m_volumeOptics, std::forward<DeducedArgs>(args)...);
	}

	template<typename... DeducedArgs>
	CookedGeometry* makeGeometry(const SdlResourceId id, DeducedArgs&&... args)
	{
		return makeCookedResourceWithID(m_idToGeometry, id, std::forward<DeducedArgs>(args)...);
	}

	template<typename... DeducedArgs>
	CookedMaterial* makeMaterial(const SdlResourceId id, DeducedArgs&&... args)
	{
		return makeCookedResourceWithID(m_idToMaterial, id, std::forward<DeducedArgs>(args)...);
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
	const CookedMaterial* getMaterial(const SdlResourceId id) const;
	const CookedMotion* getMotion(const SdlResourceId id) const;

private:
	TSynchronized<TUniquePtrVector<PrimitiveMetadata>> m_metadatas;
	TSynchronized<TUniquePtrVector<math::Transform>> m_transforms;
	TSynchronized<TUniquePtrVector<Intersectable>> m_intersectables;
	TSynchronized<TUniquePtrVector<Emitter>> m_emitters;
	TSynchronized<TUniquePtrVector<IndexedTriangleBuffer>> m_triangleBuffers;
	TSynchronized<TUniquePtrVector<SurfaceOptics>> m_surfaceOptics;
	TSynchronized<TUniquePtrVector<VolumeOptics>> m_volumeOptics;

	TSynchronized<TSdlResourceIdMap<CookedGeometry>> m_idToGeometry;
	TSynchronized<TSdlResourceIdMap<CookedMaterial>> m_idToMaterial;
	TSynchronized<TSdlResourceIdMap<CookedMotion>> m_idToMotion;

	TSynchronized<CookedNamedResource> m_namedResource;
};

}// end namespace ph
