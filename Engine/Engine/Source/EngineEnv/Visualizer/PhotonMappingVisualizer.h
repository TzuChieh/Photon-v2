#pragma once

#include "EngineEnv/Visualizer/FrameVisualizer.h"
#include "Common/primitive_type.h"
#include "Math/TVector2.h"
#include "Math/Geometry/TAABB2D.h"
#include "SDL/sdl_interface.h"
#include "EngineEnv/Visualizer/sdl_photon_mapping_mode.h"
#include "EngineEnv/Visualizer/sdl_sample_filter_type.h"
#include "Core/Filmic/SampleFilter.h"

#include <memory>

namespace ph { class IRayEnergyEstimator; }

namespace ph
{

class PhotonMappingVisualizer : public FrameVisualizer
{
public:
	inline PhotonMappingVisualizer() = default;

	void cook(const CoreCookingContext& ctx, CoreCookedUnit& cooked) override;

	EPhotonMappingMode getMode() const;
	ESampleFilter getSampleFilter() const;

protected:
	SampleFilter makeSampleFilter() const;
	std::unique_ptr<IRayEnergyEstimator> makeEstimator() const;

private:
	EPhotonMappingMode m_mode;
	ESampleFilter m_sampleFilter;
	uint64 m_numPhotons;
	uint64 m_numPasses;
	uint64 m_numSamplesPerPixel;
	real m_photonRadius;

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

		TSdlEnumField<OwnerType, ESampleFilter> sampleFilter("sample-filter", &OwnerType::m_sampleFilter);
		sampleFilter.description(
			"Sample filter for the film sampling process.");
		sampleFilter.defaultTo(ESampleFilter::BlackmanHarris);
		sampleFilter.optional();
		clazz.addField(sampleFilter);

		TSdlUInt64<OwnerType> numPhotons("num-photons", &OwnerType::m_numPhotons);
		numPhotons.description(
			"Number of photons used. For progressive techniques, this value is for a single pass.");
		numPhotons.defaultTo(200000);
		numPhotons.optional();
		clazz.addField(numPhotons);

		TSdlUInt64<OwnerType> numPasses("num-passes", &OwnerType::m_numPasses);
		numPasses.description(
			"Number of passes performed by progressive techniques.");
		numPasses.defaultTo(1);
		numPasses.optional();
		clazz.addField(numPasses);

		TSdlUInt64<OwnerType> numSamplesPerPixel("num-samples-per-pixel", &OwnerType::m_numSamplesPerPixel);
		numSamplesPerPixel.description(
			"Number of samples per pixel. Higher values can resolve image aliasing, but can consume "
			"large amounts of memory.");
		numSamplesPerPixel.defaultTo(4);
		numSamplesPerPixel.optional();
		clazz.addField(numSamplesPerPixel);

		TSdlReal<OwnerType> photonRadius("photon-radius", &OwnerType::m_photonRadius);
		photonRadius.description(
			"Energy contribution radius for each photon. For progressive techniques, this value is for "
			"setting up the initial radius.");
		photonRadius.defaultTo(0.1_r);
		photonRadius.optional();
		clazz.addField(photonRadius);

		return clazz;
	}
};

// In-header Implementations:

inline EPhotonMappingMode PhotonMappingVisualizer::getMode() const
{
	return m_mode;
}

inline ESampleFilter PhotonMappingVisualizer::getSampleFilter() const
{
	return m_sampleFilter;
}

}// end namespace ph