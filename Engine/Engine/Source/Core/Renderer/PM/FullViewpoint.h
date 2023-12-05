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

/*
	This viewpoint type stores all possible viewpoint data without any loss of
	information. It is unrecommended to use this kind of viewpoint if low memory
	usage is desired.
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
		TYPE == EViewpointData::SURFACE_HIT       ||
		TYPE == EViewpointData::RASTER_COORD      ||
		TYPE == EViewpointData::RADIUS            ||
		TYPE == EViewpointData::NUM_PHOTONS       ||
		TYPE == EViewpointData::TAU               ||
		TYPE == EViewpointData::VIEW_THROUGHPUT   || 
		TYPE == EViewpointData::VIEW_DIR          || 
		TYPE == EViewpointData::VIEW_RADIANCE)
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
	if constexpr(TYPE == EViewpointData::SURFACE_HIT) {
		return m_surfaceHit;
	}
	else if constexpr(TYPE == EViewpointData::RASTER_COORD) {
		return math::Vector2D(m_rasterCoord);
	}
	else if constexpr(TYPE == EViewpointData::RADIUS) {
		return m_radius;
	}
	else if constexpr(TYPE == EViewpointData::NUM_PHOTONS) {
		return m_numPhotons;
	}
	else if constexpr(TYPE == EViewpointData::TAU) {
		return m_tau;
	}
	else if constexpr(TYPE == EViewpointData::VIEW_THROUGHPUT) {
		return m_viewThroughput;
	}
	else if constexpr(TYPE == EViewpointData::VIEW_DIR) {
		return m_viewDir;
	}
	else if constexpr(TYPE == EViewpointData::VIEW_RADIANCE) {
		return m_viewRadiance;
	}
	else {
		PH_ASSERT_UNREACHABLE_SECTION();
		return false;
	}
}

template<EViewpointData TYPE, typename T>
inline void FullViewpoint::impl_set(const T& value)
{
	if constexpr(TYPE == EViewpointData::SURFACE_HIT) {
		m_surfaceHit = value;
	}
	else if constexpr(TYPE == EViewpointData::RASTER_COORD) {
		m_rasterCoord = math::Vector2R(value);
	}
	else if constexpr(TYPE == EViewpointData::RADIUS) {
		m_radius = value;
	}
	else if constexpr(TYPE == EViewpointData::NUM_PHOTONS) {
		m_numPhotons = value;
	}
	else if constexpr(TYPE == EViewpointData::TAU) {
		m_tau = value;
	}
	else if constexpr(TYPE == EViewpointData::VIEW_THROUGHPUT) {
		m_viewThroughput = value;
	}
	else if constexpr(TYPE == EViewpointData::VIEW_DIR) {
		m_viewDir = value;
	}
	else if constexpr(TYPE == EViewpointData::VIEW_RADIANCE) {
		m_viewRadiance = value;
	}
	else {
		PH_ASSERT_UNREACHABLE_SECTION();
	}
}

}// end namespace ph
