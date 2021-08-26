#pragma once

#include "DataIO/SDL/TSdlResourceBase.h"
#include "Core/Intersectable/Primitive.h"
#include "Math/Transform/StaticAffineTransform.h"
#include "DataIO/SDL/sdl_interface.h"

#include <vector>
#include <memory>

namespace ph
{

class Primitive;
class UvwMapper;
class PrimitiveBuildingMaterial;

// TODO: use highest precision to perform geometry related operations

class Geometry : public TSdlResourceBase<ETypeCategory::REF_GEOMETRY>
{
public:
	Geometry();

	virtual void genPrimitive(
		const PrimitiveBuildingMaterial&         data, 
		std::vector<std::unique_ptr<Primitive>>& out_primitives) const = 0;

	virtual std::shared_ptr<Geometry> genTransformed(
		const math::StaticAffineTransform& transform) const;

	virtual std::shared_ptr<Geometry> genTriangulated() const;

	const UvwMapper* getUvwMapper() const;
	void setUvwMapper(const std::shared_ptr<UvwMapper>& uvwMapper);

protected:
	std::shared_ptr<UvwMapper> m_uvwMapper;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<Geometry>)
	{
		ClassType clazz(sdl::category_to_string(CATEGORY));
		clazz.docName("Geometry");
		clazz.description("Defining the shape of scene elements.");
		return clazz;
	}
};

// In-header Implementations:

inline std::shared_ptr<Geometry> Geometry::genTransformed(
	const math::StaticAffineTransform& transform) const
{
	const auto& triangulated = genTriangulated();
	if(!triangulated)
	{
		return nullptr;
	}

	return triangulated->genTransformed(transform);
}

inline std::shared_ptr<Geometry> Geometry::genTriangulated() const
{
	return nullptr;
}

}// end namespace ph
