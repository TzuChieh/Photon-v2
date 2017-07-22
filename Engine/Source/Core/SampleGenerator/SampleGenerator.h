#pragma once

#include "Common/primitive_type.h"
#include "Core/Sample.h"
#include "Math/TVector2.h"
#include "Core/SampleGenerator/TSamplePhase.h"
#include "Core/SampleGenerator/EPhaseType.h"
#include "FileIO/SDL/ISdlResource.h"
#include "FileIO/SDL/TCommandInterface.h"

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

	virtual void genSplitted(uint32 numSplits, 
	                         std::vector<std::unique_ptr<SampleGenerator>>& out_sgs) = 0;

	bool singleSampleStart();
	void singleSampleEnd();

	TSamplePhase<real>     declare1DPhase(EPhaseType type = EPhaseType::ANY);
	TSamplePhase<Vector2R> declare2DPhase(EPhaseType type = EPhaseType::ANY);

	TSamplePhase<const real*>     declareArray1DPhase(std::size_t numElements,
	                                                  EPhaseType type = EPhaseType::ANY);
	TSamplePhase<const Vector2R*> declareArray2DPhase(std::size_t numElements,
	                                                  EPhaseType type = EPhaseType::ANY);

	real     getNext1D(const TSamplePhase<real>&     phase);
	Vector2R getNext2D(const TSamplePhase<Vector2R>& phase);

	const real*     getNextArray1D(const TSamplePhase<const real*>&     phase);
	const Vector2R* getNextArray2D(const TSamplePhase<const Vector2R*>& phase);

	inline std::size_t numSamples() const
	{
		return m_numSamples;
	}

	inline std::size_t sampleBatchSize() const
	{
		return m_sampleBatchSize;
	}

private:
	template<typename T>
	class TPhaseInfo
	{
	public:
		std::vector<T> data;
		std::size_t    head;
		EPhaseType     type;
	};

	std::size_t m_numSamples;
	std::size_t m_sampleBatchSize;
	std::size_t m_sampleHead;

	std::vector<TPhaseInfo<real>>     m_perPhase1Ds;
	std::vector<TPhaseInfo<Vector2R>> m_perPhase2Ds;

	virtual void genArray1D(real* coordArray1Ds,
	                        std::size_t num1Ds, 
	                        EPhaseType type) = 0;
	virtual void genArray2D(Vector2R* coordArray2Ds,
	                        std::size_t num2Ds, 
	                        EPhaseType type) = 0;

	void alloc1DPhase(std::size_t numElements, EPhaseType type, uint32* out_phaseIndex);
	void alloc2DPhase(std::size_t numElements, EPhaseType type, uint32* out_phaseIndex);

// command interface
public:
	SampleGenerator(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static ExitStatus ciExecute(const std::shared_ptr<SampleGenerator>& targetResource, 
	                            const std::string& functionName, 
	                            const InputPacket& packet);
};

}// end namespace ph