#include "Engine/Actor/Image/TransformedImage.h"
#include "Engine/Actor/Basic/exceptions.h"
#include "Engine/Core/Texture/Function/sample_location_operators.h"
#include "Engine/Math/TDecomposedTransform.h"
#include "Engine/Math/TMatrix4.h"

#include <memory>
#include <utility>

namespace ph
{

namespace
{

template<typename OutputType>
auto make_transformed_texture(
	std::shared_ptr<TTexture<OutputType>> inputTexture,
	const TransformInfo& transformInfo)
-> std::shared_ptr<TTexture<OutputType>>
{
	const auto transform = transformInfo.getDecomposed();
	if(transform.isIdentity())
	{
		return inputTexture;
	}

	math::Matrix4R transformMatrix;
	transform.genTransformMatrix(&transformMatrix);

	return std::make_shared<TSampleLocationOperator<OutputType, texfunc::AffineUvwTransform>>(
		std::move(inputTexture),
		texfunc::AffineUvwTransform(transformMatrix));
}

}// end namespace

std::shared_ptr<TTexture<Image::NumericType>> TransformedImage::genNumericTexture(
	const CookingContext& ctx)
{
	if(!m_input)
	{
		throw CookException("Transformed image requires an input image.");
	}

	return make_transformed_texture(
		m_input->genNumericTexture(ctx),
		m_transform);
}

std::shared_ptr<TTexture<math::Spectrum>> TransformedImage::genColorTexture(
	const CookingContext& ctx)
{
	if(!m_input)
	{
		throw CookException("Transformed image requires an input image.");
	}

	return make_transformed_texture(
		m_input->genColorTexture(ctx),
		m_transform);
}

TransformedImage& TransformedImage::setInput(std::shared_ptr<Image> input)
{
	m_input = std::move(input);
	return *this;
}

TransformedImage& TransformedImage::setTransform(const TransformInfo& transform)
{
	m_transform = transform;
	return *this;
}

Image* TransformedImage::getInput() const
{
	return m_input.get();
}

const TransformInfo& TransformedImage::getTransform() const
{
	return m_transform;
}

}// end namespace ph
