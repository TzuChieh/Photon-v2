#pragma once

#include "Common/assertion.h"
#include "Math/TVector3.h"
#include "Math/math.h"

namespace ph
{

class BsdfHelper final
{
public:
	static inline bool makeHalfVector(
		const Vector3R& L, const Vector3R& V, 
		Vector3R* const out_H)
	{
		PH_ASSERT(out_H);

		*out_H = L.add(V);
		if(out_H->isZero())
		{
			return false;
		}
		else
		{
			out_H->normalizeLocal();
			return true;
		}
	}

	static inline bool makeHalfVectorSameHemisphere(
		const Vector3R& L, const Vector3R& V, const Vector3R& N,
		Vector3R* const out_H)
	{
		PH_ASSERT(out_H);

		if(!makeHalfVector(L, V, out_H))
		{
			return false;
		}

		out_H->mulLocal(static_cast<real>(math::sign(N.dot(*out_H))));
		return !out_H->isZero();
	}
};

}// end namespace ph