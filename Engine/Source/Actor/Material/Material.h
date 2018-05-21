#pragma once

#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "FileIO/SDL/TCommandInterface.h"

namespace ph
{

class SurfaceBehavior;
class InputPacket;
class CookingContext;
class PrimitiveMetadata;

class Material : public TCommandInterface<Material>
{
public:
	Material();
	virtual ~Material() = 0;

	virtual void genBehaviors(CookingContext& context, PrimitiveMetadata& metadata) const = 0;

// command interface
public:
	explicit Material(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph