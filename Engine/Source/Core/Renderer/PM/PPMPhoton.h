#pragma once

#include "Core/Renderer/PM/TPhoton.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Math/TVector3.h"
#include "Common/assertion.h"

namespace ph
{

class PPMPhoton : public TPhoton<PPMPhoton>
{
public:
	PPMPhoton() = default;

	template<EPhotonData TYPE>
	static constexpr bool impl_has();

	template<EPhotonData TYPE>
	decltype(auto) impl_get() const;

	template<EPhotonData TYPE, typename T>
	void impl_set(const T& value);

private:
	SpectralStrength m_throughputRadiance;
	Vector3R         m_position;
	Vector3R         m_fromDir;
};

// In-header Implementations:

template<EPhotonData TYPE>
inline constexpr bool PPMPhoton::impl_has()
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
inline decltype(auto) PPMPhoton::impl_get() const
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
inline void PPMPhoton::impl_set(const T& value)
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