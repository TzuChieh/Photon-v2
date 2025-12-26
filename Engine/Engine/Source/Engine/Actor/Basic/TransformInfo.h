#pragma once

#include "Engine/Math/TVector3.h"
#include "Engine/Math/TQuaternion.h"
#include "Engine/Math/TDecomposedTransform.h"
#include "Engine/SDL/sdl_interface.h"

#include <Common/primitive_type.h>

namespace ph { class StaticAffineTransform; }
namespace ph { class StaticRigidTransform; }

namespace ph
{

class TransformInfo final
{
public:
	TransformInfo& setPos(real x, real y, real z);
	TransformInfo& setPos(const math::Vector3R& pos);
	TransformInfo& setRot(const math::Vector3R& axis, real degrees);
	TransformInfo& setRot(const math::QuaternionR& rot);
	TransformInfo& setScale(real uniformScale);
	TransformInfo& setScale(real x, real y, real z);
	TransformInfo& setScale(const math::Vector3R& scale);

	TransformInfo& translate(real amountX, real amountY, real amountZ);
	TransformInfo& translate(const math::Vector3R& amount);
	TransformInfo& rotate(const math::Vector3R& axis, real additionalDegrees);
	TransformInfo& rotate(const math::QuaternionR& additionalRot);
	TransformInfo& scale(real uniformAmount);
	TransformInfo& scale(real amountX, real amountY, real amountZ);
	TransformInfo& scale(const math::Vector3R& amount);

	math::Vector3R getPos() const;
	math::QuaternionR getRot() const;
	math::Vector3R getScale() const;

	math::TDecomposedTransform<real> getDecomposed() const;
	StaticAffineTransform getForwardStaticAffine() const;
	StaticAffineTransform getInverseStaticAffine() const;
	StaticRigidTransform getForwardStaticRigid() const;
	StaticRigidTransform getInverseStaticRigid() const;
	TransformInfo& set(const math::TDecomposedTransform<real>& decomposed);

private:
	math::Vector3R m_pos;
	math::QuaternionR m_rot;
	math::Vector3R m_scale;

public:
	PH_DEFINE_SDL_STRUCT(TransformInfo, ztruct)
	{
		ztruct.typeName("transform");
		ztruct.description("Data structure describing placement of an entity in the virtual world.");

		TSdlVector3<OwnerType> pos("pos", &OwnerType::m_pos);
		pos.description("Position of the entity.");
		pos.optional();
		pos.defaultTo({0, 0, 0});
		ztruct.addField(pos);

		TSdlQuaternion<OwnerType> rot("rot", &OwnerType::m_rot);
		rot.description("Rotation of the entity");
		rot.optional();
		rot.defaultTo(math::QuaternionR::makeNoRotation());
		ztruct.addField(rot);

		TSdlVector3<OwnerType> scale("scale", &OwnerType::m_scale);
		scale.description("Scale of the entity.");
		scale.optional();
		scale.defaultTo({1, 1, 1});
		ztruct.addField(scale);
	}
};

}// end namespace ph
