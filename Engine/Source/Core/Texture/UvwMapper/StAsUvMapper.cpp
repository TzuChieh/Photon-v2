#include "Core/Texture/UvwMapper/StAsUvMapper.h"

namespace ph
{

StAsUvMapper::~StAsUvMapper() = default;

void StAsUvMapper::map(const HitDetail& X, Vector3R* const out_uvw) const
{
	out_uvw->set(Vector3R(X.getST().x, X.getST().y, 0));
}

}// end namespace ph