#pragma once

#include "Common/primitive_type.h"
#include "Math/Vector3f.h"
#include "Image/Film.h"

namespace ph
{

class Ray;
class Sample;

class Camera
{
public:
	Camera(const uint32 filmWidthPx, const uint32 filmHeightPx);
	virtual ~Camera() = 0;

	// TODO: get aspect ratio info from camera film
	virtual void genSampleRay(const Sample& sample, Ray* const out_ray, const float32 aspectRatio) const = 0;

	inline const Vector3f& getPosition() const
	{
		return m_position;
	}

	inline const Vector3f& getDirection() const
	{
		return m_direction;
	}

	inline void getPosition(Vector3f* out_position) const
	{
		m_position.set(out_position);
	}

	inline void getDirection(Vector3f* out_direction) const
	{
		m_direction.set(out_direction);
	}

	inline void setPosition(const Vector3f& position)
	{
		m_position.set(position);
	}

	inline void setDirection(const Vector3f& direction)
	{
		m_direction.set(direction);
	}

	inline void acculumateRadiance(const uint32 x, const uint32 y, const Vector3f& radiance)
	{
		m_film.acculumateRadiance(x, y, radiance);
	}

	inline void developFilm(Frame* const out_frame) const
	{
		m_film.developFilm(out_frame);
	}

private:
	Vector3f m_position;
	Vector3f m_direction;

	Film m_film;
};

}// end namespace ph