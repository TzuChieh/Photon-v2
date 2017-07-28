#include "Core/SampleGenerator/SGStratified.h"
#include "Core/Filmic/Film.h"
#include "Core/Sample.h"
#include "Math/Random.h"
#include "FileIO/InputPacket.h"

#include <iostream>

namespace ph
{

SGStratified::SGStratified(const std::size_t numSamples,
                           const std::size_t numStrata2dX,
                           const std::size_t numStrata2dY) :
	//SampleGenerator(numSamples, numSamples)
	SampleGenerator(numSamples, 4), // HACK
	m_numStrata2dX(numStrata2dX), m_numStrata2dY(numStrata2dY)
{

}

SGStratified::~SGStratified() = default;

void SGStratified::genArray1D(SampleArray1D* const out_array)
{
	for(std::size_t i = 0; i < out_array->numElements(); i++)
	{
		const real jitter = Random::genUniformReal_i0_e1();
		out_array->set(i, i + jitter);
	}

	out_array->perElementShuffle();
}

void SGStratified::genArray2D(SampleArray2D* const out_array)
{
	const std::size_t numStrata = m_numStrata2dX * m_numStrata2dY;
	if(out_array->numElements() >= numStrata)
	{
		const real dx = 1.0_r / static_cast<real>(m_numStrata2dX);
		const real dy = 1.0_r / static_cast<real>(m_numStrata2dY);
		for(std::size_t y = 0; y < m_numStrata2dY; y++)
		{
			const std::size_t baseIndex = y * m_numStrata2dX;
			for(std::size_t x = 0; x < m_numStrata2dX; x++)
			{
				const real jitterX = Random::genUniformReal_i0_e1();
				const real jitterY = Random::genUniformReal_i0_e1();
				out_array->set(baseIndex + x, 
				               (static_cast<real>(x) + jitterX) * dx,
				               (static_cast<real>(y) + jitterY) * dy);
			}
		}

		for(std::size_t i = numStrata; i < out_array->numElements(); i++)
		{
			out_array->set(i,
			               Random::genUniformReal_i0_e1(),
			               Random::genUniformReal_i0_e1());
		}

		out_array->perElementShuffle();
	}
	else
	{
		for(std::size_t i = 0; i < out_array->numElements(); i++)
		{
			out_array->set(i, 
			               Random::genUniformReal_i0_e1(), 
			               Random::genUniformReal_i0_e1());
		}
	}
}

std::unique_ptr<SampleGenerator> SGStratified::genNewborn(const std::size_t numSamples) const
{
	return std::make_unique<SGStratified>(numSamples,
	                                      m_numStrata2dX,
	                                      m_numStrata2dY);
}

// command interface

SdlTypeInfo SGStratified::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_SAMPLE_GENERATOR, "stratified");
}

std::unique_ptr<SGStratified> SGStratified::ciLoad(const InputPacket& packet)
{
	const integer numSamples   = packet.getInteger("sample-amount",   0, DataTreatment::REQUIRED());
	const integer numStrata2dX = packet.getInteger("num-strata-2d-x", 0, DataTreatment::REQUIRED());
	const integer numStrata2dY = packet.getInteger("num-strata-2d-y", 0, DataTreatment::REQUIRED());

	// HACK: casting
	return std::make_unique<SGStratified>(static_cast<std::size_t>(numSamples), 
		static_cast<std::size_t>(numStrata2dX),
		static_cast<std::size_t>(numStrata2dY));
}

ExitStatus SGStratified::ciExecute(const std::shared_ptr<SGStratified>& targetResource,
                                   const std::string& functionName, 
                                   const InputPacket& packet)
{
	return ExitStatus::UNSUPPORTED();
}

}// end namespace ph