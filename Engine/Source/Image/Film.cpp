#include "Image/Film.h"
#include "Math/Vector3f.h"
#include "Image/Frame.h"

#include <cstddef>
#include <iostream>
#include <algorithm>

namespace ph
{

Film::Film(const uint32 widthPx, const uint32 heightPx) : 
	m_widthPx(widthPx), m_heightPx(heightPx), 
	m_pixelRadianceSensors(static_cast<std::size_t>(3) * widthPx * heightPx, 0.0), 
	m_pixelSenseCounts(static_cast<std::size_t>(widthPx) * static_cast<std::size_t>(heightPx), 0)
{

}

void Film::accumulateRadiance(const uint32 x, const uint32 y, const Vector3f& radiance)
{
	const std::size_t baseIndex = (y * static_cast<std::size_t>(m_widthPx) + x) * 3;

	m_pixelRadianceSensors[baseIndex + 0] += static_cast<float64>(radiance.x);
	m_pixelRadianceSensors[baseIndex + 1] += static_cast<float64>(radiance.y);
	m_pixelRadianceSensors[baseIndex + 2] += static_cast<float64>(radiance.z);

	m_pixelSenseCounts[baseIndex / 3] += 1;
}

void Film::accumulateRadiance(const Film& other)
{
	if(m_widthPx != other.m_widthPx || m_heightPx != other.m_heightPx)
	{
		std::cerr << "warning: at Film::accumulateRadiance(), film dimensions mismatch" << std::endl;
		return;
	}

	const std::size_t nSensors = other.m_pixelRadianceSensors.size();
	for(std::size_t i = 0; i < nSensors; i++)
	{
		m_pixelRadianceSensors[i] += other.m_pixelRadianceSensors[i];
	}

	const std::size_t nSenseCounts = other.m_pixelSenseCounts.size();
	for(std::size_t i = 0; i < nSenseCounts; i++)
	{
		m_pixelSenseCounts[i] += other.m_pixelSenseCounts[i];
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
			baseIndex = (y * static_cast<std::size_t>(m_widthPx) + x) * 3;

			sensorR = m_pixelRadianceSensors[baseIndex + 0];
			sensorG = m_pixelRadianceSensors[baseIndex + 1];
			sensorB = m_pixelRadianceSensors[baseIndex + 2];

			senseCount = static_cast<double>(m_pixelSenseCounts[baseIndex / 3]);

			sensorR /= senseCount;
			sensorG /= senseCount;
			sensorB /= senseCount;

			out_frame->setPixel(x, y, static_cast<float32>(sensorR), static_cast<float32>(sensorG), static_cast<float32>(sensorB));
		}
	}
}

void Film::clear()
{
	std::fill(m_pixelRadianceSensors.begin(), m_pixelRadianceSensors.end(), 0.0f);
	std::fill(m_pixelSenseCounts.begin(), m_pixelSenseCounts.end(), 0);
}

}// end namespace ph