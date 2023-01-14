#pragma once

#include "Actor/Image/Image.h"
#include "Common/primitive_type.h"
#include "DataIO/SDL/sdl_interface.h"

#include <vector>
#include <utility>
#include <optional>

namespace ph
{

enum class EMathImageOp
{
	Add = 0,
	Multiply
};

PH_DEFINE_SDL_ENUM(TBasicSdlEnum<EMathImageOp>)
{
	SdlEnumType sdlEnum("math-image-op");
	sdlEnum.description("The mathematical operation used on images.");

	sdlEnum.addEntry(EnumType::Add,      "ADD");
	sdlEnum.addEntry(EnumType::Multiply, "MUL");

	return sdlEnum;
}

class MathImage : public Image
{
public:
	MathImage();

	std::shared_ptr<TTexture<Image::Array>> genNumericTexture(
		CookingContext& ctx) override;

	std::shared_ptr<TTexture<math::Spectrum>> genColorTexture(
		CookingContext& ctx) override;

	MathImage& setOperation(EMathImageOp op);
	MathImage& setOperandImage(std::shared_ptr<Image> operand);
	MathImage& setScalarInput(float64 value);
	MathImage& setInputImage0(std::shared_ptr<Image> input);
	MathImage& setInputImage1(std::shared_ptr<Image> input);

private:
	EMathImageOp           m_mathOp;
	std::shared_ptr<Image> m_operandImage;
	float64                m_scalarInput;
	std::shared_ptr<Image> m_imageInput0;
	std::shared_ptr<Image> m_imageInput1;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<MathImage>)
	{
		ClassType clazz("math");
		clazz.docName("Math Image");
		clazz.description(
			"This image applies mathematical modifications on other images, such as addition and "
			"multiplication.");
		clazz.baseOn<Image>();

		TSdlEnumField<OwnerType, EMathImageOp> mathOp(&OwnerType::m_mathOp);
		mathOp.description("The mathematical operation used.");
		mathOp.defaultTo(EMathImageOp::Add);
		mathOp.required();
		clazz.addField(mathOp);

		TSdlReference<Image, OwnerType> operandImage("operand", &OwnerType::m_operandImage);
		operandImage.description("The target image that is going to be operated on.");
		operandImage.required();
		clazz.addField(operandImage);

		TSdlReal<OwnerType, float64> scalarInput("scalar", &OwnerType::m_scalarInput);
		scalarInput.description("A scalar input for the specified mathematical operation.");
		scalarInput.defaultTo(0.0);
		scalarInput.optional();
		clazz.addField(scalarInput);

		TSdlReference<Image, OwnerType> imageInput0("input-0", &OwnerType::m_imageInput0);
		imageInput0.description("First input for the specified mathematical operation.");
		imageInput0.optional();
		clazz.addField(imageInput0);

		TSdlReference<Image, OwnerType> imageInput1("input-1", &OwnerType::m_imageInput1);
		imageInput1.description("Second input for the specified mathematical operation.");
		imageInput1.optional();
		clazz.addField(imageInput1);

		return clazz;

	}
};

}// end namespace ph
