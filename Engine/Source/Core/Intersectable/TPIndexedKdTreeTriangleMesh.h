#pragma once

#include "Core/Intersectable/Primitive.h"
#include "Core/Intersectable/DataStructure/IndexedVertexBuffer.h"
#include "Math/Algorithm/IndexedKdtree/TIndexedKdtree.h"

#include <utility>

namespace ph
{

template<typename Index>
class TPIndexedKdTreeTriangleMesh : public Primitive
{
public:
	TPIndexedKdTreeTriangleMesh(
		IndexedVertexBuffer       vertexBuffer,
		const PrimitiveMetadata*  metadata,
		math::IndexedKdtreeParams params = math::IndexedKdtreeParams());

	bool isIntersecting(const Ray& ray, HitProbe& probe) const override;

	void calcIntersectionDetail(
		const Ray& ray,
		HitProbe& probe,
		HitDetail* out_detail) const override;

	math::AABB3D calcAABB() const override;

private:
	struct 

	IndexedVertexBuffer m_vertexBuffer;
};

// In-header Implementations:



}// end namespace ph
