#pragma once

#include "Core/SurfaceBehavior/bsdf_query_fwd.h"
#include "Math/TVector3.h"
#include "Core/SurfaceHit.h"
#include "Math/Color/Spectrum.h"
#include "Core/SurfaceBehavior/BsdfQueryContext.h"

#include <Common/assertion.h>

#include <utility>

namespace ph
{

class BsdfSampleInput final
{
public:
	void set(const BsdfEvalInput& evalInput);
	void set(const SurfaceHit& X, const math::Vector3R& V);

	const SurfaceHit& getX() const;
	const math::Vector3R& getV() const;

private:
	SurfaceHit     m_X;
	math::Vector3R m_V;
#if PH_DEBUG
	bool           m_hasSet{false};
#endif
};

/*!
@note It is an error to get output data if `isMeasurable()` returns `false`.
*/
class BsdfSampleOutput final
{
public:
	void setL(const math::Vector3R& L);

	/*!
	@param pdfAppliedBsdfCos The sampled BSDF in a coupled form. See `getPdfAppliedBsdfCos()` for
	more information.
	@param cos The contained Lambert's cosine term (absolute value). See `getCos()` for
	more information.
	@param inferMeasurability Whether to determine measurability from the supplied data. All supplied
	data must be sane for the sample to be measurable.
	*/
	void setPdfAppliedBsdfCos(
		const math::Spectrum& pdfAppliedBsdfCos,
		real cos,
		bool inferMeasurability = true);

	/*!
	@return Sampled direction (normalized).
	*/
	const math::Vector3R& getL() const;

	/*!
	@return Lambert's cosine term. Guaranteed to be > 0. This is the absolute value of
	@f$ N \cdot L @f$, where @f$ N @f$ is the normal vector and @f$ L @f$ is the sampled direction.
	@note This cosine term is an absolute value. You may need to consult the corresponding `SurfaceHit`
	if its sign is needed.
	*/
	real getCos() const;

	/*!
	@return Sampled BSDF with PDF (solid angle domain) applied. Guaranteed to be finite.
	Specifically, it is equivalent to @f$ BSDF / PDF_{\omega} @f$.
	*/
	math::Spectrum getPdfAppliedBsdf() const;

	/*!
	Prefer using this method when applicable instead of applying the cosine term manually.
	The coupled form often results in less computation and potentially offers better numerical
	stability as some terms can often be canceled out.
	@return Sampled BSDF with Lambert's cosine law and PDF (solid angle domain) applied.
	Guaranteed to be finite. Specifically, it is equivalent to
	@f$ BSDF * \lvert\cos\theta\rvert / PDF_{\omega} @f$.
	@note This cosine term is an absolute value. You may need to consult the corresponding `SurfaceHit`
	if its sign is needed.
	*/
	const math::Spectrum& getPdfAppliedBsdfCos() const;

	/*! @brief Tells whether this sample has potential to contribute.
	All sampled data should be usable if true is returned; otherwise, zero contribution is implied,
	and sampled data is undefined. This method is also an efficient way to decide whether the BSDF
	sample has sane value (compared to manually testing its value).
	@note Measurability has nothing to do with whether the sampled value is 0 or not.
	*/
	bool isMeasurable() const;

	/*! @brief Set measurability directly.
	*/
	void setMeasurability(bool measurability);

	/*! @brief Set measurability based on a reference spectrum.
	*/
	void setMeasurability(const math::Spectrum& reference);

private:
	math::Vector3R m_L{0};
	math::Spectrum m_pdfAppliedBsdfCos{0};
	real           m_cos{0};
	bool           m_isMeasurable{false};
};

class BsdfSampleQuery final
{
public:
	using Input  = BsdfSampleInput;
	using Output = BsdfSampleOutput;

	BsdfQueryContext context = BsdfQueryContext{};
	Input            inputs;
	Output           outputs;

	BsdfSampleQuery() = default;
	explicit BsdfSampleQuery(BsdfQueryContext context);
};

// In-header Implementations:

inline BsdfSampleQuery::BsdfSampleQuery(BsdfQueryContext context)
	: BsdfSampleQuery()
{
	this->context = std::move(context);

	// (rest of the fields are initialized via setters)
}

inline void BsdfSampleInput::set(const SurfaceHit& X, const math::Vector3R& V)
{
	// Not querying from uninitialized surface hit
	PH_ASSERT(!X.getReason().hasExactly(ESurfaceHitReason::Invalid));

	PH_ASSERT_IN_RANGE(V.lengthSquared(), 0.9_r, 1.1_r);

	m_X = X;
	m_V = V;

#if PH_DEBUG
	m_hasSet = true;
#endif
}

inline const SurfaceHit& BsdfSampleInput::getX() const
{
	PH_ASSERT(m_hasSet);

	return m_X;
}

inline const math::Vector3R& BsdfSampleInput::getV() const
{
	PH_ASSERT(m_hasSet);

	return m_V;
}

inline void BsdfSampleOutput::setL(const math::Vector3R& L)
{
	PH_ASSERT_IN_RANGE(L.lengthSquared(), 0.9_r, 1.1_r);

	m_L = L;
}

inline void BsdfSampleOutput::setPdfAppliedBsdfCos(
	const math::Spectrum& pdfAppliedBsdfCos,
	const real cos,
	const bool inferMeasurability)
{
	m_pdfAppliedBsdfCos = pdfAppliedBsdfCos;
	m_cos = cos;

	if(inferMeasurability)
	{
		setMeasurability(pdfAppliedBsdfCos);
		setMeasurability(isMeasurable() && 0.0_r < cos && cos < 1.1_r);
	}
}

inline const math::Vector3R& BsdfSampleOutput::getL() const
{
	PH_ASSERT(m_isMeasurable);
	PH_ASSERT_IN_RANGE(m_L.lengthSquared(), 0.9_r, 1.1_r);

	return m_L;
}

inline real BsdfSampleOutput::getCos() const
{
	PH_ASSERT(m_isMeasurable);
	PH_ASSERT_IN_RANGE_EXCLUSIVE(m_cos, 0.0_r, 1.1_r);

	return m_cos;
}

inline math::Spectrum BsdfSampleOutput::getPdfAppliedBsdf() const
{
	const auto pdfAppliedBsdf = getPdfAppliedBsdfCos() / getCos();
	return pdfAppliedBsdf.isFinite() ? pdfAppliedBsdf : math::Spectrum(0);
}

inline const math::Spectrum& BsdfSampleOutput::getPdfAppliedBsdfCos() const
{
	// When a sample report being measurable, it must not be some crazy values
	PH_ASSERT(m_isMeasurable);
	PH_ASSERT_MSG(m_pdfAppliedBsdfCos.isFinite(), m_pdfAppliedBsdfCos.toString());

	return m_pdfAppliedBsdfCos;
}

inline bool BsdfSampleOutput::isMeasurable() const
{
	return m_isMeasurable;
}

inline void BsdfSampleOutput::setMeasurability(const bool measurability)
{
	m_isMeasurable = measurability;
}

inline void BsdfSampleOutput::setMeasurability(const math::Spectrum& reference)
{
	setMeasurability(reference.isFinite());
}

}// end namespace ph
