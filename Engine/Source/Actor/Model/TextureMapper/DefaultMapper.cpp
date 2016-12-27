#include "Actor/Model/TextureMapper/DefaultMapper.h"
#include "Math/Vector3f.h"

namespace ph
{

DefaultMapper::~DefaultMapper() = default;

void DefaultMapper::map(const Vector3f& position, const Vector3f& uvw, Vector3f* const out_uvw) const
{
	*out_uvw = uvw;
}

}// end namespace ph