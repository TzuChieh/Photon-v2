#pragma once

#include "Common/primitive_type.h"
#include "Core/Sample.h"
#include "Math/TVector2.h"
#include "Math/TArithmeticArray.h"
#include "Core/SampleGenerator/SampleStage.h"
#include "Core/SampleGenerator/SamplesNDHandle.h"
#include "Core/SampleGenerator/SamplesND.h"
#include "Core/SampleGenerator/SamplesNDStream.h"
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
	SampleGenerator(std::size_t numSampleBatches, std::size_t numCachedBatches);
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

	SamplesNDStream getSamplesND(const SamplesNDHandle& handle);

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
