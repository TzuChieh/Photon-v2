#pragma once

#include "Math/TVector3.h"
#include "Core/Ray.h"
#include "Core/HitDetail.h"
#include "Core/SurfaceHit.h"

#include <Common/primitive_type.h>
#include <Common/assertion.h>

#include <cstddef>

namespace ph { class HitDetail; }
namespace ph { class PrimitivePosSampleOutput; }
namespace ph { class DirectEnergySamplePdfInput; }

namespace ph
{

class PrimitivePosSamplePdfInput final
{
public:
	void set(const PrimitivePosSampleOutput& sampleOutput, const SurfaceHit& X);
	void set(const DirectEnergySamplePdfInput& pdfInput);
	void set(const math::Vector3R& pos, const Ray& observationRay, uint64 faceID);

	/*! @brief Get the position for querying PDF for.
	@return The coordinates of the position. This is generally more accurate (and more performant)
	than using `getObservationRay().getHead()`.
	*/
	const math::Vector3R& getPos() const;

	const Ray& getObservationRay() const;
	uint64 getFaceID() const;

private:
	math::Vector3R m_pos{0};
	Ray            m_observationRay;
	uint64         m_faceID = HitDetail::NO_FACE_ID;
#if PH_DEBUG
	bool           m_hasSet{false};
#endif
};

class PrimitivePosSamplePdfOutput final
{
public:
	void setPdfA(real pdfA);

	real getPdfA() const;

	operator bool () const;

private:
	real m_pdfA{0};
};

class PrimitivePosSamplePdfQuery final
{
public:
	using Input  = PrimitivePosSamplePdfInput;
	using Output = PrimitivePosSamplePdfOutput;

	Input  inputs;
	Output outputs;

	PrimitivePosSamplePdfQuery() = default;
};

// In-header Implementations:

inline void PrimitivePosSamplePdfInput::set(
	const math::Vector3R& pos, 
	const Ray&            observationRay,
	const uint64          faceID)
{
	m_pos            = pos;
	m_observationRay = observationRay;
	m_faceID         = faceID;

#if PH_DEBUG
	m_hasSet = true;
#endif
}

inline const math::Vector3R& PrimitivePosSamplePdfInput::getPos() const
{
	return m_pos;
}

inline const Ray& PrimitivePosSamplePdfInput::getObservationRay() const
{
	return m_observationRay;
}

inline uint64 PrimitivePosSamplePdfInput::getFaceID() const
{
	return m_faceID;
}

inline void PrimitivePosSamplePdfOutput::setPdfA(const real pdfA)
{
	m_pdfA = pdfA;
}

inline real PrimitivePosSamplePdfOutput::getPdfA() const
{
	PH_ASSERT(*this);

	return m_pdfA;
}

inline PrimitivePosSamplePdfOutput::operator bool () const
{
	return m_pdfA > 0;
}

}// end namespace ph
