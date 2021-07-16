#pragma once

#include "Actor/SampleSource/SampleSource.h"
#include "Common/primitive_type.h"
#include "Common/assertion.h"
#include "DataIO/SDL/sdl_interface.h"

#include <cstddef>

namespace ph
{

// TODO: cached samples

class RuntimeSampleSource : public SampleSource
{
public:
	inline RuntimeSampleSource() = default;

	void cook(const CoreCookingContext& ctx, CoreCookedUnit& out_cooked) override = 0;

	std::size_t getNumSamples() const;

private:
	std::size_t m_numSamples;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<RuntimeSampleSource>)
	{
		ClassType clazz("runtime");
		clazz.description(
			"Sample sources that generate samples during render engine execution time.");
		clazz.baseOn<SampleSource>();

		TSdlSize<OwnerType> numSamples("samples", &OwnerType::m_numSamples);
		numSamples.description(
			"Number of samples that will be generated. This is the number of samples "
			"that each data unit (such as a single pixel) will receive, on average.");
		numSamples.defaultTo(1);
		numSamples.required();
		clazz.addField(numSamples);

		return clazz;
	}
};

// In-header Implementations:

inline std::size_t RuntimeSampleSource::getNumSamples() const
{
	return m_numSamples;
}

}// end namespace ph
