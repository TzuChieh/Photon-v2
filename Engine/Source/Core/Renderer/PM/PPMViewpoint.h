#pragma once

#include "Core/Renderer/PM/TViewpoint.h"
#include "Common/primitive_type.h"
#include "Core/SurfaceHit.h"
#include "Math/TVector2.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Common/assertion.h"

#include <cstddef>

namespace ph
{

class PPMViewpoint : public TViewpoint<PPMViewpoint>
{
public:
	PPMViewpoint() = default;

	template<EViewpointData TYPE>
	static constexpr bool impl_has();

	template<EViewpointData TYPE>
	decltype(auto) impl_get() const;

	template<EViewpointData TYPE, typename T>
	void impl_set(const T& value);

private:
	SurfaceHit       m_surfaceHit;
	Vector2R         m_filmNdc;
	real             m_radius;
	real             m_numPhotons;
	SpectralStrength m_tau;
	SpectralStrength m_viewThroughput;
	Vector3R         m_viewDir;
};

// In-header Implementations:

template<EViewpointData TYPE>
inline constexpr bool PPMViewpoint::impl_has()
{
	if constexpr(
		TYPE == EViewpointData::SURFACE_HIT     ||
		TYPE == EViewpointData::FILM_NDC        ||
		TYPE == EViewpointData::RADIUS          ||
		TYPE == EViewpointData::NUM_PHOTONS     ||
		TYPE == EViewpointData::TAU             ||
		TYPE == EViewpointData::VIEW_THROUGHPUT || 
		TYPE == EViewpointData::VIEW_DIR)
	{
		return true;
	}
	else
	{
		return false;
	}
}

template<EViewpointData TYPE>
inline decltype(auto) PPMViewpoint::impl_get() const
{
	if constexpr(TYPE == EViewpointData::SURFACE_HIT) {
		return m_surfaceHit;
	}
	else if constexpr(TYPE == EViewpointData::FILM_NDC) {
		return m_filmNdc;
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
	else {
		PH_ASSERT_UNREACHABLE_SECTION();
		return false;
	}
}

template<EViewpointData TYPE, typename T>
inline void PPMViewpoint::impl_set(const T& value)
{
	if constexpr(TYPE == EViewpointData::SURFACE_HIT) {
		m_surfaceHit = value;
	}
	else if constexpr(TYPE == EViewpointData::FILM_NDC) {
		m_filmNdc = value;
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
	else {
		PH_ASSERT_UNREACHABLE_SECTION();
	}
}

}// end namespace ph