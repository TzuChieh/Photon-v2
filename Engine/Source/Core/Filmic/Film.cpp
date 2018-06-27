#include "Core/Filmic/Film.h"
#include "Math/TVector3.h"
#include "Frame/TFrame.h"
#include "FileIO/SDL/InputPacket.h"

#include <iostream>

namespace ph
{

Film::Film(
	const int64 actualWidthPx, const int64 actualHeightPx) :

	Film(
		actualWidthPx, actualHeightPx,
		TAABB2D<int64>(TVector2<int64>(0, 0),
		               TVector2<int64>(actualWidthPx, actualHeightPx)))
{}

Film::Film(
	const int64 actualWidthPx, const int64 actualHeightPx,
	const TAABB2D<int64>& effectiveWindowPx) : 

	m_actualResPx      (actualWidthPx, actualHeightPx), 
	m_effectiveWindowPx()
{
	Film::setEffectiveWindowPx(effectiveWindowPx);
}

void Film::develop(HdrRgbFrame& out_frame) const
{
	developRegion(out_frame, m_effectiveWindowPx);
}

void Film::develop(HdrRgbFrame& out_frame, const TAABB2D<int64>& regionPx) const
{
	developRegion(out_frame, regionPx);
}

Film::~Film() = default;

// command interface

Film::Film(const InputPacket& packet)
{
	const integer filmWidth  = packet.getInteger("width",  0, DataTreatment::REQUIRED());
	const integer filmHeight = packet.getInteger("height", 0, DataTreatment::REQUIRED());
	const integer rectX      = packet.getInteger("rect-x", 0);
	const integer rectY      = packet.getInteger("rect-y", 0);
	const integer rectW      = packet.getInteger("rect-w", filmWidth);
	const integer rectH      = packet.getInteger("rect-h", filmHeight);

	m_actualResPx.x     = filmWidth;
	m_actualResPx.y     = filmHeight;
	m_effectiveWindowPx = TAABB2D<int64>({rectX, rectY}, {rectX + rectW, rectY + rectH});
}

SdlTypeInfo Film::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_FILM, "film");
}

void Film::ciRegister(CommandRegister& cmdRegister) {}

}// end namespace ph