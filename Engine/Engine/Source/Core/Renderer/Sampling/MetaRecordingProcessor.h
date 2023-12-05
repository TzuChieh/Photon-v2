#pragma once

#include "Core/Renderer/Sampling/IReceivedRayProcessor.h"
#include "Frame/TFrame.h"
#include "Math/TVector2.h"
#include "Utility/Timer.h"
#include "Math/Geometry/TAABB2D.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <vector>

namespace ph
{

/*
	A wrapper that will do work on behave of the wrapped processor and 
	provides additional information regarding the performed work; such as
	sample count and time spent per pixel.
*/
class MetaRecordingProcessor : public IReceivedRayProcessor
{
public:
	MetaRecordingProcessor();
	explicit MetaRecordingProcessor(IReceivedRayProcessor* processor);

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

	IReceivedRayProcessor* m_processor;
	CounterFrame           m_processCountFrame;
	CounterFrame           m_msSpentFrame;
	Timer                  m_timer;
	math::Vector2D         m_filmResPx;
	math::TAABB2D<int64>   m_recordWindowPx;
};

// In-header Implementations:

inline MetaRecordingProcessor::MetaRecordingProcessor() : 
	MetaRecordingProcessor(nullptr)
{}

inline MetaRecordingProcessor::MetaRecordingProcessor(
	IReceivedRayProcessor* const processor) :

	m_processor        (processor),
	m_processCountFrame(),
	m_msSpentFrame     (),
	m_timer            (),
	m_filmResPx        (0),
	m_recordWindowPx   ({0, 0}, {0, 0})
{}

inline void MetaRecordingProcessor::clearRecords()
{
	m_processCountFrame.fill(0);
	m_msSpentFrame.fill(0);
}

inline void MetaRecordingProcessor::setDimensions(
	const math::TVector2<int64>& filmResPx,
	const math::TAABB2D<int64>&  recordWindowPx)
{
	PH_ASSERT_MSG(filmResPx.x() > 0 && filmResPx.y() > 0,
		filmResPx.toString());
	PH_ASSERT_MSG(recordWindowPx.getExtents().x() > 0 && recordWindowPx.getExtents().y() > 0,
		recordWindowPx.toString());

	m_filmResPx      = math::Vector2D(filmResPx);
	m_recordWindowPx = recordWindowPx;

	const auto recordResPx = math::TVector2<uint32>(recordWindowPx.getExtents());
	m_processCountFrame.setSize(recordResPx.x(), recordResPx.y());
	m_msSpentFrame.setSize(recordResPx.x(), recordResPx.y());
}

}// end namespace ph
