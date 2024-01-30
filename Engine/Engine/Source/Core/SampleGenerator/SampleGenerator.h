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

/*! @brief Generates samples in [0, 1].
Generators of different types produces samples with different properties. Depending on the
implementation, the samples can be fully random, quasi random, or even fully deterministic.
Sample generators in Photon have their life cycles. Life cycle begins when the sample generator
start producing sample batches, and ends when all sample batches are exhausted.
*/
class SampleGenerator
{
public:
	SampleGenerator(std::size_t numSampleBatches, std::size_t maxCachedBatches);
	explicit SampleGenerator(std::size_t numSampleBatches);
	virtual ~SampleGenerator() = default;

	/*! @brief Make a new generator.
	@return The new generator. Newborn generator may not have the same internal state as its ancestor.
	*/
	virtual std::unique_ptr<SampleGenerator> makeNewborn(std::size_t numSampleBatches) const = 0;

	/*! @brief Reset this generator to its initial state, reviving it from dead state.
	Calling this method starts a new life cycle--all existing sample stages and values are lost,
	and references to them are invalidated. New declarations can be made after, and new sample
	values can be generated.
	@note The initial state of a generator may be different each time `rebirth()` is called. This
	depends on the actual settings used for the generator.
	*/
	virtual void rebirth();

	void genSplitted(
		std::size_t numSplits,
		std::vector<std::unique_ptr<SampleGenerator>>& out_sgs) const;

	/*! @brief Generates sample values for current sample batch.
	Advances all sample values in all stages to the next batch. Will potentially generate sample
	values for more than one batch at once (cached).
	@return Whether sample values are generated.
	*/
	bool prepareSampleBatch();

	/*! @brief Similar to `declareStageND()`, with default size hints.
	*/
	SamplesNDHandle declareStageND(std::size_t numDims, std::size_t numSamples);

	/*! @brief Declares a N-dimensional sample stage.
	A sample stage contains information for how the samples will be used, number of samples, etc.
	These information can help the sample generator to produce sample values of better quality.
	This method can only be called before sample batches are generated (before any call to
	`prepareSampleBatch()`).
	*/
	SamplesNDHandle declareStageND(
		std::size_t              numDims, 
		std::size_t              numSamples, 
		std::vector<std::size_t> dimSizeHints);

	/*! @brief Gets generated N-dimensional sample values in a stage.
	This method can only be called after sample batches are generated (after any call to
	`prepareSampleBatch()`).
	*/
	SamplesNDStream getSamplesND(const SamplesNDHandle& handle) const;

	std::size_t numSampleBatches() const;
	std::size_t maxCachedBatches() const;
	std::size_t numRemainingBatches() const;

	/*!
	@return Whether the generator has more batches to generate. If `true`, `prepareSampleBatch()` will
	return `true` also. If `false`, the generator's life cycle has ended--all samples are exhausted and
	cannot produce new samples.
	*/
	bool hasMoreBatches() const;

private:
	/*! @brief Called after `rebirth()` is called.
	*/
	virtual void onRebirth() = 0;

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
	void reset();

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
