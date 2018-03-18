#pragma once

#include "Core/HitDetail.h"
#include "Math/TVector3.h"
#include "Core/Quantity/EQuantity.h"

namespace ph
{

/*
	This class provides a temporal, higher order of abstraction over data
	regarding ray-hit information. This abstraction layer is tailored
	towards texture sampling and shall not be used in other circumstances.
	The philosophy of this abstraction layer is that texture sampling 
	processes should not care about the context of a hit, but focus on the
	actual data of the target point. 
*/
class SampleLocation final
{
public:
	inline SampleLocation(const HitDetail& hit) :
		SampleLocation(hit, EQuantity::RAW)
	{}

	inline SampleLocation(const HitDetail& hit,
	                      const EQuantity  quantity) :
		m_hit(hit), m_quantity(quantity)
	{}

	inline SampleLocation(const SampleLocation& other) :
		SampleLocation(other.m_hit, other.m_quantity)
	{}

	inline const Vector3R& uvw() const
	{
		return m_hit.getUvw();
	}

	// TODO: should use uvw remapper instead
	// or update derivatives?
	inline SampleLocation getUvwScaled(const Vector3R& scale) const
	{
		HitDetail newDetail = m_hit;
		newDetail.setMisc(m_hit.getPrimitive(), m_hit.getUvw().mul(scale));
		return SampleLocation(newDetail, m_quantity);
	}

	inline EQuantity expectedQuantity() const
	{
		return m_quantity;
	}

private:
	const HitDetail m_hit;
	const EQuantity m_quantity;
};

}// end namespace ph