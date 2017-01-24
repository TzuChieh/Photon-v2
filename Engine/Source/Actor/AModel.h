#pragma once

#include "Math/TransformInfo.h"
#include "Math/Transform.h"
#include "Actor/Actor.h"

#include <memory>
#include <vector>

namespace ph
{

class Geometry;
class Material;
class TextureMapper;
class CookedModelStorage;

class AModel final : public Actor
{
public:
	AModel();
	AModel(const std::shared_ptr<Geometry>& geometry, const std::shared_ptr<Material>& material);
	AModel(const AModel& other);
	AModel(const InputPacket& packet);
	virtual ~AModel() override;

	virtual void genCoreActor(CoreActor* const out_coreActor) const override;

	void translate(const Vector3f& translation);
	void translate(const float32 x, const float32 y, const float32 z);
	void rotate(const Vector3f& normalizedAxis, const float32 degrees);
	void scale(const Vector3f& scaleFactor);
	void scale(const float32 x, const float32 y, const float32 z);
	void scale(const float32 scaleFactor);

	friend void swap(AModel& first, AModel& second);
	AModel& operator = (AModel rhs);

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
	Transform     m_localToWorld;
	Transform     m_worldToLocal;

	void updateTransforms();
};

}// end namespace ph