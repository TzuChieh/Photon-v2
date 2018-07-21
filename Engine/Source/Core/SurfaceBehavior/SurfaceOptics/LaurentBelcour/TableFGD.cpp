#include "Core/SurfaceBehavior/SurfaceOptics/LaurentBelcour/TableFGD.h"
#include "Math/Math.h"

#include <cmath>

namespace ph
{

const Logger TableFGD::logger(LogSender("FGD Table"));

real TableFGD::sample(const real sinWi, const real alpha, const real iorN, const real iorK) const
{
	// float indices
	real fSinWi = m_numSinWi * (sinWi - m_minSinWi) / (m_maxSinWi - m_minSinWi);
	real fAlpha = m_numAlpha * (alpha - m_minAlpha) / (m_maxAlpha - m_minAlpha);
	real fIorN  = m_numIorN  * (iorN  - m_minIorN ) / (m_maxIorN  - m_minIorN );
	real fIorK  = m_numIorK  * (iorK  - m_minIorK ) / (m_maxIorK  - m_minIorK );

	// integer indices
	int iSinWi = static_cast<int>(std::floor(fSinWi));
	int iAlpha = static_cast<int>(std::floor(fAlpha));
	int iIorN  = static_cast<int>(std::floor(fIorN));
	int iIorK  = static_cast<int>(std::floor(fIorK));

	// make sure the indices stay in the limits
	iSinWi = Math::clamp(iSinWi, 0, m_numSinWi - 1);
	iAlpha = Math::clamp(iAlpha, 0, m_numAlpha - 1);
	iIorN  = Math::clamp(iIorN,  0, m_numIorN  - 1);
	iIorK  = Math::clamp(iIorK,  0, m_numIorK  - 1);

	// index of the texel
	const int index      = iIorK + m_numIorK * (iIorN + m_numIorN * (iAlpha + m_numAlpha * iSinWi));
	//const int indices[4] = {iSinWi, iAlpha, iIorN, iIorK};

	return m_table[index];

	//// calculate interpolation weights
	//real weights[4] = {fSinWi - iSinWi, fAlpha - iAlpha, fIorN - iIorN, fIorK - iIorK};
	//for(int i = 0; i < 4; ++i)
	//{
	//	weights[i] = Math::clamp(weights[i], 0.0_r, 1.0_r);
	//}

	//real result = 0.0_r;

	//// For every possible combinaison of index shift per dimension,
	//// fetch the value in memory and do linear interpolation.
	//// We fetch using shift of 0 and 1.
	////
	////     v(i+di, j+di, k+dk, l+dl),  where dk in [0,1]
	////
	//const unsigned int D = pow(2, 4);
	//for(unsigned int d = 0; d<D; ++d) {

	//	Float alpha = 1.0; // Global alpha
	//	int   cid_s = 0;   // Id shift

	//					   // Evaluate the weight of the sample d which correspond to
	//					   // one for the shifted configuration:
	//					   // The weight is the product of the weights per dimension.
	//					   //
	//	for(int i = 0; i<4; ++i) {
	//		bool  bitset = ((1 << i) & d);
	//		Float calpha = (bitset) ? alphas[i] : 1.0 - alphas[i];

	//		// Correct the shift to none if we go out of the grid
	//		if(indices[i] + 1 >= sizes[i]) {
	//			bitset = false;
	//		}

	//		alpha *= calpha;
	//		cid_s = cid_s * sizes[i] + ((bitset) ? 1 : 0);
	//	}

	//	v += alpha * buff[index + cid_s];
	//}
	//return v;
}

}// end namespace ph