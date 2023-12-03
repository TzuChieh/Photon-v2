#pragma once

#include "Core/SampleGenerator/SampleStage.h"

#include <cstddef>
#include <utility>

namespace ph
{

class SampleStageReviser final
{
public:
	explicit SampleStageReviser(SampleStage& stage);

	void increaseNumSamples(std::size_t numAddedSamples);
	void resetDimSizeHints(SampleStage::SizeHints sizeHints);

	const SampleStage& getStage() const;
	std::size_t numDims() const;
	std::size_t numSamples() const;
	std::size_t numElements() const;
	const SampleStage::SizeHints& getDimSizeHints() const;

private:
	SampleStage& m_stage;
};

// In-header Implementations:

inline SampleStageReviser::SampleStageReviser(SampleStage& stage) : 
	m_stage(stage)
{}

inline void SampleStageReviser::increaseNumSamples(const std::size_t numAddedSamples)
{
	m_stage.setNumSamples(m_stage.numSamples() + numAddedSamples);
}

inline void SampleStageReviser::resetDimSizeHints(SampleStage::SizeHints sizeHints)
{
	m_stage.setDimSizeHints(std::move(sizeHints));
}

inline const SampleStage& SampleStageReviser::getStage() const
{
	return m_stage;
}

inline std::size_t SampleStageReviser::numDims() const
{
	return m_stage.numDims();
}

inline std::size_t SampleStageReviser::numSamples() const
{
	return m_stage.numSamples();
}

inline std::size_t SampleStageReviser::numElements() const
{
	return m_stage.numElements();
}

inline const SampleStage::SizeHints& SampleStageReviser::getDimSizeHints() const
{
	return m_stage.getDimSizeHints();
}

}// end namespace ph
