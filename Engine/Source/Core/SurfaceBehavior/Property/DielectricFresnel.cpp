#include "Core/SurfaceBehavior/Property/DielectricFresnel.h"

#include <cmath>
#include <algorithm>

namespace ph
{

DielectricFresnel::DielectricFresnel(const real iorOuter, const real iorInner) :
	m_iorOuter(iorOuter), m_iorInner(iorInner)
{

}

DielectricFresnel::~DielectricFresnel() = default;

bool DielectricFresnel::calcRefractDir(const Vector3R& I, const Vector3R& N,
                                             Vector3R* const out_refractDir) const
{
	const real IoN      = I.dot(N);
	const real signIoN  = IoN < 0.0_r ? -1.0_r : 1.0_r;
	const real iorRatio = signIoN < 0.0_r ? m_iorInner / m_iorOuter : m_iorOuter / m_iorInner;
	const real sqrValue = 1.0_r - iorRatio * iorRatio * (1.0_r - IoN * IoN);

	// TIR
	if(sqrValue <= 0.0_r)
	{
		return false;
	}

	const real Nfactor = iorRatio * IoN - signIoN * std::sqrt(sqrValue);
	const real Ifactor = -iorRatio;
	*out_refractDir = N.mul(Nfactor).addLocal(I.mul(Ifactor)).normalizeLocal();

	return true;
}

}// end namespace ph