#pragma once

#include "Engine/Core/Texture/SampleLocation.h"
#include "Engine/Core/Texture/TTexture.h"
#include "Engine/Math/Color/Spectrum.h"
#include "Engine/Math/TArithmeticArray.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>

namespace ph
{

template<typename OutputType>
class TLuminanceTexture : public TTexture<OutputType>
{
public:
	explicit TLuminanceTexture(std::shared_ptr<TTexture<math::Spectrum>> inputTexture) :
		m_inputTexture(std::move(inputTexture))
	{
		PH_ASSERT(m_inputTexture);
	}

	void sample(const SampleLocation& sampleLocation, OutputType* const out_value) const override
	{
		PH_ASSERT(m_inputTexture);
		PH_ASSERT(out_value);

		math::Spectrum color;
		m_inputTexture->sample(sampleLocation, &color);
		setLuminance(color.relativeLuminance(sampleLocation.expectedUsage()), out_value);
	}

	const std::shared_ptr<TTexture<math::Spectrum>>& getInputTexture() const
	{
		return m_inputTexture;
	}

private:
	static void setLuminance(const real luminance, math::Spectrum* const out_value)
	{
		*out_value = math::Spectrum(luminance);
	}

	template<typename T, std::size_t N>
	static void setLuminance(const real luminance, math::TArithmeticArray<T, N>* const out_value)
	{
		static_assert(N > 0);

		out_value->set(0);
		(*out_value)[0] = static_cast<T>(luminance);
	}

	template<typename ArithmeticType>
	static void setLuminance(const real luminance, ArithmeticType* const out_value)
	{
		static_assert(std::is_arithmetic_v<ArithmeticType>,
			"Unsupported luminance texture output type.");

		*out_value = static_cast<ArithmeticType>(luminance);
	}

private:
	std::shared_ptr<TTexture<math::Spectrum>> m_inputTexture;
};

}// end namespace ph
