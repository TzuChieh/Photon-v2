#include "Core/SurfaceBehavior/SurfaceOptics/LaurentBelcour/TableFGD.h"
#include "Math/Math.h"

#include <cmath>

namespace ph
{

const Logger TableFGD::logger(LogSender("FGD Table"));

real TableFGD::sample(const real sinWi, const real alpha, const real iorN, const real iorK) const
{
	// parametric coordinates in [0, 1]
	real pSinWi = m_numSinWi * (sinWi - m_minSinWi) / (m_maxSinWi - m_minSinWi);
	real pAlpha = m_numAlpha * (alpha - m_minAlpha) / (m_maxAlpha - m_minAlpha);
	real pIorN  = m_numIorN  * (iorN  - m_minIorN ) / (m_maxIorN  - m_minIorN );
	real pIorK  = m_numIorK  * (iorK  - m_minIorK ) / (m_maxIorK  - m_minIorK );

	// integer indices
	int iSinWi = static_cast<int>(std::floor(pSinWi));
	int iAlpha = static_cast<int>(std::floor(pAlpha));
	int iIorN  = static_cast<int>(std::floor(pIorN));
	int iIorK  = static_cast<int>(std::floor(pIorK));

	// make sure the indices stay in the limits
	iSinWi = Math::clamp(iSinWi, 0, m_numSinWi - 1);
	iAlpha = Math::clamp(iAlpha, 0, m_numAlpha - 1);
	iIorN  = Math::clamp(iIorN,  0, m_numIorN  - 1);
	iIorK  = Math::clamp(iIorK,  0, m_numIorK  - 1);

	//*
	// Clamp the interpolation weights
	Float alphas[4] = { ta - ti, aa - ai, na - ni, ka - ki };
	for(int i = 0; i<4; ++i) {
		alphas[i] = _clamp<Float>(alphas[i], 0.0f, 1.0f);
	}

	// Index of the middle point
	const int indices[4] = { ti, ai, ni, ki };
	const int index = ki + Nk * (ni + Nn * (ai + Na * ti));

	// Result vector
	Float v = 0.0f;

	// For every possible combinaison of index shift per dimension,
	// fetch the value in memory and do linear interpolation.
	// We fetch using shift of 0 and 1.
	//
	//     v(i+di, j+di, k+dk, l+dl),  where dk in [0,1]
	//
	const unsigned int D = pow(2, 4);
	for(unsigned int d = 0; d<D; ++d) {

		Float alpha = 1.0; // Global alpha
		int   cid_s = 0;   // Id shift

						   // Evaluate the weight of the sample d which correspond to
						   // one for the shifted configuration:
						   // The weight is the product of the weights per dimension.
						   //
		for(int i = 0; i<4; ++i) {
			bool  bitset = ((1 << i) & d);
			Float calpha = (bitset) ? alphas[i] : 1.0 - alphas[i];

			// Correct the shift to none if we go out of the grid
			if(indices[i] + 1 >= sizes[i]) {
				bitset = false;
			}

			alpha *= calpha;
			cid_s = cid_s * sizes[i] + ((bitset) ? 1 : 0);
		}

		v += alpha * buff[index + cid_s];
	}
	return v;
}

}// end namespace ph