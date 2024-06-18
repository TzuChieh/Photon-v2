#pragma once

#include "Math/TVector3.h"
#include "Core/Quantity/Time.h"
#include "Core/HitDetail.h"
#include "Core/SurfaceHit.h"
#include "Core/LTA/PDF.h"

#include <Common/assertion.h>

namespace ph { class Primitive; }

namespace ph
{

/*! @brief Input for `DirectEnergyPdfQuery`.
*/
class DirectEnergyPdfInput final
{
public:
	void set(const SurfaceHit& X, const SurfaceHit& Xe);
	void set(const math::Vector3R& targetPos, const SurfaceHit& Xe);

	/*!
	@return The position that receives energy.
	*/
	const math::Vector3R& getTargetPos() const;
	
	/*!
	@return Information about the energy emitting position.
	*/
	const SurfaceHit& getXe() const;

	/*!
	@return The position that emits energy.
	*/
	math::Vector3R getEmitPos() const;

	/*!
	@return Normal of the position that emits energy.
	*/
	math::Vector3R getEmitPosNormal() const;

	/*!
	@return The surface that emits energy.
	*/
	const Primitive* getSrcPrimitive() const;

	/*!
	@return Face ID of the surface that emits energy.
	*/
	uint64 getSrcFaceID() const;

private:
	math::Vector3R m_targetPos;
	SurfaceHit     m_Xe;
#if PH_DEBUG
	bool           m_hasSet{false};
#endif
};

/*! @brief Output for `DirectEnergyPdfQuery`.
*/
class DirectEnergyPdfOutput final
{
public:
	void setPdf(const lta::PDF& pdf);

	real getPdfW() const;
	const lta::PDF& getPdf() const;

	operator bool () const;

private:
	lta::PDF m_pdf{};
};

/*! @brief Information for the probability of generating a specific sample for direct energy estimation.
*/
class DirectEnergyPdfQuery final
{
public:
	using Input  = DirectEnergyPdfInput;
	using Output = DirectEnergyPdfOutput;

	Input  inputs;
	Output outputs;

	DirectEnergyPdfQuery() = default;
};

inline void DirectEnergyPdfInput::set(const SurfaceHit& X, const SurfaceHit& Xe)
{
	set(X.getPos(), Xe);
}

inline void DirectEnergyPdfInput::set(const math::Vector3R& targetPos, const SurfaceHit& Xe)
{
	m_targetPos = targetPos;
	m_Xe        = Xe;

#if PH_DEBUG
	m_hasSet    = true;
#endif
}

inline const math::Vector3R& DirectEnergyPdfInput::getTargetPos() const
{
	PH_ASSERT(m_hasSet);
	return m_targetPos;
}

inline const SurfaceHit& DirectEnergyPdfInput::getXe() const
{
	PH_ASSERT(m_hasSet);
	return m_Xe;
}

inline math::Vector3R DirectEnergyPdfInput::getEmitPos() const
{
	PH_ASSERT(m_hasSet);
	return m_Xe.getPos();
}

inline math::Vector3R DirectEnergyPdfInput::getEmitPosNormal() const
{
	PH_ASSERT(m_hasSet);
	return m_Xe.getShadingNormal();
}

inline const Primitive* DirectEnergyPdfInput::getSrcPrimitive() const
{
	PH_ASSERT(m_hasSet);
	return m_Xe.getDetail().getPrimitive();
}

inline uint64 DirectEnergyPdfInput::getSrcFaceID() const
{
	PH_ASSERT(m_hasSet);
	return m_Xe.getDetail().getFaceID();
}

inline void DirectEnergyPdfOutput::setPdf(const lta::PDF& pdf)
{
	m_pdf = pdf;
}

inline real DirectEnergyPdfOutput::getPdfW() const
{
	return getPdf().getPdfW();
}

inline const lta::PDF& DirectEnergyPdfOutput::getPdf() const
{
	PH_ASSERT(*this);
	return m_pdf;
}

inline DirectEnergyPdfOutput::operator bool () const
{
	return !m_pdf.isEmpty();
}

}// end namespace ph
