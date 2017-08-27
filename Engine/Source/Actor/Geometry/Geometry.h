#pragma once

#include "Core/Intersectable/Primitive.h"
#include "FileIO/SDL/TCommandInterface.h"
#include "Math/Transform/StaticTransform.h"

#include <vector>
#include <memory>

namespace ph
{

class Ray;
class Intersection;
class Primitive;
class PrimitiveStorage;
class TextureMapper;
class InputPacket;
class PrimitiveBuildingMaterial;
class Transform;

// TODO: use highest precision to perform geometry related operations

class Geometry : public TCommandInterface<Geometry>
{
public:
	Geometry();
	virtual ~Geometry() = 0;

	virtual void genPrimitive(const PrimitiveBuildingMaterial& data, 
	                          std::vector<std::unique_ptr<Primitive>>& out_primitives) const = 0;
	virtual std::shared_ptr<Geometry> genTransformApplied(const StaticTransform& transform) const;

	const TextureMapper* getTextureMapper() const;
	void setTextureMapper(const std::shared_ptr<TextureMapper>& textureMapper);

protected:
	std::shared_ptr<TextureMapper> m_textureMapper;


// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static std::unique_ptr<Geometry> ciLoad(const InputPacket& packet);
	static ExitStatus ciExecute(const std::shared_ptr<Geometry>& targetResource, const std::string& functionName, const InputPacket& packet);
};

}// end namespace ph