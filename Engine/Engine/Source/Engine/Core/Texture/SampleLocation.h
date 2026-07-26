#pragma once

#include "Engine/Core/SurfaceHit.h"
#include "Engine/Math/TVector3.h"
#include "Engine/Math/TVector2.h"
#include "Engine/Math/Color/color_enums.h"

#include <Common/assertion.h>

namespace ph
{

/*!
This class provides a transient, higher-order abstraction
over data regarding ray-hit information. This abstraction layer is tailored
towards texture sampling and shall not be used in other circumstances.
The philosophy of this abstraction layer is that texture sampling
processes should not care about the context of a hit, but focus on the
actual data of the target point, unless surface hit information is required
by the current operation. A sample location and its copies must not be
retained beyond the synchronous sampling operation.
*/
class SampleLocation final
{
	// FIXME: uvw should be in float64 for better highres pixel texture sample precision 
	// (test if required)
	// FIXME: include sampling derivatives/LoD/footprint info and transform it with uvw

public:
	/*! @brief Constructs sampling state at @f$ (u, v, (w)) @f$ without surface hit information.
	*/
	///@{
	explicit SampleLocation(
		const math::Vector3R& uvw,
		math::EColorUsage usage = math::EColorUsage::Raw);
	explicit SampleLocation(
		const math::Vector2R& uv,
		math::EColorUsage usage = math::EColorUsage::Raw);
	///@}

	/*! @brief Constructs sampling state from a surface hit.
	The pointer must not be null, and the surface hit must outlive this location and all copies used
	by the current sampling operation.
	*/
	///@{
	explicit SampleLocation(const SurfaceHit* X);
	SampleLocation(const SurfaceHit* X, math::EColorUsage usage);
	///@}

	/*! @brief Gets and sets the uvw coordinates of this sample location.
	*/
	///@{
	math::Vector3R uvw() const;
	math::Vector2R uv() const;
	void setUvw(const math::Vector3R& uvw);
	void setUv(const math::Vector2R& uv);
	///@}

	// TODO: use a UVW remapper once sampling derivatives are available
	SampleLocation getUvwScaled(const math::Vector3R& scale) const;

	/*! @brief Gets expected type of the usage for the sample.
	*/
	math::EColorUsage expectedUsage() const;

	/*! @brief Whether this sample location has an associated surface hit.
	Calling `getSurfaceHit()` is only valid if this method returns `true`.
	*/
	bool hasSurfaceHit() const;

	/*! @brief Gets the surface hit associated with the current sampling operation.
	@warning Calling this method without an associated surface hit is undefined behavior. Debug
	builds assert.
	*/
	const SurfaceHit& getSurfaceHit() const;

private:
	const SurfaceHit*       m_X;
	math::Vector3R          m_uvw;
	math::EColorUsage       m_usage;
};

// In-header Implementations:

inline SampleLocation::SampleLocation(const math::Vector3R& uvw, const math::EColorUsage usage)
	: m_X(nullptr), m_uvw(uvw), m_usage(usage)
{}

inline SampleLocation::SampleLocation(const math::Vector2R& uv, const math::EColorUsage usage)
	: SampleLocation(math::Vector3R(uv.x(), uv.y(), 0), usage)
{}

inline SampleLocation::SampleLocation(const SurfaceHit* const X)
	: SampleLocation(X, math::EColorUsage::Raw)
{}

inline SampleLocation::SampleLocation(const SurfaceHit* const X, const math::EColorUsage usage)
	: m_X(X), m_uvw(getSurfaceHit().getDetail().getUVW()), m_usage(usage)
{}

inline math::Vector3R SampleLocation::uvw() const
{
	return m_uvw;
}

inline math::Vector2R SampleLocation::uv() const
{
	return {m_uvw.x(), m_uvw.y()};
}

inline void SampleLocation::setUvw(const math::Vector3R& uvw)
{
	m_uvw = uvw;
}

inline void SampleLocation::setUv(const math::Vector2R& uv)
{
	setUvw(math::Vector3R(uv.x(), uv.y(), 0.0_r));
}

inline SampleLocation SampleLocation::getUvwScaled(const math::Vector3R& scale) const
{
	SampleLocation result(*this);
	result.m_uvw.mulLocal(scale);
	return result;
}

inline math::EColorUsage SampleLocation::expectedUsage() const
{
	return m_usage;
}

inline bool SampleLocation::hasSurfaceHit() const
{
	return m_X != nullptr;
}

inline const SurfaceHit& SampleLocation::getSurfaceHit() const
{
	PH_ASSERT_MSG(m_X,
		"Surface hit information is required for the current sampling operation, "
		"but this `SampleLocation` has no associated `SurfaceHit`.");

	return *m_X;
}

}// end namespace ph
