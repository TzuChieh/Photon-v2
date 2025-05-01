#pragma once

#include "Engine/Core/Renderer/Sampling/IRasterRayProcessor.h"
#include "Engine/Frame/TFrame.h"
#include "Engine/Math/TVector2.h"
#include "Engine/Utility/Timer.h"
#include "Engine/Math/Geometry/TAABB2D.h"

#include <Common/primitive_type.h>

namespace ph
{

/*!
A thin wrapper that will do work on behave of the wrapped processor and 
provides additional information regarding the performed work; such as
sample count and time spent per pixel.
*/
class MetaRecordingProcessor : public IRasterRayProcessor
{
public:
	MetaRecordingProcessor();
	explicit MetaRecordingProcessor(IRasterRayProcessor* processor);

	void process(
		const math::Vector2D& rasterCoord,
		const Ray&            ray, 
		const math::Spectrum& quantityWeight,
		SampleFlow&           sampleFlow) override;

	void onBatchStart(uint64 batchNumber) override;
	void onBatchFinish(uint64 batchNumber) override;

	void clearRecords();
	void setDimensions(
		const math::TVector2<int64>& filmResPx,
		const math::TAABB2D<int64>&  recordWindowPx);

	// TODO: mode for getting records, e.g., replace? add? which channel?
	void getRecord(
		HdrRgbFrame*                 out_storage,
		const math::TVector2<int64>& storageOrigin) const;

private:
	using CounterFrame = TFrame<uint64, 1>;

	IRasterRayProcessor*   m_processor;
	CounterFrame           m_processCountFrame;
	CounterFrame           m_msSpentFrame;
	Timer                  m_timer;
	math::TAABB2D<int64>   m_recordWindowPx;
};

}// end namespace ph
