#pragma once

#include "Math/TVector3.h"
#include "Core/Ray.h"
#include "Core/Quantity/Time.h"
#include "Core/LTA/PDF.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>
#include <Common/logging.h>

#include <optional>

namespace ph { class DirectEnergySampleInput; }

namespace ph
{

/*! @brief Input for `PrimitivePosSampleQuery`.
*/
class PrimitivePosSampleInput final
{
public:
	void set(
		const DirectEnergySampleInput& directInput,
		const math::Vector3R& uvw = {0, 0, 0},
		const lta::PDF& uvwPdf = {},
		bool suggestDir = false);

	void set(
		const Time& time,
		const std::optional<math::Vector3R>& observationPos = std::nullopt,
		const math::Vector3R& uvw = {0, 0, 0},
		const lta::PDF& uvwPdf = {},
		bool suggestDir = false);

	const Time& getTime() const;

	/*!
	If an observation position is given, the sample operation can be treated as if viewing from that
	position (and treating the primitive as opaque). Useful for generating better sample position
	and/or selecting different set of output.
	@return A hint: an optional observation position.
	@note Given the same observation position, the sample provider should always return the same
	set of output (one observation position maps to one set of output).
	*/
	const std::optional<math::Vector3R>& getObservationPos() const;

	/*!
	@return A hint: a specifically picked surface parameterization of the sample position. This kind
	of inverse mapping may not be always possible, and can invalidate the output if the mapping failed.
	@note This will only be considered only if its corresponding PDF (`getUvwPdf()`) is non-empty.
	*/
	const math::Vector3R& getUvw() const;

	/*!
	@return A hint: the PDF of the specifically picked surface parameterization (`getUvw()`).
	*/
	const lta::PDF& getUvwPdf() const;

	bool suggestDir() const;

private:
	Time                          m_time;
	std::optional<math::Vector3R> m_observationPos;
	math::Vector3R                m_uvw;
	lta::PDF                      m_uvwPdf;
	bool                          m_suggestDir;
#if PH_DEBUG
	bool                          m_hasSet{false};
#endif
};

/*! @brief Output for `PrimitivePosSampleQuery`.
*/
class PrimitivePosSampleOutput final
{
public:
	void setPos(const math::Vector3R& pos);
	void setPdfPos(const lta::PDF& pdfPos);
	void setPdfDir(const lta::PDF& pdfDir);
	void setObservationRay(const Ray& observationRay);

	/*! @brief Get the sampled position.
	@return The coordinates of the sampled position. This is generally more accurate (and more
	performant) than using `getObservationRay().getHead()`.
	@note This is only available if `getPdfPos()` is non-empty.
	*/
	const math::Vector3R& getPos() const;

	/*! @brief Get the sampled direction.
	Some implementation may suggest a sample direction if no observation position is given.
	@return A vector representing the suggested sample direction.
	@note This is only available if `getPdfDir()` is non-empty.
	*/
	const math::Vector3R& getDir() const;

	real getPdfA() const;
	real getPdfW() const;
	const lta::PDF& getPdfPos() const;
	const lta::PDF& getPdfDir() const;

	/*! @brief Get the ray from observation position to sampled position.
	If there is no explicitly provided observation position, the sampled position will be the
	observation position. In this case, the length of the ray will be 0 and whether the direction
	of the ray may set to a meaningful value is depending on `PrimitivePosSampleInput::suggestDir()`:
	* `true`: `getObservationRay().getDir()` and `getDir()` both contain the suggested direction if
	`getPdfDir()` is non-empty.
	* `false`: No standard behavior is defined.
	@note If a hit event is associated with this ray, obtaining the sampled position by calculating
	its hit detail is more accurate but slower. Using `getObservationRay().getHead()` is faster but is
	generally less accurate (though it is useful if only a crude position is required).
	*/
	const Ray& getObservationRay() const;

