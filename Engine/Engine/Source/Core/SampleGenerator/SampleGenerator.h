#pragma once

#include "Core/Sample.h"
#include "Math/TVector2.h"
#include "Math/TArithmeticArray.h"
#include "Core/SampleGenerator/SampleStage.h"
#include "Core/SampleGenerator/SampleStageReviser.h"
#include "Core/SampleGenerator/SamplesNDHandle.h"
#include "Core/SampleGenerator/SamplesND.h"
#include "Core/SampleGenerator/SamplesNDStream.h"
#include "Math/TArithmeticArray.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <cstddef>
#include <utility>
#include <vector>
#include <memory>
#include <utility>

namespace ph
{

class Scene;
class SampleContext;

class SampleGenerator
{
public:
	SampleGenerator(std::size_t numSampleBatches, std::size_t maxCachedBatches);
	explicit SampleGenerator(std::size_t numSampleBatches);
	virtual ~SampleGenerator() = default;

	void genSplitted(std::size_t numSplits,
	                 std::vector<std::unique_ptr<SampleGenerator>>& out_sgs) const;
	std::unique_ptr<SampleGenerator> genCopied(std::size_t numSampleBatches) const;

	bool prepareSampleBatch();

	SamplesNDHandle declareStageND(std::size_t numDims, std::size_t numSamples);

	SamplesNDHandle declareStageND(
		std::size_t              numDims, 
		std::size_t              numSamples, 
		std::vector<std::size_t> dimSizeHints);

	SamplesNDStream getSamplesND(const SamplesNDHandle& handle) const;

	std::size_t numSampleBatches() const;
	std::size_t maxCachedBatches() const;
	std::size_t numRemainingBatches() const;
	bool hasMoreBatches() const;

private:
	virtual std::unique_ptr<SampleGenerator> genNewborn(std::size_t numSamples) const = 0;

	virtual void genSamples1D(
		const SampleContext& context,
		const SampleStage&   stage, 
		SamplesND            out_samples) = 0;

	virtual void genSamples2D(
		const SampleContext& context,
		const SampleStage&   stage, 
		SamplesND            out_samples) = 0;

	virtual bool isSamplesGE3DSupported() const;

	virtual void genSamplesGE3D(
		const SampleContext& context,
		const SampleStage&   stage, 
		SamplesND            out_samples);

	virtual void reviseSampleStage(SampleStageReviser reviser);

private:
	void allocSampleBuffer();
	void genSampleBatch(std::size_t cachedBatchIndex);

	std::size_t              m_numSampleBatches;
	std::size_t              m_maxCachedBatches;
	std::size_t              m_numUsedBatches;
	std::size_t              m_numUsedCaches;
	std::size_t              m_numDeclaredDims;
	std::size_t              m_totalBufferSize;
	std::vector<real>        m_sampleBuffer;
	std::vector<SampleStage> m_stages;
#if PH_DEBUG
	bool                     m_isSampleBatchPrepared;
#endif
};

// In-header Implementations:

inline std::size_t SampleGenerator::numSampleBatches() const
{
	return m_numSampleBatches;
}

inline std::size_t SampleGenerator::maxCachedBatches() const
{
	return m_maxCachedBatches;
}

inline std::size_t SampleGenerator::numRemainingBatches() const
{
	PH_ASSERT_LE(m_numUsedBatches, m_numSampleBatches);

	return m_numSampleBatches - m_numUsedBatches;
}

inline bool SampleGenerator::hasMoreBatches() const
{
	return m_numUsedBatches < m_numSampleBatches;
}

}// end namespace ph
