#pragma once

#include "Math/TVector3.h"
#include "Math/TQuaternion.h"
#include "Math/Transform/TDecomposedTransform.h"
#include "Common/primitive_type.h"
#include "SDL/sdl_interface.h"

namespace ph::math { class StaticAffineTransform; }
namespace ph::math { class StaticRigidTransform; }

namespace ph
{

class TransformInfo final
{
public:
	TransformInfo& setPosition(real x, real y, real z);
	TransformInfo& setPosition(const math::Vector3R& position);
	TransformInfo& setRotation(const math::Vector3R& axis, real degrees);
	TransformInfo& setRotation(const math::QuaternionR& rotation);
	TransformInfo& setScale(real uniformScale);
	TransformInfo& setScale(real x, real y, real z);
	TransformInfo& setScale(const math::Vector3R& scale);

	TransformInfo& translate(real amountX, real amountY, real amountZ);
	TransformInfo& translate(const math::Vector3R& amount);
	TransformInfo& rotate(const math::Vector3R& axis, real additionalDegrees);
	TransformInfo& rotate(const math::QuaternionR& additionalRotation);
	TransformInfo& scale(real uniformAmount);
	TransformInfo& scale(real amountX, real amountY, real amountZ);
	TransformInfo& scale(const math::Vector3R& amount);

	math::Vector3R getPosition() const;
	math::QuaternionR getRotation() const;
	math::Vector3R getScale() const;

	math::TDecomposedTransform<real> getDecomposed() const;
	math::StaticAffineTransform getForwardStaticAffine() const;
	math::StaticAffineTransform getInverseStaticAffine() const;
	math::StaticRigidTransform getForwardStaticRigid() const;
	math::StaticRigidTransform getInverseStaticRigid() const;
	TransformInfo& set(const math::TDecomposedTransform<real>& decomposed);

private:
	math::Vector3R m_position;
	math::QuaternionR m_rotation;
	math::Vector3R m_scale;

public:
	PH_DEFINE_SDL_STRUCT(TSdlOwnerStruct<TransformInfo>)
	{
		StructType ztruct("transform");
		ztruct.description("Data structure describing placement of an entity in the virtual world.");

		TSdlVector3<OwnerType> position("position", &OwnerType::m_position);
		position.description("Position of the entity.");
		position.optional();
		position.defaultTo({0, 0, 0});
		ztruct.addField(position);

		TSdlQuaternion<OwnerType> rotation("rotation", &OwnerType::m_rotation);
		rotation.description("Rotation of the entity");
		rotation.optional();
		rotation.defaultTo(math::QuaternionR::makeNoRotation());
		ztruct.addField(rotation);

		TSdlVector3<OwnerType> scale("scale", &OwnerType::m_scale);
		scale.description("Scale of the entity.");
		scale.optional();
		scale.defaultTo({1, 1, 1});
		ztruct.addField(scale);

		return ztruct;
	}
};

}// end namespace ph
