#include "Actor/Material/Utility/SurfaceLayerInfo.h"
#include "Core/Quantity/SpectralData.h"

namespace ph
{

SurfaceLayerInfo::SurfaceLayerInfo() :
	m_roughness(0.0_r), 
	m_iorN(1.0_r), 
	m_iorK(0.0_r),
	m_depth(0.0_r),
	m_g(1.0_r),
	m_sigmaA(0.0_r),
	m_sigmaS(0.0_r)
{}

}// end namespace ph
