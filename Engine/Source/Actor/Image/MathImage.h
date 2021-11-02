#pragma once

#include "Actor/Image/Image.h"
#include "Common/primitive_type.h"
#include "Math/TVector3.h"

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

class MathImage : public Image
{
public:
	MathImage();

	std::shared_ptr<TTexture<Image::Array>> genNumericTexture(
		ActorCookingContext& ctx) override;

	std::shared_ptr<TTexture<math::Spectrum>> genColorTexture(
		ActorCookingContext& ctx) override;

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
};

}// end namespace ph
