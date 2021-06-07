#pragma once

#include "Frame/FrameProcessingPipeline.h"
#include "Utility/IMoveOnly.h"

#include <vector>

namespace ph
{

/*! @brief Manages and executes frame processing pipelines.

Care must be taken when using an instance concurrently as things may break
if some frame operators used cached data internally (race condition).
*/
class FrameProcessor final : public IMoveOnly
{
public:
	using PipelineId = std::size_t;

	void process(
		PipelineId         pipeline, 
		const HdrRgbFrame& srcFrame, 
		HdrRgbFrame*       out_dstFrame);

	PipelineId addPipeline();
	FrameProcessingPipeline* getPipeline(PipelineId pipeline);
	const FrameProcessingPipeline* getPipeline(PipelineId pipeline) const;

private:
	std::vector<FrameProcessingPipeline> m_pipelines;

	bool checkPipelineId(PipelineId id) const;
};

}// end namespace ph
