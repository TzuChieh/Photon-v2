#include "Core/SurfaceBehavior/SurfaceOptics/LaurentBelcour/TableTIR.h"
#include "Math/Math.h"

namespace ph
{

const Logger TableTIR::logger(LogSender("TIR Table"));

real TableTIR::sample(const real cosWi, const real alpha, const real relIor) const
{
	// float indices
	real fCosWi  = m_numCosWi  * (cosWi  - m_minCosWi ) / (m_maxCosWi  - m_minCosWi );
	real fAlpha  = m_numAlpha  * (alpha  - m_minAlpha ) / (m_maxAlpha  - m_minAlpha );
	real fRelIor = m_numRelIor * (relIor - m_minRelIor) / (m_maxRelIor - m_minRelIor);

	// TODO: simple cast-to-int is already adequate (no need to use floor())?

	// integer indices
	int iCosWi = static_cast<int>(std::floor(fCosWi));
	int iAlpha  = static_cast<int>(std::floor(fAlpha));
	int iRelIor = static_cast<int>(std::floor(fRelIor));

	// make sure the indices stay in the limits
	iCosWi  = Math::clamp(iCosWi,  0, m_numCosWi - 1);
	iAlpha  = Math::clamp(iAlpha,  0, m_numAlpha  - 1);
	iRelIor = Math::clamp(iRelIor, 0, m_numRelIor - 1);

	// index of the texel
	const int index = iRelIor + m_numRelIor * (iAlpha + m_numAlpha * iCosWi);

	return m_table[index];
}

}// end namespace ph