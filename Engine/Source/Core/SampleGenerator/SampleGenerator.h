#pragma once

#include "Common/primitive_type.h"
#include "Core/Sample.h"
#include "Math/TVector2.h"
#include "Core/SampleGenerator/stages.h"
#include "Core/SampleGenerator/TSamplesNDStage.h"
#include "Core/SampleGenerator/TSamplesND.h"
#include "Math/TArithmeticArray.h"
#include "FileIO/SDL/ISdlResource.h"
#include "FileIO/SDL/TCommandInterface.h"

#include <cstddef>
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
	template<std::size_t N>
	using SizeHints = typename TSamplesNDStage<N>::SizeHints;

	SampleGenerator(std::size_t numSampleBatches, std::size_t numCachedBatches);
	virtual ~SampleGenerator() = default;

	void genSplitted(std::size_t numSplits,
	                 std::vector<std::unique_ptr<SampleGenerator>>& out_sgs) const;
	std::unique_ptr<SampleGenerator> genCopied(std::size_t numSampleBatches) const;

	bool prepareSampleBatch();

	template<std::size_t N>
	TSamplesNDStage<N> declareNDStage(
		std::size_t  numSamples,
		SizeHints<N> dimSizeHints = SizeHints<N>(1));

	template<std::size_t N>
	TSamplesND<N> getSamplesND(const TSamplesNDStage<N>& stage);

	std::size_t numSampleBatches() const;
	std::size_t numCachedBatches() const;
	bool hasMoreBatches() const;

protected:
	struct SampleStage
	{
		std::size_t              stageIndex;
		std::size_t              numDimensions;
		std::size_t              numSamples;
		std::vector<std::size_t> dimSizeHints;

		template<std::size_t N>
		TSamplesNDStage<N> toStaticStage() const;
	};

private:
	virtual std::unique_ptr<SampleGenerator> genNewborn(std::size_t numSamples) const = 0;
	virtual void genSamples1D(const Samples1DStage& stage, Samples1D* out_array) = 0;
	virtual void genSamples2D(const Samples2DStage& stage, Samples2D* out_array) = 0;
	virtual void genSamplesND(const SampleStage& stage, SamplesND* out_array) = 0;

	std::size_t              m_numSampleBatches;
	std::size_t              m_numCachedBatches;
	std::size_t              m_numUsedBatches;
	std::size_t              m_numUsedCaches;
	std::size_t              m_totalElements;
	std::vector<real>        m_sampleBuffer;
	std::vector<SampleStage> m_stages;

	void allocSampleBuffer();
	void genSampleBatch();
	void genSamples1DBatch();
	void genSamples2DBatch();
	void genSamplesNDBatch();

// command interface
public:
	//explicit SampleGenerator(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

// In-header Implementations:

template<std::size_t N>
inline TSamplesNDStage<N> SampleGenerator::declareNDStage(
	const std::size_t  numSamples,
	const SizeHints<N> dimSizeHints = SizeHints<N>(1))
{
	SampleStage stage;
	stage.stageIndex    = m_totalElements;
	stage.numDimensions = N;
	stage.numSamples    = numSamples;
	for(const auto dimSize : dimSizeHints)
	{
		stage.dimSizeHints.push_back(dimSize);
	}
	m_stages.push_back(stage);

	TSamplesNDStage<N> staticStage = stage.toStaticStage();

	m_totalElements += m_numCachedBatches * staticStage.numElements();

	return std::move(staticStage);
}

template<std::size_t N>
inline TSamplesND<N> SampleGenerator::getSamplesND(const TSamplesNDStage<N>& stage)
{
	PH_ASSERT_NE(m_numUsedCaches, 0);
	PH_ASSERT_LE(stage.getStageIndex() + m_numUsedCaches * stage.numElements(), m_sampleBuffer.size());

	// TODO: probably should make batch buffers closer to each other
	return TSamplesND<N>(
		&(m_sampleBuffer[(m_numUsedCaches - 1) * stage.numElements() + stage.getStageIndex()]),
		stage.numSamples());
}

template<std::size_t N>
inline TSamplesNDStage<N> SampleGenerator::SampleStage::toStaticStage() const
{
	PH_ASSERT_EQ(dimSizeHints.size(), N);

	SizeHints<N> sizeHints;
	for(std::size_t i = 0; i < N; ++i)
	{
		sizeHints[i] = dimSizeHints[i];
	}

	return TSamplesNDStage<N>(stageIndex, numSamples, std::move(sizeHints));
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
