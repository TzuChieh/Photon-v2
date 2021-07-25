#pragma once

#include "Actor/Actor.h"
#include "Math/Transform/StaticAffineTransform.h"
#include "Math/Transform/TDecomposedTransform.h"
#include "Math/math_fwd.h"
#include "DataIO/SDL/sdl_interface.h"

#include <optional>

namespace ph
{

class PhysicalActor : public Actor
{
public:
	PhysicalActor();
	PhysicalActor(const PhysicalActor& other);

	CookedUnit cook(ActorCookingContext& ctx) override = 0;

	void translate(const math::Vector3R& translation);
	void translate(const real x, const real y, const real z);
	void rotate(const math::Vector3R& axis, const real degrees);
	void rotate(const math::QuaternionR& rotation);
	void scale(const math::Vector3R& scaleFactor);
	void scale(const real x, const real y, const real z);
	void scale(const real scaleFactor);

	void setBaseTransform(const math::TDecomposedTransform<real>& baseLocalToWorld);

	/*const StaticTransform* getLocalToWorldTransform() const;
	const StaticTransform* getWorldToLocalTransform() const;*/

	PhysicalActor& operator = (const PhysicalActor& rhs);

	friend void swap(PhysicalActor& first, PhysicalActor& second);

protected:
	math::TDecomposedTransform<real> m_localToWorld;

	/*virtual void updateTransforms(const StaticTransform& parentTransform = StaticTransform(),
	                              const StaticTransform& parentInverseTransform = StaticTransform());*/

public:
	struct SdlTranslate
	{
		math::Vector3R amount;

		inline void operator () (PhysicalActor& actor) const
		{
			actor.translate(amount);
		}

		PH_DEFINE_SDL_FUNCTION(TSdlMethod<SdlTranslate, PhysicalActor>)
		{
			FunctionType func("translate");
			func.description("Moves the actor away from the original location with a specified amount.");

			TSdlVector3<OwnerType> amount("amount", &OwnerType::amount);
			amount.description("The amount to move on each axis.");
			amount.defaultTo({0, 0, 0});
			func.addParam(amount);

			return func;
		}
	};

	struct SdlRotate
	{
		std::optional<math::Vector3R>    axis;
		std::optional<real>              degrees;
		std::optional<math::QuaternionR> rotation;

		inline void operator () (PhysicalActor& actor) const
		{
			if(rotation)
			{
				actor.rotate((*rotation).normalize());
			}
			else if(axis && degrees)
			{
				actor.rotate((*axis).normalize(), *degrees);
			}
			else
			{
				throw SdlLoadError(
					"possible input formats are: "
					"1. A vector3 axis and real degrees; "
					"2. Specify rotation directly with a quaternion.");
			}
		}

		PH_DEFINE_SDL_FUNCTION(TSdlMethod<SdlRotate, PhysicalActor>)
		{
			FunctionType func("rotate");
			func.description("Rotates the actor along an axis with a specified amount.");

			TSdlOptionalVector3<OwnerType> axis("axis", &OwnerType::axis);
			axis.description("The axis for rotation.");
			func.addParam(axis);

			TSdlOptionalReal<OwnerType> degrees("degrees", &OwnerType::degrees);
			degrees.description("The amount of the rotation, in degrees.");
			func.addParam(degrees);

			TSdlOptionalQuaternion<OwnerType> rotation("rotation", &OwnerType::rotation);
			rotation.description("Specify the rotation with a quaternion directly.");
			func.addParam(rotation);

			return func;
		}
	};

	struct SdlScale
	{
		math::Vector3R amount;

		inline void operator () (PhysicalActor& actor) const
		{
			actor.scale(amount);
		}

		PH_DEFINE_SDL_FUNCTION(TSdlMethod<SdlScale, PhysicalActor>)
		{
			FunctionType func("scale");
			func.description("Enlarges or shrinks the actor with some specified amount.");

			TSdlVector3<OwnerType> amount("amount", &OwnerType::amount);
			amount.description("The amount to scale on each axis.");
			amount.defaultTo({1, 1, 1});
			func.addParam(amount);

			return func;
		}
	};

	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<PhysicalActor>)
	{
		ClassType clazz("physical");
		clazz.description("An actor that is visible and can be transformed.");
		clazz.baseOn<Actor>();

		clazz.addFunction<SdlTranslate>();
		clazz.addFunction<SdlRotate>();
		clazz.addFunction<SdlScale>();

		return clazz;
	}
};

}// end namespace ph
