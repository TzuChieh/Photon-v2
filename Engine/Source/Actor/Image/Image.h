#pragma once

#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "DataIO/SDL/ISdlResource.h"
#include "Core/Texture/TTexture.h"
#include "Core/Quantity/Spectrum.h"
#include "Core/Texture/TConstantTexture.h"

#include <iostream>
#include <memory>

namespace ph
{

class CookingContext;

class Image : public ISdlResource
{
public:
	static constexpr ETypeCategory CATEGORY = ETypeCategory::REF_IMAGE;

public:
	Image();

	virtual std::shared_ptr<TTexture<real>> genTextureReal(
		CookingContext& context) const;

	virtual std::shared_ptr<TTexture<math::Vector3R>> genTextureVector3R(
		CookingContext& context) const;

	virtual std::shared_ptr<TTexture<Spectrum>> genTextureSpectral(
		CookingContext& context) const;

	ETypeCategory getCategory() const;

private:
	template<typename OutputType>
	inline std::shared_ptr<TTexture<OutputType>> genDefaultTexture() const
	{
		std::cerr << "warning: at Image::genTexture(), "
	              << "no implementation provided, generating a constant one" << std::endl;

		return std::make_shared<TConstantTexture<OutputType>>(OutputType(1));
	}
};

// In-header Implementations:

inline ETypeCategory Image::getCategory() const
{
	return CATEGORY;
}

}// end namespace ph

#include "Actor/Image/Image.ipp"
