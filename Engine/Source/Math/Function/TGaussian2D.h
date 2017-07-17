#pragma once

#include "Math/Function/TMathFunction2D.h"

#include <cmath>

namespace ph
{

template<typename ValueType>
class TGaussian2D : public TMathFunction2D<ValueType>
{
public:
	TGaussian2D(const ValueType sigmaX, const ValueType sigmaY, const ValueType amplitude) : 
		TMathFunction2D<ValueType>(),
		m_xExpMultiplier(-1 / (2 * sigmaX * sigmaX)),
		m_yExpMultiplier(-1 / (2 * sigmaY * sigmaY)),
		m_amplitude(amplitude)
	{

	}

	virtual ~TGaussian2D() = 0;

	virtual ValueType evaluate(const ValueType x, const ValueType y) const override
	{
		return m_amplitude * std::exp(m_xExpMultiplier * x * x + m_yExpMultiplier * y * y);
	}

private:
	ValueType m_xExpMultiplier;
	ValueType m_yExpMultiplier;
	ValueType m_amplitude;
};

template<typename ValueType>
TGaussian2D<ValueType>::~TGaussian2D() = default;

}// end namespace ph