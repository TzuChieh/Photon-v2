#pragma once

#include "Core/HitDetail.h"
#include "Core/HitProbe.h"
#include "Core/Ray.h"

namespace ph
{

class SurfaceHit final
{
public:
	inline SurfaceHit() :
		m_incidentRay(), m_recordedProbe(), m_detail()
	{}

	inline SurfaceHit(const Ray& incidentRay, const HitProbe& probe) : 
		m_incidentRay(incidentRay), m_recordedProbe(probe), m_detail()
	{
		HitProbe(m_recordedProbe).calcIntersectionDetail(m_incidentRay, &m_detail);
	}

	SurfaceHit switchChannel(uint32 newChannel) const;

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
	HitProbe  m_recordedProbe;
	HitDetail m_detail;
};

}// end namespace ph