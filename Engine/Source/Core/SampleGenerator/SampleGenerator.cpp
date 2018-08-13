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

real SampleGenerator::getNext1D(const TSampleStage<real>& stage)
{
	auto& stageData = m_stageDataArray[stage.m_stageIndex];
	auto& data      = stageData.data;
	auto& head      = stageData.head;

	real coord1D;
	if(head < data.size())
	{
		coord1D = data[head++];
	}
	else
	{
		coord1D = Random::genUniformReal_i0_e1();
	}

	return coord1D;
}

Vector2R SampleGenerator::getNext2D(const TSampleStage<Vector2R>& stage)
{
	auto& stageData = m_stageDataArray[stage.m_stageIndex];
	auto& data      = stageData.data;
	auto& head      = stageData.head;

	Vector2R coord2D;
	if(head + 1 < data.size())
	{
		coord2D.x = data[head++];
		coord2D.y = data[head++];
	}
	else
	{
		coord2D.x = Random::genUniformReal_i0_e1();
		coord2D.y = Random::genUniformReal_i0_e1();
	}

	return coord2D;
}

Samples1D SampleGenerator::getNextArray1D(const TSampleStage<Samples1D>& stage)
{
	auto& stageData   = m_stageDataArray[stage.m_stageIndex];
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

Samples2D SampleGenerator::getNextArray2D(const TSampleStage<Samples2D>& stage)
{
	auto& stageData   = m_stageDataArray[stage.m_stageIndex];
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

TSampleStage<real> SampleGenerator::declare1DStage(const std::size_t numElements)
{
	return TSampleStage<real>(declareArray1DStage(numElements).m_stageIndex);
}

TSampleStage<Vector2R> SampleGenerator::declare2DStage(const std::size_t numElements)
{
	return TSampleStage<Vector2R>(declareArray2DStage(numElements).m_stageIndex);
}

TSampleStage<Samples1D> SampleGenerator::declareArray1DStage(const std::size_t numElements)
{
	uint32 stageIndex;
	alloc1DStage(numElements, &stageIndex);

	return TSampleStage<Samples1D>(stageIndex);
}

TSampleStage<Samples2D> SampleGenerator::declareArray2DStage(const std::size_t numElements)
{
	uint32 stageIndex;
	alloc2DStage(numElements, &stageIndex);

	return TSampleStage<Samples2D>(stageIndex);
}

void SampleGenerator::alloc1DStage(const std::size_t numElements, 
                                   uint32* const out_stageIndex)
{
	*out_stageIndex = static_cast<uint32>(m_stageDataArray.size());

	StageData stageData;
	stageData.data.resize(m_numCachedBatches * numElements);
	stageData.head        = 0;
	stageData.numElements = numElements;
	stageData.dimension   = 1;
	m_stageDataArray.push_back(stageData);
}

void SampleGenerator::alloc2DStage(const std::size_t numElements, 
                                   uint32* const out_stageIndex)
{
	*out_stageIndex = static_cast<uint32>(m_stageDataArray.size());

	StageData stageData;
	stageData.data.resize(m_numCachedBatches * numElements * 2);
	stageData.head        = 0;
	stageData.numElements = numElements;
	stageData.dimension   = 2;
	m_stageDataArray.push_back(stageData);
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
		genArray1D(&arrayProxy);
	}
}

void SampleGenerator::genSampleBatch2D(StageData& out_stage)
{
	out_stage.head = 0;
	for(std::size_t b = 0; b < m_numCachedBatches; b++)
	{
		Samples2D arrayProxy(&out_stage.data[b * out_stage.numStageReals()],
		                     out_stage.numElements);
		genArray2D(&arrayProxy);
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