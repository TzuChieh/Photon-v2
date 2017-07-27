#pragma once

#include "Core/SampleGenerator/SampleGenerator.h"
#include "Common/primitive_type.h"

namespace ph
{

class SGUniformRandom final : public SampleGenerator, public TCommandInterface<SGUniformRandom>
{
public:
	SGUniformRandom(const std::size_t numSamples);
	virtual ~SGUniformRandom() override;

	virtual void genSplitted(uint32 numSplits,
	                         std::vector<std::unique_ptr<SampleGenerator>>& out_sgs) override;

private:
	virtual void genArray1D(SampleArray1D* out_array) override;
	virtual void genArray2D(SampleArray2D* out_array) override;

	bool SGUniformRandom::canSplit(const uint32 nSplits) const;

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static std::unique_ptr<SGUniformRandom> ciLoad(const InputPacket& packet);
	static ExitStatus ciExecute(const std::shared_ptr<SGUniformRandom>& targetResource, 
	                            const std::string& functionName, 
	                            const InputPacket& packet);
};

}// end namespace ph