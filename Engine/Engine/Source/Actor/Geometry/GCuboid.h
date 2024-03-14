#pragma once

#include "Actor/Geometry/Geometry.h"
#include "Math/TVector3.h"
#include "Math/TVector4.h"
#include "SDL/sdl_interface.h"

#include <Common/primitive_type.h>

#include <array>
#include <vector>
#include <memory>

namespace ph
{

class GCuboid : public Geometry
{
public:
	void storeCooked(
		CookedGeometry& out_geometry,
		const CookingContext& ctx) const override;

	void genPrimitive(
		const PrimitiveBuildingMaterial& data,
		std::vector<std::unique_ptr<Primitive>>& out_primitives) const override;

	std::shared_ptr<Geometry> genTriangulated() const override;

	GCuboid& setSize(real sideLength);
	GCuboid& setSize(real xLen, real yLen, real zLen);
	GCuboid& setSize(const math::Vector3R& minVertex, const math::Vector3R& maxVertex);
	GCuboid& setSize(real xLen, real yLen, real zLen, const math::Vector3R& offset);

private:
	math::Vector3R m_size;
	math::Vector3R m_offset;
	math::Vector4R m_pxFaceUV;
	math::Vector4R m_nxFaceUV;
	math::Vector4R m_pyFaceUV;
	math::Vector4R m_nyFaceUV;
	math::Vector4R m_pzFaceUV;
	math::Vector4R m_nzFaceUV;

	static bool checkData(
		const PrimitiveBuildingMaterial& data, 
		const real xLen, const real yLen, const real zLen);

	static bool checkData(real xLen, real yLen, real zLen);

	static math::Vector4R makeNormalizedFaceUV();

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<GCuboid>)
	{
		ClassType clazz("cuboid");
		clazz.docName("Cuboid Geometry");
		clazz.description(
			"A shape that is similar to cube but may contain rectangular faces. "
			"It is centered around origin.");
		clazz.baseOn<Geometry>();

		TSdlVector3<OwnerType> size("size", &OwnerType::m_size);
		size.description("x, y, z dimensions of the cuboid.");
		size.defaultTo({1, 1, 1});
		clazz.addField(size);

		TSdlVector3<OwnerType> offset("offset", &OwnerType::m_offset);
		offset.description("x, y, z offsets of the cuboid, origin is at the centroid.");
		offset.defaultTo({0, 0, 0});
		offset.optional();
		clazz.addField(offset);

		TSdlVector4<OwnerType> pxFaceUV("px-face-uv", &OwnerType::m_pxFaceUV);
		pxFaceUV.description(
			"UV coordinates of the +x face (+y as upward), in (min-u, min-v, max-u, max-v).");
		pxFaceUV.defaultTo(makeNormalizedFaceUV());
		pxFaceUV.optional();
		clazz.addField(pxFaceUV);

		TSdlVector4<OwnerType> nxFaceUV("nx-face-uv", &OwnerType::m_nxFaceUV);
		nxFaceUV.description(
			"UV coordinates of the -x face (+y as upward), in (min-u, min-v, max-u, max-v).");
		nxFaceUV.defaultTo(makeNormalizedFaceUV());
		nxFaceUV.optional();
		clazz.addField(nxFaceUV);

		TSdlVector4<OwnerType> pyFaceUV("py-face-uv", &OwnerType::m_pyFaceUV);
		pyFaceUV.description(
			"UV coordinates of the +y face (-z as upward), in (min-u, min-v, max-u, max-v).");
		pyFaceUV.defaultTo(makeNormalizedFaceUV());
		pyFaceUV.optional();
		clazz.addField(pyFaceUV);

		TSdlVector4<OwnerType> nyFaceUV("ny-face-uv", &OwnerType::m_nyFaceUV);
		nyFaceUV.description(
			"UV coordinates of the -y face (+z as upward), in (min-u, min-v, max-u, max-v).");
		nyFaceUV.defaultTo(makeNormalizedFaceUV());
		nyFaceUV.optional();
		clazz.addField(nyFaceUV);

		TSdlVector4<OwnerType> pzFaceUV("pz-face-uv", &OwnerType::m_pzFaceUV);
		pzFaceUV.description(
			"UV coordinates of the +z face (+y as upward), in (min-u, min-v, max-u, max-v).");
		pzFaceUV.defaultTo(makeNormalizedFaceUV());
		pzFaceUV.optional();
		clazz.addField(pzFaceUV);

		TSdlVector4<OwnerType> nzFaceUV("nz-face-uv", &OwnerType::m_nzFaceUV);
		nzFaceUV.description(
			"UV coordinates of the -z face (+y as upward), in (min-u, min-v, max-u, max-v).");
		nzFaceUV.defaultTo(makeNormalizedFaceUV());
		nzFaceUV.optional();
		clazz.addField(nzFaceUV);

		return clazz;
	}
};

}// end namespace ph
