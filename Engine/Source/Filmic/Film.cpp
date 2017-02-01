#include "Filmic/Film.h"
#include "Math/TVector3.h"
#include "Filmic/Frame.h"
#include "FileIO/InputPacket.h"

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

Film::Film(const InputPacket& packet)
{
	m_widthPx = static_cast<uint32>(packet.getInteger("width", 0, "Film >> argument width not found"));
	m_heightPx = static_cast<uint32>(packet.getInteger("height", 0, "Film >> argument height not found"));

	const std::size_t numSensors = static_cast<std::size_t>(m_widthPx) * static_cast<std::size_t>(m_heightPx);
	m_pixelRadianceSensors = std::vector<RadianceSensor>(numSensors, RadianceSensor());
}

void Film::accumulateRadiance(const uint32 x, const uint32 y, const Vector3R& radiance)
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
	float64 reciSenseCount;
	std::size_t baseIndex;

	out_frame->resize(m_widthPx, m_heightPx);

	for(uint32 y = 0; y < m_heightPx; y++)
	{
		for(uint32 x = 0; x < m_widthPx; x++)
		{
			baseIndex = y * static_cast<std::size_t>(m_widthPx) + x;

			sensorR = m_pixelRadianceSensors[baseIndex].m_accuR;
			sensorG = m_pixelRadianceSensors[baseIndex].m_accuG;
			sensorB = m_pixelRadianceSensors[baseIndex].m_accuB;
			const float64 senseCount = static_cast<float64>(m_pixelRadianceSensors[baseIndex].m_numSenseCounts);

			// to prevent divide by zero
			reciSenseCount = senseCount == 0.0 ? 0.0 : 1.0 / senseCount;

			sensorR *= reciSenseCount;
			sensorG *= reciSenseCount;
			sensorB *= reciSenseCount;

			out_frame->setPixel(x, y, static_cast<real>(sensorR), static_cast<real>(sensorG), static_cast<real>(sensorB));
		}
	}
}

void Film::clear()
{
	std::fill(m_pixelRadianceSensors.begin(), m_pixelRadianceSensors.end(), RadianceSensor());
}

}// end namespace ph