#include "Core/SampleGenerator/SampleGenerator.h"
#include "FileIO/InputPacket.h"
#include "Math/Random.h"

#include <iostream>

namespace ph
{

SampleGenerator::SampleGenerator(const std::size_t numSamples, 
                                 const std::size_t sampleBatchSize) :
	m_numSamples(numSamples), 
	m_sampleBatchSize(sampleBatchSize),
	m_sampleHead(0)
{
	
}

SampleGenerator::~SampleGenerator() = default;

bool SampleGenerator::singleSampleStart()
{
	const bool hasMoreSamples = m_sampleHead < m_numSamples;

	if(hasMoreSamples && m_sampleHead % m_sampleBatchSize == 0)
	{
		for(auto& phase : m_phaseDataArray)
		{
			phase.head = 0;
			if(phase.dimension == 1)
			{
				for(std::size_t b = 0; b < m_sampleBatchSize; b++)
				{
					SampleArray1D arrayProxy(&phase.data[b * phase.numElements * 1], phase.numElements);
					genArray1D(&arrayProxy);
				}
			}
			else if(phase.dimension == 2)
			{
				for(std::size_t b = 0; b < m_sampleBatchSize; b++)
				{
					SampleArray2D arrayProxy(&phase.data[b * phase.numElements * 2], phase.numElements);
					genArray2D(&arrayProxy);
				}
			}
			else
			{
				std::cerr << "warning: at SampleGenerator::singleSampleStart(), "
				          << "unsupported dimension number detected: " << phase.dimension << std::endl;
			}
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
	auto& phaseData = m_phaseDataArray[phase.m_phaseIndex];
	auto& data      = phaseData.data;
	auto& head      = phaseData.head;

	real coord1D;
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
	auto& phaseData = m_phaseDataArray[phase.m_phaseIndex];
	auto& data      = phaseData.data;
	auto& head      = phaseData.head;

	Vector2R coord2D;
	if(head + 1 < data.size())
	{
		coord2D.x = data[head++];
		coord2D.y = data[head++];
	}
	else
	{
		coord2D.x = Random::genUniformReal_i0_e1();
		coord2D.y = Random::genUniformReal_i0_e1();
	}

	return coord2D;
}

SampleArray1D SampleGenerator::getNextArray1D(const TSamplePhase<SampleArray1D>& phase)
{
	auto& phaseData   = m_phaseDataArray[phase.m_phaseIndex];
	auto& data        = phaseData.data;
	auto& head        = phaseData.head;
	auto& numElements = phaseData.numElements;

	if(head + numElements - 1 < data.size())
	{
		real* arrayHead = &data[head];
		head += numElements;
		return SampleArray1D(arrayHead, numElements);
	}
	else
	{
		return SampleArray1D();
	}
}

SampleArray2D SampleGenerator::getNextArray2D(const TSamplePhase<SampleArray2D>& phase)
{
	auto& phaseData   = m_phaseDataArray[phase.m_phaseIndex];
	auto& data        = phaseData.data;
	auto& head        = phaseData.head;
	auto& numElements = phaseData.numElements;

	if(head + numElements * 2 - 1 < data.size())
	{
		real* arrayHead = &data[head];
		head += numElements * 2;
		return SampleArray2D(arrayHead, numElements);
	}
	else
	{
		return SampleArray2D();
	}
}

TSamplePhase<real> SampleGenerator::declare1DPhase(const std::size_t numElements)
{
	return TSamplePhase<real>(declareArray1DPhase(numElements).m_phaseIndex);
}

TSamplePhase<Vector2R> SampleGenerator::declare2DPhase(const std::size_t numElements)
{
	return TSamplePhase<Vector2R>(declareArray2DPhase(numElements).m_phaseIndex);
}

TSamplePhase<SampleArray1D> SampleGenerator::declareArray1DPhase(const std::size_t numElements)
{
	uint32 phaseIndex;
	alloc1DPhase(numElements, &phaseIndex);

	return TSamplePhase<SampleArray1D>(phaseIndex);
}

TSamplePhase<SampleArray2D> SampleGenerator::declareArray2DPhase(const std::size_t numElements)
{
	uint32 phaseIndex;
	alloc2DPhase(numElements, &phaseIndex);

	return TSamplePhase<SampleArray2D>(phaseIndex);
}

void SampleGenerator::alloc1DPhase(const std::size_t numElements, 
                                   uint32* const out_phaseIndex)
{
	*out_phaseIndex = static_cast<uint32>(m_phaseDataArray.size());

	PhaseData phaseData;
	phaseData.data.resize(m_sampleBatchSize * numElements);
	phaseData.head = 0;
	phaseData.numElements = numElements;
	phaseData.dimension = 1;
	m_phaseDataArray.push_back(phaseData);
}

void SampleGenerator::alloc2DPhase(const std::size_t numElements, 
                                   uint32* const out_phaseIndex)
{
	*out_phaseIndex = static_cast<uint32>(m_phaseDataArray.size());

	PhaseData phaseData;
	phaseData.data.resize(m_sampleBatchSize * numElements * 2);
	phaseData.head = 0;
	phaseData.numElements = numElements;
	phaseData.dimension = 2;
	m_phaseDataArray.push_back(phaseData);
}

// command interface

SdlTypeInfo SampleGenerator::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_SAMPLE_GENERATOR, "sample-generator");
}

ExitStatus SampleGenerator::ciExecute(const std::shared_ptr<SampleGenerator>& targetResource, 
                                      const std::string& functionName, 
                                      const InputPacket& packet)
{
	return ExitStatus::UNSUPPORTED();
}

}// end namespace ph