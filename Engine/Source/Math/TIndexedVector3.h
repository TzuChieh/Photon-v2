#pragma once

#include "Math/TVector3.h"

namespace ph
{

template<typename T>
class TIndexedVector3 : public TVector3<T>
{
public:
	inline TIndexedVector3();
	inline TIndexedVector3(const TIndexedVector3& other);
	explicit inline TIndexedVector3(const TVector3<T>& vector3);
	inline TIndexedVector3(const T x, const T y, const T z);
	virtual inline ~TIndexedVector3() override;

	inline T& operator [] (const int32 axisIndex);
	inline const T& operator [] (const int32 axisIndex) const;

	inline TIndexedVector3& operator = (const TIndexedVector3& rhs);

private:
	T* const m_entries[3];
};

}// end namespace ph

#include "Math/TIndexedVector3.ipp"