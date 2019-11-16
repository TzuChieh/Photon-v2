#pragma once

#include "Common/primitive_type.h"
#include "Core/Sample.h"
#include "Math/TVector2.h"
#include "Math/TArithmeticArray.h"
#include "Core/SampleGenerator/SampleStage.h"
#include "Core/SampleGenerator/TSamplesNDHandle.h"
#include "Core/SampleGenerator/TSamplesND.h"
#include "Math/TArithmeticArray.h"
#include "FileIO/SDL/ISdlResource.h"
#include "FileIO/SDL/TCommandInterface.h"

#include <cstddef>
#include <utility>
#include <vector>
#include <memory>
#include <utility>

namespace ph
{

class Scene;
class InputPacket;

class SampleGenerator : public TCommandInterface<SampleGenerator>
{
public:
	using SizeHints = SampleStage::SizeHints;

	SampleGenerator(std::size_t numSampleBatches, std::size_t numCachedBatches);
	explicit SampleGenerator(std::size_t numSampleBatches);
	virtual ~SampleGenerator() = default;

	void genSplitted(std::size_t numSplits,
	                 std::vector<std::unique_ptr<SampleGenerator>>& out_sgs) const;
	std::unique_ptr<SampleGenerator> genCopied(std::size_t numSampleBatches) const;

	bool prepareSampleBatch();

	template<std::size_t N>
	TSamplesNDHandle<N> declareStageND(
		std::size_t numSamples,
		SizeHints   dimSizeHints = SampleStage::makeIdentityDimSizeHints());

	template<std::size_t N>
	TSamplesND<N> getSamplesND(const TSamplesNDHandle<N>& handle);

	std::size_t numSampleBatches() const;
	std::size_t numCachedBatches() const;
	bool hasMoreBatches() const;

private:
	virtual std::unique_ptr<SampleGenerator> genNewborn(std::size_t numSamples) const = 0;
	virtual void genSamples(const SampleStage& stage, real* out_buffer) = 0;

	std::size_t              m_numSampleBatches;
	std::size_t              m_numCachedBatches;
	std::size_t              m_numUsedBatches;
	std::size_t              m_numUsedCaches;
	std::size_t              m_totalElements;
	std::vector<real>        m_sampleBuffer;
	std::vector<SampleStage> m_stages;

	void allocSampleBuffer();
	void genSampleBatch();

// command interface
public:
	//explicit SampleGenerator(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

// In-header Implementations:

template<std::size_t N>
inline TSamplesNDHandle<N> SampleGenerator::declareStageND(
	const std::size_t numSamples,
	SizeHints         dimSizeHints)
{
	const std::size_t sampleIndex = m_totalElements;
	const std::size_t stageIndex  = m_stages.size();

	const SampleStage stage(
		sampleIndex,
		N, 
		numSamples, 
		dimSizeHints);

	m_stages.push_back(stage);
	m_totalElements += m_numCachedBatches * stage.numElements();

	return TSamplesNDHandle<N>(stageIndex);
}

template<std::size_t N>
inline TSamplesND<N> SampleGenerator::getSamplesND(const TSamplesNDHandle<N>& handle)
{
	PH_ASSERT_NE(m_numUsedCaches, 0);
	PH_ASSERT_LT(handle.getStageIndex(), m_numUsedCaches);

	const SampleStage& stage = m_stages[handle.getStageIndex()];
	PH_ASSERT_LE(stage.getSampleIndex() + m_numUsedCaches * stage.numElements(), m_sampleBuffer.size());

	// TODO: probably should make batch buffers closer to each other
	return TSamplesND<N>(
		&(m_sampleBuffer[stage.getSampleIndex() + (m_numUsedCaches - 1) * stage.numElements()]),
		stage.numSamples());
}

inline std::size_t SampleGenerator::numSampleBatches() const
{
	return m_numSampleBatches;
}

inline std::size_t SampleGenerator::numCachedBatches() const
{
	return m_numCachedBatches;
}

inline bool SampleGenerator::hasMoreBatches() const
{
	return m_numUsedBatches < m_numSampleBatches;
}

}// end namespace ph

/*
	<SDL_interface>

	<category>  sample-generator </category>
	<type_name> sample-generator </type_name>

	<name> Sample Generator </name>
	<description>
		Engine component for generating sample values.
	</description>

	</SDL_interface>
*/
