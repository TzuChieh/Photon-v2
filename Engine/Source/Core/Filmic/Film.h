#pragma once

#include "Common/primitive_type.h"
#include "Core/Bound/TAABB2D.h"
#include "Math/TVector2.h"
#include "Frame/frame_fwd.h"
#include "Common/assertion.h"
#include "Utility/INoncopyable.h"

namespace ph
{
class Film : public INoncopyable
{
public:
	Film() = default;

	Film(
		int64                 actualWidthPx, 
		int64                 actualHeightPx);

	Film(
		int64                 actualWidthPx, 
		int64                 actualHeightPx,
		const TAABB2D<int64>& effectiveWindowPx);

	Film(Film&& other);

	virtual ~Film() = default;

	virtual void clear() = 0;

	virtual void setActualResPx(const TVector2<int64>& actualResPx);
	virtual void setEffectiveWindowPx(const TAABB2D<int64>& effectiveWindow);

	void develop(HdrRgbFrame& out_frame) const;
	void develop(HdrRgbFrame& out_frame, const TAABB2D<int64>& regionPx) const;

	const TVector2<int64>& getActualResPx() const;
	TVector2<int64>        getEffectiveResPx() const;
	const TAABB2D<int64>&  getEffectiveWindowPx() const;

	Film& operator = (Film&& other);

private:
	virtual void developRegion(HdrRgbFrame& out_frame, const TAABB2D<int64>& regionPx) const = 0;

	TVector2<int64> m_actualResPx;
	TAABB2D<int64>  m_effectiveWindowPx;
};

// In-header Implementations:

inline void Film::setActualResPx(const TVector2<int64>& actualResPx)
{
	PH_ASSERT_GT(actualResPx.product(), 0);

	m_actualResPx = actualResPx;
}

inline void Film::setEffectiveWindowPx(const TAABB2D<int64>& effectiveWindow)
{
	PH_ASSERT(effectiveWindow.isValid());

	m_effectiveWindowPx = effectiveWindow;
}

inline const TVector2<int64>& Film::getActualResPx() const
{
	return m_actualResPx;
}

inline TVector2<int64> Film::getEffectiveResPx() const
{
	return {m_effectiveWindowPx.getWidth(), m_effectiveWindowPx.getHeight()};
}

inline const TAABB2D<int64>& Film::getEffectiveWindowPx() const
{
	return m_effectiveWindowPx;
}

}// end namespace ph