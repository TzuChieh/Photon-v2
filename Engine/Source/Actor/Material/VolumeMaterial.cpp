#include "Actor/Material/Volume/VAbsorptionOnly.h"
#include "Core/Intersectable/PrimitiveMetadata.h"

namespace ph
{

VolumeMaterial::VolumeMaterial() : 
	Material()
{}

VolumeMaterial::~VolumeMaterial() = default;

void VolumeMaterial::genBehaviors(
	CookingContext&    context, 
	PrimitiveMetadata& metadata) const
{

}

// command interface

VolumeMaterial::VolumeMaterial(const InputPacket& packet) : 
	Material(packet)
{}

}// end namespace ph