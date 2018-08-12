#pragma once

#include "Common/primitive_type.h"
#include "Core/Sample.h"
#include "Math/TVector2.h"
#include "Core/SampleGenerator/TSampleStage.h"
#include "FileIO/SDL/ISdlResource.h"
#include "FileIO/SDL/TCommandInterface.h"
#include "Core/SampleGenerator/sample_array.h"

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

	TSampleStage<real>     declare1DStage(std::size_t numElements);
	TSampleStage<Vector2R> declare2DStage(std::size_t numElements);
	TSampleStage<SampleArray1D> declareArray1DStage(std::size_t numElements);
	TSampleStage<SampleArray2D> declareArray2DStage(std::size_t numElements);

	real     getNext1D(const TSampleStage<real>&     stage);
	Vector2R getNext2D(const TSampleStage<Vector2R>& stage);
	SampleArray1D getNextArray1D(const TSampleStage<SampleArray1D>& stage);
	SampleArray2D getNextArray2D(const TSampleStage<SampleArray2D>& stage);

	inline std::size_t numSampleBatches() const
	{
		return m_numSampleBatches;
	}

	inline std::size_t numCachedBatches() const
	{
		return m_numCachedBatches;
	}

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
	virtual void genArray1D(SampleArray1D* out_array) = 0;
	virtual void genArray2D(SampleArray2D* out_array) = 0;

	void alloc1DStage(std::size_t numElements, uint32* out_stageIndex);
	void alloc2DStage(std::size_t numElements, uint32* out_stageIndex);
	void genSampleBatch();
	void genSampleBatch1D(StageData& out_stage);
	void genSampleBatch2D(StageData& out_stage);
	bool canSplit(std::size_t numSplits) const;

// command interface
public:
	//SampleGenerator(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph