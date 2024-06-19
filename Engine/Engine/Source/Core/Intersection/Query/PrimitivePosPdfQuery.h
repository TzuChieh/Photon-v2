#pragma once

#include "Math/TVector3.h"
#include "Core/Ray.h"
#include "Core/HitDetail.h"
#include "Core/SurfaceHit.h"
#include "Core/LTA/PDF.h"

#include <Common/primitive_type.h>
#include <Common/assertion.h>

#include <optional>

namespace ph { class HitDetail; }
namespace ph { class PrimitivePosSampleInput; }
namespace ph { class DirectEnergyPdfInput; }

namespace ph
{

/*! @brief Input for `PrimitivePosPdfOutput`.
*/
class PrimitivePosPdfInput final
{
public:
	void set(const PrimitivePosSampleInput& posInput, const SurfaceHit& X);

	void set(
		const DirectEnergyPdfInput& pdfInput,
		const lta::PDF& uvwPdf = {});

	void set(
		const SurfaceHit& X,
		const std::optional<math::Vector3R>& observationPos = std::nullopt,
		const lta::PDF& uvwPdf = {});

	/*!
	@return Information about the sample position we are querying PDF for.
	*/
	const SurfaceHit& getX() const;

	/*! @copydoc ph::PrimitivePosSampleOutput::getPos().
	*/
	math::Vector3R getPos() const;

	/*! @copydoc ph::PrimitivePosSampleInput::getUvw().
	*/
	math::Vector3R getUvw() const;

	/*! @copydoc ph::PrimitivePosSampleInput::getUvwPdf().
	*/
	const lta::PDF& getUvwPdf() const;

	/*! @copydoc ph::PrimitivePosSampleInput::getObservationPos().
	*/
	const std::optional<math::Vector3R>& getObservationPos() const;

private:
	/*!
	@return Observation position inferred from `X`.
	*/
	static std::optional<math::Vector3R> inferObservationPos(const SurfaceHit& X);

	SurfaceHit                    m_X;
	std::optional<math::Vector3R> m_observationPos;
	lta::PDF                      m_uvwPdf{};
#if PH_DEBUG
	bool                          m_hasSet{false};
#endif
};

/*! @brief Output for `PrimitivePosPdfOutput`.
*/
class PrimitivePosPdfOutput final
{
public:
	void setPdf(const lta::PDF& pdf);

	real getPdfA() const;
	const lta::PDF& getPdf() const;

	operator bool () const;

private:
	lta::PDF m_pdf{};
};

/*! @brief Information for the probability of generating a specific sample point on a primitive.
*/
class PrimitivePosPdfQuery final
{
public:
	using Input  = PrimitivePosPdfInput;
	using Output = PrimitivePosPdfOutput;

	Input  inputs;
	Output outputs;

	PrimitivePosPdfQuery() = default;
};

// In-header Implementations:

inline void PrimitivePosPdfInput::set(
	const SurfaceHit& X,
	const std::optional<math::Vector3R>& observationPos,
	const lta::PDF& uvwPdf)
{
	m_X = X;
	m_observationPos = observationPos;
	m_uvwPdf = uvwPdf;

#if PH_DEBUG
	m_hasSet = true;
#endif
}

inline const SurfaceHit& PrimitivePosPdfInput::getX() const
{
	return m_X;
}

inline math::Vector3R PrimitivePosPdfInput::getPos() const
{
	return getX().getPos();
}

inline math::Vector3R PrimitivePosPdfInput::getUvw() const
{
	return getX().getDetail().getUVW();
}

inline const lta::PDF& PrimitivePosPdfInput::getUvwPdf() const
{
	return m_uvwPdf;
}

inline const std::optional<math::Vector3R>& PrimitivePosPdfInput::getObservationPos() const
{
	return m_observationPos;
}

inline void PrimitivePosPdfOutput::setPdf(const lta::PDF& pdf)
{
	m_pdf = pdf;
}

inline real PrimitivePosPdfOutput::getPdfA() const
{
	return getPdf().getPdfA();
}

inline const lta::PDF& PrimitivePosPdfOutput::getPdf() const
{
	PH_ASSERT(*this);

	return m_pdf;
}

inline PrimitivePosPdfOutput::operator bool () const
{
	return m_pdf;
}

}// end namespace ph
