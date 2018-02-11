#pragma once

#include "Core/HitDetail.h"
#include "Core/Ray.h"

namespace ph
{

class SurfaceHit final
{
public:
	inline SurfaceHit() :
		m_incidentRay(), m_detail()
	{

	}

	inline SurfaceHit(const Ray& incidentRay, const HitDetail& detail) : 
		m_incidentRay(incidentRay), m_detail(detail)
	{

	}

	inline const HitDetail& getDetail() const
	{
		return m_detail;
	}

	inline const Ray& getIncidentRay() const
	{
		return m_incidentRay;
	}

	inline const Vector3R& getPosition() const
	{
		return m_detail.getPosition();
	}

	inline const Vector3R& getShadingNormal() const
	{
		return m_detail.getShadingNormal();
	}

	inline const Vector3R& getGeometryNormal() const
	{
		return m_detail.getGeometryNormal();
	}

private:
	Ray       m_incidentRay;
	HitDetail m_detail;
};

}// end namespace ph