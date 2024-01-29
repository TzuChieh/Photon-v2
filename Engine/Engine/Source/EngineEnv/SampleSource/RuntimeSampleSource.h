#pragma once

#include "EngineEnv/SampleSource/SampleSource.h"
#include "SDL/sdl_interface.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <cstddef>

namespace ph
{

// TODO: cached samples

class RuntimeSampleSource : public SampleSource
{
public:
	void cook(const CoreCookingContext& ctx, CoreCookedUnit& cooked) override = 0;

	std::size_t getNumSamples() const;

private:
	std::size_t m_numSamples;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<RuntimeSampleSource>)
	{
		ClassType clazz("runtime");
		clazz.docName("Runtime Sample Source");
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
