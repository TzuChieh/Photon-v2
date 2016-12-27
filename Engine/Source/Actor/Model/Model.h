#pragma once

#include "Math/TransformInfo.h"
#include "Math/Transform.h"

#include <memory>
#include <vector>

namespace ph
{

class Geometry;
class Material;
class TextureMapper;
class CookedModelStorage;

class Model final
{
public:
	Model();
	Model(const std::shared_ptr<Geometry>& geometry, const std::shared_ptr<Material>& material);
	Model(const Model& other);
	~Model();

	void cookData(CookedModelStorage* const out_cookedModelStorage) const;

	void translate(const Vector3f& translation);
	void translate(const float32 x, const float32 y, const float32 z);
	void rotate(const Vector3f& normalizedAxis, const float32 degrees);
	void scale(const Vector3f& scaleFactor);
	void scale(const float32 x, const float32 y, const float32 z);
	void scale(const float32 scaleFactor);

	friend void swap(Model& first, Model& second);
	Model& operator = (Model rhs);

	const Geometry*   getGeometry() const;
	const Material*   getMaterial() const;
	const Transform*  getLocalToWorldTransform() const;
	const Transform*  getWorldToLocalTransform() const;

	void setGeometry(const std::shared_ptr<Geometry>& geometry);
	void setMaterial(const std::shared_ptr<Material>& material);

	inline const Vector3f& getPosition() const
	{
		return m_entityTransformInfo.getPosition();
	}

	inline const Quaternion& getRotation() const
	{
		return m_entityTransformInfo.getRotation();
	}

	inline const Vector3f& getScale() const
	{
		return m_entityTransformInfo.getScale();
	}

private:
	// resources
	std::shared_ptr<Geometry> m_geometry;
	std::shared_ptr<Material> m_material;
	TransformInfo m_entityTransformInfo;

	mutable Transform       m_localToWorld;
	mutable Transform       m_worldToLocal;

	void updateTransforms();
};

}// end namespace ph