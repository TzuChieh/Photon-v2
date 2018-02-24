#include "Actor/Image/ConstantImage.h"
#include "Core/Texture/TConstantTexture.h"
#include "Math/TVector3.h"
#include "FileIO/InputPacket.h"
#include "FileIO/InputPrototype.h"

#include <iostream>

namespace ph
{

ConstantImage::ConstantImage() :
	ConstantImage(1.0_r)
{}

ConstantImage::ConstantImage(const real value) : 
	m_values({value})
{}

ConstantImage::ConstantImage(const Vector3R& values) : 
	m_values({values.x, values.y, values.z})
{}

ConstantImage::ConstantImage(const std::vector<real>& values) : 
	m_values(values)
{}

ConstantImage::~ConstantImage() = default;

std::shared_ptr<TTexture<real>> ConstantImage::genTextureReal(
	CookingContext& context) const
{
	if(m_values.size() != 1)
	{
		std::cerr << "warning: at ConstantImage::genTextureReal(), "
		          << "zero or > 1 values present. "
		          << "Generating texture with value 1 or first value." << std::endl;
	}

	return std::make_shared<TConstantTexture<real>>(m_values.empty() ? 1 : m_values[0]);
}

std::shared_ptr<TTexture<Vector3R>> ConstantImage::genTextureVector3R(
	CookingContext& context) const
{
	if(m_values.size() != 3)
	{
		std::cerr << "warning: at ConstantImage::genTextureVector3R(), "
		          << "mismatched number of input values."
		          << "Generated texture may not be what you want." << std::endl;
	}

	Vector3R constVec3;
	constVec3.x = m_values.size() >= 1 ? m_values[0] : 1;
	constVec3.y = m_values.size() >= 2 ? m_values[1] : 1;
	constVec3.z = m_values.size() >= 3 ? m_values[2] : 1;
	return std::make_shared<TConstantTexture<Vector3R>>(constVec3);
}

std::shared_ptr<TTexture<SpectralStrength>> ConstantImage::genTextureSpectral(
	CookingContext& context) const
{
	SpectralStrength constSpectrum;

	// FIXME: implicitly treating it as linear sRGB, make it explicit
	if(m_values.size() == 3)
	{
		const Vector3R linearSrgb(m_values[0], m_values[1], m_values[2]);
		constSpectrum.setLinearSrgb(linearSrgb);
	}
	else
	{
		if(m_values.size() != SpectralStrength::numElements())
		{
			std::cerr << "warning: at ConstantImage::genTextureSpectral(), "
			          << "bad number of input values."
			          << "Generated texture may not be what you want." << std::endl;
		}

		for(std::size_t i = 0; i < SpectralStrength::numElements(); i++)
		{
			constSpectrum[i] = i < m_values.size() ? m_values[i] : 1;
		}
	}

	return std::make_shared<TConstantTexture<SpectralStrength>>(constSpectrum);
}

// command interface

SdlTypeInfo ConstantImage::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_IMAGE, "constant");
}

void ConstantImage::ciRegister(CommandRegister& cmdRegister)
{
	cmdRegister.setLoader(SdlLoader([](const InputPacket& packet)
	{
		InputPrototype realInput;
		realInput.addReal("value");

		InputPrototype vec3Input;
		vec3Input.addVector3r("value");

		// TODO: array input

		if(packet.isPrototypeMatched(realInput))
		{
			const real value = packet.getReal("value");
			return std::make_unique<ConstantImage>(value);
		}
		else if(packet.isPrototypeMatched(vec3Input))
		{
			const Vector3R value = packet.getVector3r("value");
			return std::make_unique<ConstantImage>(value);
		}
		else
		{
			std::cerr << "warning: ill-formed input detected during ConstantImage loading" << std::endl;
			return std::make_unique<ConstantImage>();
		}
	}));
}

}// end namespace ph