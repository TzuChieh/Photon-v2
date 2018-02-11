#pragma once

#include "Core/Quantity/SpectralStrength.h"
#include "Core/Texture/TTexture.h"
#include "Core/SurfaceHit.h"
#include "Core/Intersectable/Primitive.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/Texture/SampleLocation.h"

namespace ph
{

template<typename OutputType>
class TSampler final
{
public:
	inline OutputType sample(const TTexture<OutputType>& texture, const SurfaceHit& X)
	{
		Vector3R uvw;
		const auto& textureChannel = X.getDetail().getPrimitive()->getMetadata()->getDefaultTextureChannel();
		textureChannel.getMapper()->map(X.getDetail(), &uvw);

		OutputType value;
		texture.sample(SampleLocation(X, uvw), &value);
		return value;
	}
};

}// end namespace ph