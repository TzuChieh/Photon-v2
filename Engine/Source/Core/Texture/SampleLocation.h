#pragma once

#include "Core/HitDetail.h"
#include "Math/TVector3.h"
#include "Math/TVector2.h"
#include "Core/Quantity/EQuantity.h"

#include <limits>

namespace ph
{

/*
	This class provides a temporal, higher order of abstraction over data
	regarding ray-hit information. This abstraction layer is tailored
	towards texture sampling and shall not be used in other circumstances.
	The philosophy of this abstraction layer is that texture sampling 
	processes should not care about the context of a hit, but focus on the
	actual data of the target point. 
*/
class SampleLocation final
{
public:
	// Constructs a sample location at (u, v, (w)).
	SampleLocation(const Vector3R& uvw, EQuantity quantity);
	SampleLocation(const Vector2R& uv, EQuantity quantity);

	// Constructs a sample location from hit information.
	explicit SampleLocation(const HitDetail& hit);
	SampleLocation(const HitDetail& hit, EQuantity quantity);

	SampleLocation(const SampleLocation& other);

	// Gets and sets the uvw coordinates of this sample location.
	Vector3R uvw() const;
	void setUvw(const Vector3R& uvw);
	void setUv(const Vector2R& uv);

	// TODO: should use uvw remapper instead
	// or update derivatives?
	SampleLocation getUvwScaled(const Vector3R& scale) const;

	// Gets the expected type of the quantity being sampled.
	EQuantity expectedQuantity() const;

private:
	// TODO: seems we don't need all data in HitDetail for texture sampling;
	// perhaps just store required data here
	HitDetail m_hit;
	const EQuantity m_quantity;
};

// In-header Implementations:

inline SampleLocation::SampleLocation(const Vector3R& uvw, const EQuantity quantity) :
	SampleLocation(HitDetail().setMisc(nullptr, uvw, std::numeric_limits<real>::max()), quantity)
{}

inline SampleLocation::SampleLocation(const Vector2R& uv, const EQuantity quantity) :
	SampleLocation(Vector3R(uv.x, uv.y, 0), quantity)
{}

inline SampleLocation::SampleLocation(const HitDetail& hit) :
	SampleLocation(hit, EQuantity::RAW)
{}

inline SampleLocation::SampleLocation(const HitDetail& hit, const EQuantity quantity) :
	m_hit(hit), m_quantity(quantity)
{}

inline SampleLocation::SampleLocation(const SampleLocation& other) :
	SampleLocation(other.m_hit, other.m_quantity)
{}

inline Vector3R SampleLocation::uvw() const
{
	return m_hit.getUvw();
}

inline void SampleLocation::setUvw(const Vector3R& uvw)
{
	m_hit.setMisc(m_hit.getPrimitive(), uvw, m_hit.getRayT());
}

inline void SampleLocation::setUv(const Vector2R& uv)
{
	m_hit.setMisc(m_hit.getPrimitive(), Vector3R(uv.x, uv.y, 0.0_r), m_hit.getRayT());
}

inline SampleLocation SampleLocation::getUvwScaled(const Vector3R& scale) const
{
	HitDetail newDetail = m_hit;
	newDetail.setMisc(m_hit.getPrimitive(), m_hit.getUvw().mul(scale), m_hit.getRayT());
	return SampleLocation(newDetail, m_quantity);
}

inline EQuantity SampleLocation::expectedQuantity() const
{
	return m_quantity;
}

}// end namespace ph