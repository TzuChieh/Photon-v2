#pragma once

#include "Math/TVector3.h"
#include "Core/Ray.h"
#include "Core/Quantity/Time.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <optional>

namespace ph
{

class PrimitivePosSampleInput final
{
public:
	void set(const Time& time, const math::Vector3R& observationPos);
	void set(const Time& time);

	const Time& getTime() const;
	const std::optional<math::Vector3R>& getObservationPos() const;

private:
	void set(const Time& time, const std::optional<math::Vector3R>& observationPos);

	Time m_time;
	std::optional<math::Vector3R> m_observationPos;
#if PH_DEBUG
	bool m_hasSet{false};
#endif
};

class PrimitivePosSampleOutput final
{
public:
	void setPos(const math::Vector3R& pos);
	void setPdfA(real pdfA);
	void setObservationRay(const Ray& observationRay);
	void invalidate();

	/*! @brief Get the sampled position.
	@return The coordinates of the sampled position. This is generally more accurate (and more
	performant) than using `getObservationRay().getHead()`.
	*/
	const math::Vector3R& getPos() const;

	real getPdfA() const;

	/*! @brief Get the ray from observation position to sampled position.
	If there is no explicitly provided observation position, the sampled position will be the
	observation position.
	*/
	const Ray& getObservationRay() const;

	/*! @brief Checks the validity of the sample.
	@return `true` if the sample is valid. `false` if the sample failed. A failed sample cannot be
	used. A typical way to fail is that sampling is not even supported/possible.
	*/
	operator bool () const;

private:
	math::Vector3R m_pos{0};
	real m_pdfA{0};
	Ray m_observationRay{};
};

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
	const math::Vector3R& observationPos)
{
	set(time, std::optional<math::Vector3R>(observationPos));
}

inline void PrimitivePosSampleInput::set(const Time& time)
{
	set(time, std::nullopt);
}

inline void PrimitivePosSampleInput::set(
	const Time& time, 
	const std::optional<math::Vector3R>& observationPos)
{
	m_time = time;
	m_observationPos = observationPos;

#if PH_DEBUG
	m_hasSet = true;
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

inline void PrimitivePosSampleOutput::setObservationRay(const Ray& observationRay)
{
	m_observationRay = observationRay;
}

inline void PrimitivePosSampleOutput::setPos(const math::Vector3R& pos)
{
	m_pos = pos;
}

inline void PrimitivePosSampleOutput::setPdfA(const real pdfA)
{
	m_pdfA = pdfA;
}

inline void PrimitivePosSampleOutput::invalidate()
{
	// Must cuase `PrimitivePosSampleOutput::operator bool ()` method to evaluate to `false`
	m_pdfA = 0;

	PH_ASSERT(!(*this));
}

inline const Ray& PrimitivePosSampleOutput::getObservationRay() const
{
	PH_ASSERT(*this);
	return m_observationRay;
}

inline const math::Vector3R& PrimitivePosSampleOutput::getPos() const
{
	PH_ASSERT(*this);
	return m_pos;
}

inline real PrimitivePosSampleOutput::getPdfA() const
{
	PH_ASSERT(*this);
	return m_pdfA;
}

inline PrimitivePosSampleOutput::operator bool () const
{
	return m_pdfA > 0;
}

}// end namespace ph
