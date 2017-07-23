#pragma once

#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "FileIO/SDL/ISdlResource.h"
#include "FileIO/SDL/TCommandInterface.h"
#include "Math/Transform/TDecomposedTransform.h"

#include <iostream>
#include <memory>

namespace ph
{

class Ray;
class Sample;
class SampleGenerator;
class Film;
class InputPacket;
class Transform;

class Camera : public TCommandInterface<Camera>, public ISdlResource
{
public:
	Camera();
	Camera(const Vector3R& position, const Vector3R& direction, const Vector3R& upAxis);
	virtual ~Camera() = 0;

	// TODO: precalculate aspect ratio info from camera film
	virtual void genSensingRay(const Vector2R& rasterPosPx, Ray* const out_ray) const = 0;
	virtual void evalEmittedImportanceAndPdfW(const Vector3R& targetPos, Vector2R* const out_filmCoord, Vector3R* const out_importance, real* out_filmArea, real* const out_pdfW) const = 0;

	inline void setFilm(const std::shared_ptr<Film>& film)
	{
		if(film == nullptr)
		{
			std::cerr << "warning: at Film::setFilm(), input is null" << std::endl;
		}

		m_film = film;
		onFilmSet(m_film.get());
	}

	inline Film* getFilm() const
	{
		return m_film.get();
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

	virtual void onFilmSet(Film* newFilm);

protected:
	TDecomposedTransform<hiReal> m_cameraToWorldTransform;

private:
	Vector3R m_position;
	Vector3R m_direction;
	Vector3R m_upAxis;

	std::shared_ptr<Film> m_film;

	void updateCameraToWorldTransform(const Vector3R& position, const Vector3R& direction, const Vector3R& upAxis);

// command interface
public:
	Camera::Camera(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static ExitStatus ciExecute(const std::shared_ptr<Camera>& targetResource, const std::string& functionName, const InputPacket& packet);
};

}// end namespace ph