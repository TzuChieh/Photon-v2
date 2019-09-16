#pragma once

#include "Actor/Material/Material.h"

namespace ph
{

class VolumeOptics;
class VolumeBehavior;

class VolumeMaterial : public Material
{
public:
	enum class ESidedness
	{
		INTERIOR,
		EXTERIOR
	};

	VolumeMaterial();

	virtual void genVolume(CookingContext& context, VolumeBehavior& behavior) const = 0;

	void genBehaviors(CookingContext& context, PrimitiveMetadata& metadata) const override;
	void setSidedness(ESidedness sidedness);

private:
	ESidedness m_sidedness;

// command interface
public:
	explicit VolumeMaterial(const InputPacket& packet);
};

}// end namespace ph
