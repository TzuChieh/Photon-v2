#pragma once

#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "Core/Quantity/SpectralStrength.h"

namespace ph
{

class InputPacket;

template<typename OutputType>
class TTexture
{
public:
	TTexture();
	virtual ~TTexture() = 0;

	virtual void sample(const Vector3R& uvw, OutputType* out_value) const = 0;
};

}// end namespace ph

#include "Core/Texture/TTexture.ipp"