#pragma once

#include "Core/Renderer/PM/TPhoton.h"
#include "Math/Color/Spectrum.h"
#include "Math/TVector3.h"

#include <Common/utility.h>
#include <Common/assertion.h>
#include <Common/primitive_type.h>

namespace ph
{

/*! @brief This photon type stores all possible photon data without any loss of information.
It is unrecommended to use this kind of photon if low memory usage is desired.
*/
class FullPhoton : public TPhoton<FullPhoton>
{
public:
	template<EPhotonData TYPE>
	static constexpr bool impl_has();

	template<EPhotonData TYPE>
	decltype(auto) impl_get() const;

	template<EPhotonData TYPE, typename T>
	void impl_set(const T& value);

private:
	math::Spectrum m_throughputRadiance;
	math::Vector3R m_pos;
	math::Vector3R m_fromDir;
	math::Vector3R m_geometryNormal;
	uint32         m_pathLength;
};

// In-header Implementations:

template<EPhotonData TYPE>
inline constexpr bool FullPhoton::impl_has()
{
	if constexpr(
		TYPE == EPhotonData::ThroughputRadiance ||
		TYPE == EPhotonData::Pos                || 
		TYPE == EPhotonData::FromDir            ||
		TYPE == EPhotonData::GeometryNormal     ||
		TYPE == EPhotonData::PathLength)
	{
		return true;
	}
	else
	{
		return false;
	}
}

template<EPhotonData TYPE>
inline decltype(auto) FullPhoton::impl_get() const
{
	if constexpr(TYPE == EPhotonData::ThroughputRadiance)
	{
		return m_throughputRadiance;
	}
	else if constexpr(TYPE == EPhotonData::Pos)
	{
		return m_pos;
	}
	else if constexpr(TYPE == EPhotonData::FromDir)
	{
		return m_fromDir;
	}
	else if constexpr(TYPE == EPhotonData::GeometryNormal)
	{
		return m_geometryNormal;
	}
	else if constexpr(TYPE == EPhotonData::PathLength)
	{
		return m_pathLength;
	}
	else
	{
		PH_ASSERT_UNREACHABLE_SECTION();
		return false;
	}
}

template<EPhotonData TYPE, typename T>
inline void FullPhoton::impl_set(const T& value)
{
	if constexpr(TYPE == EPhotonData::ThroughputRadiance)
	{
		m_throughputRadiance = value;
	}
	else if constexpr(TYPE == EPhotonData::Pos)
	{
		m_pos = value;
	}
	else if constexpr(TYPE == EPhotonData::FromDir)
	{
		m_fromDir = value;
	}
	else if constexpr(TYPE == EPhotonData::GeometryNormal)
	{
		m_geometryNormal = value;
	}
	else if constexpr(TYPE == EPhotonData::PathLength)
	{
		m_pathLength = lossless_cast<uint32>(value);
	}
	else
	{
		PH_ASSERT_UNREACHABLE_SECTION();
	}
}

}// end namespace ph