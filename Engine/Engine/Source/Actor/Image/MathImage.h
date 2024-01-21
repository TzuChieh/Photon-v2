#pragma once

#include "Actor/Image/Image.h"
#include "SDL/sdl_interface.h"

#include <Common/primitive_type.h>

#include <vector>
#include <optional>

namespace ph
{

enum class EMathImageOp
{
	Add = 0,
	Subtract,
	Multiply,
	Divide
};

PH_DEFINE_SDL_ENUM(TSdlGeneralEnum<EMathImageOp>)
{
	SdlEnumType sdlEnum("math-image-op");
	sdlEnum.description("The mathematical operation used on images.");

	sdlEnum.addEntry(EnumType::Add,      "add");
	sdlEnum.addEntry(EnumType::Subtract, "sub");
	sdlEnum.addEntry(EnumType::Multiply, "mul");
	sdlEnum.addEntry(EnumType::Divide,   "div");

	return sdlEnum;
}

class MathImage : public Image
{
public:
	MathImage();

	std::shared_ptr<TTexture<Image::ArrayType>> genNumericTexture(
		const CookingContext& ctx) override;

	std::shared_ptr<TTexture<math::Spectrum>> genColorTexture(
		const CookingContext& ctx) override;

	MathImage& setOperation(EMathImageOp op);
	MathImage& setOperandImage(std::shared_ptr<Image> operand);
	MathImage& setScalarInput0(float64 value);
	MathImage& setScalarInput1(float64 value);
	MathImage& setInputImage0(std::shared_ptr<Image> input);
	MathImage& setInputImage1(std::shared_ptr<Image> input);

private:
	EMathImageOp           m_mathOp;
	std::shared_ptr<Image> m_operandImage;
	float64                m_scalarInput0;
	float64                m_scalarInput1;
	std::shared_ptr<Image> m_imageInput0;
	std::shared_ptr<Image> m_imageInput1;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<MathImage>)
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

		TSdlReal<OwnerType, float64> scalarInput0("scalar-input-0", &OwnerType::m_scalarInput0);
		scalarInput0.description(
			"First scalar input for the specified mathematical operation. This will only be used if "
			"no image input is provided. Using scalar input is also more efficient than specifying "
			"the scalar as a constant image input.");
		scalarInput0.defaultTo(0.0);
		scalarInput0.optional();
		clazz.addField(scalarInput0);

		TSdlReal<OwnerType, float64> scalarInput1("scalar-input-1", &OwnerType::m_scalarInput1);
		scalarInput1.description(
			"Second scalar input for the specified mathematical operation. This will only be used if "
			"no image input is provided. Using scalar input is also more efficient than specifying "
			"the scalar as a constant image input.");
		scalarInput1.defaultTo(0.0);
		scalarInput1.optional();
		clazz.addField(scalarInput1);

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
