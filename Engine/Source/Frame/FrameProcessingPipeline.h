#pragma once

#include "Frame/Operator/FrameOperator.h"
#include "Frame/TFrame.h"
#include "Utility/IMoveOnly.h"

#include <vector>
#include <memory>

namespace ph
{

/*! @brief Process frames with a series of operations.
*/
class FrameProcessingPipeline final : private IMoveOnly
{
public:
	inline FrameProcessingPipeline() = default;
	FrameProcessingPipeline(FrameProcessingPipeline&& other);

	void process(const HdrRgbFrame& srcFrame, HdrRgbFrame* out_dstFrame);
	void appendOperator(std::unique_ptr<FrameOperator> op);

	FrameProcessingPipeline& operator = (FrameProcessingPipeline&& rhs);

private:
	std::vector<std::unique_ptr<FrameOperator>> m_operators;
	HdrRgbFrame                                 m_tmpFrame;
};

}// end namespace ph
