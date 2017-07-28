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
	virtual ~SGStratified() override;

private:
	virtual std::unique_ptr<SampleGenerator> genNewborn(std::size_t numSamples) const override;
	virtual void genArray1D(SampleArray1D* out_array) override;
	virtual void genArray2D(SampleArray2D* out_array) override;

	std::size_t m_numStrata2dX;
	std::size_t m_numStrata2dY;

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static std::unique_ptr<SGStratified> ciLoad(const InputPacket& packet);
	static ExitStatus ciExecute(const std::shared_ptr<SGStratified>& targetResource,
	                            const std::string& functionName, 
	                            const InputPacket& packet);
};

}// end namespace ph