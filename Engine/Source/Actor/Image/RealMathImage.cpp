#include "Actor/Image/RealMathImage.h"
#include "Common/assertion.h"
#include "FileIO/SDL/InputPacket.h"

#include <memory>

namespace ph
{

RealMathImage::RealMathImage() :
	RealMathImage(EMathOp::MULTIPLY, 1.0_r)
{}

RealMathImage::RealMathImage(const EMathOp mathOp, const real value) :
	Image(),
	m_mathOp(mathOp), m_real(value), m_operandImage()
{}

std::shared_ptr<TTexture<real>> RealMathImage::genTextureReal(
	CookingContext& context) const
{
	auto operandImage = checkOperandImage();
	if(!operandImage)
	{
		return nullptr;
	}

	auto operandTexture = operandImage->genTextureReal(context);
	return genTexture<real, real>(operandTexture);
}

std::shared_ptr<TTexture<Vector3R>> RealMathImage::genTextureVector3R(
	CookingContext& context) const
{
	auto operandImage = checkOperandImage();
	if(!operandImage)
	{
		return nullptr;
	}

	auto operandTexture = operandImage->genTextureVector3R(context);
	return genTexture<Vector3R, Vector3R>(operandTexture);
}

std::shared_ptr<TTexture<SpectralStrength>> RealMathImage::genTextureSpectral(
	CookingContext& context) const
{
	auto operandImage = checkOperandImage();
	if(!operandImage)
	{
		return nullptr;
	}

	auto operandTexture = operandImage->genTextureSpectral(context);
	return genTexture<SpectralStrength, SpectralStrength>(operandTexture);
}

RealMathImage& RealMathImage::setOperandImage(const std::shared_ptr<Image>& operand)
{
	PH_ASSERT(operand != nullptr);
	m_operandImage = operand;

	return *this;
}

RealMathImage& RealMathImage::setMathOp(const EMathOp mathOp)
{
	m_mathOp = mathOp;

	return *this;
}

RealMathImage& RealMathImage::setReal(const real value)
{
	m_real = value;

	return *this;
}

std::shared_ptr<Image> RealMathImage::checkOperandImage() const
{
	const auto operandImage = m_operandImage.lock();
	if(!operandImage)
	{
		std::cout << "note: at ConstantMathImage::checkOperandImage(), "
		          << "parent image is not set" << std::endl;
	}
	return operandImage;
}

// command interface

SdlTypeInfo RealMathImage::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_IMAGE, "real-math");
}

void RealMathImage::ciRegister(CommandRegister& cmdRegister)
{
	cmdRegister.setLoader(SdlLoader([](const InputPacket& packet)
	{
		const auto& mathOpType   = packet.getString ("math-op", "multiply", DataTreatment::REQUIRED());
		const real  realValue    = packet.getReal   ("value",   1.0_r,      DataTreatment::REQUIRED());
		const auto& operandImage = packet.get<Image>("operand", DataTreatment::REQUIRED());
		
		auto image = std::make_unique<RealMathImage>();
		image->setReal(realValue);
		image->setOperandImage(operandImage);
		if(mathOpType == "multiply")
		{
			image->setMathOp(EMathOp::MULTIPLY);
		}
		else if(mathOpType == "add")
		{
			image->setMathOp(EMathOp::ADD);
		}
		else
		{
			std::cerr << "warning: no valid math-op specified while loading RealMathImage" << std::endl;
		}
		return image;
	}));
}

}// end namespace ph