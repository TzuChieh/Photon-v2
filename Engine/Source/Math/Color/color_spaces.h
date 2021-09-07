#pragma once

#include "Math/Color/EColorSpace.h"
#include "Math/Color/EColorUsage.h"
#include "Math/Color/EReferenceWhite.h"
#include "Common/primitive_type.h"
#include "Math/TVector3.h"

#include <concepts>

namespace ph::math
{

template<typename T>
concept CHasColorSpaceProperties = requires ()
{
	{ T::isTristimulus() } noexcept -> std::same_as<bool>;
	{ T::isSpectral() } noexcept -> std::same_as<bool>;
	{ T::getReferenceWhite() } noexcept -> std::same_as<EReferenceWhite>;
};

template<typename T>
concept CSupportsCIEXYZConversions = requires (const Vector3R& CIEXYZColor)
{
	{ T::toCIEXYZ() } noexcept -> std::same_as<Vector3R>;
	{ T::fromCIEXYZ(CIEXYZColor) } noexcept -> std::same_as<Vector3R>;
};

}// end namespace ph::math
