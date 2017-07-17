#pragma once

#include "Math/Function/TMathFunction2D.h"

namespace ph
{

template<typename ValueType>
class TConstant2D : public TMathFunction2D<ValueType>
{
public:
	TConstant2D(const ValueType constantValue) :
		TMathFunction2D<ValueType>(),
		m_constantValue(constantValue)
	{

	}

	virtual ~TConstant2D() = 0;

	virtual ValueType evaluate(const ValueType x, const ValueType y) const override
	{
		return m_constantValue;
	}

private:
	ValueType m_constantValue;
};

template<typename ValueType>
TConstant2D<ValueType>::~TConstant2D() = default;

}// end namespace ph