#pragma once

#include "Actor/Geometry/Geometry.h"
#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "DataIO/SDL/sdl_interface.h"

#include <vector>

namespace ph
{

class GTriangleMesh;

class GSphere : public Geometry
{
public:
	GSphere();
	explicit GSphere(real radius);

	void genPrimitive(
		const PrimitiveBuildingMaterial& data,
		std::vector<std::unique_ptr<Primitive>>& out_primitives) const override;

	std::shared_ptr<Geometry> genTransformed(
		const math::StaticAffineTransform& transform) const override;

private:
	real m_radius;

	std::size_t addVertex(const math::Vector3R& vertex, std::vector<math::Vector3R>* const out_vertices) const;
	std::size_t addMidpointVertex(const std::size_t iA, const std::size_t iB, std::vector<math::Vector3R>* const out_vertices) const;
	std::shared_ptr<GTriangleMesh> genTriangleMesh() const;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<GSphere>)
	{
		ClassType clazz("sphere");
		clazz.docName("Spherical Geometry");
		clazz.description("A perfectly round shape centering around origin.");
		clazz.baseOn<Geometry>();

		TSdlReal<OwnerType> radius("radius", &OwnerType::m_radius);
		radius.description("Size of the sphere.");
		radius.defaultTo(1);
		clazz.addField(radius);

		return clazz;
	}
};

}// end namespace ph
