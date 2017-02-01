#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"
#include "Filmic/RadianceSensor.h"

#include <vector>

namespace ph
{

class Frame;
class InputPacket;

class Film final
{
public:
	Film(const uint32 widthPx, const uint32 heightPx);
	Film(const InputPacket& packet);

	void accumulateRadiance(const uint32 x, const uint32 y, const Vector3R& radiance);
	void accumulateRadiance(const Film& other);
	void developFilm(Frame* const out_frame) const;
	void clear();

	inline uint32 getWidthPx() const
	{
		return m_widthPx;
	}

	inline uint32 getHeightPx() const
	{
		return m_heightPx;
	}

	// HACK
	inline void accumulateRadianceWithoutIncrementSenseCount(const uint32 x, const uint32 y, const Vector3R& radiance)
	{
		const std::size_t baseIndex = y * static_cast<std::size_t>(m_widthPx) + x;

		m_pixelRadianceSensors[baseIndex].m_accuR += static_cast<float64>(radiance.x);
		m_pixelRadianceSensors[baseIndex].m_accuG += static_cast<float64>(radiance.y);
		m_pixelRadianceSensors[baseIndex].m_accuB += static_cast<float64>(radiance.z);
	}

	// HACK
	inline void incrementAllSenseCounts()
	{
		for(auto& sensor : m_pixelRadianceSensors)
		{
			sensor.m_numSenseCounts++;
		}
	}

private:
	uint32 m_widthPx;
	uint32 m_heightPx;

	std::vector<RadianceSensor> m_pixelRadianceSensors;
};

}// end namespace ph