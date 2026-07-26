#include "Engine/Actor/Image/AttributeImage.h"

#include "Engine/Actor/Basic/exceptions.h"
#include "Engine/Core/ECoordSys.h"
#include "Engine/Core/HitDetail.h"
#include "Engine/Core/Intersection/GeometryInfo.h"
#include "Engine/Core/Intersection/PrimitiveMetadata.h"
#include "Engine/Core/SurfaceHit.h"
#include "Engine/Core/Texture/SampleLocation.h"
#include "Engine/Math/Color/Spectrum.h"

#include <Common/assertion.h>

#include <algorithm>
#include <cstddef>
#include <memory>
#include <type_traits>

namespace ph
{

namespace
{

template<typename OutputType, EAttributeKind KIND>
class TAttributeTexture final : public TTexture<OutputType>
{
public:
	static_assert(
		std::is_same_v<OutputType, Image::NumericType> ||
		std::is_same_v<OutputType, math::Spectrum>,
		"Unsupported attribute output type.");

	void sample(const SampleLocation& sampleLocation, OutputType* const out_value) const override
	{
		storeOutput(sampleNumeric(sampleLocation), out_value);
	}

private:
	static Image::NumericType sampleNumeric(const SampleLocation& sampleLocation)
	{
		Image::NumericType value(0);

		if constexpr(KIND == EAttributeKind::UvwFromGeometryBound)
		{
			value[0] = 0.5_r;
			value[1] = 0.5_r;
			value[2] = 0.5_r;

			if(!sampleLocation.hasSurfaceHit())
			{
				return value;
			}

			const SurfaceHit& X = sampleLocation.getSurfaceHit();
			const GeometryInfo* const geometryInfo = X.getMetadata().getGeometryInfo();
			if(!geometryInfo)
			{
				return value;
			}

			const math::AABB3D& localAABB = geometryInfo->localAABB;
			if(localAABB.isEmpty())
			{
				return value;
			}

			const math::Vector3R localPosition = X.getDetail().getPos(ECoordSys::Local);
			const math::Vector3R minVertex = localAABB.getMinVertex();
			const math::Vector3R extent = localAABB.getExtents();
			for(std::size_t i = 0; i < 3; ++i)
			{
				value[i] = extent[i] != 0.0_r
					? (localPosition[i] - minVertex[i]) / extent[i]
					: 0.5_r;
			}
		}
		else if constexpr(KIND == EAttributeKind::GeometryHitPosition)
		{
			if(!sampleLocation.hasSurfaceHit())
			{
				return value;
			}

			const SurfaceHit& X = sampleLocation.getSurfaceHit();
			const math::Vector3R localPosition = X.getDetail().getPos(ECoordSys::Local);
			value[0] = localPosition.x();
			value[1] = localPosition.y();
			value[2] = localPosition.z();
		}

		return value;
	}

	static void storeOutput(const Image::NumericType& value, Image::NumericType* const out_value)
	{
		PH_ASSERT(out_value);
		*out_value = value;
	}

	static void storeOutput(const Image::NumericType& value, math::Spectrum* const out_value)
	{
		PH_ASSERT(out_value);
		math::Spectrum& color = *out_value;

		color = math::Spectrum(0);
		const auto numComponents = std::min(value.size(), color.getColorValues().size());
		for(std::size_t i = 0; i < numComponents; ++i)
		{
			color[i] = static_cast<math::ColorValue>(value[i]);
		}
	}
};

template<typename OutputType>
std::shared_ptr<TTexture<OutputType>> make_attribute_texture(const EAttributeKind kind)
{
	switch(kind)
	{
	case EAttributeKind::UvwFromGeometryBound:
		return std::make_shared<
			TAttributeTexture<OutputType, EAttributeKind::UvwFromGeometryBound>>();

	case EAttributeKind::GeometryHitPosition:
		return std::make_shared<
			TAttributeTexture<OutputType, EAttributeKind::GeometryHitPosition>>();

	default:
		throw CookException("unsupported attribute kind");
	}
}

}// end anonymous namespace

std::shared_ptr<TTexture<Image::NumericType>> AttributeImage::genNumericTexture(
	const CookingContext& /* ctx */)
{
	return make_attribute_texture<Image::NumericType>(m_kind);
}

std::shared_ptr<TTexture<math::Spectrum>> AttributeImage::genColorTexture(
	const CookingContext& /* ctx */)
{
	return make_attribute_texture<math::Spectrum>(m_kind);
}

}// end namespace ph
