#include "Core/SurfaceBehavior/Utility/ConductorDielectricFresnel.h"

namespace ph
{

ConductorDielectricFresnel::ConductorDielectricFresnel(
	const real iorOuter,
	const SpectralStrength& iorInner,
	const SpectralStrength& iorInnerK) : 
	FresnelEffect(),
	m_iorOuter(iorOuter), m_iorInner(iorInner), m_iorInnerK(iorInnerK)
{

}

ConductorDielectricFresnel::~ConductorDielectricFresnel() = default;

}// end namespace ph