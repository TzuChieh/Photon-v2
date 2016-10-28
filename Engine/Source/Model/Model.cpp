#include "Model/Model.h"

namespace ph
{

Model::Model(const std::shared_ptr<Geometry>& geometry, const std::shared_ptr<Material>& material) : 
	m_geometry(geometry), m_material(material)
{

}

Model::Model(const Model& other) : 
	m_geometry(other.m_geometry), m_material(other.m_material)
{

}

Model& Model::operator = (const Model& rhs)
{
	m_geometry = rhs.m_geometry;
	m_material = rhs.m_material;

	return *this;
}

}// end namespace ph