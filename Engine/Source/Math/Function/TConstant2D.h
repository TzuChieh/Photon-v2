#pragma once

#include "Math/Function/TMathFunction2D.h"

namespace ph::math
{

template<typename ValueType>
class TConstant2D : public TMathFunction2D<ValueType>
{
public:
	TConstant2D(const ValueType constantValue);
	virtual ~TConstant2D() override;

	virtual ValueType evaluate(ValueType x, ValueType y) const override;

private:
	ValueType m_constantValue;
};

template<typename ValueType>
TConstant2D<ValueType>::TConstant2D(const ValueType constantValue) :
	TMathFunction2D<ValueType>(),
	m_constantValue(constantValue)
{

}

template<typename ValueType>
TConstant2D<ValueType>::~TConstant2D() = default;

template<typename ValueType>
ValueType TConstant2D<ValueType>::evaluate(const ValueType x, const ValueType y) const
{
	return m_constantValue;
}

}// end namespace ph::math
