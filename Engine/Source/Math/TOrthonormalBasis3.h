#pragma once

#include "Math/TVector3.h"

namespace ph
{

template<typename T>
class TOrthonormalBasis3 final
{
public:
	TVector3<T> xAxis;
	TVector3<T> yAxis;
	TVector3<T> zAxis;

	inline TOrthonormalBasis3();
	inline TOrthonormalBasis3(const TOrthonormalBasis3& other);
	inline TOrthonormalBasis3(const TVector3<T>& xAxis, 
	                          const TVector3<T>& yAxis, 
	                          const TVector3<T>& zAxis);

	/*inline T phi(const TVector3<T>& unitVec) const;
	inline T theta(const TVector3<T>& unitVec) const;*/

	inline TOrthonormalBasis3& renormalize();

	inline void set(const TVector3<T>& xAxis, 
	                const TVector3<T>& yAxis, 
	                const TVector3<T>& zAxis);
	
	inline TOrthonormalBasis3& operator = (const TOrthonormalBasis3& rhs);
};

}// end namespace ph

#include "Math/TOrthonormalBasis3.ipp"