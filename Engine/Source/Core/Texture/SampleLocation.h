#pragma once

#include "Core/HitDetail.h"
#include "Math/TVector3.h"
#include "Math/TVector2.h"
#include "Math/Color/color_enums.h"

#include <limits>

namespace ph
{

/*!
This class provides a temporal, higher order of abstraction over data
regarding ray-hit information. This abstraction layer is tailored
towards texture sampling and shall not be used in other circumstances.
The philosophy of this abstraction layer is that texture sampling 
processes should not care about the context of a hit, but focus on the
actual data of the target point. 
*/
// FIXME: uvw should be in float64 for better highres pixel texture sample precision (test if required)
class SampleLocation final
{
public:
	// Constructs a sample location at (u, v, (w)).
	SampleLocation(const math::Vector3R& uvw, math::EColorUsage usage);
	SampleLocation(const math::Vector2R& uv, math::EColorUsage usage);

	// Constructs a sample location from hit information.
	explicit SampleLocation(const HitDetail& hit);
	SampleLocation(const HitDetail& hit, math::EColorUsage usage);

	SampleLocation(const SampleLocation& other);

	// Gets and sets the uvw coordinates of this sample location.
	math::Vector3R uvw() const;
	math::Vector2R uv() const;
	void setUvw(const math::Vector3R& uvw);
	void setUv(const math::Vector2R& uv);

	// TODO: should use uvw remapper instead
	// or update derivatives?
	SampleLocation getUvwScaled(const math::Vector3R& scale) const;

	// Gets expected type of the usage for the sample.
	math::EColorUsage expectedUsage() const;

private:
	// TODO: seems we don't need all data in HitDetail for texture sampling;
	// perhaps just store required data here
	HitDetail m_hit;
	const math::EColorUsage m_usage;
};

// In-header Implementations:

inline SampleLocation::SampleLocation(const math::Vector3R& uvw, const math::EColorUsage usage) :
	SampleLocation(HitDetail().setMisc(nullptr, uvw, std::numeric_limits<real>::max()), usage)
{}

inline SampleLocation::SampleLocation(const math::Vector2R& uv, const math::EColorUsage usage) :
	SampleLocation(math::Vector3R(uv.x(), uv.y(), 0), usage)
{}

inline SampleLocation::SampleLocation(const HitDetail& hit) :
	SampleLocation(hit, math::EColorUsage::RAW)
{}

inline SampleLocation::SampleLocation(const HitDetail& hit, const math::EColorUsage usage) :
	m_hit(hit), m_usage(usage)
{}

inline SampleLocation::SampleLocation(const SampleLocation& other) :
	SampleLocation(other.m_hit, other.m_usage)
{}

inline math::Vector3R SampleLocation::uvw() const
{
	return m_hit.getUvw();
}

inline math::Vector2R SampleLocation::uv() const
{
	const auto uvwCoords = uvw();
	return {uvwCoords.x, uvwCoords.y};
}

inline void SampleLocation::setUvw(const math::Vector3R& uvw)
{
	m_hit.setMisc(m_hit.getPrimitive(), uvw, m_hit.getRayT());
}

inline void SampleLocation::setUv(const math::Vector2R& uv)
{
	m_hit.setMisc(m_hit.getPrimitive(), math::Vector3R(uv.x(), uv.y(), 0.0_r), m_hit.getRayT());
}

inline SampleLocation SampleLocation::getUvwScaled(const math::Vector3R& scale) const
{
	HitDetail newDetail = m_hit;
	newDetail.setMisc(m_hit.getPrimitive(), m_hit.getUvw().mul(scale), m_hit.getRayT());
	return SampleLocation(newDetail, m_usage);
}

inline math::EColorUsage SampleLocation::expectedUsage() const
{
	return m_usage;
}

}// end namespace ph
