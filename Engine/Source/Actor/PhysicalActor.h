#pragma once

#include "Actor/Actor.h"
#include "Math/Transform/StaticAffineTransform.h"
#include "Math/Transform/TDecomposedTransform.h"
#include "Math/math_fwd.h"
#include "DataIO/SDL/TCommandInterface.h"
#include "DataIO/SDL/SdlExecutor.h"
#include "DataIO/SDL/CommandRegister.h"

namespace ph
{

class PhysicalActor : public Actor, public TCommandInterface<PhysicalActor>
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

// command interface
public:
	explicit PhysicalActor(const InputPacket& packet);

	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);

	static ExitStatus ciTranslate(
		const std::shared_ptr<PhysicalActor>& targetResource, 
		const InputPacket& packet);
	static ExitStatus ciRotate(
		const std::shared_ptr<PhysicalActor>& targetResource,
		const InputPacket& packet);
	static ExitStatus ciScale(
		const std::shared_ptr<PhysicalActor>& targetResource,
		const InputPacket& packet);

	template<typename Derived>
	static void registerTransformFuncs(CommandRegister& cmdRegister);
};

// In-header Implementations:

template<typename PhysicalActorType>
void PhysicalActor::registerTransformFuncs(CommandRegister& cmdRegister)
{
	SdlExecutor translateSE;
	translateSE.setName("translate");
	translateSE.setFunc<PhysicalActorType>(ciTranslate);
	cmdRegister.addExecutor(translateSE);

	SdlExecutor rotateSE;
	rotateSE.setName("rotate");
	rotateSE.setFunc<PhysicalActorType>(ciRotate);
	cmdRegister.addExecutor(rotateSE);

	SdlExecutor scaleSE;
	scaleSE.setName("scale");
	scaleSE.setFunc<PhysicalActorType>(ciScale);
	cmdRegister.addExecutor(scaleSE);
}

}// end namespace ph

/*
	<SDL_interface>

	<category>  actor       </category>
	<type_name> physical    </type_name>
	<extend>    actor.actor </extend>

	<name> Physical Actor </name>
	<description>
		An actor that is visible and can be transformed.
	</description>

	<command type="executor" name="translate">
		<description>
			Moves the actor away from the original location with a specified amount.
		</description>
		<input name="factor" type="vector3">
			<description>The amount to move in each axis.</description>
		</input>
	</command>

	<command type="executor" name="rotate">
		<description>
			Rotates the actor along an axis with a specified amount.
		</description>
		<input name="axis" type="vector3">
			<description>The axis for rotation.</description>
		</input>
		<input name="degree" type="real">
			<description>The amount of the rotation.</description>
		</input>
		<input name="factor" type="quaternion">
			<description>Specifying the rotation with a quaternion directly.</description>
		</input>
	</command>

	<command type="executor" name="scale">
		<description>
			Enlarges or shrinks the actor with some specified amount.
		</description>
		<input name="factor" type="vector3">
			<description>The amount to scale in each axis.</description>
		</input>
	</command>

	</SDL_interface>
*/
