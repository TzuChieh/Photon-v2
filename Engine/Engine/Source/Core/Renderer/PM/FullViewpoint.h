#pragma once

#include "Core/Renderer/PM/TViewpoint.h"
#include "Core/SurfaceHit.h"
#include "Math/TVector2.h"
#include "Math/Color/Spectrum.h"
#include "Core/SurfaceBehavior/surface_optics_fwd.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <cstddef>

namespace ph
{

/*! @brief This viewpoint type stores all possible viewpoint data without any loss of information.
It is unrecommended to use this kind of viewpoint if low memory usage is desired.
*/
class FullViewpoint : public TViewpoint<FullViewpoint>
{
public:
	FullViewpoint() = default;

	template<EViewpointData TYPE>
	static constexpr bool impl_has();

	template<EViewpointData TYPE>
	decltype(auto) impl_get() const;

	template<EViewpointData TYPE, typename T>
	void impl_set(const T& value);

private:
	SurfaceHit     m_surfaceHit;
	math::Vector2R m_rasterCoord;
	real           m_radius;
	real           m_numPhotons;
	math::Spectrum m_tau;
	math::Spectrum m_viewThroughput;
	math::Vector3R m_viewDir;
	math::Spectrum m_viewRadiance;
};

// In-header Implementations:

template<EViewpointData TYPE>
inline constexpr bool FullViewpoint::impl_has()
{
	if constexpr(
		TYPE == EViewpointData::SurfaceHit     ||
		TYPE == EViewpointData::RasterCoord    ||
		TYPE == EViewpointData::Radius         ||
		TYPE == EViewpointData::NumPhotons     ||
		TYPE == EViewpointData::Tau            ||
		TYPE == EViewpointData::ViewThroughput || 
		TYPE == EViewpointData::ViewDir        || 
		TYPE == EViewpointData::ViewRadiance)
	{
		return true;
	}
	else
	{
		return false;
	}
}

template<EViewpointData TYPE>
inline decltype(auto) FullViewpoint::impl_get() const
{
	if constexpr(TYPE == EViewpointData::SurfaceHit)
	{
		return m_surfaceHit;
	}
	else if constexpr(TYPE == EViewpointData::RasterCoord)
	{
		return math::Vector2D(m_rasterCoord);
	}
	else if constexpr(TYPE == EViewpointData::Radius)
	{
		return m_radius;
	}
	else if constexpr(TYPE == EViewpointData::NumPhotons)
	{
		return m_numPhotons;
	}
	else if constexpr(TYPE == EViewpointData::Tau)
	{
		return m_tau;
	}
	else if constexpr(TYPE == EViewpointData::ViewThroughput)
	{
		return m_viewThroughput;
	}
	else if constexpr(TYPE == EViewpointData::ViewDir)
	{
		return m_viewDir;
	}
	else if constexpr(TYPE == EViewpointData::ViewRadiance)
	{
		return m_viewRadiance;
	}
	else
	{
		PH_ASSERT_UNREACHABLE_SECTION();
		return false;
	}
}

template<EViewpointData TYPE, typename T>
inline void FullViewpoint::impl_set(const T& value)
{
	if constexpr(TYPE == EViewpointData::SurfaceHit)
	{
		m_surfaceHit = value;
	}
	else if constexpr(TYPE == EViewpointData::RasterCoord)
	{
		m_rasterCoord = math::Vector2R(value);
	}
	else if constexpr(TYPE == EViewpointData::Radius)
	{
		m_radius = value;
	}
	else if constexpr(TYPE == EViewpointData::NumPhotons)
	{
		m_numPhotons = value;
	}
	else if constexpr(TYPE == EViewpointData::Tau)
	{
		m_tau = value;
	}
	else if constexpr(TYPE == EViewpointData::ViewThroughput)
	{
		m_viewThroughput = value;
	}
	else if constexpr(TYPE == EViewpointData::ViewDir)
	{
		m_viewDir = value;
	}
	else if constexpr(TYPE == EViewpointData::ViewRadiance)
	{
		m_viewRadiance = value;
	}
	else
	{
		PH_ASSERT_UNREACHABLE_SECTION();
	}
}

}// end namespace ph
