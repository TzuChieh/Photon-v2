#pragma once

#include "Core/Intersection/Primitive.h"
#include "Core/Intersection/DataStructure/TIndexedPolygonBuffer.h"
#include "Math/Algorithm/IndexedKdtree/TIndexedKdtree.h"
#include "Math/Geometry/TWatertightTriangle.h"

#include <Common/assertion.h>

#include <utility>

namespace ph
{

template<typename Index>
class TPIndexedKdTreeTriangleMesh : public Primitive
{
public:
	explicit TPIndexedKdTreeTriangleMesh(
		IndexedTriangleBuffer* triangleBuffer,
		math::IndexedKdtreeParams params = math::IndexedKdtreeParams());

	bool isIntersecting(const Ray& ray, HitProbe& probe) const override;

	bool reintersect(
		const Ray& ray,
		HitProbe& probe,
		const Ray& /* srcRay */,
		HitProbe& srcProbe) const override;

	void calcHitDetail(
		const Ray& ray,
		HitProbe&  probe,
		HitDetail* out_detail) const override;

	math::AABB3D calcAABB() const override;

private:
	using Triangle = math::TWatertightTriangle<real>;

	struct IndexToTriangle
	{
		IndexedTriangleBuffer* triangleBuffer;

		Triangle operator () (const std::size_t idx) const
		{
			PH_ASSERT(triangleBuffer);
			PH_ASSERT_LT(idx, triangleBuffer->numFaces());

			return Triangle(triangleBuffer->getPositions(idx));
		}
	};

	struct TriangleToAABB
	{
		math::AABB3D operator () (const Triangle& triangle) const
		{
			return triangle.getAABB();
		}
	};

	struct ClosestHitProbeResult
	{
		math::Vector3R bary;
		Index faceIdx;
	};

	using KdTree = math::TIndexedKdtree<
		IndexToTriangle,
		TriangleToAABB,
		Index>;

	IndexedTriangleBuffer* m_triangleBuffer;
	KdTree m_kdTree;
};

}// end namespace ph

#include "Core/Intersection/TPIndexedKdTreeTriangleMesh.ipp"
