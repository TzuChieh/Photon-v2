#pragma once

#include "Actor/Actor.h"
#include "Math/Transform/StaticTransform.h"
#include "Math/Transform/TDecomposedTransform.h"
#include "Math/math_fwd.h"
#include "FileIO/SDL/TCommandInterface.h"

namespace ph
{

class PhysicalActor : public Actor, public TCommandInterface<PhysicalActor>
{
public:
	PhysicalActor();
	PhysicalActor(const PhysicalActor& other);
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

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);

	static void ciRegisterExecutors(CommandRegister& cmdRegister);

	static ExitStatus ciTranslate(
		const std::shared_ptr<PhysicalActor>& targetResource, 
		const InputPacket& packet);
	static ExitStatus ciRotate(
		const std::shared_ptr<PhysicalActor>& targetResource,
		const InputPacket& packet);
	static ExitStatus ciScale(
		const std::shared_ptr<PhysicalActor>& targetResource,
		const InputPacket& packet);
};

}// end namespace ph