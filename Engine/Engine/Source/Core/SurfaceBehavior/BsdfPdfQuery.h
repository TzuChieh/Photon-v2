#pragma once

#include "Core/SurfaceBehavior/bsdf_query_fwd.h"
#include "Core/SurfaceBehavior/BsdfQueryContext.h"
#include "Math/TVector3.h"
#include "Core/SurfaceHit.h"
#include "Core/LTA/PDF.h"

#include <Common/assertion.h>

#include <string>

namespace ph
{

/*! @brief Input for `BsdfPdfQuery`.
*/
class BsdfPdfInput final
{
public:
	void set(const BsdfEvalInput& evalInput);
	void set(const BsdfSampleQuery& sample);
	void set(const BsdfSampleInput& sampleInput, const BsdfSampleOutput& sampleOutput);

	void set(
		const SurfaceHit&     X, 
		const math::Vector3R& L,
		const math::Vector3R& V);

	const SurfaceHit& getX() const;
	const math::Vector3R& getL() const;
	const math::Vector3R& getV() const;

private:
	SurfaceHit     m_X;
	math::Vector3R m_L;
	math::Vector3R m_V;
#if PH_DEBUG
	bool           m_hasSet{false};
#endif
};

/*! @brief Output for `BsdfPdfQuery`.
*/
class BsdfPdfOutput final
{
public:
	void setSampleDirPdf(const lta::PDF& pdf);

	real getSampleDirPdfW() const;

	/*!
	@return PDF of a BSDF sample being on a specific direction. Guaranteed to be non-empty.
	*/
	const lta::PDF& getSampleDirPdf() const;

private:
	lta::PDF m_sampleDirPdf{};
};

/*! @brief Information for the probability of generating a specific BSDF sample.
*/
class BsdfPdfQuery final
{
public:
	using Input  = BsdfPdfInput;
	using Output = BsdfPdfOutput;

	BsdfQueryContext context = BsdfQueryContext{};
	Input            inputs;
	Output           outputs;

	BsdfPdfQuery() = default;
	explicit BsdfPdfQuery(BsdfQueryContext context);
};

// In-header Implementations:

inline BsdfPdfQuery::BsdfPdfQuery(BsdfQueryContext context)
	: BsdfPdfQuery()
{
	this->context = std::move(context);

	// (rest of the fields are initialized via setters)
}

inline void BsdfPdfInput::set(
	const SurfaceHit&     X,
	const math::Vector3R& L,
	const math::Vector3R& V)
{
	// Not querying from uninitialized surface hit
	PH_ASSERT(!X.getReason().hasExactly(ESurfaceHitReason::Invalid));

	PH_ASSERT_IN_RANGE(L.lengthSquared(), 0.9_r, 1.1_r);
	PH_ASSERT_IN_RANGE(V.lengthSquared(), 0.9_r, 1.1_r);

	m_X = X;
	m_L = L;
	m_V = V;

#if PH_DEBUG
	m_hasSet = true;
#endif
}

inline const SurfaceHit& BsdfPdfInput::getX() const
{
	PH_ASSERT(m_hasSet);

	return m_X;
}

inline const math::Vector3R& BsdfPdfInput::getL() const
{
	PH_ASSERT(m_hasSet);

	return m_L;
}

inline const math::Vector3R& BsdfPdfInput::getV() const
{
	PH_ASSERT(m_hasSet);

	return m_V;
}

inline void BsdfPdfOutput::setSampleDirPdf(const lta::PDF& pdf)
{
	m_sampleDirPdf = pdf;
}

inline real BsdfPdfOutput::getSampleDirPdfW() const
{
	return getSampleDirPdf().getPdfW();
}

inline const lta::PDF& BsdfPdfOutput::getSampleDirPdf() const
{
	// Query responder is responsible to provide non-empty PDF
	PH_ASSERT_MSG(!m_sampleDirPdf.isEmpty(), "pdf = " + std::to_string(m_sampleDirPdf.value));

	return m_sampleDirPdf;
}

}// end namespace ph
