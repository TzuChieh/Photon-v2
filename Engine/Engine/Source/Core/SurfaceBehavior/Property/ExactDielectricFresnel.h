#pragma once

#include "Core/SurfaceBehavior/Property/DielectricFresnel.h"

namespace ph
{

/*! @brief Dielectric-dielectric interface Fresnel effect.

The formulae used are exact ones, i.e., no approximation is made.
*/
class ExactDielectricFresnel : public DielectricFresnel
{
public:
	ExactDielectricFresnel(real iorOuter, real iorInner);

	math::Spectrum calcReflectance(real cosThetaIncident) const override;
};

}// end namespace ph
