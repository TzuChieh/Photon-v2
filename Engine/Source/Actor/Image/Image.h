#pragma once

#include "DataIO/SDL/TSdlResourceBase.h"
#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "Core/Texture/TTexture.h"
#include "Math/Color/Spectrum.h"
#include "Core/Texture/TConstantTexture.h"
#include "Common/config.h"

#include <iostream>
#include <memory>
#include <array>

namespace ph { class ActorCookingContext; }

namespace ph
{

class Image : public TSdlResourceBase<ETypeCategory::REF_IMAGE>
{
public:
	using NumericArray = std::array<float64, PH_NUMERIC_TEXTURE_MAX_ELEMENTS>;
	
	inline static constexpr auto NUMERIC_ARRAY_SIZE = std::tuple_size_v<NumericArray>;

public:
	Image();

	virtual std::shared_ptr<TTexture<NumericArray>> genNumericTexture(
		ActorCookingContext& ctx) const;

	virtual std::shared_ptr<TTexture<math::Spectrum>> genColorTexture(
		ActorCookingContext& ctx) const;

	virtual std::shared_ptr<TTexture<real>> genTextureReal(
		ActorCookingContext& ctx) const;

	virtual std::shared_ptr<TTexture<math::Vector3R>> genTextureVector3R(
		ActorCookingContext& ctx) const;

	virtual std::shared_ptr<TTexture<math::Spectrum>> genTextureSpectral(
		ActorCookingContext& ctx) const;

private:
	template<typename OutputType>
	inline std::shared_ptr<TTexture<OutputType>> genDefaultTexture() const
	{
		std::cerr << "warning: at Image::genTexture(), "
	              << "no implementation provided, generating a constant one" << std::endl;

		return std::make_shared<TConstantTexture<OutputType>>(OutputType(1));
	}
};

}// end namespace ph
