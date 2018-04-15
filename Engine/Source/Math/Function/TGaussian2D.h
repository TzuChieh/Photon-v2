#pragma once

#include "Math/Function/TMathFunction2D.h"

#include <cmath>
#include <algorithm>

namespace ph
{

template<typename ValueType>
class TGaussian2D : public TMathFunction2D<ValueType>
{
public:
	TGaussian2D(ValueType sigmaX, ValueType sigmaY, ValueType amplitude);

	virtual ~TGaussian2D() override;

	virtual ValueType evaluate(ValueType x, ValueType y) const override;

	// Sink the gaussian with specified amount; effectively subtract then
	// clamp to zero.
	//
	inline void setSubmergeAmount(const ValueType amt)
	{
		m_submergeAmount = amt;
	}

private:
	ValueType m_xExpMultiplier;
	ValueType m_yExpMultiplier;
	ValueType m_amplitude;
	ValueType m_submergeAmount;
};

template<typename ValueType>
TGaussian2D<ValueType>::TGaussian2D(const ValueType sigmaX, 
                                    const ValueType sigmaY, 
                                    const ValueType amplitude) :
	TMathFunction2D<ValueType>(),
	m_xExpMultiplier(static_cast<ValueType>(-1) / (static_cast<ValueType>(2) * sigmaX * sigmaX)),
	m_yExpMultiplier(static_cast<ValueType>(-1) / (static_cast<ValueType>(2) * sigmaY * sigmaY)),
	m_amplitude(amplitude),
	m_submergeAmount(0)
{}

template<typename ValueType>
TGaussian2D<ValueType>::~TGaussian2D() = default;

template<typename ValueType>
ValueType TGaussian2D<ValueType>::evaluate(const ValueType x, const ValueType y) const
{
	const ValueType func = m_amplitude * std::exp(m_xExpMultiplier * x * x + m_yExpMultiplier * y * y);
	return std::max(func - m_submergeAmount, static_cast<ValueType>(0));
}

}// end namespace ph