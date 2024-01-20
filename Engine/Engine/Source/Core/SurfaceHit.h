#pragma once

#include "Core/HitDetail.h"
#include "Core/HitProbe.h"
#include "Core/Ray.h"

namespace ph
{

class SurfaceOptics;
class VolumeOptics;
class Emitter;

class SurfaceHit final
{
public:
	SurfaceHit();
	SurfaceHit(const Ray& incidentRay, const HitProbe& probe);

	SurfaceHit switchChannel(uint32 newChannel) const;

	bool hasSurfaceOptics() const;
	bool hasInteriorOptics() const;
	bool hasExteriorOptics() const;

	const HitDetail& getDetail() const;
	const Ray& getIncidentRay() const;
	math::Vector3R getPosition() const;
	math::Vector3R getShadingNormal() const;
	math::Vector3R getGeometryNormal() const;

	const Emitter* getSurfaceEmitter() const;
	const SurfaceOptics* getSurfaceOptics() const;
	const VolumeOptics* getInteriorOptics() const;
	const VolumeOptics* getExteriorOptics() const;

private:
	Ray       m_incidentRay;
	HitProbe  m_recordedProbe;
	HitDetail m_detail;
};

// In-header Implementations:

inline SurfaceHit::SurfaceHit() :
	m_incidentRay(), m_recordedProbe(), m_detail()
{}

inline SurfaceHit::SurfaceHit(const Ray& incidentRay, const HitProbe& probe) :
	m_incidentRay(incidentRay), m_recordedProbe(probe), m_detail()
{
	HitProbe(m_recordedProbe).calcIntersectionDetail(m_incidentRay, &m_detail);
}

inline const HitDetail& SurfaceHit::getDetail() const
{
	return m_detail;
}

inline const Ray& SurfaceHit::getIncidentRay() const
{
	return m_incidentRay;
}

inline math::Vector3R SurfaceHit::getPosition() const
{
	return m_detail.getPosition();
}

inline math::Vector3R SurfaceHit::getShadingNormal() const
{
	return m_detail.getShadingNormal();
}

inline math::Vector3R SurfaceHit::getGeometryNormal() const
{
	return m_detail.getGeometryNormal();
}

}// end namespace ph
