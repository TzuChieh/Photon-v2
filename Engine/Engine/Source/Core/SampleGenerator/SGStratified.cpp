#include "Core/SampleGenerator/SGStratified.h"
#include "Core/Sample.h"
#include "Math/Random/Random.h"
#include "Core/SampleGenerator/SamplesND.h"
#include "Math/TVector2.h"
#include "Core/SampleGenerator/SampleStageReviser.h"

#include <Common/assertion.h>

#include <algorithm>
#include <iterator>

namespace ph
{

SGStratified::SGStratified(const std::size_t numSamples) :
	//SampleGenerator(numSamples, numSamples)
	SampleGenerator(numSamples, 4)// HACK
{}

void SGStratified::genSamples1D(
	const SampleContext& context,
	const SampleStage&   stage,
	SamplesND            out_samples)
{
	PH_ASSERT_EQ(stage.numDims(), 1);
	PH_ASSERT_GE(stage.numSamples(), 1);

	const real dx = 1.0_r / static_cast<real>(stage.numSamples());

	for(std::size_t x = 0; x < stage.numSamples(); ++x)
	{
		const real jitter = math::Random::sample();
		out_samples.setSample<1>(x, {(static_cast<real>(x) + jitter) * dx});
	}
	out_samples.shuffle();
}

void SGStratified::genSamples2D(
	const SampleContext& context,
	const SampleStage&   stage,
	SamplesND            out_samples)
{
	PH_ASSERT_EQ(stage.numDims(), 2);
	PH_ASSERT_GE(stage.numSamples(), 1);
	PH_ASSERT_EQ(stage.getDimSizeHints().size(), 2);

	const auto        strataSizes = math::Vector2S(stage.getDimSizeHints()[0], stage.getDimSizeHints()[1]);
	const std::size_t numStrata   = strataSizes.product();
	PH_ASSERT_GT(numStrata, 0);
	
	// OPT: It is possible to precompute how many samples will be in a
	// stratum and generate them together. 

	// Tries to generate <numStrata> samples over and over again until there 
	// is no room in <out_array> to fit another <numStrata> samples.

	const real dx = 1.0_r / static_cast<real>(strataSizes.x());
	const real dy = 1.0_r / static_cast<real>(strataSizes.y());

	std::size_t currentIndex = 0;
	while(currentIndex + numStrata <= stage.numSamples())
	{
		for(std::size_t y = 0; y < strataSizes.y(); ++y)
		{
			for(std::size_t x = 0; x < strataSizes.x(); ++x)
			{
				const real jitterX = math::Random::sample();
				const real jitterY = math::Random::sample();
				out_samples.setSample<2>(
					currentIndex, 
					{(static_cast<real>(x) + jitterX) * dx, (static_cast<real>(y) + jitterY) * dy});

				++currentIndex;
			}
		}
	}

	// There is no room to fit another <numStrata> samples. We fill the remaining
	// spaces with random ones.
	PH_ASSERT_LT(out_samples.numSamples() - currentIndex, numStrata);

	// TODO: use hypercube sampling?
	for(std::size_t i = currentIndex; i < out_samples.numSamples(); ++i)
	{
		out_samples.setSample<2>(i, math::Random::sampleND<2>());
	}

	// TODO: shuffle only the quasi part?
	out_samples.shuffle();
}

std::unique_ptr<SampleGenerator> SGStratified::genNewborn(const std::size_t numSamples) const
{
	return std::make_unique<SGStratified>(numSamples);
}

void SGStratified::reviseSampleStage(SampleStageReviser reviser)
{
	// No revision needed for 1-D samples
	if(reviser.numDims() == 1)
	{
		return;
	}

	reviser.resetDimSizeHints(reviseDimSizeHints(reviser.numSamples(), reviser.getDimSizeHints()));
}

std::vector<std::size_t> SGStratified::reviseDimSizeHints(
	const std::size_t               numSamples,
	const std::vector<std::size_t>& originalDimSizeHints)
{
	PH_ASSERT_GE(numSamples, 1);
	PH_ASSERT_GE(originalDimSizeHints.size(), 1);

	const std::size_t NUM_DIMS = originalDimSizeHints.size();

	std::vector<std::size_t> revisedDimSizeHints(originalDimSizeHints.size());

	// Ensure sensible size hints and calculates total hinted size
	std::size_t hintSize = 1;
	for(std::size_t di = 0; di < originalDimSizeHints.size(); ++di)
	{
		revisedDimSizeHints[di] = std::max(originalDimSizeHints[di], std::size_t(1));
		hintSize *= revisedDimSizeHints[di];
	}

	// Scale size hints towards number of samples 
	// (proportionally, and not being greater)
	const auto sizeRatio = static_cast<double>(numSamples) / hintSize;
	const auto shrinkRatio = std::pow(sizeRatio, 1.0 / static_cast<double>(NUM_DIMS));
	for(auto& dimSizeHint : revisedDimSizeHints)
	{
		dimSizeHint = static_cast<std::size_t>(shrinkRatio * dimSizeHint);
		dimSizeHint = std::max(dimSizeHint, std::size_t(1));
	}

	// Numerical error on floating point operations may cause the revised hint
	// size being greater than the number of samples. In such case, decrease 
	// the size of largest dimension iteratively until the condition being satisfied.
	while(math::product(revisedDimSizeHints) > numSamples)
	{
		auto max = std::max_element(revisedDimSizeHints.begin(), revisedDimSizeHints.end());
		*max = *max > 1 ? *max - 1 : 1;
	}

	PH_ASSERT_IN_RANGE_INCLUSIVE(math::product(revisedDimSizeHints), 1, numSamples);
	return revisedDimSizeHints;
}

}// end namespace ph
