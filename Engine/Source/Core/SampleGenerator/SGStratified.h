#pragma once

#include "Core/SampleGenerator/SampleGenerator.h"
#include "Common/primitive_type.h"

namespace ph
{

class SGStratified final : public SampleGenerator, public TCommandInterface<SGStratified>
{
public:
	SGStratified(std::size_t numSamples,
	             std::size_t numStrata2dX, 
	             std::size_t numStrata2dY);

private:
	std::unique_ptr<SampleGenerator> genNewborn(std::size_t numSamples) const override;
	void genSamples1D(Samples1D* out_array) override;
	void genSamples2D(Samples2D* out_array) override;
	void genSamplesND(SamplesND* out_array) override;

	std::size_t m_numStrata2dX;
	std::size_t m_numStrata2dY;

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
	static std::unique_ptr<SGStratified> ciLoad(const InputPacket& packet);
};

}// end namespace ph