#include "Core/SurfaceBehavior/Property/ConductorDielectricFresnel.h"

namespace ph
{

ConductorDielectricFresnel::ConductorDielectricFresnel() : 
	ConductorDielectricFresnel(1, SpectralStrength(1), SpectralStrength(0))
{}

ConductorDielectricFresnel::ConductorDielectricFresnel(
	const real iorOuter,
	const SpectralStrength& iorInner,
	const SpectralStrength& iorInnerK) : 
	FresnelEffect(),
	m_iorOuter(iorOuter), m_iorInner(iorInner), m_iorInnerK(iorInnerK)
{}

ConductorDielectricFresnel::~ConductorDielectricFresnel() = default;

}// end namespace ph