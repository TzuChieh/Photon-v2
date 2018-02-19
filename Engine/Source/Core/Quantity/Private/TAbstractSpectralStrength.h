#pragma once

#include "Common/primitive_type.h"
#include "Math/TArithmeticArray.h"
#include "Math/TVector3.h"
#include "Core/Quantity/Private/SpectralSample.h"

#include <cstddef>
#include <vector>

namespace ph
{

template<typename DerivedType, std::size_t N>
class TAbstractSpectralStrength : public TArithmeticArray<real, N>
{
public:
	using TArithmeticArray<real, N>::TArithmeticArray;
	inline TAbstractSpectralStrength();
	inline TAbstractSpectralStrength(const TAbstractSpectralStrength& other);
	inline TAbstractSpectralStrength(const TArithmeticArray<real, N>& other);
	virtual inline ~TAbstractSpectralStrength() override = default;

	inline Vector3R genRgb() const;
	inline void setRgb(const Vector3R& rgb);
	inline void setSampled(const std::vector<SpectralSample>& samples);

	using TArithmeticArray<real, N>::operator = ;
};

}// end namespace ph

#include "Core/Quantity/Private/TAbstractSpectralStrength.ipp"