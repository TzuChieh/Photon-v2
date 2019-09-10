#include "Core/SurfaceBehavior/Property/ConductorFresnel.h"

namespace ph
{

ConductorFresnel::ConductorFresnel() :
	ConductorFresnel(1, SpectralStrength(1.5_r), SpectralStrength(0))
{}

ConductorFresnel::ConductorFresnel(
	const real              iorOuter,
	const SpectralStrength& iorInnerN,
	const SpectralStrength& iorInnerK) : 

	FresnelEffect(),

	m_iorOuter (iorOuter), 
	m_iorInnerN(iorInnerN), 
	m_iorInnerK(iorInnerK)
{}

}// end namespace ph
