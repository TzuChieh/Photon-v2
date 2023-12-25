#pragma once

#include "Math/Function/TMathFunction2D.h"

#include <cmath>
#include <algorithm>

namespace ph::math
{

template<typename Value>
class TGaussian2D : public TMathFunction2D<Value>
{
public:
	TGaussian2D(Value sigmaX, Value sigmaY, Value amplitude);

	Value evaluate(Value x, Value y) const override;

	/*! @brief Sink the gaussian with specified amount.
	Effectively subtract then clamp to zero.
	*/
	void setSubmergeAmount(Value amt);

private:
	Value m_xExpMultiplier;
	Value m_yExpMultiplier;
	Value m_amplitude;
	Value m_submergeAmount;
};

template<typename Value>
inline TGaussian2D<Value>::TGaussian2D(
	const Value sigmaX,
	const Value sigmaY,
	const Value amplitude)

	: m_xExpMultiplier(static_cast<Value>(-1) / (static_cast<Value>(2) * sigmaX * sigmaX))
	, m_yExpMultiplier(static_cast<Value>(-1) / (static_cast<Value>(2) * sigmaY * sigmaY))
	, m_amplitude     (amplitude)
	, m_submergeAmount(0)
{}

template<typename Value>
inline Value TGaussian2D<Value>::evaluate(const Value x, const Value y) const
{
	const Value func = m_amplitude * std::exp(m_xExpMultiplier * x * x + m_yExpMultiplier * y * y);
	return std::max(func - m_submergeAmount, static_cast<Value>(0));
}

template<typename Value>
inline void TGaussian2D<Value>::setSubmergeAmount(const Value amt)
{
	m_submergeAmount = amt;
}

}// end namespace ph::math
