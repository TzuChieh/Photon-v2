#pragma once

#include "Math/TVector3.h"

namespace ph
{

class Primitive;

class IntersectionDetail final
{
public:
	IntersectionDetail();

	inline const Vector3R& getPosition() const
	{
		return m_position;
	}

	inline const Vector3R& getSmoothNormal() const
	{
		return m_smoothNormal;
	}

	inline const Vector3R& getGeoNormal() const
	{
		return m_geoNormal;
	}

	inline const Vector3R& getUVW() const
	{
		return m_uvw;
	}

	inline const Primitive* getPrimitive() const
	{
		return m_primitive;
	}

	inline real getRayT() const
	{
		return m_rayT;
	}

	inline const Vector3R& getdPdU() const
	{
		return m_dPdU;
	}

	inline const Vector3R& getdPdV() const
	{
		return m_dPdV;
	}

	inline void setAttributes(
		const Primitive* primitive,
		const Vector3R&  position,
		const Vector3R&  smoothNormal,
		const Vector3R&  geoNormal,
		const Vector3R&  uvw, 
		const real       rayT)
	{
		m_primitive    = primitive;
		m_position     = position;
		m_smoothNormal = smoothNormal;
		m_geoNormal    = geoNormal;
		m_uvw          = uvw;
		m_rayT         = rayT;
	}

	inline void setDerivatives(
		const Vector3R& dPdU,
		const Vector3R& dPdV)
	{
		m_dPdU = dPdU;
		m_dPdV = dPdV;
	}

private:
	const Primitive* m_primitive;
	Vector3R         m_position;
	Vector3R         m_smoothNormal;
	Vector3R         m_geoNormal;
	Vector3R         m_uvw;
	real             m_rayT;

	Vector3R m_dPdU;
	Vector3R m_dPdV;
};

}// end namespace ph