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
	virtual void genArray1D(real* coordArray1Ds,
	                        std::size_t num1Ds, 
	                        EPhaseType type) override;
	virtual void genArray2D(Vector2R* coordArray2Ds,
	                        std::size_t num2Ds, 
	                        EPhaseType type) override;

	bool SGUniformRandom::canSplit(const uint32 nSplits) const;

// command interface
public:
	SGUniformRandom(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static std::unique_ptr<SGUniformRandom> ciLoad(const InputPacket& packet);
	static ExitStatus ciExecute(const std::shared_ptr<SGUniformRandom>& targetResource, 
	                            const std::string& functionName, 
	                            const InputPacket& packet);
};

}// end namespace ph