#pragma once

#include "Core/SampleGenerator/SampleGenerator.h"
#include "Common/primitive_type.h"
#include "FileIO/SDL/ISdlResource.h"
#include "FileIO/SDL/TCommandInterface.h"

namespace ph
{

class PixelJitterSampleGenerator final : public SampleGenerator, public TCommandInterface<PixelJitterSampleGenerator>, public ISdlResource
{
public:
	PixelJitterSampleGenerator(const uint32 sppBudget);
	virtual ~PixelJitterSampleGenerator() override;

	virtual bool hasMoreSamples() const override;
	virtual void analyze(const Scene& scene, const Film& film) override;
	virtual void requestMoreSamples(std::vector<Sample>* const out_samples) override;
	virtual void split(const uint32 nSplits, std::vector<std::unique_ptr<SampleGenerator>>* const out_sampleGenerators) override;

private:
	uint32 m_numDispatchedSpp;
	uint32 m_filmWidthPx;
	uint32 m_filmHeightPx;

	bool canSplit(const uint32 nSplits) const;

// command interface
public:
	PixelJitterSampleGenerator(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static ExitStatus ciExecute(const std::shared_ptr<PixelJitterSampleGenerator>& targetResource, const std::string& functionName, const InputPacket& packet);
};

}// end namespace ph