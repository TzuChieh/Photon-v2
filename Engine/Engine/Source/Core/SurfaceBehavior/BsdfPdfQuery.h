#pragma once

#include "Core/SurfaceBehavior/bsdf_query_fwd.h"
#include "Core/SurfaceBehavior/BsdfQueryContext.h"
#include "Math/TVector3.h"
#include "Core/SurfaceHit.h"

#include <Common/assertion.h>

#include <cmath>
#include <string>

namespace ph
{

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

class BsdfPdfOutput final
{
public:
	void setSampleDirPdfW(real pdfW);

	/*!
	@return PDF (solid angle domain) of a BSDF sample being on a specific direction. Guaranteed to
	be finite.
	*/
	real getSampleDirPdfW() const;

private:
	real m_sampleDirPdfW{0};
};

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

inline void BsdfPdfOutput::setSampleDirPdfW(const real pdfW)
{
	m_sampleDirPdfW = pdfW;
}

inline real BsdfPdfOutput::getSampleDirPdfW() const
{
	// Optics implementation is responsible to provide non-infinite and non-NaN PDF
	PH_ASSERT_MSG(std::isfinite(m_sampleDirPdfW), "pdfW = " + std::to_string(m_sampleDirPdfW));

	return m_sampleDirPdfW;
}

}// end namespace ph
