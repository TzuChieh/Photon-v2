#pragma once

#include "Core/Intersection/Primitive.h"
#include "Core/Intersection/DataStructure/IndexedTriMesh.h"
#include "Core/Intersection/Intersectable.h"
#include "Math/Algorithm/IndexedKdtree/TIndexedKdtree.h"
#include "Math/Geometry/TWatertightTriangle.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <memory>

namespace ph
{

//template<typename Index>
//class TPKdTreeTriangleMesh : public Primitive
//{
//public:
//	explicit TPKdTreeTriangleMesh(
//		const std::shared_ptr<IndexedTriMesh>& triMesh,
//		math::IndexedKdtreeParams              params = math::IndexedKdtreeParams());
//
//	bool isIntersecting(const Ray& ray, HitProbe& probe) const override;
//
//	void calcIntersectionDetail(
//		const Ray& ray, 
//		HitProbe&  probe,
//		HitDetail* out_detail) const override;
//
//	math::AABB3D calcAABB() const override;
//
//private:
//	struct IndexedTriFaces
//	{
//		std::shared_ptr<IndexedTriMesh> triMesh;
//
//		TriFace operator () (const std::size_t index) const
//		{
//			PH_ASSERT(triMesh);
//			PH_ASSERT_LT(index, triMesh->numFaces());
//
//			return triMesh->getFace(index);
//		}
//	};
//
//	using Triangle = math::TWatertightTriangle<real>;
//
//	struct TriFaceAABBCalculator
//	{
//		math::AABB3D operator () (const TriFace& triFace) const
//		{
//			return Triangle(triFace.getVertices()).getAABB();
//		}
//	};
//
//	using Tree = math::TIndexedKdtree<
//		IndexedTriFaces,
//		TriFaceAABBCalculator,
//		Index>;
//
//	std::shared_ptr<IndexedTriMesh> m_triMesh;
//	Tree                            m_tree;
//};

}// end namespace ph

#include "Core/Intersection/TPKdTreeTriangleMesh.ipp"
