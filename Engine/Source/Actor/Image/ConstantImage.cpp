#include "Actor/Image/ConstantImage.h"
#include "Core/Texture/constant_textures.h"
#include "Math/TVector3.h"
#include "Common/logging.h"

#include <iostream>
#include <utility>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP();

ConstantImage::ConstantImage() :
	ConstantImage(1.0_r)
{}

ConstantImage::ConstantImage(const real value) : 
	ConstantImage(value, math::EColorSpace::UNSPECIFIED)
{}

ConstantImage::ConstantImage(const math::Vector3R& values) :
	ConstantImage(values, math::EColorSpace::UNSPECIFIED)
{}

ConstantImage::ConstantImage(std::vector<real> values) : 
	ConstantImage(values, math::EColorSpace::UNSPECIFIED)
{}

ConstantImage::ConstantImage(const real value, math::EColorSpace colorSpace) :
	ConstantImage(std::vector<real>{value}, colorSpace)
{}

ConstantImage::ConstantImage(const math::Vector3R& values, math::EColorSpace colorSpace) :
	ConstantImage(std::vector<real>{values.x, values.y, values.z}, colorSpace)
{}

ConstantImage::ConstantImage(std::vector<real> values, math::EColorSpace colorSpace) :
	m_values(std::move(values)), m_colorSpace(colorSpace)
{}

std::shared_ptr<TTexture<Image::NumericArray>> ConstantImage::genNumericTexture(
	ActorCookingContext& ctx)
{

}

std::shared_ptr<TTexture<math::Spectrum>> ConstantImage::genColorTexture(
	ActorCookingContext& ctx)
{

}

std::shared_ptr<TTexture<real>> ConstantImage::genTextureReal(
	ActorCookingContext& ctx) const
{
	if(m_values.size() != 1)
	{
		std::cerr << "warning: at ConstantImage::genTextureReal(), "
		          << "zero or > 1 values present. "
		          << "Generating texture with value 1 or first value." << std::endl;
	}

	if(m_type != EType::RAW)
	{
		std::cerr << "warning: at ConstantImage::genTextureReal(), "
		          << "non-raw type for real texture is unsupported, using raw" << std::endl;
	}

	const real value = m_values.empty() ? 1 : m_values[0];
	return std::make_shared<TConstantTexture<real>>(value);
}

std::shared_ptr<TTexture<math::Vector3R>> ConstantImage::genTextureVector3R(
	ActorCookingContext& ctx) const
{
	math::Vector3R values;
	if(m_values.size() == 1)
	{
		values.set(m_values[0], m_values[0], m_values[0]);
	}
	else if(m_values.size() == 3)
	{
		values.set(m_values[0], m_values[1], m_values[2]);
	}
	else
	{
		std::cerr << "warning: at ConstantImage::genTextureVector3R(), "
		          << "mismatched number of input values."
		          << "Generated texture may not be what you want." << std::endl;
		values.x = m_values.size() >= 1 ? m_values[0] : 1;
		values.y = m_values.size() >= 2 ? m_values[1] : 1;
		values.z = m_values.size() >= 3 ? m_values[2] : 1;
	}

	if(m_type != EType::RAW)
	{
		std::cerr << "warning: at ConstantImage::genTextureVector3R(), "
		          << "non-raw type for vec3 texture is unsupported, using raw" << std::endl;
	}

	return std::make_shared<TConstantTexture<math::Vector3R>>(values);
}

std::shared_ptr<TTexture<math::Spectrum>> ConstantImage::genTextureSpectral(
	ActorCookingContext& ctx) const
{
	math::Spectrum values;
	if(m_values.size() == 1)
	{
		switch(m_type)
		{
		case EType::RAW:
			values.setColorValues(m_values[0]);
			break;
			
		case EType::EMR_LINEAR_SRGB:
			values.setLinearSRGB(math::Vector3R(m_values[0]).toArray(), math::EColorUsage::EMR);
			break;

		case EType::ECF_LINEAR_SRGB:
			values.setLinearSRGB(math::Vector3R(m_values[0]).toArray(), math::EColorUsage::ECF);
			break;

		default:
			std::cerr << "warning: at ConstantImage::genTextureSpectral(), "
			          << "unsupported value type, using raw" << std::endl;
			values.setColorValues(m_values[0]);
			break;
		}
	}
	else if(m_values.size() == 3)
	{
		switch(m_type)
		{
		case EType::EMR_LINEAR_SRGB:
			values.setLinearSRGB(math::Vector3R(m_values[0], m_values[1], m_values[2]).toArray(), math::EColorUsage::EMR);
			break;

		case EType::ECF_LINEAR_SRGB:
			values.setLinearSRGB(math::Vector3R(m_values[0], m_values[1], m_values[2]).toArray(), math::EColorUsage::ECF);
			break;

		case EType::RAW_LINEAR_SRGB:
			values.setLinearSRGB(math::Vector3R(m_values[0], m_values[1], m_values[2]).toArray(), math::EColorUsage::RAW);
			break;

		default:
			std::cerr << "warning: at ConstantImage::genTextureSpectral(), "
			          << "unsupported value type, assuming ECF linear sRGB" << std::endl;
			values.setLinearSRGB(math::Vector3R(m_values[0], m_values[1], m_values[2]).toArray(), math::EColorUsage::ECF);
			break;
		}
	}
	else
	{
		if(m_values.size() != math::Spectrum::NUM_VALUES)
		{
			std::cerr << "warning: at ConstantImage::genTextureSpectral(), "
			          << "bad number of input values."
			          << "Generated texture may not be what you want." << std::endl;
		}

		if(m_type != EType::RAW)
		{
			std::cerr << "warning: at ConstantImage::genTextureSpectral(), "
			          << "only raw type is supported." << std::endl;
		}

		for(std::size_t i = 0; i < math::Spectrum::NUM_VALUES; i++)
		{
			values[i] = i < m_values.size() ? m_values[i] : 1;
		}
	}

	return std::make_shared<TConstantTexture<math::Spectrum>>(values);
}

}// end namespace ph
