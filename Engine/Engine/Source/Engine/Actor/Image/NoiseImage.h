#pragma once

#include "Engine/Actor/Image/Image.h"
#include "Engine/Actor/Image/sdl_image_enums.h"
#include "Engine/SDL/sdl_interface.h"

#include <Common/primitive_type.h>

#include <memory>

namespace ph
{

struct FbmNoiseTextureConfig;

class NoiseImage : public Image
{
public:
	std::shared_ptr<TTexture<Image::NumericType>> genNumericTexture(
		const CookingContext& ctx) override;

	std::shared_ptr<TTexture<math::Spectrum>> genColorTexture(
		const CookingContext& ctx) override;

	std::shared_ptr<TTexture<real>> genRealTexture(
		const CookingContext& ctx) override;

	std::shared_ptr<TTexture<math::Vector2R>> genVector2RTexture(
		const CookingContext& ctx) override;

	std::shared_ptr<TTexture<math::Vector3R>> genVector3RTexture(
		const CookingContext& ctx) override;

	std::shared_ptr<TTexture<math::Vector4R>> genVector4RTexture(
		const CookingContext& ctx) override;

	NoiseImage& setAlgorithm(ENoiseAlgorithm algorithm);
	NoiseImage& setDimensions(ENoiseDimension dimensions);
	NoiseImage& setNormalize(bool normalize);
	NoiseImage& setCoordinates(std::shared_ptr<Image> coordinates);
	NoiseImage& setFrequency(real frequency);
	NoiseImage& setFrequencyMap(std::shared_ptr<Image> map);
	NoiseImage& setNumLayers(real numLayers);
	NoiseImage& setNumLayersMap(std::shared_ptr<Image> map);
	NoiseImage& setAmplitudeRatio(real ratio);
	NoiseImage& setAmplitudeRatioMap(std::shared_ptr<Image> map);
	NoiseImage& setFrequencyRatio(real ratio);
	NoiseImage& setFrequencyRatioMap(std::shared_ptr<Image> map);
	NoiseImage& setWarp(real amount);
	NoiseImage& setWarpMap(std::shared_ptr<Image> map);

private:
	FbmNoiseTextureConfig makePerlinFbmConfig(const CookingContext& ctx) const;

	ENoiseAlgorithm m_algorithm;
	ENoiseDimension m_dimensions;
	bool m_normalize;
	std::shared_ptr<Image> m_coordinates;
	real m_frequency;
	std::shared_ptr<Image> m_frequencyMap;
	real m_numLayers;
	std::shared_ptr<Image> m_numLayersMap;
	real m_amplitudeRatio;
	std::shared_ptr<Image> m_amplitudeRatioMap;
	real m_frequencyRatio;
	std::shared_ptr<Image> m_frequencyRatioMap;
	real m_warp;
	std::shared_ptr<Image> m_warpMap;

public:
	PH_DEFINE_SDL_CLASS(NoiseImage, clazz)
	{
		clazz.typeName("noise");
		clazz.docName("Noise Image");
		clazz.description(
			"Generates deterministic procedural noise. Algorithms share coordinate, frequency, "
			"layer, and warp controls where applicable. Numeric output stores the scalar result "
			"in channel 0 and zero in remaining channels. Color output broadcasts the result "
			"across all color channels. "
			"Mapped inputs override paired constants. Sample UVW supplies coordinates when no "
			"coordinate map is provided.");
		clazz.baseOn<Image>();

		TSdlEnumField<OwnerType, ENoiseAlgorithm> algorithm("algorithm", &OwnerType::m_algorithm);
		algorithm.description("The procedural noise algorithm to evaluate.");
		algorithm.defaultTo(ENoiseAlgorithm::PerlinFbm);
		algorithm.optional();
		clazz.addField(algorithm);

		TSdlEnumField<OwnerType, ENoiseDimension> dimensions("dimensions", &OwnerType::m_dimensions);
		dimensions.description("The number of coordinates in the noise domain.");
		dimensions.defaultTo(ENoiseDimension::D3);
		dimensions.optional();
		clazz.addField(dimensions);

		TSdlBool<OwnerType> normalize("normalize", &OwnerType::m_normalize);
		normalize.description("Normalize noise output when supported by the selected algorithm.");
		normalize.defaultTo(true);
		normalize.optional();
		clazz.addField(normalize);

		TSdlReference<Image, OwnerType> coordinates("coordinates", &OwnerType::m_coordinates);
		coordinates.description(
			"One- to four-component domain coordinates. Leading components are used according "
			"to dimensions. Sample UVW with a zero fourth component is used when omitted.");
		coordinates.optional();
		clazz.addField(coordinates);

		TSdlReal<OwnerType> frequency("frequency", &OwnerType::m_frequency);
		frequency.description("Uniform multiplier applied to domain coordinates.");
		frequency.defaultTo(5.0_r);
		frequency.optional();
		clazz.addField(frequency);

		TSdlReference<Image, OwnerType> frequencyMap("frequency-map", &OwnerType::m_frequencyMap);
		frequencyMap.description("Image input that overrides frequency.");
		frequencyMap.optional();
		clazz.addField(frequencyMap);

		TSdlReal<OwnerType> numLayers("num-layers", &OwnerType::m_numLayers);
		numLayers.description(
			"Number of fractal layers; fractional values blend in the final layer. "
			"At least one layer is evaluated.");
		numLayers.defaultTo(3.0_r);
		numLayers.optional();
		clazz.addField(numLayers);

		TSdlReference<Image, OwnerType> numLayersMap("num-layers-map", &OwnerType::m_numLayersMap);
		numLayersMap.description("Image input that overrides num-layers.");
		numLayersMap.optional();
		clazz.addField(numLayersMap);

		TSdlReal<OwnerType> amplitudeRatio("amplitude-ratio", &OwnerType::m_amplitudeRatio);
		amplitudeRatio.description("Amplitude ratio between consecutive fractal layers.");
		amplitudeRatio.defaultTo(0.5_r);
		amplitudeRatio.optional();
		clazz.addField(amplitudeRatio);

		TSdlReference<Image, OwnerType> amplitudeRatioMap("amplitude-ratio-map", &OwnerType::m_amplitudeRatioMap);
		amplitudeRatioMap.description("Image input that overrides amplitude-ratio.");
		amplitudeRatioMap.optional();
		clazz.addField(amplitudeRatioMap);

		TSdlReal<OwnerType> frequencyRatio("frequency-ratio", &OwnerType::m_frequencyRatio);
		frequencyRatio.description("Frequency ratio between consecutive fractal layers.");
		frequencyRatio.defaultTo(1.99_r);
		frequencyRatio.optional();
		clazz.addField(frequencyRatio);

		TSdlReference<Image, OwnerType> frequencyRatioMap("frequency-ratio-map", &OwnerType::m_frequencyRatioMap);
		frequencyRatioMap.description("Image input that overrides frequency-ratio.");
		frequencyRatioMap.optional();
		clazz.addField(frequencyRatioMap);

		TSdlReal<OwnerType> warp("warp", &OwnerType::m_warp);
		warp.description("Strength of coordinate perturbation before noise evaluation.");
		warp.defaultTo(0.0_r);
		warp.optional();
		clazz.addField(warp);

		TSdlReference<Image, OwnerType> warpMap("warp-map", &OwnerType::m_warpMap);
		warpMap.description("Image input that overrides warp.");
		warpMap.optional();
		clazz.addField(warpMap);
	}
};

}// end namespace ph
