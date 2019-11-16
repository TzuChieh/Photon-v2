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
	m_totalElements   (0),
	m_sampleBuffer    (),
	m_stages          ()
{
	PH_ASSERT_GT(numCachedBatches, 0);
}

SampleGenerator::SampleGenerator(const std::size_t numSampleBatches) : 
	SampleGenerator(numSampleBatches, 4)
{}

bool SampleGenerator::prepareSampleBatch()
{
	PH_ASSERT_LE(m_numUsedBatches, m_numSampleBatches);
	PH_ASSERT_LE(m_numUsedCaches,  m_numCachedBatches);

	if(hasMoreBatches())
	{
		const bool needsNewCache = m_numUsedCaches == m_numCachedBatches;
		if(needsNewCache)
		{
			allocSampleBuffer();
			genSampleBatch();
			m_numUsedCaches = 0;
		}

		++m_numUsedBatches;
		++m_numUsedCaches;

		return true;
	}
	else
	{
		return false;
	}
}

void SampleGenerator::genSplitted(const std::size_t numSplits,
                                  std::vector<std::unique_ptr<SampleGenerator>>& out_sgs) const
{
	PH_ASSERT_GT(numSplits, 0);

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

void SampleGenerator::allocSampleBuffer()
{
	m_sampleBuffer.resize(m_totalElements);
}

void SampleGenerator::genSampleBatch()
{
	// TODO: probably should make batch buffers closer to each other

	for(const auto& stage : m_stages)
	{
		for(std::size_t bi = 0; bi < m_numCachedBatches; ++bi)
		{
			genSamples(
				stage, 
				&(m_sampleBuffer[stage.getSampleIndex() + bi * stage.numElements()]));
		}
	}
}

// command interface

//SampleGenerator::SampleGenerator(const InputPacket& packet)
//{}

SdlTypeInfo SampleGenerator::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_SAMPLE_GENERATOR, "sample-generator");
}

void SampleGenerator::ciRegister(CommandRegister& cmdRegister)
{}

}// end namespace ph
