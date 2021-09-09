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
	{ T::getReferenceWhite() } noexcept -> std::same_as<EReferenceWhite>;
};

template<typename T>
concept CSupportsCIEXYZConversions = requires (const Vector3R& thisColor, const Vector3R& CIEXYZColor)
{
	{ T::toCIEXYZ(thisColor) } noexcept -> std::same_as<Vector3R>;
	{ T::fromCIEXYZ(CIEXYZColor) } noexcept -> std::same_as<Vector3R>;
};

template<typename T>
concept CSupportsSpectralConversions = requires ()
{
	{ T::toSampled() }
	{ T::fromSampled() }
};

template<typename T>
concept CColorSpaceDefinition = CHasColorSpaceProperties<T>;

template<EColorSpace COLOR_SPACE>
class TColorSpaceDefinition final
{
	static_assert(COLOR_SPACE == EColorSpace::UNSPECIFIED,
		"No definition for the specified COLOR_SPACE.");
};

template<>
class TColorSpaceDefinition<EColorSpace::CIE_XYZ> final
{
public:
	inline static bool isTristimulus()
	{
		return true;
	}

	inline static EReferenceWhite getReferenceWhite()
	{
		return EReferenceWhite::D65;
	}

	inline static Vector3R toCIEXYZ(const Vector3R& thisColor)
	{
		// Already in CIE-XYZ space, simply return the source color.
		return thisColor;
	}

	inline static Vector3R fromCIEXYZ(const Vector3R& CIEXYZColor)
	{
		// Already in CIE-XYZ space, simply return the source color.
		return CIEXYZColor;
	}
};

}// end namespace ph::math
