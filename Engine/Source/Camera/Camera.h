#pragma once

#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "Math/Vector2f.h"

#include <iostream>

namespace ph
{

class Ray;
class Sample;
class SampleGenerator;
class Film;
class InputPacket;

class Camera
{
public:
	Camera();
	Camera(const InputPacket& packet);
	virtual ~Camera() = 0;

	// TODO: precalculate aspect ratio info from camera film
	virtual void genSensingRay(const Sample& sample, Ray* const out_ray) const = 0;
	virtual void evalEmittedImportanceAndPdfW(const Vector3R& targetPos, Vector2f* const out_filmCoord, Vector3R* const out_importance, real* out_filmArea, real* const out_pdfW) const = 0;

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

	inline const Vector3R& getPosition() const
	{
		return m_position;
	}

	inline const Vector3R& getDirection() const
	{
		return m_direction;
	}

	inline const Vector3R& getUpAxis() const
	{
		return m_upAxis;
	}

	inline void getPosition(Vector3R* const out_position) const
	{
		m_position.set(out_position);
	}

	inline void getDirection(Vector3R* const out_direction) const
	{
		m_direction.set(out_direction);
	}

	inline void setPosition(const Vector3R& position)
	{
		m_position.set(position);
	}

	inline void setDirection(const Vector3R& direction)
	{
		// TODO: check the input camera direction is valid

		m_direction.set(direction);
		m_direction.normalizeLocal();
	}

	inline void setUpAxis(const Vector3R& upAxis)
	{
		// TODO: check the input up-axis is valid (e.g., apart enough to camera direction, not zero vector...)

		m_upAxis = upAxis;
		m_upAxis.normalizeLocal();
	}

private:
	Vector3R m_position;
	Vector3R m_direction;
	Vector3R m_upAxis;

	Film* m_film;
};

}// end namespace ph