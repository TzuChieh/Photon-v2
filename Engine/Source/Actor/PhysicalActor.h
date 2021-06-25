#pragma once

#include "Actor/Actor.h"
#include "Math/Transform/StaticAffineTransform.h"
#include "Math/Transform/TDecomposedTransform.h"
#include "Math/math_fwd.h"

namespace ph
{

class PhysicalActor : public Actor
{
public:
	PhysicalActor();
	PhysicalActor(const PhysicalActor& other);

	CookedUnit cook(CookingContext& context) override = 0;

	// FIXME: precision loss using real
	void translate(const math::Vector3R& translation);
	void translate(const real x, const real y, const real z);
	void rotate(const math::Vector3R& axis, const real degrees);
	void rotate(const math::QuaternionR& rotation);
	void scale(const math::Vector3R& scaleFactor);
	void scale(const real x, const real y, const real z);
	void scale(const real scaleFactor);

	void setBaseTransform(const math::TDecomposedTransform<hiReal>& baseLocalToWorld);

	/*const StaticTransform* getLocalToWorldTransform() const;
	const StaticTransform* getWorldToLocalTransform() const;*/

	PhysicalActor& operator = (const PhysicalActor& rhs);

	friend void swap(PhysicalActor& first, PhysicalActor& second);

protected:
	math::TDecomposedTransform<hiReal> m_localToWorld;

	/*virtual void updateTransforms(const StaticTransform& parentTransform = StaticTransform(),
	                              const StaticTransform& parentInverseTransform = StaticTransform());*/
};

}// end namespace ph
