#pragma once

namespace ph::math
{

template<typename ValueType>
class TMathFunction2D
{
public:
	virtual ~TMathFunction2D();

	virtual ValueType evaluate(ValueType x, ValueType y) const = 0;
};

// implementations:

template<typename ValueType>
TMathFunction2D<ValueType>::~TMathFunction2D() = default;

}// end namespace ph::math
