#pragma once

#include "Engine/EngineEnv/Visualizer/FrameVisualizer.h"
#include "Engine/Math/TVector2.h"
#include "Engine/Math/Geometry/TAABB2D.h"
#include "Engine/SDL/sdl_interface.h"
#include "Engine/EngineEnv/Visualizer/sdl_photon_mapping_mode.h"
#include "Engine/EngineEnv/Visualizer/sdl_sample_filter_type.h"
#include "Engine/Core/Renderer/PM/PMCommonParams.h"

#include <Common/primitive_type.h>

#include <memory>

namespace ph { class IRayEnergyEstimator; }
namespace ph { class SampleFilter; }

namespace ph
{

class PhotonMappingVisualizer : public FrameVisualizer
{
public:
	void cook(const CoreCookingContext& ctx, CoreCookedUnit& cooked) override;

	EPhotonMappingMode getMode() const;

protected:
	PMCommonParams makeCommonParams() const;

private:
	EPhotonMappingMode m_mode;
	uint64 m_numPhotons;
	uint64 m_numPasses;
	uint64 m_numSamplesPerPixel;
	real m_photonRadius;
	uint32 m_glossyMergeBeginLengthHint;
	uint32 m_stochasticViewSampleBeginLengthHint;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<PhotonMappingVisualizer>)
	{
		ClassType clazz("photon-mapping");
		clazz.docName("Photon Mapping Visualizer");
		clazz.description("Render frames with common photon mapping methods.");
		clazz.baseOn<FrameVisualizer>();

		TSdlEnumField<OwnerType, EPhotonMappingMode> mode("mode", &OwnerType::m_mode);
		mode.description(
			"The photon mapping technique used by the visualizer.");
		mode.defaultTo(EPhotonMappingMode::Vanilla);
		mode.optional();
		clazz.addField(mode);

		// Borrow the default values there
		const PMCommonParams commonParams{};

		TSdlUInt64<OwnerType> numPhotons("num-photons", &OwnerType::m_numPhotons);
		numPhotons.description(
			"Number of photons used. For progressive techniques, this value is for a single pass.");
		numPhotons.defaultTo(commonParams.numPhotons);
		numPhotons.optional();
		clazz.addField(numPhotons);

		TSdlUInt64<OwnerType> numPasses("num-passes", &OwnerType::m_numPasses);
		numPasses.description(
			"Number of passes performed by progressive techniques.");
		numPasses.defaultTo(commonParams.numPasses);
		numPasses.optional();
		clazz.addField(numPasses);

		TSdlUInt64<OwnerType> numSamplesPerPixel("num-samples-per-pixel", &OwnerType::m_numSamplesPerPixel);
		numSamplesPerPixel.description(
			"Number of samples per pixel. Higher values can resolve image aliasing, but can consume "
			"large amounts of memory for some algorithms. This value can also mean the number of "
			"statistics gathered in a single pixel for some techniques. If the value is not a "
			"power-of-2 number, it may be adjusted.");
		numSamplesPerPixel.defaultTo(commonParams.numSamplesPerPixel);
		numSamplesPerPixel.optional();
		clazz.addField(numSamplesPerPixel);

		TSdlReal<OwnerType> photonRadius("photon-radius", &OwnerType::m_photonRadius);
		photonRadius.description(
			"Energy contribution radius for each photon. For progressive techniques, this value is for "
			"setting up the initial radius.");
		photonRadius.defaultTo(commonParams.kernelRadius);
		photonRadius.optional();
		clazz.addField(photonRadius);

		TSdlUInt32<OwnerType> glossyMergeBeginLengthHint("glossy-merge-begin-length-hint", &OwnerType::m_glossyMergeBeginLengthHint);
		glossyMergeBeginLengthHint.description(
			"Hint for the minimum path length to start estimating energy using photons on glossy surface."
			"If the scene contains diffuse surface and is easily reachable by photons, it is recommended "
			"to set this to a lower value.");
		glossyMergeBeginLengthHint.defaultTo(commonParams.glossyMergeBeginLengthHint);
		glossyMergeBeginLengthHint.optional();
		clazz.addField(glossyMergeBeginLengthHint);

		TSdlUInt32<OwnerType> stochasticViewSampleBeginLengthHint("stochastic-view-sample-begin-length-hint", &OwnerType::m_stochasticViewSampleBeginLengthHint);
		stochasticViewSampleBeginLengthHint.description(
			"Hint for the view path length to start random path sampling. If this value differ too much "
			"from the mean specular path length from the scene, the energy estimation result may contain "
			"higher variance or bias. Beware when using higher values as non-stochastic path may be "
			"branched, which can result in exponential growth of number of rays.");
		stochasticViewSampleBeginLengthHint.defaultTo(commonParams.stochasticViewSampleBeginLength);
		stochasticViewSampleBeginLengthHint.optional();
		clazz.addField(stochasticViewSampleBeginLengthHint);

		return clazz;
	}
};

// In-header Implementations:

inline EPhotonMappingMode PhotonMappingVisualizer::getMode() const
{
	return m_mode;
}

}// end namespace ph
