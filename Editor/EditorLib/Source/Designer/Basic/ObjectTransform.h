#pragma once

#include <Math/TVector3.h>
#include <Math/TQuaternion.h>
#include <Math/Transform/TDecomposedTransform.h>
#include <Common/primitive_type.h>
#include <SDL/sdl_interface.h>

namespace ph::math { class StaticAffineTransform; }
namespace ph::math { class StaticRigidTransform; }

namespace ph::editor
{

class ObjectTransform final
{
public:
	ObjectTransform& setPos(real x, real y, real z);
	ObjectTransform& setPos(const math::Vector3R& pos);
	ObjectTransform& setRot(const math::Vector3R& axis, real degrees);
	ObjectTransform& setRot(const math::QuaternionR& rot);
	ObjectTransform& setScale(real uniformScale);
	ObjectTransform& setScale(real x, real y, real z);
	ObjectTransform& setScale(const math::Vector3R& scale);

	ObjectTransform& translate(real amountX, real amountY, real amountZ);
	ObjectTransform& translate(const math::Vector3R& amount);
	ObjectTransform& rotate(const math::Vector3R& axis, real additionalDegrees);
	ObjectTransform& rotate(const math::QuaternionR& additionalRotation);
	ObjectTransform& scale(real uniformAmount);
	ObjectTransform& scale(real amountX, real amountY, real amountZ);
	ObjectTransform& scale(const math::Vector3R& amount);

	math::Vector3R getPos() const;
	math::QuaternionR getRot() const;
	math::Vector3R getScale() const;

	math::TDecomposedTransform<real> getDecomposed() const;
	math::StaticAffineTransform getForwardStaticAffine() const;
	math::StaticAffineTransform getInverseStaticAffine() const;
	math::StaticRigidTransform getForwardStaticRigid() const;
	math::StaticRigidTransform getInverseStaticRigid() const;
	ObjectTransform& set(const math::TDecomposedTransform<real>& decomposed);

private:
	math::Vector3R m_pos;
	math::QuaternionR m_rot;
	math::Vector3R m_scale;

public:
	PH_DEFINE_SDL_STRUCT(TSdlOwnerStruct<ObjectTransform>)
	{
		StructType ztruct("obj-transform");
		ztruct.description("Data structure describing placement of an object in the virtual world.");

		TSdlVector3<OwnerType> pos("pos", &OwnerType::m_pos);
		pos.description("Position of the object.");
		pos.optional();
		pos.defaultTo({0, 0, 0});
		ztruct.addField(pos);

		TSdlQuaternion<OwnerType> rot("rot", &OwnerType::m_rot);
		rot.description("Rotation of the object");
		rot.optional();
		rot.defaultTo(math::QuaternionR::makeNoRotation());
		ztruct.addField(rot);

		TSdlVector3<OwnerType> scale("scale", &OwnerType::m_scale);
		scale.description("Scale of the object.");
		scale.optional();
		scale.defaultTo({1, 1, 1});
		ztruct.addField(scale);

		return ztruct;
	}
};

}// end namespace ph::editor
