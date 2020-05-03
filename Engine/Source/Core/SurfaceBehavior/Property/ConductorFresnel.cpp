#include "Core/SurfaceBehavior/Property/ConductorFresnel.h"

namespace ph
{

ConductorFresnel::ConductorFresnel() :
	ConductorFresnel(1, Spectrum(1.5_r), Spectrum(0))
{}

ConductorFresnel::ConductorFresnel(
	const real      iorOuter,
	const Spectrum& iorInnerN,
	const Spectrum& iorInnerK) :

	FresnelEffect(),

	m_iorOuter (iorOuter), 
	m_iorInnerN(iorInnerN), 
	m_iorInnerK(iorInnerK)
{}

}// end namespace ph
