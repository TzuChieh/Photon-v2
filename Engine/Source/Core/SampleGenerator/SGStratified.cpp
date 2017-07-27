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

void SGStratified::genSplitted(uint32 numSplits,
                               std::vector<std::unique_ptr<SampleGenerator>>& out_sgs)
{
	if(!canSplit(numSplits))
	{
		return;
	}

	const std::size_t splittedNumSamples = numSamples() / numSplits;
	for(uint32 i = 0; i < numSplits; i++)
	{
		auto sampleGenerator = std::make_unique<SGStratified>(splittedNumSamples, m_numStrata2dX, m_numStrata2dY);
		out_sgs.push_back(std::move(sampleGenerator));
	}
}

bool SGStratified::canSplit(const uint32 nSplits) const
{
	if(nSplits == 0)
	{
		std::cerr << "warning: at PixelJitterSampleGenerator::canSplit(), " 
		          << "number of splits is 0" << std::endl;
		return false;
	}

	if(numSamples() % nSplits != 0)
	{
		std::cerr << "warning: at PixelJitterSampleGenerator::canSplit(), " 
		          << "generator cannot evenly split into " << nSplits << " parts" << std::endl;
		std::cerr << "(sample count: " << numSamples() << ")" << std::endl;
		return false;
	}

	return true;
}

void SGStratified::genArray2dStrata(Vector2R* const out_coordArray2Ds, 
                                    const std::size_t numXs, const std::size_t numYs)
{
	const real dx = 1.0_r / static_cast<real>(numXs);
	const real dy = 1.0_r / static_cast<real>(numYs);
	for(std::size_t y = 0; y < numYs; y++)
	{
		const std::size_t baseIndex = y * numYs;
		for(std::size_t x = 0; x < numXs; x++)
		{
			const real biasX = Random::genUniformReal_i0_e1();
			const real biasY = Random::genUniformReal_i0_e1();
			out_coordArray2Ds[baseIndex + x].x = (static_cast<real>(x) + biasX) * dx;
			out_coordArray2Ds[baseIndex + x].y = (static_cast<real>(y) + biasY) * dy;
		}
	}
}

void SGStratified::genArray2dLatinHypercube(Vector2R* const out_coordArray2Ds, 
                                            const std::size_t num2Ds)
{

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