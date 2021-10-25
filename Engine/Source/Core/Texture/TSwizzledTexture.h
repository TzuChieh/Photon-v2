#pragma once

#include "Core/Texture/TTexture.h"
#include "Common/primitive_type.h"
#include "Common/assertion.h"
#include "Core/Texture/SampleLocation.h"
#include "Utility/traits.h"

#include <cstddef>
#include <array>
#include <memory>
#include <utility>
#include <type_traits>

namespace ph
{

/*! @brief Swizzle an array-like input type to the other array-like output type.
The output object can have size greater or lesser than the input object (talking about max index). 
*/
template<typename InputT, typename OutputT, std::size_t OUTPUT_N>
class TSwizzledTexture : public TTexture<OutputT>
{
	static_assert(CSubscriptable<InputT>,
		"Input type must be subscriptable.");

	static_assert(CSubscriptable<OutputT>,
		"Output type must be subscriptable.");

public:
	using InputTexRes   = std::shared_ptr<TTexture<InputT>>;
	using InputElement  = std::remove_cvref_t<decltype(std::declval<InputT>()[std::declval<std::size_t>()])>;
	using OutputElement = std::remove_cvref_t<decltype(std::declval<OutputT>()[std::declval<std::size_t>()])>;

	static_assert(std::is_convertible_v<InputElement, OutputElement>,
		"Array element of input type is not convertible to the array element of output type.");

	/*!
	@param swizzleMap How each output slot maps to the input slot. Stores indices to the input slots.
	*/
	TSwizzledTexture(
		InputTexRes                 inputTexture,
		std::array<uint8, OUTPUT_N> swizzleMap) :

		TTexture<OutputT>(),

		m_inputTexture(std::move(inputTexture)),
		m_swizzleMap  (std::move(swizzleMap))
	{}

	void sample(const SampleLocation& sampleLocation, OutputT* const out_value) const override
	{
		PH_ASSERT(m_inputTexture);
		PH_ASSERT(out_value);

		InputT inputValue;
		m_inputTexture->sample(sampleLocation, &inputValue);

		for(std::size_t oi = 0; oi < OUTPUT_N; ++oi)
		{
			const std::size_t mappedIndex = m_swizzleMap[oi];
			(*out_value)[oi] = static_cast<OutputElement>(inputValue[mappedIndex]);
		}
	}

private:
	InputTexRes                 m_inputTexture;
	std::array<uint8, OUTPUT_N> m_swizzleMap;
};

}// end namespace ph
