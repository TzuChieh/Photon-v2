#pragma once

#include "Core/SurfaceHit.h"
#include "Math/TVector3.h"

namespace ph
{

class SampleLocation final
{
	/*
		This class provides a temporal, higher order of abstraction over data 
		regarding ray-hit information. This abstraction layer is tailored 
		towards texture sampling and shall not be used in other circumstances.
	*/

public:
	inline SampleLocation(const SurfaceHit& hit, const Vector3R& uvw) :
		m_hit(hit), m_uvw(uvw) {}

	inline const Vector3R& uvw() const
	{
		return m_uvw;
	}

private:
	const SurfaceHit& m_hit;
	const Vector3R&   m_uvw;
};

}// end namespace ph