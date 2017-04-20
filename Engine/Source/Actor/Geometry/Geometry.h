#pragma once

#include "Core/Primitive/Primitive.h"
#include "FileIO/SDL/TCommandInterface.h"
#include "FileIO/SDL/ISdlResource.h"

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

class Geometry : public TCommandInterface<Geometry>, public ISdlResource
{
public:
	Geometry();
	virtual ~Geometry() = 0;

	virtual void discretize(const PrimitiveBuildingMaterial& data, 
	                        std::vector<std::unique_ptr<Primitive>>& out_primitives) const = 0;

	const TextureMapper* getTextureMapper() const;
	void setTextureMapper(const std::shared_ptr<TextureMapper>& textureMapper);

protected:
	std::shared_ptr<TextureMapper> m_textureMapper;


// command interface
public:
	Geometry(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static ExitStatus ciExecute(const std::shared_ptr<Geometry>& targetResource, const std::string& functionName, const InputPacket& packet);
};

}// end namespace ph