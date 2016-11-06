#include "Image/Film.h"
#include "Math/Vector3f.h"
#include "Image/HDRFrame.h"

#include <cstddef>

namespace ph
{

Film::Film(const uint32 widthPx, const uint32 heightPx) : 
	m_widthPx(widthPx), m_heightPx(heightPx), 
	m_pixelRadianceSensors(static_cast<std::size_t>(3) * widthPx * heightPx, 0.0), 
	m_pixelSenseCounts(static_cast<std::size_t>(widthPx) * static_cast<std::size_t>(heightPx), 0)
{

}

void Film::acculumateRadiance(const uint32 x, const uint32 y, const Vector3f& radiance)
{
	const std::size_t baseIndex = (y * static_cast<std::size_t>(m_widthPx) + x) * 3;

	m_pixelRadianceSensors[baseIndex + 0] += static_cast<float64>(radiance.x);
	m_pixelRadianceSensors[baseIndex + 1] += static_cast<float64>(radiance.y);
	m_pixelRadianceSensors[baseIndex + 2] += static_cast<float64>(radiance.z);

	m_pixelSenseCounts[baseIndex / 3] += 1;
}

void Film::developFilm(HDRFrame* out_hdrFrame) const
{
	/*float64 sensorR;
	float64 sensorG;
	float64 sensorB;
	uint32 senseCount

	for(uint32 y = 0; y < m_heightPx; y++)
	{
		for(uint32 x = 0; x < m_widthPx; x++)
		{
			const std::size_t baseIndex = (y * static_cast<std::size_t>(m_widthPx) + x) * 3;

			sensorR = m_pixelRadianceSensors[baseIndex + 0];
			sensorG = m_pixelRadianceSensors[baseIndex + 1];
			sensorB = m_pixelRadianceSensors[baseIndex + 2];

			sensorR /= static_cast<double>()

			pixelRadianceSensor 

			out_hdrFrame->setPixel

			out_frame->getPixel(x, y, &pixel);
			pixel.addLocal(accuRadiance.div());
			out_frame->setPixel(x, y, pixel.x, pixel.y, pixel.z);
		}
	}*/
}

}// end namespace ph