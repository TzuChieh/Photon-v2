#pragma once

#include "Actor/Geometry/Geometry.h"
#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "Math/Geometry/TAABB2D.h"
#include "DataIO/SDL/sdl_interface.h"

#include <array>
#include <vector>
#include <memory>

namespace ph
{

class GCuboid : public Geometry
{
public:
	GCuboid();
	explicit GCuboid(real sideLength);
	GCuboid(real sideLength, const math::Vector3R& offset);
	GCuboid(real xLen, real yLen, real zLen);
	GCuboid(const math::Vector3R& minVertex, const math::Vector3R& maxVertex);
	GCuboid(real xLen, real yLen, real zLen, const math::Vector3R& offset);

	void cook(
		CookedGeometry& out_geometry,
		const CookingContext& ctx,
		const GeometryCookConfig& config) const override;

	void genPrimitive(
		const PrimitiveBuildingMaterial& data,
		std::vector<std::unique_ptr<Primitive>>& out_primitives) const override;

	std::shared_ptr<Geometry> genTriangulated() const override;

private:
	math::Vector3R              m_size;
	math::Vector3R              m_offset;
	std::array<math::AABB2D, 6> m_faceUVs;

	static bool checkData(
		const PrimitiveBuildingMaterial& data, 
		const real xLen, const real yLen, const real zLen);

	static bool checkData(real xLen, real yLen, real zLen);

	static std::array<math::AABB2D, 6> genNormalizedFaceUVs();

public:
	// TODO: math::AABB to vec4
	/*PH_DEFINE_SDL_CLASS(TOwnerSdlClass<GCuboid>)
	{
		ClassType clazz("cuboid");
		clazz.setDescription(
			"A shape that is similar to cube but may contain rectangular faces. "
			"It is centered around origin.");
		clazz.setBase(&Geometry::getSdlClass());

		TSdlVector3<OwnerType> size("size", &OwnerType::m_size);
		size.description("x, y, z dimensions of the cuboid.");
		size.defaultTo({1, 1, 1});
		clazz.addField(size);

		TSdlVector3<OwnerType> offset("offset", &OwnerType::m_offset);
		offset.description("x, y, z offsets of the cuboid, origin is at the centroid.");
		offset.defaultTo({0, 0, 0});
		offset.optional();
		clazz.addField(offset);

		const std::array<math::AABB2D, 6> normalizedFaceUVs = genNormalizedFaceUVs();

		TSdlQuaternion<OwnerType> pxFaceUV("px-face-uv", &OwnerType::m_faceUVs[0]);
		pxFaceUV.description("UV coordinates of the +x face (+y as upward), in (min-u, min-v, max-u, max-v).");
		pxFaceUV.defaultTo(normalizedFaceUVs[0]);
		pxFaceUV.optional();
		clazz.addField(pxFaceUV);

		return clazz;
	}*/
};

}// end namespace ph
