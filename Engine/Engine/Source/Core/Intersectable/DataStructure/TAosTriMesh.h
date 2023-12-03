#pragma once

#include "Core/Intersectable/DataStructure/IndexedTriMesh.h"

#include <cstddef>
#include <memory>
#include <vector>
#include <type_traits>

namespace ph
{

template<typename Index, typename Vertex, typename VerticesToTriFace>
class TAosTriMesh : public IndexedTriMesh
{
	static_assert(std::is_invocable_r_v<
		TriFace, VerticesToTriFace, std::size_t, Vertex, Vertex, Vertex>);

public:
	TAosTriMesh(
		std::vector<Vertex> vertices, 
		std::vector<Index>  indices,
		VerticesToTriFace   verticesToTriFace);

	TAosTriMesh(
		const Vertex*     vertices, 
		std::size_t       numVertices, 
		const Index*      indices,
		std::size_t       numIndices,
		VerticesToTriFace verticesToTriFace);

	TriFace getFace(std::size_t faceIndex) const override;

private:
	std::unique_ptr<Vertex[]> m_vertices;
	std::size_t               m_numVertices;
	std::unique_ptr<Index[]>  m_indices;
	std::size_t               m_numIndices;
	VerticesToTriFace         m_verticesToTriFace;
};

}// end namespace ph

#include "Core/Intersectable/DataStructure/TAosTriMesh.ipp"
