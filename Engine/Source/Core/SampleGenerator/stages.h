#pragma once

#include "Common/primitive_type.h"

#include <cstddef>
#include <vector>

namespace ph
{

class SamplesStageBase
{
public:
	inline uint32 numDim() const
	{
		return m_numDim;
	}

	inline std::size_t numSamples() const
	{
		return m_numSamples;
	}

	inline std::size_t numElements() const
	{
		return m_numSamples * m_numDim;
	}

protected:
	inline SamplesStageBase(

		const std::size_t stageIndex,
		const uint32      numDim,
		const std::size_t numSamples) :

		m_stageIndex(stageIndex),
		m_numDim    (numDim),
		m_numSamples(numSamples)
	{}

	inline std::size_t getStageIndex() const
	{
		return m_stageIndex;
	}

private:
	std::size_t m_stageIndex;
	uint32      m_numDim;
	std::size_t m_numSamples;
};

class Samples1DStage : public SamplesStageBase
{
	friend class SampleGenerator;

protected:
	inline Samples1DStage(

		const std::size_t stageIndex,
		const std::size_t numSamples) :

		SamplesStageBase(stageIndex, 1, numSamples)
	{}
};

class Samples2DStage : public SamplesStageBase
{
	friend class SampleGenerator;

protected:
	inline Samples2DStage(

		const std::size_t stageIndex,
		const std::size_t numSamples) :

		SamplesStageBase(stageIndex, 2, numSamples)
	{}
};

class SamplesNDStage : public SamplesStageBase
{
	friend class SampleGenerator;

protected:
	inline SamplesNDStage(

		const std::size_t stageIndex,
		const uint32      numDim,
		const std::size_t numSamples) :

		SamplesStageBase(stageIndex, numDim, numSamples)
	{}
};

}// end namespace ph