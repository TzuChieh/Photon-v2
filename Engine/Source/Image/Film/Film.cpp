#include "Image/Film/Film.h"
#include "Math/Vector3f.h"
#include "Image/Frame.h"

#include <cstddef>
#include <iostream>
#include <algorithm>

namespace ph
{

Film::Film(const uint32 widthPx, const uint32 heightPx) : 
	m_widthPx(widthPx), m_heightPx(heightPx), 
	m_pixelRadianceSensors(static_cast<std::size_t>(widthPx) * static_cast<std::size_t>(heightPx), RadianceSensor())
{

}

void Film::accumulateRadiance(const uint32 x, const uint32 y, const Vector3f& radiance)
{
	const std::size_t baseIndex = y * static_cast<std::size_t>(m_widthPx) + x;

	m_pixelRadianceSensors[baseIndex].m_accuR += static_cast<float64>(radiance.x);
	m_pixelRadianceSensors[baseIndex].m_accuG += static_cast<float64>(radiance.y);
	m_pixelRadianceSensors[baseIndex].m_accuB += static_cast<float64>(radiance.z);
	m_pixelRadianceSensors[baseIndex].m_numSenseCounts += 1;
}

void Film::accumulateRadiance(const Film& other)
{
	if(m_widthPx != other.m_widthPx || m_heightPx != other.m_heightPx)
	{
		std::cerr << "warning: at Film::accumulateRadiance(), film dimensions mismatch" << std::endl;
		return;
	}

	const std::size_t numSensors = other.m_pixelRadianceSensors.size();
	for(std::size_t i = 0; i < numSensors; i++)
	{
		m_pixelRadianceSensors[i].m_accuR += other.m_pixelRadianceSensors[i].m_accuR;
		m_pixelRadianceSensors[i].m_accuG += other.m_pixelRadianceSensors[i].m_accuG;
		m_pixelRadianceSensors[i].m_accuB += other.m_pixelRadianceSensors[i].m_accuB;
		m_pixelRadianceSensors[i].m_numSenseCounts += other.m_pixelRadianceSensors[i].m_numSenseCounts;
	}
}

void Film::developFilm(Frame* const out_frame) const
{
	float64 sensorR;
	float64 sensorG;
	float64 sensorB;
	double senseCount;
	std::size_t baseIndex;

	for(uint32 y = 0; y < m_heightPx; y++)
	{
		for(uint32 x = 0; x < m_widthPx; x++)
		{
			baseIndex = y * static_cast<std::size_t>(m_widthPx) + x;

			sensorR = m_pixelRadianceSensors[baseIndex].m_accuR;
			sensorG = m_pixelRadianceSensors[baseIndex].m_accuG;
			sensorB = m_pixelRadianceSensors[baseIndex].m_accuB;
			senseCount = static_cast<double>(m_pixelRadianceSensors[baseIndex].m_numSenseCounts);

			sensorR /= senseCount;
			sensorG /= senseCount;
			sensorB /= senseCount;

			out_frame->setPixel(x, y, static_cast<float32>(sensorR), static_cast<float32>(sensorG), static_cast<float32>(sensorB));
		}
	}
}

void Film::clear()
{
	std::fill(m_pixelRadianceSensors.begin(), m_pixelRadianceSensors.end(), RadianceSensor());
}

}// end namespace ph