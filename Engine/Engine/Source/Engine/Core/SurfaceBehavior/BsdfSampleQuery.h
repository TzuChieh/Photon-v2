#pragma once

#include "Engine/Core/SurfaceBehavior/bsdf_query_fwd.h"
#include "Engine/Core/SurfaceBehavior/surface_optics_fwd.h"
#include "Engine/Core/SurfaceBehavior/bsdf_query_common.h"
#include "Engine/Math/TVector3.h"
#include "Engine/Core/SurfaceHit.h"
#include "Engine/Math/Color/Spectrum.h"
#include "Engine/Core/SurfaceBehavior/BsdfQueryContext.h"

#include <Common/assertion.h>

#include <cmath>
#include <string>
#include <utility>

namespace ph
{

/*! @brief Input for `BsdfSampleQuery`.
*/
class BsdfSampleInput final
{
public:
	void set(const BsdfEvalInput& evalInput);

	void set(
		const BsdfInputBase& base,
		const SurfaceHit& X,
		const math::Vector3R& V);

	const BsdfInputBase& getBase() const;
	const SurfaceHit& getX() const;

	/*!
	@return Direction (normalized) of the exiting ray.
	*/
	const math::Vector3R& getV() const;

private:
	BsdfInputBase m_base;
	SurfaceHit m_X;
	math::Vector3R m_V;
#if PH_DEBUG
	bool m_hasSet{false};
#endif
};

/*! @brief Output for `BsdfSampleQuery`.
@note It is an error to get output data if `isContributable()` returns `false`.
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
	@param inferContributability Whether to determine contributability from the supplied data. All supplied
	data must be sane for the sample to be contributable.
	*/
	void setPdfAppliedBsdfCos(
		const math::Spectrum& pdfAppliedBsdfCos,
		real cos,
		bool inferContributability = true);

	/*!
	@return Sampled direction (normalized) of the incident ray.
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
	@return Sampled BSDF with PDF (on the same domain) applied. Guaranteed to be finite.
	Specifically, it is equivalent to @f$ BSDF / PDF_{\omega} @f$.
	*/
	math::Spectrum getPdfAppliedBsdf() const;

	/*!
	Prefer using this method when applicable instead of applying the cosine term manually.
	The coupled form often results in less computation and potentially offers better numerical
	stability as some terms can often be canceled out.
	@return Sampled BSDF with Lambert's cosine law and PDF (on the same domain) applied.
	Guaranteed to be finite. Specifically, it is equivalent to
	@f$ BSDF * \lvert\cos\theta\rvert / PDF_{\omega} @f$.
	@note This cosine term is an absolute value. You may need to consult the corresponding `SurfaceHit`
	if its sign is needed.
	*/
	const math::Spectrum& getPdfAppliedBsdfCos() const;

	/*!
	@return Relative index of refraction of the sampled interface. The value is calculated as
	@f$ \eta_V / \eta_L @f$, with @f$ V @f$ on the exiting side and @f$ L @f$ on the incident side.
	If the ray does not cross an interface, 1 is returned.
	*/
	real getRelativeIor() const;

	/*!
	@return Squared value of `getRelativeIor()`.
	*/
	real getRelativeIor2() const;

	/*! @brief Tells whether this sample has potential to contribute.
	All sampled data should be usable if true is returned; otherwise, zero contribution is implied,
	and sampled data is undefined. This method is also an efficient way to decide whether the BSDF
	sample has sane value (compared to manually testing its value).
	@note Contributability has nothing to do with whether the sampled value is 0 or not.
	*/
	bool isContributable() const;

	/*! @brief Set contributability directly.
	*/
	void setContributability(bool contributability);

	/*! @brief Set contributability based on a reference spectrum.
	*/
	void setContributability(const math::Spectrum& reference);

	/*!
	@param relativeIor See `getRelativeIor()` for more information.
	*/
	void setRelativeIor(real relativeIor);

	/*! @brief Convenient method for `isContributable()`.
	*/
	operator bool () const;

private:
	math::Vector3R m_L{0};
	math::Spectrum m_pdfAppliedBsdfCos{0};
	real m_cos{0};
	real m_relativeIor{1};
	bool m_isContributable{false};
};

/*! @brief Information for generating a BSDF sample.
*/
class BsdfSampleQuery final
{
public:
	using Input  = BsdfSampleInput;
	using Output = BsdfSampleOutput;

	BsdfQueryContext context = BsdfQueryContext{};
	Input            inputs;
	Output           outputs;

	inline BsdfSampleQuery() = default;
	explicit BsdfSampleQuery(BsdfQueryContext context);
};

// In-header Implementations:

inline BsdfSampleQuery::BsdfSampleQuery(BsdfQueryContext context)
	: BsdfSampleQuery()
{
	this->context = std::move(context);

	// (rest of the fields are initialized via setters)
}

inline void BsdfSampleInput::set(
	const BsdfInputBase& base,
	const SurfaceHit& X,
	const math::Vector3R& V)
{
	// Not querying from uninitialized surface hit
	PH_ASSERT(!X.getReason().hasExactly(ESurfaceHitReason::Invalid));

	PH_ASSERT_IN_RANGE(V.lengthSquared(), 0.9_r, 1.1_r);

	m_base = base;
	m_X = X;
	m_V = V;

#if PH_DEBUG
	m_hasSet = true;
#endif
}

inline const BsdfInputBase& BsdfSampleInput::getBase() const
{
	PH_ASSERT(m_hasSet);

	return m_base;
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
	const bool inferContributability)
{
	m_pdfAppliedBsdfCos = pdfAppliedBsdfCos;
	m_cos = cos;

	if(inferContributability)
	{
		setContributability(pdfAppliedBsdfCos);
		setContributability(isContributable() && 0.0_r < cos && cos < 1.1_r);
	}
}

inline const math::Vector3R& BsdfSampleOutput::getL() const
{
	PH_ASSERT(isContributable());
	PH_ASSERT_IN_RANGE(m_L.lengthSquared(), 0.9_r, 1.1_r);

	return m_L;
}

inline real BsdfSampleOutput::getCos() const
{
	PH_ASSERT(isContributable());
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
	// When a sample report being contributable, it must not be some crazy values
	PH_ASSERT(isContributable());
	PH_ASSERT_MSG(m_pdfAppliedBsdfCos.isFinite(), m_pdfAppliedBsdfCos.toString());

	return m_pdfAppliedBsdfCos;
}

inline real BsdfSampleOutput::getRelativeIor() const
{
	PH_ASSERT(isContributable());
	PH_ASSERT_MSG(std::isfinite(m_relativeIor) && m_relativeIor > 0, std::to_string(m_relativeIor));

	return m_relativeIor;
}

inline real BsdfSampleOutput::getRelativeIor2() const
{
	const real relativeIor = getRelativeIor();
	return relativeIor * relativeIor;
}

inline bool BsdfSampleOutput::isContributable() const
{
	return m_isContributable;
}

inline void BsdfSampleOutput::setContributability(const bool contributability)
{
	m_isContributable = contributability;
}

inline void BsdfSampleOutput::setContributability(const math::Spectrum& reference)
{
	setContributability(reference.isFinite());
}

inline void BsdfSampleOutput::setRelativeIor(const real relativeIor)
{
	m_relativeIor = relativeIor;
}

inline BsdfSampleOutput::operator bool () const
{
	return isContributable();
}

}// end namespace ph
