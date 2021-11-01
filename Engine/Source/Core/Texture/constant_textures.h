#pragma once

#include "Core/Texture/TTexture.h"
#include "Common/assertion.h"
#include "Math/Color/color_enums.h"
#include "Math/Color/color_basics.h"
#include "Math/Color/Spectrum.h"
#include "Core/Texture/SampleLocation.h"

#include <utility>

namespace ph
{

/*! @brief Texture storing only one single constant.
This texture provides only a constant value. Input color usages are ignored.
For color-related constants, consider using @p TConstantTristimulusTexture or 
@p TConstantSpectralTexture; otherwise, it is recommended to bake color-related
side effects (e.g., color usages) into the constant.
*/
template<typename OutputType>
class TConstantTexture : public TTexture<OutputType>
{
public:
	explicit TConstantTexture(OutputType value) :
		TTexture<OutputType>(),
		m_value(std::move(value))
	{}

	void sample(const SampleLocation& sampleLocation, OutputType* const out_value) const override
	{
		PH_ASSERT(out_value);

		*out_value = m_value;
	}

private:
	OutputType m_value;
};

/*! @brief A constant color texture that can adapt values in tristimulus color space to spectrum.
*/
template<math::EColorSpace COLOR_SPACE = math::EColorSpace::Linear_sRGB>
class TConstantTristimulusTexture : public TTexture<math::Spectrum>
{
public:
	explicit TConstantTristimulusTexture(math::TristimulusValues value) :
		TTexture<math::Spectrum>(),
		m_value(std::move(value))
	{}

	void sample(const SampleLocation& sampleLocation, math::Spectrum* const out_value) const override
	{
		PH_ASSERT(out_value);

		out_value->setTransformed<COLOR_SPACE>(m_value, sampleLocation.expectedUsage());
	}

private:
	math::TristimulusValues m_value;
};

/*! @brief A constant color texture that can adapt spectral values to spectrum.
*/
template<math::EColorSpace COLOR_SPACE = math::EColorSpace::Spectral>
class TConstantSpectralTexture : public TTexture<math::Spectrum>
{
public:
	explicit TConstantSpectralTexture(math::SpectralSampleValues value) :
		TTexture<math::Spectrum>(),
		m_value(std::move(value))
	{}

	void sample(const SampleLocation& sampleLocation, math::Spectrum* const out_value) const override
	{
		PH_ASSERT(out_value);

		out_value->setSpectral<COLOR_SPACE>(m_value, sampleLocation.expectedUsage());
	}

private:
	math::SpectralSampleValues m_value;
};

}// end namespace ph
