#pragma once

#include "Common/primitive_type.h"

#include <vector>

namespace ph
{

class Vector3f;
class Frame;

class Film
{
public:
	Film(const uint32 widthPx, const uint32 heightPx);

	void acculumateRadiance(const uint32 x, const uint32 y, const Vector3f& radiance);
	void developFilm(Frame* const out_frame) const;

private:
	uint32 m_widthPx;
	uint32 m_heightPx;

	std::vector<float64> m_pixelRadianceSensors;
	std::vector<uint32> m_pixelSenseCounts;
};

}// end namespace ph