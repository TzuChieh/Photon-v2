#pragma once

#include "Engine/World/Foundation/CookedResourceCollectionBase.h"
#include "Engine/World/Foundation/TCookedResourceKey.h"
#include "Engine/Utility/TUniquePtrVector.h"
#include "Engine/World/Foundation/CookedGeometry.h"
#include "Engine/World/Foundation/CookedMaterial.h"
#include "Engine/World/Foundation/CookedMotion.h"
#include "Engine/Utility/Concurrent/TSynchronized.h"
#include "Engine/Utility/traits.h"
#include "Engine/Core/Transform/Transform.h"
#include "Engine/Core/Intersection/PrimitiveMetadata.h"
#include "Engine/Core/Intersection/Intersectable.h"
#include "Engine/Core/Intersection/DataStructure/TIndexedPolygonBuffer.h"
#include "Engine/Core/Emitter/Emitter.h"
#include "Engine/Core/SurfaceBehavior/SurfaceOptics.h"
#include "Engine/Core/VolumeBehavior/VolumeOptics.h"
#include "Engine/World/Foundation/CookedNamedResource.h"

#include <Common/logging.h>

#include <string>
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

	template<CDerived<Transform> TransformType, typename... DeducedArgs>
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
	CookedGeometry* makeGeometry(const CookedGeometryKey& key, DeducedArgs&&... args)
	{
		return makeCookedResourceWithKey(m_keyToGeometry, key, std::forward<DeducedArgs>(args)...);
	}

	template<typename... DeducedArgs>
	CookedMaterial* makeMaterial(const CookedMaterialKey& key, DeducedArgs&&... args)
	{
		return makeCookedResourceWithKey(m_keyToMaterial, key, std::forward<DeducedArgs>(args)...);
	}

	template<typename... DeducedArgs>
	CookedMotion* makeMotion(const CookedMotionKey& key, DeducedArgs&&... args)
	{
		return makeCookedResourceWithKey(m_keyToMotion, key, std::forward<DeducedArgs>(args)...);
	}

	/*! @brief Get the named resource sub-storage.
	@return A thread-safe storage.
	*/
	TSynchronized<CookedNamedResource>& getNamed();

	const CookedGeometry* getGeometry(const CookedGeometryKey& key) const;
	const CookedMaterial* getMaterial(const CookedMaterialKey& key) const;
	const CookedMotion* getMotion(const CookedMotionKey& key) const;

private:
	TSynchronized<TUniquePtrVector<PrimitiveMetadata>> m_metadatas;
	TSynchronized<TUniquePtrVector<Transform>> m_transforms;
	TSynchronized<TUniquePtrVector<Intersectable>> m_intersectables;
	TSynchronized<TUniquePtrVector<Emitter>> m_emitters;
	TSynchronized<TUniquePtrVector<IndexedTriangleBuffer>> m_triangleBuffers;
	TSynchronized<TUniquePtrVector<SurfaceOptics>> m_surfaceOptics;
	TSynchronized<TUniquePtrVector<VolumeOptics>> m_volumeOptics;

	TSynchronized<TCookedResourceKeyMap<CookedGeometryKey, CookedGeometry>> m_keyToGeometry;
	TSynchronized<TCookedResourceKeyMap<CookedMaterialKey, CookedMaterial>> m_keyToMaterial;
	TSynchronized<TCookedResourceKeyMap<CookedMotionKey, CookedMotion>> m_keyToMotion;

	TSynchronized<CookedNamedResource> m_namedResource;
};

}// end namespace ph
