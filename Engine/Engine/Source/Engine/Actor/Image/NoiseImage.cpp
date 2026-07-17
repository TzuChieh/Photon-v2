#include "Engine/Actor/Image/NoiseImage.h"

#include "Engine/Actor/Basic/exceptions.h"
#include "Engine/Core/Texture/TFbmNoiseTexture.h"
#include "Engine/Core/Texture/Function/unary_texture_operators.h"
#include "Engine/Math/Color/Spectrum.h"
#include "Engine/Math/TVector2.h"
#include "Engine/Math/TVector3.h"
#include "Engine/Math/TVector4.h"

#include <utility>

namespace ph
{

namespace
{

template<typename NoiseToOutput>
auto make_noise_texture(
	const ENoiseAlgorithm algorithm,
	FbmNoiseTextureConfig config,
	NoiseToOutput noiseToOutput)
{
	switch(algorithm)
	{
	case ENoiseAlgorithm::PerlinFbm:
		return std::make_shared<TFbmNoiseTexture<NoiseToOutput>>(
			std::move(config), std::move(noiseToOutput));
	default:
		throw CookException("unsupported noise algorithm");
	}
}

}// end anonymous namespace

std::shared_ptr<TTexture<Image::NumericType>> NoiseImage::genNumericTexture(const CookingContext& ctx)
{
	auto noiseToNumeric = [](const real noise)
	{
		Image::NumericType numeric(0);
		numeric[0] = noise;
		return numeric;
	};

	return make_noise_texture(
		m_algorithm, makePerlinFbmConfig(ctx), std::move(noiseToNumeric));
}

std::shared_ptr<TTexture<math::Spectrum>> NoiseImage::genColorTexture(const CookingContext& ctx)
{
	auto noiseToColor = [](const real noise)
	{
		return math::Spectrum(static_cast<math::ColorValue>(noise));
	};

	return make_noise_texture(
		m_algorithm, makePerlinFbmConfig(ctx), std::move(noiseToColor));
}

std::shared_ptr<TTexture<real>> NoiseImage::genRealTexture(const CookingContext& ctx)
{
	return make_noise_texture(m_algorithm, makePerlinFbmConfig(ctx), fbm::ScalarOutput());
}

std::shared_ptr<TTexture<math::Vector2R>> NoiseImage::genVector2RTexture(const CookingContext& ctx)
{
	auto noiseToVector = [](const real noise)
	{
		return math::Vector2R(noise, 0.0_r);
	};

	return make_noise_texture(
		m_algorithm, makePerlinFbmConfig(ctx), std::move(noiseToVector));
}

std::shared_ptr<TTexture<math::Vector3R>> NoiseImage::genVector3RTexture(const CookingContext& ctx)
{
	auto noiseToVector = [](const real noise)
	{
		return math::Vector3R(noise, 0.0_r, 0.0_r);
	};

	return make_noise_texture(
		m_algorithm, makePerlinFbmConfig(ctx), std::move(noiseToVector));
}

std::shared_ptr<TTexture<math::Vector4R>> NoiseImage::genVector4RTexture(const CookingContext& ctx)
{
	auto noiseToVector = [](const real noise)
	{
		return math::Vector4R(noise, 0.0_r, 0.0_r, 0.0_r);
	};

	return make_noise_texture(
		m_algorithm, makePerlinFbmConfig(ctx), std::move(noiseToVector));
}

FbmNoiseTextureConfig NoiseImage::makePerlinFbmConfig(const CookingContext& ctx) const
{
	FbmNoiseTextureConfig config;
	config.dimensions = static_cast<int>(m_dimensions);
	config.normalize = m_normalize;
	config.coordinatesMap = m_coordinates
		? m_coordinates->genVector4RTexture(ctx)
		: nullptr;
	config.scaleMap = m_frequencyMap ? m_frequencyMap->genRealTexture(ctx) : nullptr;
	auto numLayersToDetail = [](const real numLayers) { return numLayers - 1.0_r; };
	config.detailMap = m_numLayersMap
		? std::make_shared<TUnaryTextureOperator<real, real, decltype(numLayersToDetail)>>(
			m_numLayersMap->genRealTexture(ctx), std::move(numLayersToDetail))
		: nullptr;
	config.roughnessMap = m_amplitudeRatioMap
		? m_amplitudeRatioMap->genRealTexture(ctx)
		: nullptr;
	config.lacunarityMap = m_frequencyRatioMap
		? m_frequencyRatioMap->genRealTexture(ctx)
		: nullptr;
	config.distortionMap = m_warpMap
		? m_warpMap->genRealTexture(ctx)
		: nullptr;
	config.scale = m_frequency;
	config.detail = m_numLayers - 1.0_r;
	config.roughness = m_amplitudeRatio;
	config.lacunarity = m_frequencyRatio;
	config.distortion = m_warp;
	return config;
}

NoiseImage& NoiseImage::setAlgorithm(const ENoiseAlgorithm algorithm)
{
	m_algorithm = algorithm;
	return *this;
}

NoiseImage& NoiseImage::setDimensions(const ENoiseDimension dimensions)
{
	m_dimensions = dimensions;
	return *this;
}

NoiseImage& NoiseImage::setNormalize(const bool normalize)
{
	m_normalize = normalize;
	return *this;
}

NoiseImage& NoiseImage::setCoordinates(std::shared_ptr<Image> coordinates)
{
	m_coordinates = std::move(coordinates);
	return *this;
}

NoiseImage& NoiseImage::setFrequency(const real frequency)
{
	m_frequency = frequency;
	return *this;
}

NoiseImage& NoiseImage::setFrequencyMap(std::shared_ptr<Image> map)
{
	m_frequencyMap = std::move(map);
	return *this;
}

NoiseImage& NoiseImage::setNumLayers(const real numLayers)
{
	m_numLayers = numLayers;
	return *this;
}

NoiseImage& NoiseImage::setNumLayersMap(std::shared_ptr<Image> map)
{
	m_numLayersMap = std::move(map);
	return *this;
}

NoiseImage& NoiseImage::setAmplitudeRatio(const real ratio)
{
	m_amplitudeRatio = ratio;
	return *this;
}

NoiseImage& NoiseImage::setAmplitudeRatioMap(std::shared_ptr<Image> map)
{
	m_amplitudeRatioMap = std::move(map);
	return *this;
}

NoiseImage& NoiseImage::setFrequencyRatio(const real ratio)
{
	m_frequencyRatio = ratio;
	return *this;
}

NoiseImage& NoiseImage::setFrequencyRatioMap(std::shared_ptr<Image> map)
{
	m_frequencyRatioMap = std::move(map);
	return *this;
}

NoiseImage& NoiseImage::setWarp(const real amount)
{
	m_warp = amount;
	return *this;
}

NoiseImage& NoiseImage::setWarpMap(std::shared_ptr<Image> map)
{
	m_warpMap = std::move(map);
	return *this;
}

}// end namespace ph
