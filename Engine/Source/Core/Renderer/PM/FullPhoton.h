#pragma once

#include "Core/Renderer/PM/TPhoton.h"
#include "Core/Quantity/Spectrum.h"
#include "Math/TVector3.h"
#include "Common/assertion.h"

namespace ph
{

/*
	This photon type stores all possible photon data without any loss of 
	information. It is unrecommended to use this kind of photon if low memory
	usage is desired.
*/
class FullPhoton : public TPhoton<FullPhoton>
{
public:
	FullPhoton() = default;

	template<EPhotonData TYPE>
	static constexpr bool impl_has();

	template<EPhotonData TYPE>
	decltype(auto) impl_get() const;

	template<EPhotonData TYPE, typename T>
	void impl_set(const T& value);

private:
	Spectrum       m_throughputRadiance;
	math::Vector3R m_position;
	math::Vector3R m_fromDir;
};

// In-header Implementations:

template<EPhotonData TYPE>
inline constexpr bool FullPhoton::impl_has()
{
	if constexpr(
		TYPE == EPhotonData::THROUGHPUT_RADIANCE ||
		TYPE == EPhotonData::POSITION            || 
		TYPE == EPhotonData::FROM_DIR)
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
	if constexpr(TYPE == EPhotonData::THROUGHPUT_RADIANCE)
	{
		return m_throughputRadiance;
	}
	else if constexpr(TYPE == EPhotonData::POSITION)
	{
		return m_position;
	}
	else if constexpr(TYPE == EPhotonData::FROM_DIR)
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
	if constexpr(TYPE == EPhotonData::THROUGHPUT_RADIANCE)
	{
		m_throughputRadiance = value;
	}
	else if constexpr(TYPE == EPhotonData::POSITION)
	{
		m_position = value;
	}
	else if constexpr(TYPE == EPhotonData::FROM_DIR)
	{
		m_fromDir = value;
	}
	else
	{
		PH_ASSERT_UNREACHABLE_SECTION();
	}
}

}// end namespace ph