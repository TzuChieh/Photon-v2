#pragma once

#include "Common/primitive_type.h"
#include "Core/Sample.h"
#include "Math/TVector2.h"
#include "Core/SampleGenerator/TSamplePhase.h"
#include "FileIO/SDL/ISdlResource.h"
#include "FileIO/SDL/TCommandInterface.h"
#include "Core/SampleGenerator/sample_array.h"

#include <vector>
#include <memory>
#include <utility>

namespace ph
{

class Film;
class Scene;
class InputPacket;

class SampleGenerator : public ISdlResource, TCommandInterface<SampleGenerator>
{
public:
	SampleGenerator(std::size_t numSamples, std::size_t sampleBatchSize);
	virtual ~SampleGenerator() = 0;

	void genSplitted(std::size_t numSplits,
	                 std::vector<std::unique_ptr<SampleGenerator>>& out_sgs) const;
	std::unique_ptr<SampleGenerator> genCopied() const;

	bool singleSampleStart();
	void singleSampleEnd();

	TSamplePhase<real>     declare1DPhase(std::size_t numElements);
	TSamplePhase<Vector2R> declare2DPhase(std::size_t numElements);
	TSamplePhase<SampleArray1D> declareArray1DPhase(std::size_t numElements);
	TSamplePhase<SampleArray2D> declareArray2DPhase(std::size_t numElements);

	real     getNext1D(const TSamplePhase<real>&     phase);
	Vector2R getNext2D(const TSamplePhase<Vector2R>& phase);
	SampleArray1D getNextArray1D(const TSamplePhase<SampleArray1D>& phase);
	SampleArray2D getNextArray2D(const TSamplePhase<SampleArray2D>& phase);

	inline std::size_t numSamples() const
	{
		return m_numSamples;
	}

	inline std::size_t sampleBatchSize() const
	{
		return m_sampleBatchSize;
	}

private:
	class PhaseData
	{
	public:
		std::vector<real> data;
		std::size_t       head;
		std::size_t       numElements;
		std::size_t       dimension;

		inline std::size_t numPhaseReals() const { return numElements * dimension; }
	};

	std::size_t m_numSamples;
	std::size_t m_sampleBatchSize;
	std::size_t m_sampleHead;

	std::vector<PhaseData> m_phaseDataArray;

	virtual std::unique_ptr<SampleGenerator> genNewborn(std::size_t numSamples) const = 0;
	virtual void genArray1D(SampleArray1D* out_array) = 0;
	virtual void genArray2D(SampleArray2D* out_array) = 0;

	void alloc1DPhase(std::size_t numElements, uint32* out_phaseIndex);
	void alloc2DPhase(std::size_t numElements, uint32* out_phaseIndex);
	void genSampleBatch();
	void genSampleBatch1D(PhaseData& out_phase);
	void genSampleBatch2D(PhaseData& out_phase);
	bool canSplit(std::size_t numSplits) const;

// command interface
public:
	//SampleGenerator(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static ExitStatus ciExecute(const std::shared_ptr<SampleGenerator>& targetResource, 
	                            const std::string& functionName, 
	                            const InputPacket& packet);
};

}// end namespace ph