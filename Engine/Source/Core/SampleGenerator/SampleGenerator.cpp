#include "Core/SampleGenerator/SampleGenerator.h"
#include "FileIO/SDL/InputPacket.h"
#include "Math/Random.h"
#include "Common/assertion.h"

#include <iostream>

namespace ph
{

SampleGenerator::SampleGenerator(const std::size_t numSampleBatches,
                                 const std::size_t numCachedBatches) :
	m_numSampleBatches(numSampleBatches),
	m_numCachedBatches(numCachedBatches),
	m_numUsedBatches  (0),
	m_numUsedCaches   (numCachedBatches),
	m_totalElements   (0)
{
	PH_ASSERT(numCachedBatches > 0);
}

SampleGenerator::~SampleGenerator() = default;

bool SampleGenerator::prepareSampleBatch()
{
	PH_ASSERT(m_numUsedBatches <= m_numSampleBatches &&
	          m_numUsedCaches  <= m_numCachedBatches);

	const bool hasMoreBatches = m_numUsedBatches < m_numSampleBatches;
	const bool needsNewCache  = m_numUsedCaches == m_numCachedBatches;

	if(hasMoreBatches && needsNewCache)
	{
		allocSampleBuffer();
		genSampleBatch();
		m_numUsedCaches = 0;
	}

	if(hasMoreBatches)
	{
		m_numUsedBatches++;
		m_numUsedCaches++;
	}
	return hasMoreBatches;
}

void SampleGenerator::genSplitted(const std::size_t numSplits,
                                  std::vector<std::unique_ptr<SampleGenerator>>& out_sgs) const
{
	if(!canSplit(numSplits))
	{
		return;
	}

	const std::size_t splittedNumSamples = numSampleBatches() / numSplits;
	for(std::size_t i = 0; i < numSplits; i++)
	{
		out_sgs.push_back(genNewborn(splittedNumSamples));
	}
}

std::unique_ptr<SampleGenerator> SampleGenerator::genCopied() const
{
	return genNewborn(m_numSampleBatches);
}

Samples1D SampleGenerator::getSamples1D(const Samples1DStage& stage)
{
	PH_ASSERT(
		m_numUsedCaches != 0 && 
		stage.getStageIndex() + m_numUsedCaches * stage.numElements() <= m_sampleBuffer.size());

	return Samples1D(
		&(m_sampleBuffer[stage.getStageIndex() + (m_numUsedCaches - 1) * stage.numElements()]), 
		stage.numSamples());
}

Samples2D SampleGenerator::getSamples2D(const Samples2DStage& stage)
{
	PH_ASSERT(
		m_numUsedCaches != 0 &&
		stage.getStageIndex() + m_numUsedCaches * stage.numElements() <= m_sampleBuffer.size());

	return Samples2D(
		&(m_sampleBuffer[stage.getStageIndex() + (m_numUsedCaches - 1) * stage.numElements()]),
		stage.numSamples());
}

SamplesND SampleGenerator::getSamplesND(const SamplesNDStage& stage)
{
	PH_ASSERT(
		m_numUsedCaches != 0 &&
		stage.getStageIndex() + m_numUsedCaches * stage.numElements() <= m_sampleBuffer.size());

	return SamplesND(
		&(m_sampleBuffer[stage.getStageIndex() + (m_numUsedCaches - 1) * stage.numElements()]),
		stage.numSamples());
}

Samples1DStage SampleGenerator::declare1DStage(const std::size_t numSamples)
{
	const std::size_t stageIndex = m_totalElements;
	Samples1DStage stage(stageIndex, numSamples);
	m_1DStages.push_back(stage);

	m_totalElements += m_numCachedBatches * stage.numElements();

	return stage;
}

Samples2DStage SampleGenerator::declare2DStage(const std::size_t numSamples)
{
	const std::size_t stageIndex = m_totalElements;
	Samples2DStage stage(stageIndex, numSamples);
	m_2DStages.push_back(stage);

	m_totalElements += m_numCachedBatches * stage.numElements();

	return stage;
}

SamplesNDStage SampleGenerator::declareNDStage(const std::size_t numElements)
{
	// TODO
	return SamplesNDStage(0, 0, 0);
}

void SampleGenerator::allocSampleBuffer()
{
	m_sampleBuffer.resize(m_totalElements);
}

void SampleGenerator::genSampleBatch()
{
	genSamples1DBatch();
	genSamples2DBatch();
	genSamplesNDBatch();
}

void SampleGenerator::genSamples1DBatch()
{
	for(const auto& stage1D : m_1DStages)
	{
		for(std::size_t b = 0; b < m_numCachedBatches; b++)
		{
			Samples1D samples(
				&(m_sampleBuffer[stage1D.getStageIndex() + b * stage1D.numElements()]),
				stage1D.numSamples());

			genSamples1D(&samples);
		}
	}
}

void SampleGenerator::genSamples2DBatch()
{
	for(const auto& stage2D : m_2DStages)
	{
		for(std::size_t b = 0; b < m_numCachedBatches; b++)
		{
			Samples2D samples(
				&(m_sampleBuffer[stage2D.getStageIndex() + b * stage2D.numElements()]),
				stage2D.numSamples());

			genSamples2D(&samples);
		}
	}
}

void SampleGenerator::genSamplesNDBatch()
{
	for(const auto& stageND : m_NDStages)
	{
		for(std::size_t b = 0; b < m_numCachedBatches; b++)
		{
			SamplesND samples(
				&(m_sampleBuffer[stageND.getStageIndex() + b * stageND.numElements()]),
				stageND.numSamples());

			genSamplesND(&samples);
		}
	}
}

bool SampleGenerator::canSplit(const std::size_t numSplits) const
{
	if(numSplits == 0)
	{
		std::cerr << "warning: at SampleGenerator::canSplit(), "
		          << "number of splits is 0" << std::endl;
		return false;
	}

	if(m_numSampleBatches % numSplits != 0)
	{
		std::cerr << "warning: at SampleGenerator::canSplit(), "
		          << "generator cannot evenly split into " << numSplits << " parts" << std::endl;
		std::cerr << "(sample batches: " << m_numSampleBatches << ")" << std::endl;
		return false;
	}

	return true;
}

// command interface

SdlTypeInfo SampleGenerator::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_SAMPLE_GENERATOR, "sample-generator");
}

void SampleGenerator::ciRegister(CommandRegister& cmdRegister) {}

}// end namespace ph