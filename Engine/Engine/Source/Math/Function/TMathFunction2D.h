#pragma once

namespace ph::math
{

template<typename Value>
class TMathFunction2D
{
public:
	virtual ~TMathFunction2D() = default;

	virtual Value evaluate(Value x, Value y) const = 0;
};

}// end namespace ph::math
