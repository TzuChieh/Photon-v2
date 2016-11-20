#pragma once

#include "Math/Transform.h"

#include <memory>

namespace ph
{

class Geometry;
class Material;

class Model final
{
public:
	Model();
	Model(const std::shared_ptr<Geometry>& geometry, const std::shared_ptr<Material>& material);
	Model(const Model& other);

	Model& operator = (const Model& rhs);

	inline const Geometry* getGeometry() const
	{
		return m_geometry.get();
	}

	inline const Material* getMaterial() const
	{
		return m_material.get();
	}

	inline const Transform* getTransform() const
	{
		return &m_transform;
	}

	inline void setGeometry(const std::shared_ptr<Geometry>& geometry)
	{
		m_geometry = geometry;
	}

	inline void setMaterial(const std::shared_ptr<Material>& material)
	{
		m_material = material;
	}

private:
	std::shared_ptr<Geometry> m_geometry;
	std::shared_ptr<Material> m_material;

	Transform m_transform;
};

}// end namespace ph