#include "Model/Model.h"

namespace ph
{

Model::Model(const std::shared_ptr<Geometry>& geometry, const std::shared_ptr<Material>& material) : 
	m_geometry(geometry), m_material(material)
{

}

}// end namespace ph