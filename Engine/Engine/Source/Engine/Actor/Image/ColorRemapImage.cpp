#include "Engine/Actor/Image/ColorRemapImage.h"

#include "Engine/Actor/Basic/exceptions.h"
#include "Engine/Core/Texture/Function/unary_texture_operators.h"
#include "Engine/Core/Texture/TRgbLutTexture.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <utility>

namespace ph
{

namespace
{

/*! Uniformly sampled RGB lookup with explicit endpoint extrapolation slopes. */
class RgbLut final
{
public:
	RgbLut(
		const std::vector<math::Vector3R>& rgbValues,
		const math::Vector3R& lowerExtrapolationSlope,
		const math::Vector3R& upperExtrapolationSlope)
		: m_packedValues(rgbValues.size() + 2)
		, m_numIntervals(static_cast<real>(rgbValues.size()) - 1.0_r)
	{
		if(rgbValues.size() < 2)
		{
			throw CookException("Color remap image requires at least two RGB LUT entries.");
		}

		for(const auto& rgbValue : rgbValues)
		{
			if(!rgbValue.isFinite())
			{
				throw CookException("Color remap image LUT entries must be finite.");
			}
		}

		if(!lowerExtrapolationSlope.isFinite() || !upperExtrapolationSlope.isFinite())
		{
			throw CookException("Color remap image extrapolation slopes must be finite.");
		}

		std::copy(rgbValues.begin(), rgbValues.end(), m_packedValues.begin() + 1);
		for(std::size_t ci = 0; ci < 3; ++ci)
		{
			m_packedValues.front()[ci] = rgbValues.front()[ci] - lowerExtrapolationSlope[ci] / m_numIntervals;
			m_packedValues.back()[ci] = rgbValues.back()[ci] + upperExtrapolationSlope[ci] / m_numIntervals;
		}
	}

	math::TristimulusValues operator () (const math::TristimulusValues& rgb) const
	{
		return {
			evaluate(rgb[0], 0),
			evaluate(rgb[1], 1),
			evaluate(rgb[2], 2)};
	}

private:
	real evaluate(const real value, const std::size_t channel) const
	{
		const real tableX = value * m_numIntervals;

		std::size_t lowerIndex;
		real segmentT;
		if(tableX < 0.0_r)
		{
			lowerIndex = 0;
			segmentT = tableX + 1.0_r;
		}
		else if(tableX >= m_numIntervals)
		{
			lowerIndex = m_packedValues.size() - 2;
			segmentT = tableX - m_numIntervals;
		}
		else
		{
			lowerIndex = static_cast<std::size_t>(tableX) + 1;
			segmentT = tableX - static_cast<real>(lowerIndex - 1);
		}

		const real lowerValue = m_packedValues[lowerIndex][channel];
		const real upperValue = m_packedValues[lowerIndex + 1][channel];
		return lowerValue + (upperValue - lowerValue) * segmentT;
	}

	std::vector<math::Vector3R> m_packedValues;
	real m_numIntervals;
};

bool isIdentityRgbLut(
	const std::vector<math::Vector3R>& rgbValues,
	const math::Vector3R& lowerExtrapolationSlope,
	const math::Vector3R& upperExtrapolationSlope)
{
	if(rgbValues.size() < 2)
	{
		return false;
	}

	const real numIntervals = static_cast<real>(rgbValues.size() - 1);
	constexpr real tolerance = 1.0_r / 256.0_r / 1000.0_r;

	for(std::size_t i = 0; i < rgbValues.size(); ++i)
	{
		const real expected = static_cast<real>(i) / numIntervals;
		for(std::size_t ci = 0; ci < 3; ++ci)
		{
			// Negated comparison also rejects non-finite values
			if(!(std::abs(rgbValues[i][ci] - expected) <= tolerance))
			{
				return false;
			}
		}
	}

	for(std::size_t ci = 0; ci < 3; ++ci)
	{
		if(lowerExtrapolationSlope[ci] != 1.0_r || upperExtrapolationSlope[ci] != 1.0_r)
		{
			return false;
		}
	}

	return true;
}

}// end namespace

std::shared_ptr<TTexture<Image::NumericType>> ColorRemapImage::genNumericTexture(
	const CookingContext& ctx)
{
	if(!m_input)
	{
		throw CookException("Color remap image requires an input image.");
	}

	if(isIdentityRgbLut(m_rgbValues, m_lowerExtrapolationSlope, m_upperExtrapolationSlope))
	{
		return m_input->genNumericTexture(ctx);
	}

	using RgbTexture = TRgbLutTexture<
		math::EColorSpace::Linear_sRGB,
		math::TristimulusValues,
		RgbLut>;

	auto rgbTexture = std::make_shared<RgbTexture>(
		m_input->genColorTexture(ctx),
		RgbLut(m_rgbValues, m_lowerExtrapolationSlope, m_upperExtrapolationSlope),
		m_factor,
		m_factorMap ? m_factorMap->genRealTexture(ctx) : nullptr);

	auto rgbToNumeric = [](const math::TristimulusValues& rgb)
	{
		Image::NumericType numeric(0);
		for(std::size_t i = 0; i < rgb.size(); ++i)
		{
			numeric[i] = static_cast<Image::NumericElementType>(rgb[i]);
		}
		return numeric;
	};

	return std::make_shared<TUnaryTextureOperator<
		math::TristimulusValues,
		Image::NumericType,
		decltype(rgbToNumeric)>>(
			std::move(rgbTexture),
			std::move(rgbToNumeric));
}

std::shared_ptr<TTexture<math::Spectrum>> ColorRemapImage::genColorTexture(
	const CookingContext& ctx)
{
	if(!m_input)
	{
		throw CookException("Color remap image requires an input image.");
	}

	if(isIdentityRgbLut(m_rgbValues, m_lowerExtrapolationSlope, m_upperExtrapolationSlope))
	{
		return m_input->genColorTexture(ctx);
	}

	using RgbTexture = TRgbLutTexture<
		math::EColorSpace::Linear_sRGB,
		math::Spectrum,
		RgbLut>;

	return std::make_shared<RgbTexture>(
		m_input->genColorTexture(ctx),
		RgbLut(m_rgbValues, m_lowerExtrapolationSlope, m_upperExtrapolationSlope),
		m_factor,
		m_factorMap ? m_factorMap->genRealTexture(ctx) : nullptr);
}

ColorRemapImage& ColorRemapImage::setInput(std::shared_ptr<Image> image)
{
	m_input = std::move(image);
	return *this;
}

ColorRemapImage& ColorRemapImage::setRgbValues(std::vector<math::Vector3R> rgbValues)
{
	m_rgbValues = std::move(rgbValues);
	return *this;
}

ColorRemapImage& ColorRemapImage::setLowerExtrapolationSlope(const math::Vector3R& slope)
{
	m_lowerExtrapolationSlope = slope;
	return *this;
}

ColorRemapImage& ColorRemapImage::setUpperExtrapolationSlope(const math::Vector3R& slope)
{
	m_upperExtrapolationSlope = slope;
	return *this;
}

ColorRemapImage& ColorRemapImage::setFactor(const real factor)
{
	m_factor = factor;
	return *this;
}

ColorRemapImage& ColorRemapImage::setFactorMap(std::shared_ptr<Image> map)
{
	m_factorMap = std::move(map);
	return *this;
}

}// end namespace ph
