#include "Core/SampleGenerator/SampleGenerator.h"
#include "FileIO/SDL/InputPacket.h"
#include "Math/Random.h"
#include "Common/assertion.h"

#include <iostream>
#include <algorithm>

namespace ph
{

// TODO: if numSampleBatches cannot be evenly divided by numCachedBatches,
// some sample batches will be wasted

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
	PH_ASSERT(numSplits > 0);

	const std::size_t batchesPerSplit  = std::max(numSampleBatches() / numSplits, std::size_t(1));
	std::size_t       remainingBatches = numSampleBatches();
	for(std::size_t i = 0; i < numSplits && remainingBatches > 0; ++i)
	{
		if(remainingBatches >= batchesPerSplit)
		{
			out_sgs.push_back(genNewborn(batchesPerSplit));
			remainingBatches -= batchesPerSplit;
		}
		else
		{
			out_sgs.push_back(genNewborn(remainingBatches));
			return;
		}
	}
}

std::unique_ptr<SampleGenerator> SampleGenerator::genCopied(const std::size_t numSampleBatches) const
{
	return genNewborn(numSampleBatches);
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

Samples2DStage SampleGenerator::declare2DStage(
	const std::size_t numSamples, 
	const Vector2S&   dimSizeHints)
{
	const std::size_t stageIndex = m_totalElements;
	Samples2DStage stage(stageIndex, numSamples, dimSizeHints);
	m_2DStages.push_back(stage);

	m_totalElements += m_numCachedBatches * stage.numElements();

	return stage;
}

SamplesNDStage SampleGenerator::declareNDStage(
	const std::size_t               numElements,
	const std::vector<std::size_t>& dimSizeHints)
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

			genSamples1D(stage1D, &samples);
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

			genSamples2D(stage2D, &samples);
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

			genSamplesND(stageND, &samples);
		}
	}
}

// command interface

SdlTypeInfo SampleGenerator::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_SAMPLE_GENERATOR, "sample-generator");
}

void SampleGenerator::ciRegister(CommandRegister& cmdRegister) {}

}// end namespace ph