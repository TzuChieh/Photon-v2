#pragma once

#include "Common/primitive_type.h"
#include "Math/Vector3f.h"
#include "Math/Vector2f.h"

#include <iostream>

namespace ph
{

class Ray;
class Sample;
class SampleGenerator;
class Film;

class Camera
{
public:
	Camera();
	virtual ~Camera() = 0;

	// TODO: precalculate aspect ratio info from camera film
	virtual void genSensingRay(const Sample& sample, Ray* const out_ray) const = 0;
	virtual void evalEmittedImportanceAndPdfW(const Vector3f& targetPos, Vector2f* const out_filmCoord, Vector3f* const out_importance, float32* const out_pdfW) const = 0;

	inline void setFilm(Film* film)
	{
		if(film == nullptr)
		{
			std::cerr << "warning: at Film::setFilm(), input is null" << std::endl;
		}

		m_film = film;
	}

	inline Film* getFilm() const
	{
		return m_film;
	}

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

private:
	Vector3f m_position;
	Vector3f m_direction;

	Film* m_film;
};

}// end namespace ph