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
	virtual ~SampleGenerator();

	void genSplitted(std::size_t numSplits,
	                 std::vector<std::unique_ptr<SampleGenerator>>& out_sgs) const;
	std::unique_ptr<SampleGenerator> genCopied() const;

	bool prepareSampleBatch();

	Samples1DStage declare1DStage(std::size_t numElements);
	Samples2DStage declare2DStage(std::size_t numElements);
	SamplesNDStage declareNDStage(std::size_t numElements);

	Samples1D getSamples1D(const Samples1DStage& stage);
	Samples2D getSamples2D(const Samples2DStage& stage);
	SamplesND getSamplesND(const SamplesNDStage& stage);

	std::size_t numSampleBatches() const;
	std::size_t numCachedBatches() const;

private:
	class StageData
	{
	public:
		std::vector<real> data;
		std::size_t       head;
		std::size_t       numElements;
		std::size_t       dimension;

		inline std::size_t numStageReals() const { return numElements * dimension; }
	};

	std::size_t m_numSampleBatches;
	std::size_t m_numCachedBatches;
	std::size_t m_currentBatchNumber;

	std::vector<StageData> m_stageDataArray;

	virtual std::unique_ptr<SampleGenerator> genNewborn(std::size_t numSamples) const = 0;
	virtual void genSamples1D(Samples1D* out_array) = 0;
	virtual void genSamples2D(Samples2D* out_array) = 0;
	virtual void genSamplesND(SamplesND* out_array) = 0;

	void alloc1DStage(std::size_t numElements, uint32* out_stageIndex);
	void alloc2DStage(std::size_t numElements, uint32* out_stageIndex);
	void allocNDStage(std::size_t numElements, uint32* out_stageIndex);
	void genSampleBatch();
	void genSampleBatch1D(StageData& out_stage);
	void genSampleBatch2D(StageData& out_stage);
	void genSampleBatchND(StageData& out_stage);
	bool canSplit(std::size_t numSplits) const;

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

}// end namespace ph