#pragma once

#include "Core/Intersectable/DataStructure/TriFace.h"
#include "Math/TVector3.h"
#include "Common/assertion.h"

#include <cstddef>
#include <array>

namespace ph
{

class IndexedTriMesh
{
public:
	explicit IndexedTriMesh(std::size_t numFaces);
	virtual ~IndexedTriMesh() = default;

	virtual TriFace getFace(std::size_t index) const = 0;

	virtual std::array<math::Vector3R, 3> getVertices(std::size_t index) const;

	std::size_t numFaces() const;

	// TODO: interface for aquiring new UV from a different channel
	// TODO: do we need to know how many UV channels this mesh is supporting?

protected:
	std::size_t m_numFaces;
};

// In-header Implementations:

inline IndexedTriMesh::IndexedTriMesh(const std::size_t numFaces) : 
	m_numFaces(numFaces)
{
	PH_ASSERT_GT(numFaces, 0);
}

inline std::array<math::Vector3R, 3> IndexedTriMesh::getVertices(const std::size_t index) const
{
	return getFace(index).getVertices();
}

inline std::size_t IndexedTriMesh::numFaces() const
{
	return m_numFaces;
}

}// end namespace ph
