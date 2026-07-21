#pragma once

#include "Engine/Core/Texture/TTexture.h"
#include "Engine/Math/TVector4.h"

#include <Common/assertion.h>
#include <Common/compiler.h>
#include <Common/primitive_type.h>

#include <concepts>
#include <memory>
#include <type_traits>
#include <utility>

namespace ph
{

/*! @brief Configuration for an fBM noise texture.
Coordinate inputs are selected before uniform scaling as follows:

| `dimensions` | Noise-domain position |
|--------------|-----------------------|
| 1            | `coordinates.x`       |
| 2            | `coordinates.xy`      |
| 3            | `coordinates.xyz`     |
| 4            | `coordinates.xyzw`    |

`coordinatesMap` supplies all four components. When absent, coordinates are sample UVW followed
by a zero fourth component. `scaleMap`, when present, overrides `scale`; the selected value
multiplies every coordinate. Other mapped inputs similarly override their corresponding constants.
Detail is the number of additional octaves; its fractional part linearly blends the next octave.
Normalization shifts the noise midpoint from 0 to 0.5 and keeps output in `[0, 1]`.
*/
struct FbmNoiseTextureConfig final
{
	int dimensions = 3;
	bool normalize = true;

	std::shared_ptr<TTexture<math::Vector4R>> coordinatesMap;
	std::shared_ptr<TTexture<real>> scaleMap;
	std::shared_ptr<TTexture<real>> detailMap;
	std::shared_ptr<TTexture<real>> roughnessMap;
	std::shared_ptr<TTexture<real>> lacunarityMap;
	std::shared_ptr<TTexture<real>> distortionMap;

	real scale = 5.0_r;
	real detail = 2.0_r;
	real roughness = 0.5_r;
	real lacunarity = 2.0_r;
	real distortion = 0.0_r;
};

namespace fbm
{

struct ScalarOutput final
{
	real operator () (real noise) const
	{
		return noise;
	}
};

real evaluate_texture(const FbmNoiseTextureConfig& config, const SampleLocation& sampleLocation);

}// end namespace fbm

/*! @brief fBM noise texture with inline output conversion.
@tparam NoiseToOutput Callable that converts scalar noise to the texture output type.
*/
template<typename NoiseToOutput = fbm::ScalarOutput>
class TFbmNoiseTexture :
	public TTexture<std::remove_cvref_t<std::invoke_result_t<const NoiseToOutput&, real>>>
{
public:
	using OutputType = std::remove_cvref_t<std::invoke_result_t<const NoiseToOutput&, real>>;

	explicit TFbmNoiseTexture(FbmNoiseTextureConfig config)
		requires std::default_initializable<NoiseToOutput>
		: TFbmNoiseTexture(std::move(config), NoiseToOutput())
	{}

	TFbmNoiseTexture(FbmNoiseTextureConfig config, NoiseToOutput noiseToOutput)
		: m_config(std::move(config))
		, m_noiseToOutput(std::move(noiseToOutput))
	{}

	void sample(const SampleLocation& sampleLocation, OutputType* const out_value) const override
	{
		PH_ASSERT(out_value);

		*out_value = m_noiseToOutput(fbm::evaluate_texture(m_config, sampleLocation));
	}

private:
	FbmNoiseTextureConfig m_config;

	[[PH_NO_UNIQUE_ADDRESS]]
	NoiseToOutput m_noiseToOutput;
};

using FbmNoiseTexture = TFbmNoiseTexture<>;

}// end namespace ph
