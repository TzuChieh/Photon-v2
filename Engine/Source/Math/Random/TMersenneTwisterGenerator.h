#pragma once

#include "Math/Random/TUniformRandomBitGenerator.h"
#include "Common/primitive_type.h"
#include "Utility/utility.h"

#include <random>
#include <type_traits>

namespace ph::math
{

template<typename T>
class TMersenneTwisterGenerator final 
	: public TUniformRandomBitGenerator<TMersenneTwisterGenerator<T>, T>
{
public:
	PH_DEFINE_INLINE_RULE_OF_5_MEMBERS(TMersenneTwisterGenerator);

	explicit TMersenneTwisterGenerator(T sequenceIndex);

	T impl_generate();
	void impl_jumpAhead(uint64 distance);

private:
	static_assert(std::is_same_v<T, uint32> || std::is_same_v<T, uint64>);

	// There are two standard varaints: 32 & 64 bit versions, select one based on `T`
	using StdGeneratorType = std::conditional_t<std::is_same_v<T, uint32>, 
		std::mt19937, std::mt19937_64>;

	StdGeneratorType m_generator;
};

template<typename T>
inline T TMersenneTwisterGenerator<T>::impl_generate()
{
	return static_cast<T>(m_generator());
}

template<typename T>
inline void TMersenneTwisterGenerator<T>::impl_jumpAhead(const uint64 distance)
{
	m_generator.discard(distance);
}

}// end namespace ph::math
