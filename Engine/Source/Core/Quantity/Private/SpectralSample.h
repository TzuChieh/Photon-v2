#pragma once

#include "Common/primitive_type.h"
#include "Common/assertion.h"

namespace ph
{
	
class SpectralSample final
{
public:
	inline SpectralSample() :
		SpectralSample(0.0_r, 0.0_r)
	{}

	inline SpectralSample(const real lambdaNm, const real value) :
		m_lambdaNm(lambdaNm), m_value(value)
	{
		PH_ASSERT(m_lambdaNm >= 0.0_r);
	}

	inline real getLambdaNm() const
	{
		return m_lambdaNm;
	}

	inline real getValue() const
	{
		return m_value;
	}

	inline SpectralSample& operator = (const SpectralSample& other)
	{
		m_lambdaNm = other.m_lambdaNm;
		m_value    = other.m_value;
	}

private:
	real m_lambdaNm;
	real m_value;
};

}// end namespace ph