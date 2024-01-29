#pragma once

#include "EngineEnv/SampleSource/RuntimeSampleSource.h"
#include "EngineEnv/SampleSource/sdl_halton_randomization_types.h"
#include "SDL/sdl_interface.h"

namespace ph
{

class HaltonSampleSource : public RuntimeSampleSource
{
public:
	void cook(const CoreCookingContext& ctx, CoreCookedUnit& cooked) override;

private:
	EHaltonPermutation m_permutation;
	EHaltonSequence m_sequence;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<HaltonSampleSource>)
	{
		ClassType clazz("halton");
		clazz.docName("Halton Sample Source");
		clazz.description(
			"Generating samples based on the Halton sequence. The samples generated are somewhat "
			"deterministic and can lead to visible patterns if the number of samples is too low. "
			"Also, when Halton sequence is used with a low sample count generator, this often "
			"results in visible artifact along edges even after a large number of iterations. "
			"This is partially due to low sample index being repeatedly used. When this happens, "
			"try more advanced permutation scheme or use back-to-back sample indices.");
		clazz.baseOn<RuntimeSampleSource>();

		TSdlEnumField<OwnerType, EHaltonPermutation> permutation("permutation", &OwnerType::m_permutation);
		permutation.description("Permutation scheme of Halton sequence.");
		permutation.defaultTo(EHaltonPermutation::Fixed);
		permutation.optional();
		clazz.addField(permutation);

		TSdlEnumField<OwnerType, EHaltonSequence> sequence("sequence", &OwnerType::m_sequence);
		sequence.description("Generated sequence of Halton sample generator.");
		sequence.defaultTo(EHaltonSequence::Original);
		sequence.optional();
		clazz.addField(sequence);

		return clazz;
	}
};

}// end namespace ph
