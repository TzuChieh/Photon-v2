#pragma once

#include "Core/HitDetail.h"
#include "Math/TVector3.h"
#include "Math/TVector2.h"
#include "Math/Color/color_enums.h"

#include <limits>

namespace ph
{

/*!
This class provides a temporal, higher order abstraction over data
regarding ray-hit information. This abstraction layer is tailored
towards texture sampling and shall not be used in other circumstances.
The philosophy of this abstraction layer is that texture sampling 
processes should not care about the context of a hit, but focus on the
actual data of the target point. 
*/
class SampleLocation final
{
	// FIXME: uvw should be in float64 for better highres pixel texture sample precision 
	// (test if required)
	// FIXME: include LoD/footprint info

public:
	/*! @brief Constructs a sample location at @f$ (u, v, (w)) @f$.
	*/
	///@{
	SampleLocation(const math::Vector3R& uvw, math::EColorUsage usage);
	SampleLocation(const math::Vector2R& uv, math::EColorUsage usage);
	///@}

	/*! @brief Constructs a sample location from hit information.
	*/
	///@{
	explicit SampleLocation(const HitDetail& hit);
	SampleLocation(const HitDetail& hit, math::EColorUsage usage);
	///@}

	SampleLocation(const SampleLocation& other);

	/*! @brief Gets and sets the uvw coordinates of this sample location.
	*/
	///@{
	math::Vector3R uvw() const;
	math::Vector2R uv() const;
	void setUvw(const math::Vector3R& uvw);
	void setUv(const math::Vector2R& uv);
	///@}

	// TODO: should use uvw remapper instead
	// or update derivatives?
	SampleLocation getUvwScaled(const math::Vector3R& scale) const;

	/*! @brief Gets expected type of the usage for the sample.
	*/
	math::EColorUsage expectedUsage() const;

private:
	// TODO: seems we don't need all data in HitDetail for texture sampling;
	// perhaps just store required data here
	HitDetail m_hit;
	const math::EColorUsage m_usage;
};

// In-header Implementations:

inline SampleLocation::SampleLocation(const math::Vector3R& uvw, const math::EColorUsage usage) :
	SampleLocation(HitDetail().setHitIntrinsics(nullptr, uvw, std::numeric_limits<real>::max()), usage)
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
	return m_hit.getUVW();
}

inline math::Vector2R SampleLocation::uv() const
{
	const auto uvwCoords = uvw();
	return {uvwCoords.x(), uvwCoords.y()};
}

inline void SampleLocation::setUvw(const math::Vector3R& uvw)
{
	m_hit.setHitIntrinsics(
		m_hit.getPrimitive(), 
		uvw, 
		m_hit.getRayT(), 
		m_hit.getFaceID(),
		m_hit.getFaceTopology());
}

inline void SampleLocation::setUv(const math::Vector2R& uv)
{
	setUvw(math::Vector3R(uv.x(), uv.y(), 0.0_r));
}

inline SampleLocation SampleLocation::getUvwScaled(const math::Vector3R& scale) const
{
	SampleLocation result(*this);
	result.setUvw(m_hit.getUVW().mul(scale));
	return result;
}

inline math::EColorUsage SampleLocation::expectedUsage() const
{
	return m_usage;
}

}// end namespace ph
