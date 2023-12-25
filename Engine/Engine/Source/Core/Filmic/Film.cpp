#include "Core/Filmic/Film.h"
#include "Math/TVector3.h"
#include "Frame/TFrame.h"

namespace ph
{

Film::Film()
	: Film(0, 0)
{}

Film::Film(
	const int64 actualWidthPx, 
	const int64 actualHeightPx)

	: Film(
		actualWidthPx, 
		actualHeightPx,
		math::TAABB2D<int64>(
			math::TVector2<int64>(0, 0),
			math::TVector2<int64>(actualWidthPx, actualHeightPx)))
{}

Film::Film(
	const int64                 actualWidthPx, 
	const int64                 actualHeightPx,
	const math::TAABB2D<int64>& effectiveWindowPx)

	: m_actualResPx(0)
	, m_effectiveWindowPx({0, 0}, {0, 0})
{
	Film::setActualResPx({actualWidthPx, actualHeightPx});
	Film::setEffectiveWindowPx(effectiveWindowPx);
}

void Film::develop(HdrRgbFrame& out_frame) const
{
	developRegion(out_frame, m_effectiveWindowPx);
}

void Film::develop(HdrRgbFrame& out_frame, const math::TAABB2D<int64>& regionPx) const
{
	developRegion(out_frame, regionPx);
}

}// end namespace ph
