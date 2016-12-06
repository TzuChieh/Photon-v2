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

class Model final
{
public:
	Model();
	Model(const std::shared_ptr<Geometry>& geometry, const std::shared_ptr<Material>& material);
	Model(const Model& other);

	void translate(const Vector3f& translation);
	void translate(const float32 x, const float32 y, const float32 z);
	void rotate(const Vector3f& normalizedAxis, const float32 degrees);
	void scale(const Vector3f& scaleFactor);
	void scale(const float32 x, const float32 y, const float32 z);
	void scale(const float32 scaleFactor);

	void setGeometry(const std::shared_ptr<Geometry>& geometry);
	void setMaterial(const std::shared_ptr<Material>& material);
	void setTextureMapper(const std::shared_ptr<TextureMapper>& textureMapper);

	bool hasChildren() const;

	inline const Geometry* getGeometry() const
	{
		return m_geometry.get();
	}

	inline const Material* getMaterial() const
	{
		return m_material.get();
	}

	inline const TextureMapper* getTextureMapper() const
	{
		return m_textureMapper.get();
	}

	inline const Transform* getModelToWorldTransform() const
	{
		return &m_modelToWorld;
	}

	inline const Transform* getWorldToModelTransform() const
	{
		return &m_worldToModel;
	}

	inline const Vector3f& getPosition() const
	{
		return m_transformInfo.getPosition();
	}

	inline const Quaternion& getRotation() const
	{
		return m_transformInfo.getRotation();
	}

	inline const Vector3f& getScale() const
	{
		return m_transformInfo.getScale();
	}

private:
	std::shared_ptr<Geometry>      m_geometry;
	std::shared_ptr<Material>      m_material;
	std::shared_ptr<TextureMapper> m_textureMapper;

	TransformInfo m_transformInfo;
	Transform m_modelToWorld;
	Transform m_worldToModel;

	std::vector<Model> m_childrenModels;

	void updateTransforms();
};

}// end namespace ph