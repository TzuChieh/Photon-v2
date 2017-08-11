#pragma once

#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "Core/Quantity/SpectralStrength.h"

namespace ph
{

class InputPacket;

class Texture
{
public:
	Texture();
	Texture(const InputPacket& packet);
	virtual ~Texture() = 0;

	virtual void sample(const Vector3R& uvw, SpectralStrength* out_value) const = 0;
};

}// end namespace ph