#pragma once

#include "Common/primitive_type.h"

#include <cstddef>

namespace ph
{

class SamplesStageBase
{
protected:
	inline explicit SamplesStageBase(const std::size_t stageIndex) :
		m_stageIndex(stageIndex)
	{}

	std::size_t m_stageIndex;
};

class Samples1DStage : public SamplesStageBase
{
	friend class SampleGenerator;

protected:
	inline explicit Samples1DStage(const std::size_t stageIndex) :
		SamplesStageBase(stageIndex)
	{}
};

class Samples2DStage : public SamplesStageBase
{
	friend class SampleGenerator;

protected:
	inline explicit Samples2DStage(const std::size_t stageIndex) :
		SamplesStageBase(stageIndex)
	{}
};

class SamplesNDStage : public SamplesStageBase
{
	friend class SampleGenerator;

protected:
	inline explicit SamplesNDStage(const std::size_t stageIndex) :
		SamplesStageBase(stageIndex)
	{}
};

}// end namespace ph