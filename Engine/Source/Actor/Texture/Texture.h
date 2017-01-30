#pragma once

#include "Common/primitive_type.h"
#include "Math/TVector3.h"

namespace ph
{

class InputPacket;

class Texture
{
public:
	Texture();
	Texture(const InputPacket& packet);
	virtual ~Texture() = 0;

	virtual void sample(const Vector3R& uvw, Vector3R* const out_value) const = 0;
};

}// end namespace ph