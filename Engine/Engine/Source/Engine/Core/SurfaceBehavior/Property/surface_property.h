#pragma once

#include "Engine/Core/SurfaceHit.h"
#include "Engine/Core/Texture/TSampler.h"
#include "Engine/Core/Texture/TTexture.h"
#include "Engine/Math/Color/color_enums.h"

#include <Common/assertion.h>

#include <concepts>
#include <memory>
#include <utility>

namespace ph
{

template<typename Property, typename Value>
concept CSurfaceProperty = requires (
	const Property& property,
	const SurfaceHit& X)
{
	{ property(X) } -> std::convertible_to<Value>;
};

template<typename T>
struct TConstantSurfaceProperty
{
	T constant;

	explicit TConstantSurfaceProperty(T constant)
		: constant(std::move(constant))
	{}

	T operator () (const SurfaceHit& /* X */) const
	{
		return constant;
	}
};

template<typename T, math::EColorUsage USAGE = math::EColorUsage::Raw>
struct TTexturedSurfaceProperty
{
	std::shared_ptr<TTexture<T>> texture;

	explicit TTexturedSurfaceProperty(std::shared_ptr<TTexture<T>> texture)
		: texture(std::move(texture))
	{}

	T operator () (const SurfaceHit& X) const
	{
		PH_ASSERT(texture);
		return TSampler<T>(USAGE).sample(*texture, X);
	}
};

}// end namespace ph