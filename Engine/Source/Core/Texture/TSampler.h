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
	inline TSampler() :
		TSampler(EQuantity::RAW)
	{}

	inline TSampler(const EQuantity sampledQuantity) : 
		m_sampledQuantity(sampledQuantity)
	{}

	inline OutputType sample(const TTexture<OutputType>& texture, const SurfaceHit& X) const
	{
		Vector3R uvw;
		const auto& textureChannel = X.getDetail().getPrimitive()->getMetadata()->getDefaultTextureChannel();
		textureChannel.getMapper()->map(X.getDetail(), &uvw);

		OutputType value;
		texture.sample(SampleLocation(X, uvw, m_sampledQuantity), &value);
		return value;
	}

private:
	EQuantity m_sampledQuantity;
};

}// end namespace ph