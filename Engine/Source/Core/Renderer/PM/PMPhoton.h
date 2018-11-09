#pragma once

#include "Core/Renderer/PM/TPhoton.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Math/TVector3.h"
#include "Common/assertion.h"

namespace ph
{

class PMPhoton : public TPhoton<PMPhoton>
{
public:
	PMPhoton() = default;

	template<EPhotonData TYPE>
	static constexpr bool impl_has();

	template<EPhotonData TYPE>
	decltype(auto) impl_get() const;

	template<EPhotonData TYPE, typename T>
	void impl_set(const T& value);

private:
	SpectralStrength m_radiance;
	SpectralStrength m_throughput;
	Vector3R         m_position;
	Vector3R         m_incidentDir;
};

template<EPhotonData TYPE>
inline constexpr bool PMPhoton::impl_has()
{
	if constexpr(
		TYPE == EPhotonData::RADIANCE   || 
		TYPE == EPhotonData::THROUGHPUT || 
		TYPE == EPhotonData::POSITION   || 
		TYPE == EPhotonData::INCIDENT_DIR)
	{
		return true;
	}
	else
	{
		return false;
	}
}

template<EPhotonData TYPE>
inline decltype(auto) PMPhoton::impl_get() const
{
	if constexpr(TYPE == EPhotonData::RADIANCE)
	{
		return m_radiance;
	}
	else if constexpr(TYPE == EPhotonData::THROUGHPUT)
	{
		return m_throughput;
	}
	else if constexpr(TYPE == EPhotonData::POSITION)
	{
		return m_position;
	}
	else if constexpr(TYPE == EPhotonData::INCIDENT_DIR)
	{
		return m_incidentDir;
	}
	else
	{
		PH_ASSERT_UNREACHABLE_SECTION();
		return false;
	}
}

template<EPhotonData TYPE, typename T>
inline void PMPhoton::impl_set(const T& value)
{
	if constexpr(TYPE == EPhotonData::RADIANCE)
	{
		m_radiance = value;
	}
	else if constexpr(TYPE == EPhotonData::THROUGHPUT)
	{
		m_throughput = value;
	}
	else if constexpr(TYPE == EPhotonData::POSITION)
	{
		m_position = value;
	}
	else if constexpr(TYPE == EPhotonData::INCIDENT_DIR)
	{
		m_incidentDir = value;
	}
	else
	{
		PH_ASSERT_UNREACHABLE_SECTION();
	}
}

}// end namespace ph