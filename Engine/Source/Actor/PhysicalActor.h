#pragma once

#include "Actor/Actor.h"
#include "Math/Transform/StaticTransform.h"
#include "Math/Transform/TDecomposedTransform.h"
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

	// FIXME: precision loss using real
	void translate(const Vector3R& translation);
	void translate(const real x, const real y, const real z);
	void rotate(const Vector3R& axis, const real degrees);
	void rotate(const QuaternionR& rotation);
	void scale(const Vector3R& scaleFactor);
	void scale(const real x, const real y, const real z);
	void scale(const real scaleFactor);

	/*const StaticTransform* getLocalToWorldTransform() const;
	const StaticTransform* getWorldToLocalTransform() const;*/

	PhysicalActor& operator = (const PhysicalActor& rhs);

	friend void swap(PhysicalActor& first, PhysicalActor& second);

protected:
	TDecomposedTransform<hiReal> m_localToWorld;

	/*virtual void updateTransforms(const StaticTransform& parentTransform = StaticTransform(),
	                              const StaticTransform& parentInverseTransform = StaticTransform());*/
};

}// end namespace ph