#include "Core/Renderer/Sampling/MetaRecordingProcessor.h"
#include "Math/math.h"

#include <cmath>

namespace ph
{

void MetaRecordingProcessor::process(const math::Vector2D& filmNdc, const Ray& ray)
{
	PH_ASSERT(m_processor);

	m_timer.start();
	m_processor->process(filmNdc, ray);
	m_timer.finish();

	// only record if processed position is in bound
	const auto rasterPos = math::TVector2<int64>((filmNdc * m_filmResPx).floor());
	if(rasterPos.x < m_recordWindowPx.minVertex.x || rasterPos.x >= m_recordWindowPx.maxVertex.x ||
	   rasterPos.y < m_recordWindowPx.minVertex.y || rasterPos.y >= m_recordWindowPx.maxVertex.y)
	{
		return;
	}

	const auto pixelCoord = math::TVector2<uint32>(rasterPos - m_recordWindowPx.minVertex);

	const auto processCount = m_processCountFrame.getPixel(pixelCoord);
	m_processCountFrame.setPixel(pixelCoord, processCount.add(1));

	const auto msSpent = m_msSpentFrame.getPixel(pixelCoord);
	m_msSpentFrame.setPixel(pixelCoord, msSpent.add(m_timer.getDeltaMs()));
}

void MetaRecordingProcessor::onBatchStart(const uint64 batchNumber)
{
	PH_ASSERT(m_processor);

	m_processor->onBatchStart(batchNumber);
}

void MetaRecordingProcessor::onBatchFinish(const uint64 batchNumber)
{
	PH_ASSERT(m_processor);

	m_processor->onBatchFinish(batchNumber);
}

void MetaRecordingProcessor::getRecord(
	HdrRgbFrame* const           out_storage,
	const math::TVector2<int64>& storageOrigin) const
{
	PH_ASSERT(out_storage);

	const math::TAABB2D<int64> storageWindowPx(
		storageOrigin, 
		storageOrigin + math::TVector2<int64>(out_storage->getSizePx()));

	const auto overlappedWindowPx = m_recordWindowPx.getIntersected(storageWindowPx);
	const auto overlappedRegionPx = math::TAABB2D<uint32>(
		math::TAABB2D<int64>(
			overlappedWindowPx.minVertex - storageOrigin,
			overlappedWindowPx.maxVertex - storageOrigin));
	PH_ASSERT_MSG(overlappedRegionPx.isValid(), overlappedRegionPx.toString());

	out_storage->forEachPixel(
		overlappedRegionPx,
		[this, &storageOrigin](const uint32 x, const uint32 y, const HdrRgbFrame::Pixel& pixel)
		{
			const auto recordCoord = math::TVector2<uint32>(
				math::TVector2<int64>(x, y) + storageOrigin - m_recordWindowPx.minVertex);

			const auto processCount = m_processCountFrame.getPixel(recordCoord);
			const auto msSpent      = m_msSpentFrame.getPixel(recordCoord);

			// TODO: ms spent

			HdrRgbFrame::Pixel record;
			record[0] = pixel[0] + static_cast<HdrComponent>(processCount[0]);
			record[1] = pixel[1] + static_cast<HdrComponent>(processCount[0]);
			record[2] = pixel[2] + static_cast<HdrComponent>(processCount[0]);
			return record;
		});
}

}// end namespace ph
