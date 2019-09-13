#pragma once

#include "Core/Intersectable/Primitive.h"
#include "FileIO/SDL/TCommandInterface.h"
#include "Math/Transform/StaticAffineTransform.h"

#include <vector>
#include <memory>

namespace ph
{

class Primitive;
class UvwMapper;
class InputPacket;
class PrimitiveBuildingMaterial;

// TODO: use highest precision to perform geometry related operations

class Geometry : public TCommandInterface<Geometry>
{
public:
	Geometry();
	virtual ~Geometry() = default;

	virtual void genPrimitive(
		const PrimitiveBuildingMaterial&         data, 
		std::vector<std::unique_ptr<Primitive>>& out_primitives) const = 0;

	virtual std::shared_ptr<Geometry> genTransformed(
		const StaticAffineTransform& transform) const;

	virtual std::shared_ptr<Geometry> genTriangulated() const;

	const UvwMapper* getUvwMapper() const;
	void setUvwMapper(const std::shared_ptr<UvwMapper>& uvwMapper);

protected:
	std::shared_ptr<UvwMapper> m_uvwMapper;

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

// In-header Implementations:

inline std::shared_ptr<Geometry> Geometry::genTransformed(
	const StaticAffineTransform& transform) const
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

/*
	<SDL_interface>

	<category>  geometry </category>
	<type_name> geometry </type_name>

	<name> Geometry </name>
	<description>
		Defining the shape of scene elements.
	</description>

	</SDL_interface>
*/
