#pragma once

#include "Actor/Actor.h"
#include "Math/Transform/StaticAffineTransform.h"
#include "Actor/Basic/TransformInfo.h"
#include "Math/TVector3.h"
#include "Math/TQuaternion.h"
#include "SDL/sdl_interface.h"

#include <Common/primitive_type.h>

#include <optional>

namespace ph
{

class PhysicalActor : public Actor
{
public:
	TransientVisualElement cook(const CookingContext& ctx, const PreCookReport& report) const override = 0;

	void translate(const real amountX, const real amountY, const real amountZ);
	void translate(const math::Vector3R& amount);
	void rotate(const math::Vector3R& axis, const real additionanDegrees);
	void rotate(const math::QuaternionR& additionalRotation);
	void scale(const real uniformAmount);
	void scale(const real amountX, const real amountY, const real amountZ);
	void scale(const math::Vector3R& amount);

	void setBaseTransform(const math::TDecomposedTransform<real>& baseLocalToWorld);

protected:
	TransformInfo m_localToWorld;

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

	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<PhysicalActor>)
	{
		ClassType clazz("physical");
		clazz.docName("Physical Actor");
		clazz.description("An actor that is visible and can be transformed.");
		clazz.baseOn<Actor>();

		clazz.addStruct(&OwnerType::m_localToWorld);

		clazz.addFunction<SdlTranslate>();
		clazz.addFunction<SdlRotate>();
		clazz.addFunction<SdlScale>();

		return clazz;
	}
};

}// end namespace ph
