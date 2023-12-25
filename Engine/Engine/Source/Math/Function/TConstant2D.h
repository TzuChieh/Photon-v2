#pragma once

#include "Math/Function/TMathFunction2D.h"

namespace ph::math
{

template<typename Value>
class TConstant2D : public TMathFunction2D<Value>
{
public:
	explicit TConstant2D(Value constantValue);

	Value evaluate(Value x, Value y) const override;

private:
	Value m_constantValue;
};

template<typename Value>
inline TConstant2D<Value>::TConstant2D(const Value constantValue)
	: m_constantValue(constantValue)
{}

template<typename Value>
inline Value TConstant2D<Value>::evaluate(const Value x, const Value y) const
{
	return m_constantValue;
}

}// end namespace ph::math
