#include "Core/Renderer/Sampling/MetaRecordingProcessor.h"

#include <cmath>

namespace ph
{

void MetaRecordingProcessor::process(const Vector2D& filmNdc, const Ray& ray)
{
	PH_ASSERT(m_processor);

	m_timer.start();
	m_processor->process(filmNdc, ray);
	m_timer.finish();

	// only record if processed position is in bound
	const auto rasterPos = TVector2<int64>((filmNdc * m_filmResPx).floor());
	if(rasterPos.x < m_recordWindowPx.minVertex.x || rasterPos.x >= m_recordWindowPx.maxVertex.x ||
	   rasterPos.y < m_recordWindowPx.minVertex.y || rasterPos.y >= m_recordWindowPx.maxVertex.y)
	{
		return;
	}

	const auto pixelCoord = TVector2<uint32>(rasterPos - m_recordWindowPx.minVertex);

	CounterFrame::Pixel processCount = m_processCountFrame.getPixel(pixelCoord);
	m_processCountFrame.setPixel(pixelCoord, processCount.add(1));

	CounterFrame::Pixel msSpent = m_msSpentFrame.getPixel(pixelCoord);
	m_msSpentFrame.setPixel(pixelCoord, msSpent.add(m_timer.getDeltaMs()));
}

void MetaRecordingProcessor::getRecord(
	HdrRgbFrame* const     out_storage,
	const TVector2<int64>& storageOrigin) const
{
	PH_ASSERT(out_storage);

	const TAABB2D<int64> storageWindowPx(
		storageOrigin, 
		storageOrigin + TVector2<int64>(out_storage->getSizePx()));

	const auto overlappedWindowPx = m_recordWindowPx.getIntersected(storageWindowPx);
	const auto overlappedRegionPx = TAABB2D<uint32>(
		TAABB2D<int64>(
			overlappedWindowPx.minVertex - storageOrigin,
			overlappedWindowPx.maxVertex - storageOrigin));
	PH_ASSERT_MSG(overlappedRegionPx.isValid(), overlappedRegionPx.toString());

	out_storage->forEachPixel(
		overlappedRegionPx,
		[this, &storageOrigin](const uint32 x, const uint32 y, const HdrRgbFrame::Pixel& pixel)
		{
			const auto recordCoord = TVector2<uint32>(
				TVector2<int64>(x, y) + storageOrigin - m_recordWindowPx.minVertex);

			const CounterFrame::Pixel processCount = m_processCountFrame.getPixel(recordCoord);
			const CounterFrame::Pixel msSpent      = m_msSpentFrame.getPixel(recordCoord);

			HdrRgbFrame::Pixel record;
			record[0] = static_cast<HdrComponent>(processCount[0]);
			record[1] = static_cast<HdrComponent>(msSpent[0]);
			record[2] = 0;

			return record;
		});
}

}// end namespace ph