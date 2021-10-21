#pragma once

#include "Core/Texture/TTexture.h"
#include "Common/primitive_type.h"
#include "Common/assertion.h"
#include "Core/Texture/SampleLocation.h"

#include <cstddef>
#include <array>
#include <memory>
#include <utility>

namespace ph
{

/*!
The output array can have size greater or lesser than the input array. One can also change/cast
the output type.
*/
template<typename InputT, std::size_t INPUT_N, typename OutputT = InputT, std::size_t OUTPUT_N = INPUT_N>
class TSwizzledTexture : public TTexture<std::array<OutputT, OUTPUT_N>>
{
public:
	using InputArray  = std::array<InputT, INPUT_N>;
	using OutputArray = std::array<OutputT, OUTPUT_N>;
	using InputTexRes = std::shared_ptr<TTexture<InputArray>>;

	/*!
	@param swizzleMap How each output slot maps to the input slot. Stores indices to the input slots.
	*/
	TSwizzledTexture(
		InputTexRes                 inputTexture,
		std::array<uint8, OUTPUT_N> swizzleMap) :

		TTexture<OutputArray>(),

		m_inputTexture(std::move(inputTexture)),
		m_swizzleMap  (std::move(swizzleMap))
	{}

	void sample(const SampleLocation& sampleLocation, OutputArray* const out_value) const override
	{
		PH_ASSERT(m_inputTexture);
		PH_ASSERT(out_value);

		InputArray inputArray;
		m_inputTexture->sample(sampleLocation, &inputArray);

		for(std::size_t oi = 0; oi < OUTPUT_N; ++oi)
		{
			const std::size_t inputIndex = m_swizzleMap[oi];
			PH_ASSERT_LT(inputIndex, INPUT_N);

			(*out_value)[oi] = static_cast<OutputT>(inputArray[inputIndex]);
		}
	}

private:
	InputTexRes                 m_inputTexture;
	std::array<uint8, OUTPUT_N> m_swizzleMap;
};

}// end namespace ph
