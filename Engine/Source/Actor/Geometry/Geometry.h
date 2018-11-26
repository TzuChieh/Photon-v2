#pragma once

#include "Core/Intersectable/Primitive.h"
#include "FileIO/SDL/TCommandInterface.h"
#include "Math/Transform/StaticAffineTransform.h"

#include <vector>
#include <memory>

namespace ph
{

class Primitive;
class PrimitiveStorage;
class UvwMapper;
class InputPacket;
class PrimitiveBuildingMaterial;
class Transform;
class PrimitiveMetadata;

// TODO: use highest precision to perform geometry related operations

class Geometry : public TCommandInterface<Geometry>
{
public:
	Geometry();
	virtual ~Geometry();

	virtual void genPrimitive(
		const PrimitiveBuildingMaterial&         data, 
		std::vector<std::unique_ptr<Primitive>>& out_primitives) const = 0;

	virtual std::shared_ptr<Geometry> genTransformApplied(const StaticAffineTransform& transform) const;

	const UvwMapper* getUvwMapper() const;
	void setUvwMapper(const std::shared_ptr<UvwMapper>& uvwMapper);

protected:
	std::shared_ptr<UvwMapper> m_uvwMapper;

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

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