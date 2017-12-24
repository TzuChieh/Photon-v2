#include "Actor/Image/ConstantImage.h"
#include "Core/Texture/TConstantTexture.h"

#include <iostream>

namespace ph
{

ConstantImage::ConstantImage(const real value) : 
	m_values({value})
{

}

ConstantImage::ConstantImage(const std::vector<real>& values) : 
	m_values(values)
{

}

ConstantImage::~ConstantImage() = default;

void ConstantImage::genTexture(
	CookingContext& context,
	std::shared_ptr<TTexture<real>>* const out_texture) const
{
	if(m_values.size() != 1)
	{
		std::cerr << "warning: at ConstantImage::genTexture(), "
		          << "zero or > 1 values present. "
		          << "Generating texture with value 1 or first value." << std::endl;
	}

	*out_texture = std::make_shared<TConstantTexture<real>>(m_values.empty() ? 1 : m_values[0]);
}

void ConstantImage::genTexture(
	CookingContext& context,
	std::shared_ptr<TTexture<Vector3R>>* const out_texture) const
{
	if(m_values.size() != 3)
	{
		std::cerr << "warning: at ConstantImage::genTexture(), "
		          << "bad number of input values."
		          << "Generated texture may not be what you want." << std::endl;
	}

	Vector3R constVec3;
	constVec3.x = m_values.size() >= 1 ? m_values[0] : 1;
	constVec3.y = m_values.size() >= 2 ? m_values[1] : 1;
	constVec3.z = m_values.size() >= 3 ? m_values[2] : 1;
	*out_texture = std::make_shared<TConstantTexture<Vector3R>>(constVec3);
}

void ConstantImage::genTexture(
	CookingContext& context,
	std::shared_ptr<TTexture<SpectralStrength>>* const out_texture) const
{
	if(m_values.size() != SpectralStrength::numElements())
	{
		std::cerr << "warning: at ConstantImage::genTexture(), "
		          << "bad number of input values."
		          << "Generated texture may not be what you want." << std::endl;
	}

	SpectralStrength constSpectrum;
	for(std::size_t i = 0; i < SpectralStrength::numElements(); i++)
	{
		constSpectrum[i] = m_values.size() >= i + 1 ? m_values[i] : 1;
	}
	*out_texture = std::make_shared<TConstantTexture<SpectralStrength>>(constSpectrum);
}

// command interface

SdlTypeInfo ConstantImage::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_IMAGE, "constant");
}

void ConstantImage::ciRegister(CommandRegister& cmdRegister)
{
	// TODO
}

}// end namespace ph