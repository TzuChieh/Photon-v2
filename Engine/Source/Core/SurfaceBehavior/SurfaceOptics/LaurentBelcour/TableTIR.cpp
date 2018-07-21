#include "Core/SurfaceBehavior/SurfaceOptics/LaurentBelcour/TableTIR.h"
#include "Math/Math.h"

namespace ph
{

const Logger TableTIR::logger(LogSender("TIR Table"));

real TableTIR::sample(const real sinWt, const real alpha, const real relIor) const
{
	// float indices
	real fSinWt  = m_numSinWt  * (sinWt  - m_minSinWt)  / (m_maxSinWt  - m_minSinWt );
	real fAlpha  = m_numAlpha  * (alpha  - m_minAlpha)  / (m_maxAlpha  - m_minAlpha );
	real fRelIor = m_numRelIor * (relIor - m_minRelIor) / (m_maxRelIor - m_minRelIor);

	// TODO: simple cast-to-int is already adequate (no need to use floor())?

	// integer indices
	int iSinWt  = static_cast<int>(std::floor(fSinWt));
	int iAlpha  = static_cast<int>(std::floor(fAlpha));
	int iRelIor = static_cast<int>(std::floor(fRelIor));

	// make sure the indices stay in the limits
	iSinWt  = Math::clamp(iSinWt,  0, m_numSinWt  - 1);
	iAlpha  = Math::clamp(iAlpha,  0, m_numAlpha  - 1);
	iRelIor = Math::clamp(iRelIor, 0, m_numRelIor - 1);

	// index of the texel
	const int index = iRelIor + m_numRelIor * (iAlpha + m_numAlpha * iSinWt);

	return m_table[index];
}

}// end namespace ph