#pragma once

#include "Common/primitive_type.h"
#include "Math/TVector2.h"
#include "Common/assertion.h"

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

public:
	math::Vector2S getDimSizeHints() const;

protected:
	inline Samples2DStage(

		const std::size_t stageIndex,
		const std::size_t numSamples) :

		Samples2DStage(stageIndex, numSamples, {1, 1})
	{}

	Samples2DStage(
		std::size_t           stageIndex,
		std::size_t           numSamples,
		const math::Vector2S& dimSizeHints);

private:
	math::Vector2S m_dimSizeHints;
};

class SamplesNDStage : public SamplesStageBase
{
	friend class SampleGenerator;

public:
	std::size_t getDimSizeHint(std::size_t dimIndex) const;

protected:
	inline SamplesNDStage(

		const std::size_t stageIndex,
		const uint32      numDim,
		const std::size_t numSamples) :

		SamplesNDStage(stageIndex, numDim, numSamples, {})
	{}

	SamplesNDStage(
		const std::size_t               stageIndex,
		const uint32                    numDim,
		const std::size_t               numSamples,
		const std::vector<std::size_t>& dimSizeHints);

private:
	std::vector<std::size_t> m_dimSizeHints;
};

// In-header Implementations:

inline math::Vector2S Samples2DStage::getDimSizeHints() const
{
	return m_dimSizeHints;
}

inline std::size_t SamplesNDStage::getDimSizeHint(const std::size_t dimIndex) const
{
	PH_ASSERT(0 <= dimIndex && dimIndex < m_dimSizeHints.size());

	return m_dimSizeHints[dimIndex];
}

}// end namespace ph
