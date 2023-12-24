#pragma once

#include "Core/Renderer/PM/TPhoton.h"
#include "Math/Color/Spectrum.h"
#include "Math/TVector3.h"

#include <Common/assertion.h>

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
	math::Vector3R m_position;
	math::Vector3R m_fromDir;
};

// In-header Implementations:

template<EPhotonData TYPE>
inline constexpr bool FullPhoton::impl_has()
{
	if constexpr(
		TYPE == EPhotonData::ThroughputRadiance ||
		TYPE == EPhotonData::Position           || 
		TYPE == EPhotonData::FromDir)
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
	else if constexpr(TYPE == EPhotonData::Position)
	{
		return m_position;
	}
	else if constexpr(TYPE == EPhotonData::FromDir)
	{
		return m_fromDir;
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
	else if constexpr(TYPE == EPhotonData::Position)
	{
		m_position = value;
	}
	else if constexpr(TYPE == EPhotonData::FromDir)
	{
		m_fromDir = value;
	}
	else
	{
		PH_ASSERT_UNREACHABLE_SECTION();
	}
}

}// end namespace ph