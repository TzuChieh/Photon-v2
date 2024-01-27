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
	void set(const BsdfEvalQuery& eval);
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
It is an error to get output data if `isMeasurable()` returns `false`.
*/
class BsdfSampleOutput final
{
public:
	void setL(const math::Vector3R& L);

	void setPdfAppliedBsdf(
		const math::Spectrum& pdfAppliedBsdf, 
		bool inferMeasurabilityFromThis = true);

	/*!
	@return Sampled direction (normalized).
	*/
	const math::Vector3R& getL() const;

	/*!
	@return Sampled BSDF with PDF (solid angle domain) applied. Guaranteed to be finite.
	*/
	const math::Spectrum& getPdfAppliedBsdf() const;

	/*! @brief Tells whether this sample has sane contribution.
	All sampled data should be usable if true is returned; otherwise, zero contribution is implied,
	and sampled data is undefined. This method is an efficient way to decide whether the BSDF sample
	has potential to contribute.
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
	math::Spectrum m_pdfAppliedBsdf{0};
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
	m_L = L;
}

inline void BsdfSampleOutput::setPdfAppliedBsdf(
	const math::Spectrum& pdfAppliedBsdf, 
	const bool inferMeasurabilityFromThis)
{
	m_pdfAppliedBsdf = pdfAppliedBsdf;

	if(inferMeasurabilityFromThis)
	{
		setMeasurability(pdfAppliedBsdf);
	}
}

inline const math::Vector3R& BsdfSampleOutput::getL() const
{
	PH_ASSERT(m_isMeasurable);
	PH_ASSERT_MSG(0.95_r < m_L.length() && m_L.length() < 1.05_r, m_L.toString());

	return m_L;
}

inline const math::Spectrum& BsdfSampleOutput::getPdfAppliedBsdf() const
{
	// When a sample report being measurable, it must not be some crazy values
	PH_ASSERT(m_isMeasurable);
	PH_ASSERT_MSG(m_pdfAppliedBsdf.isFinite(), m_pdfAppliedBsdf.toString());

	return m_pdfAppliedBsdf;
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
