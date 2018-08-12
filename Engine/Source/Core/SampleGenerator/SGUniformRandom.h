#pragma once

#include "Core/SampleGenerator/SampleGenerator.h"
#include "Common/primitive_type.h"

namespace ph
{

class SGUniformRandom final : public SampleGenerator, public TCommandInterface<SGUniformRandom>
{
public:
	SGUniformRandom(const std::size_t numSamples);

private:
	virtual std::unique_ptr<SampleGenerator> genNewborn(std::size_t numSamples) const override;
	virtual void genArray1D(SampleArray1D* out_array) override;
	virtual void genArray2D(SampleArray2D* out_array) override;

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
	static std::unique_ptr<SGUniformRandom> ciLoad(const InputPacket& packet);
};

}// end namespace ph