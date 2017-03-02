#pragma once

#include "Actor/Actor.h"
#include "Math/TransformInfo.h"
#include "Math/Transform.h"
#include "Math/math_fwd.h"

namespace ph
{

class PhysicalActor : public Actor
{
public:
	PhysicalActor();
	PhysicalActor(const PhysicalActor& other);
	PhysicalActor(const InputPacket& packet);
	virtual ~PhysicalActor() override;

	virtual void cook(CookedActor* const out_cookedActor) const = 0;

	void translate(const Vector3R& translation);
	void translate(const real x, const real y, const real z);
	void rotate(const Vector3R& axis, const real degrees);
	void rotate(const QuaternionR& rotation);
	void scale(const Vector3R& scaleFactor);
	void scale(const real x, const real y, const real z);
	void scale(const real scaleFactor);

	const Transform* getLocalToWorldTransform() const;
	const Transform* getWorldToLocalTransform() const;

	PhysicalActor& operator = (const PhysicalActor& rhs);

	friend void swap(PhysicalActor& first, PhysicalActor& second);

protected:
	TransformInfo m_transformInfo;
	Transform     m_localToWorld;
	Transform     m_worldToLocal;

	virtual void updateTransforms(const Transform& parentTransform = Transform(), 
	                              const Transform& parentInverseTransform = Transform());
};

}// end namespace ph