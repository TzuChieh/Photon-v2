#include "Engine/Core/Texture/TFbmNoiseTexture.h"

#include "Engine/Core/Texture/SampleLocation.h"
#include "Engine/Math/hash.h"
#include "Engine/Math/noise.h"
#include "Engine/Math/TVector2.h"
#include "Engine/Math/TVector3.h"
#include "Engine/Math/TVector4.h"

#include <Common/assertion.h>

#include <algorithm>
#include <cmath>

namespace ph
{

/*
- Reference for `safe_coordinate` and `scaled_perlin`: Blender Cycles
  `src/kernel/svm/noise.h`, functions `noise_scale1`, `noise_scale2`, `noise_scale3`,
  `noise_scale4`, `snoise_1d`, `snoise_2d`, `snoise_3d`, and `snoise_4d`:
  https://github.com/blender/cycles/blob/97dbe6f57cdf4ede2d2b75ebdda507c8712edb7a/src/kernel/svm/noise.h#L665-L754
- Reference for `noise_fbm`: Blender Cycles `src/kernel/svm/fractal_noise.h`, overloads of
  `noise_fbm`:
  https://github.com/blender/cycles/blob/97dbe6f57cdf4ede2d2b75ebdda507c8712edb7a/src/kernel/svm/fractal_noise.h#L13-L110
- Reference for `random_float_offset`, `random_float2_offset`, `random_float3_offset`, and
  `random_float4_offset`: Blender Cycles `src/kernel/svm/noisetex.h`, same functions:
  https://github.com/blender/cycles/blob/97dbe6f57cdf4ede2d2b75ebdda507c8712edb7a/src/kernel/svm/noisetex.h#L21-L44
- Reference for `distort` and `evaluate`: Blender Cycles `src/kernel/svm/noisetex.h`,
  functions `noise_select`, `noise_texture_1d`, `noise_texture_2d`, `noise_texture_3d`, and
  `noise_texture_4d`:
  https://github.com/blender/cycles/blob/97dbe6f57cdf4ede2d2b75ebdda507c8712edb7a/src/kernel/svm/noisetex.h#L47-L244
- Reference for `sanitize_params`, `sample_input`, and `evaluate_scalar`: Blender Cycles
  `src/kernel/svm/noisetex.h`, function `svm_node_tex_noise`:
  https://github.com/blender/cycles/blob/97dbe6f57cdf4ede2d2b75ebdda507c8712edb7a/src/kernel/svm/noisetex.h#L246-L334
*/
namespace fbm
{

struct SampledFbmParams final
{
	math::Vector4F coordinates;
	float32 detail;
	float32 roughness;
	float32 lacunarity;
	float32 distortion;
	bool normalize;
};

inline float32 safe_coordinate(float32 coordinate)
{
	const float32 precisionCorrection = std::abs(coordinate) >= 1000000.0f ? 0.5f : 0.0f;
	// Repeat every 100000 unit to avoid precision issues;
	// this will cause discontinuities, but should be large enough to get away with it
	return std::fmod(coordinate, 100000.0f) + precisionCorrection;
}

// Empirical dimension scales make raw Perlin outputs approximately comparable in [-1, 1]
inline float32 scaled_perlin(float32 position)
{
	return 0.25f * math::noise::perlin(safe_coordinate(position));
}

inline float32 scaled_perlin(const math::Vector2F& position)
{
	return 0.6616f * math::noise::perlin(math::Vector2F(
		safe_coordinate(position.x()),
		safe_coordinate(position.y())));
}

inline float32 scaled_perlin(const math::Vector3F& position)
{
	return 0.9820f * math::noise::perlin(math::Vector3F(
		safe_coordinate(position.x()),
		safe_coordinate(position.y()),
		safe_coordinate(position.z())));
}

inline float32 scaled_perlin(const math::Vector4F& position)
{
	return 0.8344f * math::noise::perlin(math::Vector4F(
		safe_coordinate(position.x()),
		safe_coordinate(position.y()),
		safe_coordinate(position.z()),
		safe_coordinate(position.w())));
}

template<typename Position>
inline float32 noise_fbm(
	const Position& position,
	const float32 detail,
	const float32 roughness,
	const float32 lacunarity,
	const bool normalize)
{
	float32 frequency = 1.0f;
	float32 amplitude = 1.0f;
	float32 maxAmplitude = 0.0f;
	float32 sum = 0.0f;

	for(int octave = 0; octave <= static_cast<int>(detail); ++octave)
	{
		sum += scaled_perlin(frequency * position) * amplitude;
		maxAmplitude += amplitude;
		amplitude *= roughness;
		frequency *= lacunarity;
	}

	const float32 remainder = detail - std::floor(detail);
	if(remainder != 0.0f)
	{
		const float32 extendedSum = sum + scaled_perlin(frequency * position) * amplitude;
		if(normalize)
		{
			const float32 normalized = 0.5f * sum / maxAmplitude + 0.5f;
			const float32 extendedNormalized = 0.5f * extendedSum / (maxAmplitude + amplitude) + 0.5f;
			return normalized + remainder * (extendedNormalized - normalized);
		}
		return sum + remainder * (extendedSum - sum);
	}
	return normalize ? 0.5f * sum / maxAmplitude + 0.5f : sum;
}

inline float32 random_float_offset(const float32 seed)
{
	return 100.0f + math::jenkins_lookup3_to_unit(seed) * 100.0f;
}

inline math::Vector2F random_float2_offset(const float32 seed)
{
	return math::Vector2F(
		100.0f + math::jenkins_lookup3_to_unit(math::Vector2F(seed, 0.0f).toView()) * 100.0f,
		100.0f + math::jenkins_lookup3_to_unit(math::Vector2F(seed, 1.0f).toView()) * 100.0f);
}

inline math::Vector3F random_float3_offset(const float32 seed)
{
	return math::Vector3F(
		100.0f + math::jenkins_lookup3_to_unit(math::Vector2F(seed, 0.0f).toView()) * 100.0f,
		100.0f + math::jenkins_lookup3_to_unit(math::Vector2F(seed, 1.0f).toView()) * 100.0f,
		100.0f + math::jenkins_lookup3_to_unit(math::Vector2F(seed, 2.0f).toView()) * 100.0f);
}

inline math::Vector4F random_float4_offset(const float32 seed)
{
	return math::Vector4F(
		100.0f + math::jenkins_lookup3_to_unit(math::Vector2F(seed, 0.0f).toView()) * 100.0f,
		100.0f + math::jenkins_lookup3_to_unit(math::Vector2F(seed, 1.0f).toView()) * 100.0f,
		100.0f + math::jenkins_lookup3_to_unit(math::Vector2F(seed, 2.0f).toView()) * 100.0f,
		100.0f + math::jenkins_lookup3_to_unit(math::Vector2F(seed, 3.0f).toView()) * 100.0f);
}

inline SampledFbmParams sanitize_params(SampledFbmParams params)
{
	params.detail = std::clamp(params.detail, 0.0f, 15.0f);
	params.roughness = std::max(params.roughness, 0.0f);
	return params;
}

inline float32 distort(float32 position, const float32 distortion)
{
	return position + scaled_perlin(position + random_float_offset(0.0f)) * distortion;
}

inline math::Vector2F distort(const math::Vector2F& position, const float32 distortion)
{
	return position + math::Vector2F(
		scaled_perlin(position + random_float2_offset(0.0f)) * distortion,
		scaled_perlin(position + random_float2_offset(1.0f)) * distortion);
}

inline math::Vector3F distort(const math::Vector3F& position, const float32 distortion)
{
	return position + math::Vector3F(
		scaled_perlin(position + random_float3_offset(0.0f)) * distortion,
		scaled_perlin(position + random_float3_offset(1.0f)) * distortion,
		scaled_perlin(position + random_float3_offset(2.0f)) * distortion);
}

inline math::Vector4F distort(const math::Vector4F& position, const float32 distortion)
{
	return position + math::Vector4F(
		scaled_perlin(position + random_float4_offset(0.0f)) * distortion,
		scaled_perlin(position + random_float4_offset(1.0f)) * distortion,
		scaled_perlin(position + random_float4_offset(2.0f)) * distortion,
		scaled_perlin(position + random_float4_offset(3.0f)) * distortion);
}

template<typename Position>
inline float32 evaluate(const Position& position, const SampledFbmParams& params)
{
	const Position distortedPosition = params.distortion == 0.0f
		? position
		: distort(position, params.distortion);
	return noise_fbm(
		distortedPosition,
		params.detail,
		params.roughness,
		params.lacunarity,
		params.normalize);
}

template<typename T>
inline T sample_or(
	const std::shared_ptr<TTexture<T>>& texture,
	const SampleLocation& sampleLocation,
	const T& fallback)
{
	if(!texture)
	{
		return fallback;
	}

	T value;
	texture->sample(sampleLocation, &value);
	return value;
}

inline SampledFbmParams sample_input(
	const FbmNoiseTextureConfig& config,
	const SampleLocation& sampleLocation)
{
	PH_ASSERT_IN_RANGE_INCLUSIVE(config.dimensions, 1, 4);

	const math::Vector3R uvw = sampleLocation.uvw();
	const math::Vector4R fallbackCoordinates(uvw.x(), uvw.y(), uvw.z(), 0.0_r);
	const math::Vector4R coordinates = sample_or(config.coordinatesMap, sampleLocation, fallbackCoordinates);
	const real scale = sample_or(config.scaleMap, sampleLocation, config.scale);

	return SampledFbmParams{
		.coordinates = math::Vector4F(coordinates) * static_cast<float32>(scale),
		.detail = static_cast<float32>(sample_or(config.detailMap, sampleLocation, config.detail)),
		.roughness = static_cast<float32>(sample_or(config.roughnessMap, sampleLocation, config.roughness)),
		.lacunarity = static_cast<float32>(sample_or(config.lacunarityMap, sampleLocation, config.lacunarity)),
		.distortion = static_cast<float32>(sample_or(config.distortionMap, sampleLocation, config.distortion)),
		.normalize = config.normalize};
}

inline float32 evaluate_scalar(SampledFbmParams params, const int dimensions)
{
	params = sanitize_params(params);

	switch(dimensions)
	{
	case 1:
		return evaluate(
			params.coordinates.x(),
			params);
	case 2:
		return evaluate(
			math::Vector2F(params.coordinates.x(), params.coordinates.y()),
			params);
	case 3:
		return evaluate(
			math::Vector3F(params.coordinates.x(), params.coordinates.y(), params.coordinates.z()),
			params);
	case 4:
		return evaluate(
			params.coordinates,
			params);
	default:
		PH_ASSERT_UNREACHABLE_SECTION();
		return 0.0f;
	}
}

real evaluate_texture(const FbmNoiseTextureConfig& config, const SampleLocation& sampleLocation)
{
	return static_cast<real>(evaluate_scalar(sample_input(config, sampleLocation), config.dimensions));
}

}// end namespace fbm

}// end namespace ph
