#include "Core/SampleGenerator/SampleGenerator.h"
#include "FileIO/InputPacket.h"
#include "Math/Random.h"

#include <iostream>

namespace ph
{

SampleGenerator::SampleGenerator(const std::size_t numSamples, 
                                 const std::size_t sampleBatchSize) :
	m_numSamples(numSamples), 
	m_sampleBatchSize(std::min(numSamples, sampleBatchSize)),
	m_sampleHead(0)
{
	
}

SampleGenerator::~SampleGenerator() = default;

bool SampleGenerator::singleSampleStart()
{
	const bool hasMoreSamples = m_sampleHead < m_numSamples;

	//std::cerr << "singleSampleStart" << std::endl;

	if(hasMoreSamples && m_sampleHead % m_sampleBatchSize == 0)
	{
		//std::cerr << "cond" << std::endl;

		for(auto& phase1D : m_perPhase1Ds)
		{
			phase1D.head = 0;
			genArray1D(phase1D.data.data(), phase1D.data.size(), phase1D.type);
		}

		for(auto& phase2D : m_perPhase2Ds)
		{
			phase2D.head = 0;
			genArray2D(phase2D.data.data(), phase2D.data.size(), phase2D.type);
		}
	}

	return hasMoreSamples;
}

void SampleGenerator::singleSampleEnd()
{
	m_sampleHead++;
}

real SampleGenerator::getNext1D(const TSamplePhase<real>& phase)
{
	real coord1D;
	auto& data = m_perPhase1Ds[phase.m_phaseIndex].data;
	auto& head = m_perPhase1Ds[phase.m_phaseIndex].head;

	if(head < data.size())
	{
		coord1D = data[head++];
	}
	else
	{
		coord1D = Random::genUniformReal_i0_e1();
	}

	return coord1D;
}

Vector2R SampleGenerator::getNext2D(const TSamplePhase<Vector2R>& phase)
{
	Vector2R coord2D;
	auto& data = m_perPhase1Ds[phase.m_phaseIndex].data;
	auto& head = m_perPhase1Ds[phase.m_phaseIndex].head;

	if(head + 1 < data.size())
	{
		coord2D.x = data[head];
		coord2D.y = data[head + 1];
		head += 2;
	}
	else
	{
		coord2D.x = Random::genUniformReal_i0_e1();
		coord2D.y = Random::genUniformReal_i0_e1();
	}

	return coord2D;
}

const real* SampleGenerator::getNextArray1D(const TSamplePhase<const real*>& phase)
{
	auto& data = m_perPhase1Ds[phase.m_phaseIndex].data;
	auto& head = m_perPhase1Ds[phase.m_phaseIndex].head;

	if(head + phase.m_numElements - 1 < data.size())
	{
		const real* arrayHead = &data[head];
		head += phase.m_numElements;
		return arrayHead;
	}
	else
	{
		return nullptr;
	}
}

const Vector2R* SampleGenerator::getNextArray2D(const TSamplePhase<const Vector2R*>& phase)
{
	auto& data = m_perPhase2Ds[phase.m_phaseIndex].data;
	auto& head = m_perPhase2Ds[phase.m_phaseIndex].head;

	if(head + phase.m_numElements - 1 < data.size())
	{
		const Vector2R* arrayHead = &data[head];
		head += phase.m_numElements;
		return arrayHead;
	}
	else
	{
		return nullptr;
	}
}

TSamplePhase<real> SampleGenerator::declare1DPhase(const EPhaseType type)
{
	uint32 phaseIndex;
	alloc1DPhase(1, type, &phaseIndex);

	return TSamplePhase<real>(phaseIndex, 1);
}

TSamplePhase<Vector2R> SampleGenerator::declare2DPhase(const EPhaseType type)
{
	uint32 phaseIndex;
	alloc2DPhase(1, type, &phaseIndex);

	return TSamplePhase<Vector2R>(phaseIndex, 1);
}

TSamplePhase<const real*> SampleGenerator::declareArray1DPhase(const std::size_t numElements,
                                                               const EPhaseType type)
{
	uint32 phaseIndex;
	alloc1DPhase(numElements, type, &phaseIndex);

	return TSamplePhase<const real*>(phaseIndex, numElements);
}

TSamplePhase<const Vector2R*> SampleGenerator::declareArray2DPhase(const std::size_t numElements,
                                                                   const EPhaseType type)
{
	uint32 phaseIndex;
	alloc2DPhase(numElements, type, &phaseIndex);

	return TSamplePhase<const Vector2R*>(phaseIndex, numElements);
}

void SampleGenerator::alloc1DPhase(const std::size_t numElements,
                                   const EPhaseType type, 
                                   uint32* const out_phaseIndex)
{
	*out_phaseIndex = static_cast<uint32>(m_perPhase1Ds.size());

	TPhaseInfo<real> phaseInfo;
	phaseInfo.data.resize(m_sampleBatchSize * numElements);
	phaseInfo.head = 0;
	phaseInfo.type = type;
	m_perPhase1Ds.push_back(phaseInfo);
}

void SampleGenerator::alloc2DPhase(const std::size_t numElements,
                                   const EPhaseType type, 
                                   uint32* const out_phaseIndex)
{
	*out_phaseIndex = static_cast<uint32>(m_perPhase2Ds.size());

	TPhaseInfo<Vector2R> phaseInfo;
	phaseInfo.data.resize(m_sampleBatchSize * numElements);
	phaseInfo.head = 0;
	phaseInfo.type = type;
	m_perPhase2Ds.push_back(phaseInfo);
}

// command interface

SampleGenerator::SampleGenerator(const InputPacket& packet) :
	SampleGenerator(0, 0)
{
	m_numSamples = packet.getInteger("samples", 0, DataTreatment::REQUIRED());
	m_sampleBatchSize = m_numSamples;
}

SdlTypeInfo SampleGenerator::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_SAMPLER, "sample-generator");
}

ExitStatus SampleGenerator::ciExecute(const std::shared_ptr<SampleGenerator>& targetResource, 
                                      const std::string& functionName, 
                                      const InputPacket& packet)
{
	return ExitStatus::UNSUPPORTED();
}

}// end namespace ph