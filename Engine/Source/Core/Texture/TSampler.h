#pragma once

#include "Core/Quantity/SpectralStrength.h"
#include "Core/Texture/TTexture.h"
#include "Core/HitDetail.h"
#include "Core/Intersectable/Primitive.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/Texture/SampleLocation.h"

namespace ph
{

template<typename OutputType>
class TSampler final
{
public:
	inline OutputType sample(const TTexture<OutputType>& texture, const HitDetail& X)
	{
		Vector3R uvw;
		X.getPrimitive()->getMetadata()->getDefaultTextureChannel().getMapper()->map(X, &uvw);

		OutputType value;
		texture.sample(SampleLocation(X, uvw), &value);
		return value;
	}
};

}// end namespace ph