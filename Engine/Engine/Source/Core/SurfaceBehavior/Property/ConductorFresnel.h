#pragma once

#include "Core/SurfaceBehavior/Property/FresnelEffect.h"

#include <Common/primitive_type.h>

namespace ph
{

/*! @brief Conductor-dielectric interface Fresnel effect.

Fresnel effect where the inner side is conductor and the outer side is 
dielectric. We do not explicitly state conductor-dielectric in the type name
since light energy does not penetrate conductors significantly, so the 
situation where a dedicated type for conductor-conductor interface that 
conflicts with the current name is highly unlikely.
*/
class ConductorFresnel : public FresnelEffect
{
public:
	ConductorFresnel() = default;

	math::Spectrum calcReflectance(real cosThetaIncident) const override = 0;
};

}// end namespace ph
