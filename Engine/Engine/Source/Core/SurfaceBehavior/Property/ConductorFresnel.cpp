#include "Core/SurfaceBehavior/Property/ConductorFresnel.h"

namespace ph
{

ConductorFresnel::ConductorFresnel() :
	ConductorFresnel(1, math::Spectrum(1.5_r), math::Spectrum(0))
{}

ConductorFresnel::ConductorFresnel(
	const real            iorOuter,
	const math::Spectrum& iorInnerN,
	const math::Spectrum& iorInnerK) :

	FresnelEffect(),

	m_iorOuter (iorOuter), 
	m_iorInnerN(iorInnerN), 
	m_iorInnerK(iorInnerK)
{}

}// end namespace ph
