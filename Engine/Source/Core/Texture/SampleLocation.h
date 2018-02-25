#pragma once

#include "Core/SurfaceHit.h"
#include "Math/TVector3.h"

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
	inline SampleLocation(const SurfaceHit& hit, const Vector3R& uvw) :
		m_hit(hit), m_uvw(uvw)
	{}

	inline SampleLocation(const SampleLocation& other) :
		SampleLocation(other.m_hit, other.m_uvw)
	{}

	inline const Vector3R& uvw() const
	{
		return m_uvw;
	}

	inline SampleLocation getUvwScaled(const Vector3R& scale) const
	{
		return SampleLocation(m_hit, m_uvw.mul(scale));
	}

private:
	const SurfaceHit& m_hit;
	const Vector3R&   m_uvw;
};

}// end namespace ph