#pragma once

#include "Math/TVector3.h"
#include "Math/TVector2.h"

#include <cstddef>
#include <array>
#include <optional>
#include <utility>

namespace ph
{

class TriFace final
{
public:
	TriFace(std::size_t index, std::array<math::Vector3R, 3> vertices);

	void setTexCoords(std::array<math::Vector3R, 3> texCoords);
	void setShadingNormals(std::array<math::Vector3R, 3> shadingNormals);

	std::size_t getIndex() const;
	const std::array<math::Vector3R, 3>& getVertices() const;
	const std::array<math::Vector3R, 3>& getTexCoords() const;
	const std::array<math::Vector3R, 3>& getShadingNormals() const;
	bool hasTexCoords() const;
	bool hasShadingNormals() const;

	// TODO: getXxxOrDefault() methods

private:
	std::size_t                                  m_index;
	std::array<math::Vector3R, 3>                m_vertices;
	std::optional<std::array<math::Vector3R, 3>> m_uvws;
	std::optional<std::array<math::Vector3R, 3>> m_normals;
};

// In-header Implementations:

inline TriFace::TriFace(const std::size_t index, std::array<math::Vector3R, 3> vertices) :
	m_index   (index),
	m_vertices(std::move(vertices)),
	m_uvws    (),
	m_normals ()
{}

inline void TriFace::setTexCoords(std::array<math::Vector3R, 3> texCoords)
{
	m_uvws = std::move(texCoords);
}

inline void TriFace::setShadingNormals(std::array<math::Vector3R, 3> shadingNormals)
{
	m_normals = std::move(shadingNormals);
}

inline std::size_t TriFace::getIndex() const
{
	return m_index;
}

inline const std::array<math::Vector3R, 3>& TriFace::getVertices() const
{
	return m_vertices;
}

inline const std::array<math::Vector3R, 3>& TriFace::getTexCoords() const
{
	PH_ASSERT(m_uvws);

	return *m_uvws;
}

inline const std::array<math::Vector3R, 3>& TriFace::getShadingNormals() const
{
	PH_ASSERT(m_normals);

	return *m_normals;
}

inline bool TriFace::hasTexCoords() const
{
	return m_uvws.has_value();
}

inline bool TriFace::hasShadingNormals() const
{
	return m_normals.has_value();
}

}// end namespace ph
