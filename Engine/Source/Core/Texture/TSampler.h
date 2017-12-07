#pragma once

#include "Core/Quantity/SpectralStrength.h"
#include "Core/Texture/Texture.h"
#include "Core/HitDetail.h"
#include "Core/Intersectable/Primitive.h"
#include "Core/Intersectable/PrimitiveMetadata.h"

namespace ph
{

template<typename ValueType>
class TSampler final
{
public:
	inline ValueType sample(const Texture& texture, const HitDetail& X)
	{
		Vector3R uvw;
		X.getPrimitive()->getMetadata()->getDefaultTextureChannel().getMapper()->map(X, &uvw);

		ValueType value;
		texture.sample(uvw, &value);

		return value;
	}
};

}// end namespace ph