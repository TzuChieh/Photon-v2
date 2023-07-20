//#pragma once
//
//#include "Common/assertion.h"
//#include "Common/primitive_type.h"
//#include "Math/math_table.h"
//#include "Math/Random/shuffle.h"
//#include "Core/SampleGenerator/Detail/halton.h"
//
//#include <vector>
//#include <cstddef>
//#include <utility>
//#include <limits>
//
//namespace ph::halton::detail
//{
//
//class RadicalInversePermutations final
//{
//public:
//	using Digit = uint16;
//	static_assert(math::table::PRIME_TABLE[MAX_DIMENSIONS] <= std::numeric_limits<Digit>::max(),
//		"Digit may overflow");
//
//	explicit RadicalInversePermutations(std::size_t numDims);
//
//	auto getPermutationForDim(std::size_t dimIndex) const
//		-> const Digit*;
//
//private:
//	std::vector<Digit> m_permutations;
//
//	using Offset = decltype(math::table::PRIME_TABLE)::value_type;
//	static auto PERMUTATION_OFFSETS(std::size_t dimIndex) 
//		-> Offset;
//
//	static auto makePermutationOffsets()
//		-> std::vector<Offset>;
//};
//
//// In-header Implementations:
//
//inline RadicalInversePermutations::RadicalInversePermutations(const std::size_t numDims)
//{
//	PH_ASSERT_LE(numDims, MAX_DIMENSIONS);
//
//	// The offset after index <numDims - 1> is the total size of the buffer
//	m_permutations.resize(PERMUTATION_OFFSETS(numDims));
//
//	// Generate random digit permutation/mapping for each dimension
//	for(std::size_t di = 0; di < numDims; ++di)
//	{
//		const auto   base           = math::table::PRIME_TABLE[di];
//		Digit* const permutationPtr = &(m_permutations[PERMUTATION_OFFSETS(di)]);
//
//		for(Digit digit = 0; digit < base; ++digit)
//		{
//			permutationPtr[digit] = digit;
//		}
//		math::shuffle_durstenfeld(permutationPtr, permutationPtr + base);
//	}
//}
//
//inline auto RadicalInversePermutations::getPermutationForDim(const std::size_t dimIndex) const
//	-> const Digit*
//{
//	PH_ASSERT_LT(dimIndex, MAX_DIMENSIONS);
//
//	const auto offset = PERMUTATION_OFFSETS(dimIndex);
//	PH_ASSERT_LT(offset + math::table::PRIME_TABLE[dimIndex], m_permutations.size());
//
//	return &(m_permutations[offset]);
//}
//
//inline auto RadicalInversePermutations::PERMUTATION_OFFSETS(const std::size_t dimIndex)
//	-> Offset
//{
//	static std::vector<Offset> offsets(makePermutationOffsets());
//
//	PH_ASSERT_LT(dimIndex, offsets.size());
//	return offsets[dimIndex];
//}
//
//inline auto RadicalInversePermutations::makePermutationOffsets()
//	-> std::vector<Offset>
//{
//	// The offsets are actually a prefix sum of the prime table, but start
//	// with 0.
//
//	std::vector<Offset> offsets(MAX_DIMENSIONS + 1);
//	offsets[0] = math::table::PRIME_TABLE[0];
//	for(std::size_t i = 1; i < offsets.size(); ++i)
//	{
//		offsets[i] = offsets[i - 1] + math::table::PRIME_TABLE[i - 1];
//
//		PH_ASSERT_MSG(offsets[i] > offsets[i - 1], 
//			"offsets overflow");
//	}
//	return std::move(offsets);
//}
//
//}// end namespace ph::halton::detail
