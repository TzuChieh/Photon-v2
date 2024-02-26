#pragma once

#include "Math/TVector3.h"
#include "Core/Ray.h"
#include "Core/Quantity/Time.h"
#include "Core/HitDetail.h"
#include "Core/SurfaceHit.h"
#include "Math/Color/Spectrum.h"

#include <Common/assertion.h>

namespace ph { class Primitive; }

namespace ph
{

class DirectEnergySamplePdfInput final
{
public:
	void set(const SurfaceHit& X, const SurfaceHit& Xe);

	void set(
		const SurfaceHit& X, 
		const SurfaceHit& Xe, 
		const Ray& observationRay);

	void set(
		const math::Vector3R& targetPos, 
		const SurfaceHit& Xe,
		const Ray& observationRay);

	const math::Vector3R& getTargetPos() const;
	const SurfaceHit& getXe() const;
	const Ray& getObservationRay() const;
	math::Vector3R getEmitPos() const;
	math::Vector3R getEmitPosNormal() const;
	const Primitive* getSrcPrimitive() const;
	uint64 getSrcFaceID() const;
	const Time& getTime() const;

private:
	math::Vector3R m_targetPos;
	SurfaceHit m_Xe;
	Ray m_observationRay;
#if PH_DEBUG
	bool m_hasSet{false};
#endif
};

class DirectEnergySamplePdfOutput final
{
public:
	void setPdfW(real pdfW);

	real getPdfW() const;

	operator bool () const;

private:
	real m_pdfW{0};
};

class DirectEnergySamplePdfQuery final
{
public:
	using Input  = DirectEnergySamplePdfInput;
	using Output = DirectEnergySamplePdfOutput;

	Input  inputs;
	Output outputs;

	DirectEnergySamplePdfQuery() = default;
};

inline void DirectEnergySamplePdfInput::set(const SurfaceHit& X, const SurfaceHit& Xe)
{
	PH_ASSERT(X.getTime() == Xe.getTime());

	set(
		X, 
		Xe, 
		Ray(
			X.getPosition(),
			Xe.getPosition() - X.getPosition(),
			0,
			1,
			X.getTime()));
}

inline void DirectEnergySamplePdfInput::set(
	const SurfaceHit& X,
	const SurfaceHit& Xe,
	const Ray& observationRay)
{
	set(
		X.getPosition(), 
		Xe, 
		observationRay);
}

inline void DirectEnergySamplePdfInput::set(
	const math::Vector3R& targetPos,
	const SurfaceHit& Xe,
	const Ray& observationRay)
{
	m_targetPos = targetPos;
	m_Xe = Xe;
	m_observationRay = observationRay;

#if PH_DEBUG
	m_hasSet = true;
#endif
}

inline const math::Vector3R& DirectEnergySamplePdfInput::getTargetPos() const
{
	PH_ASSERT(m_hasSet);
	return m_targetPos;
}

inline const SurfaceHit& DirectEnergySamplePdfInput::getXe() const
{
	PH_ASSERT(m_hasSet);
	return m_Xe;
}

inline const Ray& DirectEnergySamplePdfInput::getObservationRay() const
{
	PH_ASSERT(m_hasSet);
	return m_observationRay;
}

inline math::Vector3R DirectEnergySamplePdfInput::getEmitPos() const
{
	PH_ASSERT(m_hasSet);
	return m_Xe.getPosition();
}

inline math::Vector3R DirectEnergySamplePdfInput::getEmitPosNormal() const
{
	PH_ASSERT(m_hasSet);
	return m_Xe.getShadingNormal();
}

inline const Primitive* DirectEnergySamplePdfInput::getSrcPrimitive() const
{
	PH_ASSERT(m_hasSet);
	return m_Xe.getDetail().getPrimitive();
}

inline uint64 DirectEnergySamplePdfInput::getSrcFaceID() const
{
	PH_ASSERT(m_hasSet);
	return m_Xe.getDetail().getFaceID();
}

inline const Time& DirectEnergySamplePdfInput::getTime() const
{
	PH_ASSERT(m_hasSet);
	return m_observationRay.getTime();
}

inline void DirectEnergySamplePdfOutput::setPdfW(const real pdfW)
{
	m_pdfW = pdfW;
}

inline real DirectEnergySamplePdfOutput::getPdfW() const
{
	PH_ASSERT(*this);
	return m_pdfW;
}

inline DirectEnergySamplePdfOutput::operator bool () const
{
	return m_pdfW > 0;
}

}// end namespace ph
