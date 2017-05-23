#pragma once

#include "Common/primitive_type.h"
#include "Math/TVector3.tpp"
#include "Math/Vector2f.h"
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
	virtual ~Camera() = 0;

	// TODO: precalculate aspect ratio info from camera film
	virtual void genSensingRay(const Sample& sample, Ray* const out_ray) const = 0;
	virtual void evalEmittedImportanceAndPdfW(const Vector3R& targetPos, Vector2f* const out_filmCoord, Vector3R* const out_importance, real* out_filmArea, real* const out_pdfW) const = 0;

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

	inline void setPosition(const Vector3R& position)
	{
		m_position.set(position);

		updateCameraToWorldTransform(m_position, m_direction, m_upAxis);
	}

	inline void setDirection(const Vector3R& direction)
	{
		// TODO: check the input camera direction is valid

		m_direction.set(direction);
		m_direction.normalizeLocal();

		updateCameraToWorldTransform(m_position, m_direction, m_upAxis);
	}

	inline void setUpAxis(const Vector3R& upAxis)
	{
		// TODO: check the input up-axis is valid (e.g., apart enough to camera direction, not zero vector...)

		m_upAxis = upAxis;
		m_upAxis.normalizeLocal();

		updateCameraToWorldTransform(m_position, m_direction, m_upAxis);
	}

	virtual void onFilmSet(Film* newFilm);

protected:
	TDecomposedTransform<hiReal> m_cameraToWorldTransform;

private:
	Vector3R m_position;
	Vector3R m_direction;
	Vector3R m_upAxis;

	std::shared_ptr<Film> m_film;

	inline void updateCameraToWorldTransform(const Vector3R& position, const Vector3R& direction, const Vector3R& upAxis)
	{
		m_cameraToWorldTransform.setPosition(TVector3<hiReal>(position));
		m_cameraToWorldTransform.setScale(TVector3<hiReal>(0.001, 0.001, 0.001));

		const TVector3<hiReal> zAxis             = TVector3<hiReal>(direction.mul(-1.0f)).normalizeLocal();
		const TVector3<hiReal> xAxis             = TVector3<hiReal>(direction.cross(upAxis)).normalizeLocal();
		const TVector3<hiReal> yAxis             = zAxis.cross(xAxis).normalizeLocal();
		const TMatrix4<hiReal> worldToViewRotMat = TMatrix4<hiReal>().initRotation(xAxis, yAxis, zAxis);
		m_cameraToWorldTransform.setRotation(TQuaternion<hiReal>(worldToViewRotMat).conjugateLocal());
	}

// command interface
public:
	Camera(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static ExitStatus ciExecute(const std::shared_ptr<Camera>& targetResource, const std::string& functionName, const InputPacket& packet);
};

}// end namespace ph