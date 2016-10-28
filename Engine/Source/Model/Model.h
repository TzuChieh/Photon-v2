#pragma once

#include <memory>

namespace ph
{

class Geometry;
class Material;

class Model final
{
public:
	Model(const std::shared_ptr<Geometry>& geometry, const std::shared_ptr<Material>& material);

	inline const Geometry* getGeometry() const
	{
		return m_geometry.get();
	}

	inline const Material* getMaterial() const
	{
		return m_material.get();
	}

private:
	std::shared_ptr<Geometry> m_geometry;
	std::shared_ptr<Material> m_material;
};

}// end namespace ph