#pragma once

#include "Common/primitive_type.h"
#include "Math/Vector3f.h"

namespace ph
{

class Vector3f;
class InputPacket;

class Texture
{
public:
	Texture();
	Texture(const InputPacket& packet);
	virtual ~Texture() = 0;

	virtual void sample(const Vector3f& uvw, Vector3f* const out_value) const = 0;
};

}// end namespace ph