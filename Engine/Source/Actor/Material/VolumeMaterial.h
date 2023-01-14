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

	virtual void genVolume(CookingContext& ctx, VolumeBehavior& behavior) const = 0;

	void genBehaviors(CookingContext& ctx, PrimitiveMetadata& metadata) const override;
	void setSidedness(ESidedness sidedness);

private:
	ESidedness m_sidedness;
};

}// end namespace ph
