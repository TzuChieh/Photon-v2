#include "Actor/TextureMapper/DefaultMapper.h"
#include "Math/TVector3.h"

namespace ph
{

DefaultMapper::~DefaultMapper() = default;

void DefaultMapper::map(const Vector3R& position, const Vector3R& uvw, Vector3R* const out_uvw) const
{
	*out_uvw = uvw;
}

}// end namespace ph