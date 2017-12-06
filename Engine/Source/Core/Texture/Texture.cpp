#include "Core/Texture/Texture.h"

namespace ph
{

Texture::Texture() = default;

Texture::Texture(const InputPacket& packet)
{

}

Texture::~Texture() = default;

void Texture::sample(const Vector3R& uvw, SpectralStrength* const out_value) const
{
	out_value->set(0.0_r);
}

void Texture::sample(const Vector3R& uvw, Vector3R* const out_value) const
{
	out_value->set(0.0_r);
}

void Texture::sample(const Vector3R& uvw, real* const out_value) const
{
	*out_value = 0.0_r;
}

}// end namespace ph