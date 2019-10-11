#pragma once

namespace ph::math
{

template<typename ValueType>
class TMathFunction2D
{
public:
	virtual ~TMathFunction2D() = default;

	virtual ValueType evaluate(ValueType x, ValueType y) const = 0;
};

}// end namespace ph::math
