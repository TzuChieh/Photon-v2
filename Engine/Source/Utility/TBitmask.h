#pragma once

namespace ph
{

template<typename T>
class TBitmask final
{
public:
	// Initializes all bits to zero.
	//
	TBitmask();



private:
	T m_bits;
};

}// end namespace ph