#include "Core/SampleGenerator/stages.h"
#include "Common/Logger.h"
#include "Common/assertion.h"
#include "Math/math.h"

#include <cmath>

namespace ph
{

namespace
{
	const Logger logger(LogSender("Sample Stage"));
}

Samples2DStage::Samples2DStage(

	const std::size_t stageIndex,
	const std::size_t numSamples,
	const Vector2S&   dimSizeHints) :

	SamplesStageBase(stageIndex, 2, numSamples),
	m_dimSizeHints(dimSizeHints.max({1, 1}))
{
	// Scaling down dimensional size hints if they have more number of samples
	// then required.
	const std::size_t hintSize = dimSizeHints.x * dimSizeHints.y;
	if(hintSize > numSamples)
	{
		// FIXME: the product of the new size hints may still be larger due to numerical errors?

		const float sizeRatio   = static_cast<float>(numSamples) / static_cast<float>(hintSize);
		const float shrinkRatio = std::sqrt(sizeRatio);

		m_dimSizeHints.x = static_cast<std::size_t>(shrinkRatio * static_cast<float>(dimSizeHints.x));
		m_dimSizeHints.y = static_cast<std::size_t>(shrinkRatio * static_cast<float>(dimSizeHints.y));
		m_dimSizeHints = m_dimSizeHints.max({1, 1});

		logger.log(ELogLevel::NOTE_MED, 
			"hint size " + dimSizeHints.toString() + " exceeds # of samples <" + 
			std::to_string(numSamples) + ">, adjusted to " + m_dimSizeHints.toString());
	}

	PH_ASSERT(
		m_dimSizeHints.x * m_dimSizeHints.y > 0 &&
		m_dimSizeHints.x * m_dimSizeHints.y <= numSamples);
}

SamplesNDStage::SamplesNDStage(

	const std::size_t               stageIndex,
	const uint32                    numDim,
	const std::size_t               numSamples,
	const std::vector<std::size_t>& dimSizeHints) :

	SamplesStageBase(stageIndex, numDim, numSamples),
	m_dimSizeHints(dimSizeHints)
{
	// Ensuring correct number of size hints.
	m_dimSizeHints.resize(numDim, 1);

	// Ensuring sensible size hints and calculates total hinted size.
	std::size_t hintSize = 1;
	for(auto& dimSize : m_dimSizeHints)
	{
		dimSize = std::max(dimSize, std::size_t(1));
		hintSize *= dimSize;
	}

	// Scaling down dimensional size hints if they have more number of samples
	// then required.
	if(hintSize > numSamples)
	{
		const float sizeRatio   = static_cast<float>(numSamples) / static_cast<float>(hintSize);
		const float shrinkRatio = std::pow(sizeRatio, 1.0f / static_cast<float>(numDim));
		for(auto& dimSize : m_dimSizeHints)
		{
			dimSize = static_cast<std::size_t>(shrinkRatio * static_cast<float>(dimSize));
			dimSize = std::max(dimSize, std::size_t(1));
		}

		logger.log(ELogLevel::NOTE_MED, 
			"hinted size <" + std::to_string(hintSize) + "> exceeds # of samples <" +
			std::to_string(numSamples) + ">, adjusted to " + std::to_string(math::product(m_dimSizeHints)));
	}

	PH_ASSERT(
		math::product(m_dimSizeHints) > 0 &&
		math::product(m_dimSizeHints) <= numSamples);
}

}// end namespace ph