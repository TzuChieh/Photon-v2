#pragma once

#include "Common/primitive_type.h"
#include "Math/TArithmeticArray.h"

#include <cstddef>

namespace ph
{

template<typename DerivedType, std::size_t N>
class TAbstractSpectralStrength : public TArithmeticArray<real, N>
{
public:
	using TArithmeticArray<real, N>::TArithmeticArray;

	virtual inline ~TAbstractSpectralStrength() override;


};

}// end namespace ph

#include "Core/Quantity/Private/TAbstractSpectralStrength.ipp"