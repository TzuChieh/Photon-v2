#include "Engine/Actor/Image/Image.h"
#include "Engine/Math/TVector2.h"
#include "Engine/Math/TVector3.h"
#include "Engine/Math/TVector4.h"
#include "Engine/Core/Texture/Function/unary_texture_operators.h"

#include <memory>

namespace ph
{

std::shared_ptr<TTexture<real>> Image::genRealTexture(const CookingContext& ctx)
{
	auto numericArrayToReal = [](const Image::ArrayType& inputValue)
	{
		return static_cast<real>(inputValue[0]);
	};

	return std::make_shared<TUnaryTextureOperator<Image::ArrayType, real, decltype(numericArrayToReal)>>(
		genNumericTexture(ctx), std::move(numericArrayToReal));
}

std::shared_ptr<TTexture<math::Vector2R>> Image::genVector2RTexture(const CookingContext& ctx)
{
	auto numericArrayToVec2 = [](const Image::ArrayType& inputValue)
	{
		return math::Vector2R(inputValue[0], inputValue[1]);
	};

	return std::make_shared<TUnaryTextureOperator<Image::ArrayType, math::Vector2R, decltype(numericArrayToVec2)>>(
		genNumericTexture(ctx), std::move(numericArrayToVec2));
}

std::shared_ptr<TTexture<math::Vector3R>> Image::genVector3RTexture(const CookingContext& ctx)
{
	auto numericArrayToVec3 = [](const Image::ArrayType& inputValue)
	{
		return math::Vector3R(inputValue[0], inputValue[1], inputValue[2]);
	};

	return std::make_shared<TUnaryTextureOperator<Image::ArrayType, math::Vector3R, decltype(numericArrayToVec3)>>(
		genNumericTexture(ctx), std::move(numericArrayToVec3));
}

std::shared_ptr<TTexture<math::Vector4R>> Image::genVector4RTexture(const CookingContext& ctx)
{
	auto numericArrayToVec4 = [](const Image::ArrayType& inputValue)
	{
		return math::Vector4R(inputValue[0], inputValue[1], inputValue[2], inputValue[3]);
	};

	return std::make_shared<TUnaryTextureOperator<Image::ArrayType, math::Vector4R, decltype(numericArrayToVec4)>>(
		genNumericTexture(ctx), std::move(numericArrayToVec4));
}

}// end namespace ph
