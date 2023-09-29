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
	ObjectTransform& setPosition(real x, real y, real z);
	ObjectTransform& setPosition(const math::Vector3R& position);
	ObjectTransform& setRotation(const math::Vector3R& axis, real degrees);
	ObjectTransform& setRotation(const math::QuaternionR& rotation);
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

	math::Vector3R getPosition() const;
	math::QuaternionR getRotation() const;
	math::Vector3R getScale() const;

	math::TDecomposedTransform<real> getDecomposed() const;
	math::StaticAffineTransform getForwardStaticAffine() const;
	math::StaticAffineTransform getInverseStaticAffine() const;
	math::StaticRigidTransform getForwardStaticRigid() const;
	math::StaticRigidTransform getInverseStaticRigid() const;
	ObjectTransform& set(const math::TDecomposedTransform<real>& decomposed);

private:
	math::Vector3R m_position;
	math::QuaternionR m_rotation;
	math::Vector3R m_scale;

public:
	PH_DEFINE_SDL_STRUCT(TSdlOwnerStruct<ObjectTransform>)
	{
		StructType ztruct("obj-transform");
		ztruct.description("Data structure describing placement of an object in the virtual world.");

		TSdlVector3<OwnerType> position("position", &OwnerType::m_position);
		position.description("Position of the object.");
		position.optional();
		position.defaultTo({0, 0, 0});
		ztruct.addField(position);

		TSdlQuaternion<OwnerType> rotation("rotation", &OwnerType::m_rotation);
		rotation.description("Rotation of the object");
		rotation.optional();
		rotation.defaultTo(math::QuaternionR::makeNoRotation());
		ztruct.addField(rotation);

		TSdlVector3<OwnerType> scale("scale", &OwnerType::m_scale);
		scale.description("Scale of the object.");
		scale.optional();
		scale.defaultTo({1, 1, 1});
		ztruct.addField(scale);

		return ztruct;
	}
};

}// end namespace ph::editor
