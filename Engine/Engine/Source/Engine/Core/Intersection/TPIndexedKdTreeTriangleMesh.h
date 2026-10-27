#pragma once

#include "Engine/Core/Intersection/Primitive.h"
#include "Engine/Core/Intersection/DataStructure/TIndexedPolygonBuffer.h"
#include "Engine/Math/Algorithm/IndexedKdtree/TIndexedKdtree.h"
#include "Engine/Math/Geometry/TWatertightTriangle.h"

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

	bool isIntersecting(HitEvent& event) const override;
	bool isOccluding(const Ray& ray) const override;

	bool reintersect(HitEvent& event, HitEvent& srcEvent) const override;

	void calcHitDetail(HitEvent& event, HitDetail* out_detail) const override;

	[[nodiscard]]
	std::size_t getAttribute(
		EPrimitiveAttribute attribute,
		EAttributeDomain domain,
		uint64 faceID,
		TSpan<math::Vector3R> out_values) const override;

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

#include "Engine/Core/Intersection/TPIndexedKdTreeTriangleMesh.ipp"
