#pragma once

#include "Core/Renderer/Sampling/ISensedRayProcessor.h"
#include "Common/assertion.h"
#include "Frame/TFrame.h"
#include "Math/TVector2.h"
#include "Common/primitive_type.h"
#include "Utility/Timer.h"

#include <vector>

namespace ph
{

/*
	A wrapper that will do work on behave of the wrapped processor and 
	provides additional information regarding the performed work; such as
	sample count and time spent per pixel.
*/
class MetaRecordingProcessor : public ISensedRayProcessor
{
public:
	MetaRecordingProcessor();
	explicit MetaRecordingProcessor(ISensedRayProcessor* processor);

	void process(const Vector2D& filmNdc, const Ray& ray) override;
	void onBatchStart(uint64 batchNumber) override;
	void onBatchFinish(uint64 batchNumber) override;

	void clearRecords();
	void setDimensions(
		const TVector2<int64>& filmResPx, 
		const TAABB2D<int64>&  recordWindowPx);

	void getRecord(
		HdrRgbFrame*           out_storage,
		const TVector2<int64>& storageOrigin) const;

private:
	using CounterFrame = TFrame<uint64, 1>;

	ISensedRayProcessor* m_processor;
	CounterFrame         m_processCountFrame;
	CounterFrame         m_msSpentFrame;
	Timer                m_timer;
	Vector2D             m_filmResPx;
	TAABB2D<int64>       m_recordWindowPx;
};

// In-header Implementations:

inline MetaRecordingProcessor::MetaRecordingProcessor() : 
	MetaRecordingProcessor(nullptr)
{}

inline MetaRecordingProcessor::MetaRecordingProcessor(
	ISensedRayProcessor* const processor) :

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
	const TVector2<int64>& filmResPx,
	const TAABB2D<int64>&  recordWindowPx)
{
	PH_ASSERT_MSG(filmResPx.x > 0 && filmResPx.y > 0, 
		filmResPx.toString());
	PH_ASSERT_MSG(recordWindowPx.getExtents().x > 0 && recordWindowPx.getExtents().y > 0,
		recordWindowPx.toString());

	m_filmResPx      = Vector2D(filmResPx);
	m_recordWindowPx = recordWindowPx;

	const auto recordResPx = TVector2<uint32>(recordWindowPx.getExtents());
	m_processCountFrame.setSize(recordResPx.x, recordResPx.y);
	m_msSpentFrame.setSize(recordResPx.x, recordResPx.y);
}

}// end namespace ph