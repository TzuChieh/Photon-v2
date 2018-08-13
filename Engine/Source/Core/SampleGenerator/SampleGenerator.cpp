#include "Core/SampleGenerator/SampleGenerator.h"
#include "FileIO/SDL/InputPacket.h"
#include "Math/Random.h"

#include <iostream>

namespace ph
{

SampleGenerator::SampleGenerator(const std::size_t numSampleBatches,
                                 const std::size_t numCachedBatches) :
	m_numSampleBatches(numSampleBatches),
	m_numCachedBatches(numCachedBatches),
	m_currentBatchNumber(0)
{}

SampleGenerator::~SampleGenerator() = default;

bool SampleGenerator::prepareSampleBatch()
{
	const bool hasMoreBatches = m_currentBatchNumber < m_numSampleBatches;
	const bool needsNewCache  = m_currentBatchNumber % m_numCachedBatches == 0;
	if(hasMoreBatches && needsNewCache)
	{
		genSampleBatch();
	}

	m_currentBatchNumber++;

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
	auto& stageData   = m_stageDataArray[stage.getStageIndex()];
	auto& data        = stageData.data;
	auto& head        = stageData.head;
	auto& numElements = stageData.numElements;

	if(head + numElements - 1 < data.size())
	{
		real* arrayHead = &data[head];
		head += numElements;
		return Samples1D(arrayHead, numElements);
	}
	else
	{
		return Samples1D();
	}
}

Samples2D SampleGenerator::getSamples2D(const Samples2DStage& stage)
{
	auto& stageData   = m_stageDataArray[stage.getStageIndex()];
	auto& data        = stageData.data;
	auto& head        = stageData.head;
	auto& numElements = stageData.numElements;

	if(head + numElements * 2 - 1 < data.size())
	{
		real* arrayHead = &data[head];
		head += numElements * 2;
		return Samples2D(arrayHead, numElements);
	}
	else
	{
		return Samples2D();
	}
}

SamplesND SampleGenerator::getSamplesND(const SamplesNDStage& stage)
{
	// TODO
	return SamplesND();
}

Samples1DStage SampleGenerator::declare1DStage(const std::size_t numSamples)
{
	std::size_t stageIndex;
	alloc1DStage(numSamples, &stageIndex);

	return Samples1DStage(stageIndex, numSamples);
}

Samples2DStage SampleGenerator::declare2DStage(const std::size_t numSamples)
{
	std::size_t stageIndex;
	alloc2DStage(numSamples, &stageIndex);

	return Samples2DStage(stageIndex, numSamples);
}

SamplesNDStage SampleGenerator::declareNDStage(const std::size_t numElements)
{
	// TODO
	return SamplesNDStage(0, 0, 0);
}

void SampleGenerator::alloc1DStage(const std::size_t  numSamples, 
                                   std::size_t* const out_stageIndex)
{
	*out_stageIndex = m_stageDataArray.size();

	StageData stageData;
	stageData.data.resize(m_numCachedBatches * numSamples);
	stageData.head        = 0;
	stageData.numElements = numSamples;
	stageData.dimension   = 1;
	m_stageDataArray.push_back(stageData);
}

void SampleGenerator::alloc2DStage(const std::size_t  numSamples,
                                   std::size_t* const out_stageIndex)
{
	*out_stageIndex = m_stageDataArray.size();

	StageData stageData;
	stageData.data.resize(m_numCachedBatches * numSamples * 2);
	stageData.head        = 0;
	stageData.numElements = numSamples;
	stageData.dimension   = 2;
	m_stageDataArray.push_back(stageData);
}

void SampleGenerator::allocNDStage(std::size_t numElements, std::size_t* const out_stageIndex)
{
	// TODO
}

void SampleGenerator::genSampleBatch()
{
	for(auto& stage : m_stageDataArray)
	{
		if(stage.dimension == 1)
		{
			genSampleBatch1D(stage);
		}
		else if(stage.dimension == 2)
		{
			genSampleBatch2D(stage);
		}
		else
		{
			std::cerr << "warning: at SampleGenerator::genSampleBatch(), "
			          << "unsupported number of dimensions detected: " << stage.dimension << std::endl;
		}
	}
}

void SampleGenerator::genSampleBatch1D(StageData& out_stage)
{
	out_stage.head = 0;
	for(std::size_t b = 0; b < m_numCachedBatches; b++)
	{
		Samples1D arrayProxy(&out_stage.data[b * out_stage.numStageReals()],
		                     out_stage.numElements);
		genSamples1D(&arrayProxy);
	}
}

void SampleGenerator::genSampleBatch2D(StageData& out_stage)
{
	out_stage.head = 0;
	for(std::size_t b = 0; b < m_numCachedBatches; b++)
	{
		Samples2D arrayProxy(&out_stage.data[b * out_stage.numStageReals()],
		                     out_stage.numElements);
		genSamples2D(&arrayProxy);
	}
}

void SampleGenerator::genSampleBatchND(StageData& out_stage)
{
	// TODO
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