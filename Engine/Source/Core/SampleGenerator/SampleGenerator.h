#pragma once

#include "Common/primitive_type.h"
#include "Core/Sample.h"
#include "Math/TVector2.h"
#include "Core/SampleGenerator/stages.h"
#include "FileIO/SDL/ISdlResource.h"
#include "FileIO/SDL/TCommandInterface.h"
#include "Core/SampleGenerator/samples.h"

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
	SampleGenerator(std::size_t numSampleBatches, std::size_t numCachedBatches);
	virtual ~SampleGenerator() = default;

	void genSplitted(std::size_t numSplits,
	                 std::vector<std::unique_ptr<SampleGenerator>>& out_sgs) const;
	std::unique_ptr<SampleGenerator> genCopied(std::size_t numSampleBatches) const;

	bool prepareSampleBatch();

	Samples1DStage declare1DStage(std::size_t numElements);
	Samples2DStage declare2DStage(
		std::size_t numElements, 
		const math::Vector2S& dimSizeHints = {1, 1});
	SamplesNDStage declareNDStage(
		std::size_t numElements, 
		const std::vector<std::size_t>& dimSizeHints = {});

	// TODO: these three methods can use a common helper method (input SamplesStageBase)
	Samples1D getSamples1D(const Samples1DStage& stage);
	Samples2D getSamples2D(const Samples2DStage& stage);
	SamplesND getSamplesND(const SamplesNDStage& stage);

	std::size_t numSampleBatches() const;
	std::size_t numCachedBatches() const;
	bool        hasMoreBatches()   const;

private:
	virtual std::unique_ptr<SampleGenerator> genNewborn(std::size_t numSamples) const = 0;
	virtual void genSamples1D(const Samples1DStage& stage, Samples1D* out_array) = 0;
	virtual void genSamples2D(const Samples2DStage& stage, Samples2D* out_array) = 0;
	virtual void genSamplesND(const SamplesNDStage& stage, SamplesND* out_array) = 0;

	std::size_t m_numSampleBatches;
	std::size_t m_numCachedBatches;
	std::size_t m_numUsedBatches;
	std::size_t m_numUsedCaches;
	std::size_t m_totalElements;

	std::vector<real>           m_sampleBuffer;
	std::vector<Samples1DStage> m_1DStages;
	std::vector<Samples2DStage> m_2DStages;
	std::vector<SamplesNDStage> m_NDStages;

	void allocSampleBuffer();
	void genSampleBatch();
	void genSamples1DBatch();
	void genSamples2DBatch();
	void genSamplesNDBatch();

// command interface
public:
	//SampleGenerator(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

// In-header Implementations:

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
