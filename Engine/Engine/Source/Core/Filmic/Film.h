#pragma once

#include "Math/Geometry/TAABB2D.h"
#include "Math/TVector2.h"
#include "Frame/frame_fwd.h"
#include "Utility/IMoveOnly.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

namespace ph
{

class Film : private IMoveOnly
{
public:
	Film();

	Film(
		int64                       actualWidthPx, 
		int64                       actualHeightPx);

	Film(
		int64                       actualWidthPx, 
		int64                       actualHeightPx,
		const math::TAABB2D<int64>& effectiveWindowPx);

	Film(Film&& other) = default;
	Film& operator = (Film&& other) = default;

	virtual ~Film() = default;

	virtual void clear() = 0;

	/*! @brief Set the apparent size of the film.
	If only a sub-region of the film is going to be used, set the effective window accordingly may
	improve performance especially in memory usage.
	*/
	virtual void setActualResPx(const math::TVector2<int64>& actualResPx);

	/*! @brief Set the region where the film will be used.
	Implementation is advised to take advantage of this and only allocate memory for the part
	within the effective window.
	*/
	virtual void setEffectiveWindowPx(const math::TAABB2D<int64>& effectiveWindow);

	void develop(HdrRgbFrame& out_frame) const;
	void develop(HdrRgbFrame& out_frame, const math::TAABB2D<int64>& regionPx) const;

	const math::TVector2<int64>& getActualResPx() const;
	math::TVector2<int64> getEffectiveResPx() const;
	const math::TAABB2D<int64>& getEffectiveWindowPx() const;

private:
	virtual void developRegion(HdrRgbFrame& out_frame, const math::TAABB2D<int64>& regionPx) const = 0;

	math::TVector2<int64> m_actualResPx;
	math::TAABB2D<int64>  m_effectiveWindowPx;
};

// In-header Implementations:

inline void Film::setActualResPx(const math::TVector2<int64>& actualResPx)
{
	PH_ASSERT_GE(actualResPx.x(), 0);
	PH_ASSERT_GE(actualResPx.y(), 0);

	m_actualResPx = actualResPx;
}

inline void Film::setEffectiveWindowPx(const math::TAABB2D<int64>& effectiveWindow)
{
	PH_ASSERT(!effectiveWindow.isEmpty());

	m_effectiveWindowPx = effectiveWindow;
}

inline const math::TVector2<int64>& Film::getActualResPx() const
{
	return m_actualResPx;
}

inline math::TVector2<int64> Film::getEffectiveResPx() const
{
	return {m_effectiveWindowPx.getWidth(), m_effectiveWindowPx.getHeight()};
}

inline auto Film::getEffectiveWindowPx() const
	-> const math::TAABB2D<int64>&
{
	return m_effectiveWindowPx;
}

}// end namespace ph
