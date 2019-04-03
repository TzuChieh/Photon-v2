#include "Core/SurfaceBehavior/Property/ConductorFresnel.h"

namespace ph
{

ConductorFresnel::ConductorFresnel() :
	ConductorFresnel(1, SpectralStrength(1), SpectralStrength(0))
{}

ConductorFresnel::ConductorFresnel(
	const real              iorOuter,
	const SpectralStrength& iorInner,
	const SpectralStrength& iorInnerK) : 

	FresnelEffect(),

	m_iorOuter (iorOuter), 
	m_iorInner (iorInner), 
	m_iorInnerK(iorInnerK)
{}

}// end namespace ph