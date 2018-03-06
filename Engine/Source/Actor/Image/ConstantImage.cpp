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
	ConstantImage(value, EType::RAW)
{}

ConstantImage::ConstantImage(const Vector3R& values) : 
	ConstantImage(values, EType::RAW)
{}

ConstantImage::ConstantImage(const std::vector<real>& values) : 
	ConstantImage(values, EType::RAW)
{}

ConstantImage::ConstantImage(const real value, const EType type) : 
	ConstantImage(std::vector<real>{value}, type)
{}

ConstantImage::ConstantImage(const Vector3R& values, const EType type) : 
	ConstantImage(std::vector<real>{values.x, values.y, values.z}, type)
{}

ConstantImage::ConstantImage(const std::vector<real>& values, const EType type) : 
	m_values(values), m_type(type)
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

	if(m_type != EType::RAW)
	{
		std::cerr << "warning: at ConstantImage::genTextureReal(), "
		          << "non-raw type for real texture is unsupported, using raw" << std::endl;
	}

	const real value = m_values.empty() ? 1 : m_values[0];
	return std::make_shared<TConstantTexture<real>>(value);
}

std::shared_ptr<TTexture<Vector3R>> ConstantImage::genTextureVector3R(
	CookingContext& context) const
{
	Vector3R values;
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

	return std::make_shared<TConstantTexture<Vector3R>>(values);
}

std::shared_ptr<TTexture<SpectralStrength>> ConstantImage::genTextureSpectral(
	CookingContext& context) const
{
	SpectralStrength values;
	if(m_values.size() == 1)
	{
		switch(m_type)
		{
		case EType::RAW:
			values.setValues(m_values[0]);
			break;
			
		case EType::EMR_LINEAR_SRGB:
			values.setLinearSrgb(Vector3R(m_values[0]), EQuantity::EMR);
			break;

		case EType::ECF_LINEAR_SRGB:
			values.setLinearSrgb(Vector3R(m_values[0]), EQuantity::ECF);
			break;

		default:
			std::cerr << "warning: at ConstantImage::genTextureSpectral(), "
			          << "unsupported value type, using raw" << std::endl;
			values.setValues(m_values[0]);
			break;
		}
	}
	else if(m_values.size() == 3)
	{
		switch(m_type)
		{
		case EType::EMR_LINEAR_SRGB:
			values.setLinearSrgb(Vector3R(m_values[0], m_values[1], m_values[2]), EQuantity::EMR);
			break;

		case EType::ECF_LINEAR_SRGB:
			values.setLinearSrgb(Vector3R(m_values[0], m_values[1], m_values[2]), EQuantity::ECF);
			break;

		default:
			std::cerr << "warning: at ConstantImage::genTextureSpectral(), "
			          << "unsupported value type, assuming ECF linear sRGB" << std::endl;
			values.setLinearSrgb(Vector3R(m_values[0], m_values[1], m_values[2]), EQuantity::ECF);
			break;
		}
	}
	else
	{
		if(m_values.size() != SpectralStrength::NUM_VALUES)
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

		for(std::size_t i = 0; i < SpectralStrength::NUM_VALUES; i++)
		{
			values[i] = i < m_values.size() ? m_values[i] : 1;
		}
	}

	return std::make_shared<TConstantTexture<SpectralStrength>>(values);
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
		const auto& typeString = packet.getString("value-type", "raw");
		EType type;
		if(typeString == "raw")
		{
			type = EType::RAW;
		}
		else if(typeString == "emr-linear-srgb")
		{
			type = EType::EMR_LINEAR_SRGB;
		}
		else if(typeString == "ecf-linear-srgb")
		{
			type = EType::ECF_LINEAR_SRGB;
		}

		InputPrototype realInput;
		realInput.addReal("value");

		InputPrototype vec3Input;
		vec3Input.addVector3r("value");

		// TODO: array input

		if(packet.isPrototypeMatched(realInput))
		{
			const real value = packet.getReal("value");
			return std::make_unique<ConstantImage>(value, type);
		}
		else if(packet.isPrototypeMatched(vec3Input))
		{
			const Vector3R value = packet.getVector3r("value");
			return std::make_unique<ConstantImage>(value, type);
		}
		else
		{
			std::cerr << "warning: ill-formed input detected during ConstantImage loading" << std::endl;
			return std::make_unique<ConstantImage>();
		}
	}));
}

}// end namespace ph