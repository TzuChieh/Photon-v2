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

class Entity final
{
public:
	Entity();
	Entity(const std::shared_ptr<Geometry>& geometry, const std::shared_ptr<Material>& material);
	Entity(const Entity& other);
	~Entity();

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
	std::vector<Entity>& getChildren();
	const std::vector<Entity>& getChildren() const;
	void addChild(const Entity& child);

	friend void swap(Entity& first, Entity& second);
	Entity& operator = (Entity rhs);

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

	inline const Transform* getLocalToWorldTransform() const
	{
		return &m_localToWorld;
	}

	inline const Transform* getWorldToLocalTransform() const
	{
		return &m_worldToLocal;
	}

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
	std::shared_ptr<Geometry>      m_geometry;
	std::shared_ptr<Material>      m_material;
	std::shared_ptr<TextureMapper> m_textureMapper;

	TransformInfo m_entityTransformInfo;
	Transform m_localToWorld;
	Transform m_worldToLocal;

	std::vector<Entity> m_childrenEntities;
	const Entity* m_parentEntity;

	void updateAllTransforms();
	void updateTransforms();
	void setParent(const Entity* const parent);
};

}// end namespace ph