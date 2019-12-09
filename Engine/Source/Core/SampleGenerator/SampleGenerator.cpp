#include "Core/SampleGenerator/SampleGenerator.h"
#include "FileIO/SDL/InputPacket.h"
#include "Math/Random.h"
#include "Common/assertion.h"
#include "Core/SampleGenerator/SampleStageReviser.h"

#include <iostream>
#include <algorithm>

namespace ph
{

SampleGenerator::SampleGenerator(const std::size_t numSampleBatches,
                                 const std::size_t maxCachedBatches) :
	m_numSampleBatches(numSampleBatches),
	m_maxCachedBatches(maxCachedBatches),
	m_numUsedBatches  (0),
	m_numUsedCaches   (maxCachedBatches),
	m_totalBufferSize (0),
	m_sampleBuffer    (),
	m_stages          ()
{
	PH_ASSERT_GE(numSampleBatches, 1);
	PH_ASSERT_GE(maxCachedBatches, 1);
}

SampleGenerator::SampleGenerator(const std::size_t numSampleBatches) : 
	SampleGenerator(numSampleBatches, 4)
{}

bool SampleGenerator::prepareSampleBatch()
{
	PH_ASSERT_LE(m_numUsedBatches, m_numSampleBatches);
	PH_ASSERT_LE(m_numUsedCaches,  m_maxCachedBatches);

	if(hasMoreBatches())
	{
		const bool needsNewCache = m_numUsedCaches == m_maxCachedBatches;
		if(needsNewCache)
		{
			allocSampleBuffer();

			// Not caching more than required number of samples
			const auto numCachedBatches = std::min(m_maxCachedBatches, numRemainingBatches());
			for(std::size_t bi = 0; bi < numCachedBatches; ++bi)
			{
				genSampleBatch(bi);
			}

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

SamplesNDHandle SampleGenerator::declareStageND(const std::size_t numDims, const std::size_t numSamples)
{
	return declareStageND(numDims, numSamples, SampleStage::makeIdentityDimSizeHints(numDims));
}

SamplesNDHandle SampleGenerator::declareStageND(
	const std::size_t              numDims,
	const std::size_t              numSamples,
	const std::vector<std::size_t> dimSizeHints)
{
	const std::size_t bufferIndex = m_totalBufferSize;
	const std::size_t stageIndex  = m_stages.size();

	SampleStage stage(
		bufferIndex,
		numDims, 
		numSamples, 
		std::move(dimSizeHints));

	std::size_t bufferSizeWithCache = 0;
	if(numDims <= 2 || isSamplesGE3DSupported())
	{
		reviseSampleStage(SampleStageReviser(stage));
		bufferSizeWithCache += m_maxCachedBatches * stage.getBufferSize();
		m_stages.push_back(stage);
	}
	// Break the stage into 1-D and 2-D stages if >= 3-D is not supported
	else
	{
		// Split, revise and gather popped stages
		std::vector<SampleStage> poppedStages;
		while(stage.numDims() > 0)
		{
			SampleStage poppedStage;
			if(stage.numDims() >= 2)
			{
				poppedStage = stage.popFirstND(2);
			}
			else
			{
				poppedStage = stage.popFirstND(1);
			}

			reviseSampleStage(SampleStageReviser(poppedStage));
			poppedStages.push_back(poppedStage);
		}

		// Later, make every stage sample count equal to the maximum one so 
		// that samples in stages can be combined to form higher dimensional samples

		std::size_t maxNumSamples = 0;
		for(auto& poppedStage : poppedStages)
		{
			maxNumSamples = std::max(poppedStage.numSamples(), maxNumSamples);
		}
		PH_ASSERT_GT(maxNumSamples, 0);

		for(auto& poppedStage : poppedStages)
		{
			poppedStage.setNumSamples(maxNumSamples);

			bufferSizeWithCache += m_maxCachedBatches * poppedStage.getBufferSize();
			m_stages.push_back(poppedStage);
		}
	}
	PH_ASSERT_GT(bufferSizeWithCache, 0);
	m_totalBufferSize += bufferSizeWithCache;

	return SamplesNDHandle(stageIndex, numDims);
}

SamplesNDStream SampleGenerator::getSamplesND(const SamplesNDHandle& handle)
{
	PH_ASSERT_NE(m_numUsedCaches, 0);
	PH_ASSERT_LT(handle.getStageIndex(), m_stages.size());

	const SampleStage& stage = m_stages[handle.getStageIndex()];

	// These stage attribute should be used instead due to current stage splitting implementation
	const auto numDims     = stage.getStrideSize();
	const auto numElements = stage.getBufferSize();

	PH_ASSERT_LE(stage.getBufferIndex() + m_numUsedCaches * numElements, m_sampleBuffer.size());

	// TODO: probably should make batch buffers closer to each other
	return SamplesNDStream(
		&(m_sampleBuffer[stage.getBufferIndex() + (m_numUsedCaches - 1) * numElements]),
		numDims,
		stage.numSamples());
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

bool SampleGenerator::isSamplesGE3DSupported() const
{
	return false;
}

void SampleGenerator::genSamplesGE3D(const SampleStage& stage, SamplesND& out_samples)
{}

void SampleGenerator::reviseSampleStage(SampleStageReviser& reviser)
{}

void SampleGenerator::allocSampleBuffer()
{
	m_sampleBuffer.resize(m_totalBufferSize);
}

void SampleGenerator::genSampleBatch(const std::size_t cachedBatchIndex)
{
	PH_ASSERT_LT(cachedBatchIndex, m_maxCachedBatches);

	// TODO: probably should make batch buffers closer to each other

	for(const auto& stage : m_stages)
	{
		// Actual buffer index depends on batch number
		const auto bufferIndex = stage.getBufferIndex() + cachedBatchIndex * stage.getBufferSize();

		PH_ASSERT_LE(bufferIndex + stage.getBufferSize(), m_sampleBuffer.size());
		real* const bufferPtr = &(m_sampleBuffer[bufferIndex]);

		switch(stage.numDims())
		{
		case 1:
			genSamples1D(
				stage, 
				SamplesND(
					bufferPtr,
					stage.numDims(),
					stage.numSamples(),
					stage.getStrideSize(),
					stage.getOffsetInStride()));
			break;

		case 2:
			genSamples2D(
				stage,
				SamplesND(
					bufferPtr,
					stage.numDims(),
					stage.numSamples(),
					stage.getStrideSize(),
					stage.getOffsetInStride()));
			break;

		default:
			PH_ASSERT_GE(stage.numDims(), 3);
			PH_ASSERT(isSamplesGE3DSupported());

			genSamplesGE3D(
				stage,
				SamplesND(
					bufferPtr,
					stage.numDims(),
					stage.numSamples(),
					stage.getStrideSize(),
					stage.getOffsetInStride()));
			break;
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
