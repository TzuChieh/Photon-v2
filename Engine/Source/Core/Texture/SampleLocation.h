#pragma once

#include "Core/SurfaceHit.h"
#include "Math/TVector3.h"
#include "Core/Quantity/EQuantity.h"

namespace ph
{

/*
	This class provides a temporal, higher order of abstraction over data
	regarding ray-hit information. This abstraction layer is tailored
	towards texture sampling and shall not be used in other circumstances.
*/
class SampleLocation final
{
public:
	inline SampleLocation(const SurfaceHit& hit, 
	                      const Vector3R&   uvw) :
		SampleLocation(hit, uvw, EQuantity::RAW)
	{}

	inline SampleLocation(const SurfaceHit& hit, 
	                      const Vector3R&   uvw, 
	                      const EQuantity   quantity) :
		m_hit(hit), m_uvw(uvw), m_quantity(quantity)
	{}

	inline SampleLocation(const SampleLocation& other) :
		SampleLocation(other.m_hit, other.m_uvw, other.m_quantity)
	{}

	inline const Vector3R& uvw() const
	{
		return m_uvw;
	}

	inline SampleLocation getUvwScaled(const Vector3R& scale) const
	{
		return SampleLocation(m_hit, m_uvw.mul(scale), m_quantity);
	}

	inline EQuantity expectedQuantity() const
	{
		return m_quantity;
	}

private:
	const SurfaceHit& m_hit;
	const Vector3R&   m_uvw;
	const EQuantity   m_quantity;
};

}// end namespace ph