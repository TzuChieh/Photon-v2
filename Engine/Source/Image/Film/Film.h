#pragma once

#include "Common/primitive_type.h"
#include "Image/Film/RadianceSensor.h"

#include <vector>

namespace ph
{

class Vector3f;
class Frame;

class Film
{
public:
	Film(const uint32 widthPx, const uint32 heightPx);

	void accumulateRadiance(const uint32 x, const uint32 y, const Vector3f& radiance);
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

private:
	uint32 m_widthPx;
	uint32 m_heightPx;

	std::vector<RadianceSensor> m_pixelRadianceSensors;
};

}// end namespace ph