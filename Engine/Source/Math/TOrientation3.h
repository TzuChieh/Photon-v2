#pragma once

#include "Math/TVector3.h"

namespace ph
{

template<typename T>
class TOrientation3 : public TVector3<T>
{
public:
	using TVector3<T>::TVector3;

	virtual inline ~TOrientation3() override;
};

}// end namespace ph

#include "Math/TOrientation3.ipp"