	/*! @brief Checks the validity of the sample.
	@return `true` if the sample is valid. `false` if the sample failed. A failed sample cannot be
	used. A typical way to fail is that sampling is not even supported/possible.
	*/
	operator bool () const;

private:
	math::Vector3R m_pos{0};
	lta::PDF       m_pdfPos{};
	lta::PDF       m_pdfDir{};
	Ray            m_observationRay{};
};

/*! @brief Information for generating a sample point on a primitive.
*/
class PrimitivePosSampleQuery final
{
public:
	using Input  = PrimitivePosSampleInput;
	using Output = PrimitivePosSampleOutput;

	Input  inputs;
	Output outputs;

	PrimitivePosSampleQuery() = default;
};

// In-header Implementations:

inline void PrimitivePosSampleInput::set(
	const Time& time,
	const std::optional<math::Vector3R>& observationPos,
	const math::Vector3R& uvw,
	const lta::PDF& uvwPdf,
	const bool suggestDir)
{
	m_time = time;
	m_observationPos = observationPos;
	m_uvw = uvw;
	m_uvwPdf = uvwPdf;
	m_suggestDir = suggestDir;

#if PH_DEBUG
	m_hasSet = true;

	if(observationPos && suggestDir)
	{
		PH_DEFAULT_LOG(WarningOnce,
			"Requesting suggested direction in `PrimitivePosSampleInput` has no effect when "
			"observation position is present.");
	}
#endif
}

inline const Time& PrimitivePosSampleInput::getTime() const
{
	PH_ASSERT(m_hasSet);
	return m_time;
}

inline const std::optional<math::Vector3R>& PrimitivePosSampleInput::getObservationPos() const
{
	PH_ASSERT(m_hasSet);
	return m_observationPos;
}

inline const math::Vector3R& PrimitivePosSampleInput::getUvw() const
{
	PH_ASSERT(m_hasSet);
	return m_uvw;
}

inline const lta::PDF& PrimitivePosSampleInput::getUvwPdf() const
{
	PH_ASSERT(m_hasSet);
	return m_uvwPdf;
}

inline bool PrimitivePosSampleInput::suggestDir() const
{
	PH_ASSERT(m_hasSet);
	return m_suggestDir;
}

inline void PrimitivePosSampleOutput::setObservationRay(const Ray& observationRay)
{
	m_observationRay = observationRay;
}

inline void PrimitivePosSampleOutput::setPos(const math::Vector3R& pos)
{
	m_pos = pos;
}

inline void PrimitivePosSampleOutput::setPdfPos(const lta::PDF& pdfPos)
{
	m_pdfPos = pdfPos;
}

inline void PrimitivePosSampleOutput::setPdfDir(const lta::PDF& pdfDir)
{
	m_pdfDir = pdfDir;
}

inline const Ray& PrimitivePosSampleOutput::getObservationRay() const
{
	PH_ASSERT(*this);
	return m_observationRay;
}

inline const math::Vector3R& PrimitivePosSampleOutput::getPos() const
{
	PH_ASSERT(*this);
	PH_ASSERT(m_pdfPos);
	return m_pos;
}

inline const math::Vector3R& PrimitivePosSampleOutput::getDir() const
{
	PH_ASSERT(m_pdfDir);
	return getObservationRay().getDir();
}

inline real PrimitivePosSampleOutput::getPdfA() const
{
	return getPdfPos().getPdfA();
}

inline real PrimitivePosSampleOutput::getPdfW() const
{
	return getPdfDir().getPdfW();
}

inline const lta::PDF& PrimitivePosSampleOutput::getPdfPos() const
{
	PH_ASSERT(*this);
	return m_pdfPos;
}

inline const lta::PDF& PrimitivePosSampleOutput::getPdfDir() const
{
	PH_ASSERT(*this);
	return m_pdfDir;
}

inline PrimitivePosSampleOutput::operator bool () const
{
	return m_pdfPos;
}

}// end namespace ph
